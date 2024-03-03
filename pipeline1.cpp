#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include <iomanip>
#include <map>
#include <algorithm>
#include <sstream>
#include <functional>

class Core {
public:
    std::unordered_map<std::string, int> registers;
    int pc;
    int cycleCount;
    int stallCount;
    std::vector<std::vector<std::string>> program;
    std::map<std::string, int> labels;
    std::vector<std::vector<std::string>> IF_ID_register;
    std::vector<std::vector<std::string>> ID_EX_register;
    std::vector<std::vector<std::string>> EX_MEM_register;
    std::vector<std::vector<std::string>> MEM_WB_register;

    int IF_latency;
    int ID_latency;
    int EX_latency;
    int MEM_latency;
    int WB_latency;

    Core() : Core(0, 0, 0, 0, 0) {}
    Core(int if_lat, int id_lat, int ex_lat, int mem_lat, int wb_lat)
        : program(), labels(), pc(0), IF_latency(if_lat), ID_latency(id_lat), EX_latency(ex_lat), MEM_latency(mem_lat), WB_latency(wb_lat), registers(), IF_ID_register(), ID_EX_register(), EX_MEM_register(), MEM_WB_register() {}

    void execute(std::vector<int>& memory, bool enableForwarding, const std::unordered_map<std::string, int>& customLatencies);
    int getRegister(const std::string& reg);
    void setRegister(std::string reg, int num);
     int getRegisterValue(const std::string& reg, bool enableForwarding);
    void setProgram(std::pair<std::vector<std::vector<std::string>>, std::map<std::string, int>> parsedProgram);
    const std::unordered_map<std::string, int>& getRegisters() const;
    void reset();

    enum PipelineStage {
        FETCH,
        DECODE,
        EXECUTE,
        MEMORY,
        WRITEBACK,
        NUM_STAGES
    };

    std::map<std::string, int> instructionLatencies;
    const std::unordered_map<std::string, int> getLatencyMap() const;
    Core::PipelineStage currentStage;
    int stageCounter;
    int ticks;

    void pipelineFetch();
    void pipelineDecode();
    void pipelineExecute(std::vector<int>& memory, bool enableForwarding, const std::unordered_map<std::string, int>& customLatencies);
    void pipelineMemory(std::vector<int>& memory);
    void pipelineWriteBack(bool enableForwarding);
    void advancePipeline();
    void executeArithmetic(const std::string& opcode);
    bool predict(int currentPC); // Branch prediction function
};

void Core::setProgram(std::pair<std::vector<std::vector<std::string>>, std::map<std::string, int>> parsedProgram) {
    program = parsedProgram.first;
    labels = parsedProgram.second;
    pc = 0;  // Reset the program counter
}

void Core::advancePipeline() {
    currentStage = static_cast<PipelineStage>((currentStage + 1) % NUM_STAGES);
    stageCounter = 0; // Reset the stage counter when moving to the next stage
}

void Core::execute(std::vector<int>& memory, bool enableForwarding, const std::unordered_map<std::string, int>& customLatencies) {
    while (pc < program.size() || currentStage != WRITEBACK) {
        switch (currentStage) {
            case FETCH:
                pipelineFetch();
                break;
            case DECODE:
                pipelineDecode();
                break;
            case EXECUTE:
                pipelineExecute(memory, enableForwarding, customLatencies);
                break;
            case MEMORY:
                pipelineMemory(memory);
                break;
            case WRITEBACK:
                pipelineWriteBack(enableForwarding);
                break;
            default:
                break;
        }
        if (currentStage == EXECUTE) {
            int latency = customLatencies.find(ID_EX_register[0][0]) != customLatencies.end() ? customLatencies.at(ID_EX_register[0][0]) : 1;
            stageCounter += latency;
        }


        if (currentStage == WRITEBACK && pc >= program.size()) {
            // Reached the end of the program
            break;
        }

        // Increment cycle count
        cycleCount++;

        // Introduce stall if needed
        if (currentStage != WRITEBACK && stageCounter < instructionLatencies[ID_EX_register[0][0]]) {
            // Stall
            stallCount++;
        } else {
            if (stageCounter >= instructionLatencies[ID_EX_register[0][0]]) {
                advancePipeline();
            }

            // Increment ticks only when not stalling
            ticks++;
        }
    }
}

void Core::pipelineFetch() {
    if (pc < program.size()) {
        IF_ID_register = { program[pc] }; // Wrap the vector in another vector
        pc += 1;
    } else {
        IF_ID_register.clear(); // Clear the IF_ID_register when the program is done
    }
}

void Core::pipelineDecode() {
    if (!IF_ID_register.empty()) {
        ID_EX_register = IF_ID_register;
        IF_ID_register.clear(); // Clear the IF_ID_register after using it
    }
}

void Core::pipelineExecute(std::vector<int>& memory, bool enableForwarding, const std::unordered_map<std::string, int>& customLatencies) {
    if (!ID_EX_register.empty()) {
        std::string opcode = ID_EX_register[0][0]; // Access the opcode in the first vector

        // Execute branch prediction
        bool branchPredictionResult = predict(pc);

        if (opcode == "beq") {
            // Handle branch instruction
            if (branchPredictionResult) {
                // Branch taken
                pc = labels[ID_EX_register[0][3]]; // Use the label to get the branch target
                IF_ID_register.clear(); // Clear the IF_ID_register since we are taking a branch
            }
        } else {
            if (opcode == "jal" || opcode == "j") {
                // Handle jump instruction
                pc = labels[ID_EX_register[0][1]]; // Use the label to get the jump target
                IF_ID_register.clear(); // Clear the IF_ID_register since we are jumping
            } else {
                // Execute other instructions
                executeArithmetic(opcode);
            }
        }
    }
}

void Core::pipelineMemory(std::vector<int>& memory) {
    if (!EX_MEM_register.empty()) {
        MEM_WB_register = EX_MEM_register;
        EX_MEM_register.clear(); // Clear the EX_MEM_register after using it
    }
}
int Core::getRegisterValue(const std::string& reg, bool enableForwarding) {
    if (!enableForwarding) {
        return getRegister(reg);
    }

    // Forward from MEM/WB stage
    if (!MEM_WB_register.empty() && MEM_WB_register[0][1] == reg) {
        return std::stoi(MEM_WB_register[0][3]);
    }

    // Forward from EX stage
    if (!EX_MEM_register.empty() && EX_MEM_register[0][1] == reg) {
        return std::stoi(EX_MEM_register[0][3]);
    }

    // Forward from ID stage
    if (!ID_EX_register.empty() && ID_EX_register[0][1] == reg) {
        return std::stoi(ID_EX_register[0][3]);
    }

    // No forwarding, get value from registers
    return getRegister(reg);
}

void Core::pipelineWriteBack(bool enableForwarding) {
    if (!MEM_WB_register.empty()) {
        if (ID_EX_register[0][0] == "add" || ID_EX_register[0][0] == "sub") {
            std::string rd = ID_EX_register[0][1]; // Accessing the opcode of the instruction
            if (MEM_WB_register[0][0] == "add" || MEM_WB_register[0][0] == "sub") {
                std::string fwdReg = MEM_WB_register[0][1]; // Accessing the opcode of the forwarded instruction
                if (fwdReg == rd && enableForwarding) {
                    registers[rd] = std::stoi(MEM_WB_register[0][3]); // Assuming the fourth element is a string representation of an integer
                }
            }
        }
    }
}

void Core::executeArithmetic(const std::string& opcode) {
    if (ID_EX_register.size() >= 4) {
        std::string rd = ID_EX_register[0][1];
        std::string rs1 = ID_EX_register[0][2];
        std::string rs2 = ID_EX_register[0][3];

        // Check for data hazards and forward data if forwarding is enabled
        int rs1Value = getRegister(rs1);
        int rs2Value = getRegister(rs2);

        if (opcode != "jal" && opcode != "j") {
            // Exclude jump instructions from data hazard checks
            if (ID_EX_register[0][0] == "add" || ID_EX_register[0][0] == "sub") {
                std::string fwdReg = ID_EX_register[0][1]; // Accessing the opcode of the forwarded instruction
                if (fwdReg == rs1) {
                    rs1Value = std::stoi(EX_MEM_register[0][3]);
                }
            }
        }

        if (opcode != "jal" && opcode != "j") {
            // Exclude jump instructions from data hazard checks
            if (ID_EX_register[0][0] == "add" || ID_EX_register[0][0] == "sub") {
                std::string fwdReg = ID_EX_register[0][1]; // Accessing the opcode of the forwarded instruction
                if (fwdReg == rs2) {
                    rs2Value = std::stoi(EX_MEM_register[0][3]);
                }
            }
        }

        // Execute arithmetic operation
        if (opcode == "add") {
            registers[rd] = rs1Value + rs2Value;
        } else if (opcode == "sub") {
            registers[rd] = rs1Value - rs2Value;
        }

        // Increment stage counter based on the latency of the executed instruction
        int latency = instructionLatencies.find(opcode) != instructionLatencies.end() ? instructionLatencies.at(opcode) : 1;
stageCounter += latency;
    }
}

bool Core::predict(int currentPC) {
    // Dummy implementation: Always predict not taken
    return false;
}

int Core::getRegister(const std::string& reg) {
    if (registers.find(reg) != registers.end()) {
        return registers[reg];
    } else {
        std::cerr << "Register " << reg << " not found." << std::endl;
        return 0;
    }
}

void Core::setRegister(std::string reg, int num) {
    if (registers.find(reg) != registers.end() && reg != "zero") {
        registers[reg] = num;
    } else {
        std::cerr << "Register " << reg << " not found." << std::endl;
    }
}

const std::unordered_map<std::string, int>& Core::getRegisters() const {
    return registers;
}

void Core::reset() {
    registers = {
        {"zero", 0}, {"ra", 0}, {"sp", 0}, {"gp", 0}, {"tp", 0}, {"t0", 0}, {"t1", 0}, {"t2", 0}, {"s0", 0}, {"s1", 0},
        {"a0", 0}, {"a1", 0}, {"a2", 0}, {"a3", 0}, {"a4", 0}, {"a5", 0}, {"a6", 0}, {"a7", 0}, {"s2", 0}, {"s3", 0},
        {"s4", 0}, {"s5", 0}, {"s6", 0}, {"s7", 0}, {"s8", 0}, {"s9", 0}, {"s10", 0}, {"s11", 0}, {"t3", 0}, {"t4", 0},
        {"t5", 0}, {"t6", 0}
    };
    std::cout << "Reset processor" << std::endl;
}

class Processor {
public:
    std::vector<int> memory;
    int clock;
    std::vector<Core> cores;

    Processor();
    void run(bool enableForwarding, const std::unordered_map<std::string, int>& customLatencies);
    void setInitialMemory(int wordAddress, int value);
    void setMemory(int wordAddress, int value);  // Remove this line if you don't need it.
    int getMemory(int wordAddress);
    const std::vector<int>& getMemoryContents() const;
    const std::unordered_map<std::string, int>& getCoreRegisters(int coreIndex) const;
};

Processor::Processor() : memory(4096, 0), clock(0), cores(2) {}

void Processor::run(bool enableForwarding, const std::unordered_map<std::string, int>& customLatencies) {
    int totalCycles = 0;
    int totalStalls = 0;

    while (true) {
        bool allCoresCompleted = true;
        for (int i = 0; i < cores.size(); ++i) {
            if (cores[i].pc < cores[i].program.size() || cores[i].currentStage != Core::WRITEBACK) {
                allCoresCompleted = false;
                cores[i].execute(memory, enableForwarding, customLatencies);
            }
        }

        if (allCoresCompleted) {
            break;
        }

        totalCycles++;
    }

    // Print statistics
    std::cout << "Simulation completed." << std::endl;
    std::cout << "Total Cycles: " << totalCycles << std::endl;

    for (int i = 0; i < cores.size(); ++i) {
        totalStalls += cores[i].stallCount;
        std::cout << "Core " << i << " - Stalls: " << cores[i].stallCount << " | IPC: " << std::fixed << std::setprecision(2) << static_cast<double>(cores[i].cycleCount) / totalCycles << std::endl;
    }

    std::cout << "Total Stalls: " << totalStalls << std::endl;
}

void Processor::setInitialMemory(int wordAddress, int value) {
    if (wordAddress >= 0 && wordAddress < memory.size()) {
        memory[wordAddress] = value;
    } else {
        std::cerr << "Invalid memory address: " << wordAddress << std::endl;
    }
}

void Processor::setMemory(int wordAddress, int value) {
    if (wordAddress >= 0 && wordAddress < memory.size()) {
        memory[wordAddress] = value;
    } else {
        std::cerr << "Invalid memory address: " << wordAddress << std::endl;
    }
}

int Processor::getMemory(int wordAddress) {
    if (wordAddress >= 0 && wordAddress < memory.size()) {
        return memory[wordAddress];
    } else {
        std::cerr << "Invalid memory address: " << wordAddress << std::endl;
        return 0;
    }
}

const std::vector<int>& Processor::getMemoryContents() const {
    return memory;
}

const std::unordered_map<std::string, int>& Processor::getCoreRegisters(int coreIndex) const {
    if (coreIndex >= 0 && coreIndex < cores.size()) {
        return cores[coreIndex].getRegisters();
    } else {
        std::cerr << "Invalid core index: " << coreIndex << std::endl;
        return cores[0].getRegisters(); // Return registers of the first core by default
    }
}

std::pair<std::vector<std::vector<std::string>>, std::map<std::string, int>> parseProgram(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<std::vector<std::string>> program;
    std::map<std::string, int> labels;
    std::string line;

    int lineCount = 0;  // Track line numbers for labels

    while (std::getline(file, line)) {
        // Split the line into tokens
        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::string token;

        while (iss >> token) {
            tokens.push_back(token);
        }

        if (!tokens.empty()) {
            // Check if the line contains a label
            if (tokens[0].back() == ':') {
                // Label found
                std::string label = tokens[0].substr(0, tokens[0].size() - 1);
                labels[label] = lineCount;
                tokens.erase(tokens.begin()); // Remove the label from tokens
            }

            program.push_back(tokens);
            lineCount++;
        }
    }

    return {program, labels};
}

int main() {
    Processor processor;
    processor.setInitialMemory(0, 5);
    processor.setInitialMemory(4, 10);
    processor.setInitialMemory(8, 15);

    // Example programs
    std::string filename1 = "code1.txt";
    std::string filename2 = "code2.txt";
    auto result1 = parseProgram(filename1);
    auto program1 = result1.first;
    auto labels1 = result1.second;
    auto result2 = parseProgram(filename2);
    auto program2 = result2.first;
    auto labels2 = result2.second;

    // Set latencies for each instruction (default latencies if not specified)
    std::unordered_map<std::string, int> customLatencies = {
        {"add", 1}, {"sub", 1}, {"mul", 3}, {"beq", 1}, {"jal", 1}, {"j", 1}
    };

    for (int i = 0; i < processor.cores.size(); ++i) {
        // Alternate between programs for each core
        if (i % 2 == 0) {
            processor.cores[i].setProgram({program1, labels1});
        } else {
            processor.cores[i].setProgram({program2, labels2});
        }
    }

    // Enable forwarding and run the simulation
    processor.run(true, customLatencies);

    // Display final memory contents
    std::cout << "\nFinal Memory Contents:" << std::endl;
    const auto& memoryContents = processor.getMemoryContents();
    for (int i = 0; i < memoryContents.size(); ++i) {
        std::cout << "Memory[" << i << "] = " << memoryContents[i] << std::endl;
    }

    // Display final register contents for each core
    for (int i = 0; i < processor.cores.size(); ++i) {
        std::cout << "\nFinal Register Contents (Core " << i << "):" << std::endl;
        const auto& registers = processor.getCoreRegisters(i);
        for (auto it = registers.begin(); it != registers.end(); ++it) {
            const auto& reg = it->first;
            const auto& value = it->second;
            std::cout << reg << " = " << value << std::endl;
        }
    }

    return 0;
}
