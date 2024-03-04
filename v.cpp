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
    std::string  stageToString(Core::PipelineStage stage);
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
    // Increment the current stage
    currentStage = static_cast<PipelineStage>((currentStage + 1) % NUM_STAGES);

    // Reset the stage counter for all stages except WRITEBACK
    stageCounter = (currentStage == FETCH) ? 0 : 1;

    // Increment the program counter (FETCH stage)
    if (currentStage == WRITEBACK) {
        pc += 1;
    }

    // Add debug information to trace the pipeline advancement
    std::cout << "Advanced to Stage: " << currentStage << " | Stage Counter Reset: " << stageCounter << " | New PC: " << pc << std::endl;
}




std::string Core :: stageToString(Core::PipelineStage stage) {
    switch (stage) {
        case Core::FETCH:
            return "FETCH";
        case Core::DECODE:
            return "DECODE";
        case Core::EXECUTE:
            return "EXECUTE";
        case Core::MEMORY:
            return "MEMORY";
        case Core::WRITEBACK:
            return "WRITEBACK";
        default:
            return "UNKNOWN";
    }
}
void Core::execute(std::vector<int>& memory, bool enableForwarding, const std::unordered_map<std::string, int>& customLatencies) {
    while (pc < program.size() || currentStage != WRITEBACK) {
        std::cout << "Current PC: " << pc << " | Current Stage: " << stageToString(currentStage) << std::endl;

        switch (currentStage) {
            case WRITEBACK:
                pipelineWriteBack(enableForwarding);
                advancePipeline();
                break;
            case MEMORY:
                pipelineMemory(memory);
                advancePipeline();
                break;
            case EXECUTE:
                pipelineExecute(memory, enableForwarding, customLatencies);
                advancePipeline();
                break;
            case DECODE:
                pipelineDecode();
                advancePipeline();
                break;
            case FETCH:
                pipelineFetch();
                advancePipeline();
                break;
            default:
                std::cerr << "Unknown stage encountered: " << currentStage << ". Exiting." << std::endl;
                return;
        }

        // Add debug prints to track the loop conditions
        std::cout << "Loop Condition: pc < program.size() = " << (pc < program.size()) << " | currentStage != WRITEBACK = " << (currentStage != WRITEBACK) << std::endl;
    }

    // Increment cycle count
    cycleCount++;

    // Introduce stall if needed
    if (currentStage != WRITEBACK && stageCounter < instructionLatencies[ID_EX_register[0][0]]) {
        // Stall
        stallCount++;
        advancePipeline();  // Add this line to advance the pipeline during a stall
    } else {
        if (stageCounter >= instructionLatencies[ID_EX_register[0][0]]) {
            advancePipeline();
        }

        // Increment ticks only when not stalling
        ticks++;

        // Increment stage counter
        stageCounter++;
    }
}

       




void Core::pipelineFetch() {
    if (pc < program.size()) {
        IF_ID_register = { program[pc] }; // Wrap the vector in another vector
        pc += 1;
    } else {
        IF_ID_register.clear(); // Clear the IF_ID_register when the program is done
    }

    std::cout << "Entering pipelineFetch() after fetch decode" << std::endl;
}



void Core::pipelineDecode() {
    std::cout << "Entering pipelineDecode()" << std::endl;

    if (!IF_ID_register.empty()) {
        std::cout << "IF_ID_register is not empty. Contents: ";
        for (const auto& elem : IF_ID_register[0]) {
            std::cout << elem << " ";
        }
        std::cout << std::endl;

        ID_EX_register = IF_ID_register;
        IF_ID_register.clear(); // Clear the IF_ID_register after using it
        std::cout << "Moved content to ID_EX_register." << std::endl;
    } else {
        std::cout << "IF_ID_register is empty." << std::endl;
    }
}


void Core::pipelineExecute(std::vector<int>& memory, bool enableForwarding, const std::unordered_map<std::string, int>& customLatencies) {
    if (!ID_EX_register.empty()) {
        std::string opcode = ID_EX_register[0][0]; // Access the opcode in the first vector
        std::cout << "Executing Instruction: " << opcode << std::endl;

        // Execute branch prediction
        bool branchPredictionResult = predict(pc);

        if (opcode == "beq") {
            // Handle branch instruction
            if (branchPredictionResult) {
                // Branch taken
                pc = labels[ID_EX_register[0][3]]; // Use the label to get the branch target
                IF_ID_register.clear(); // Clear the IF_ID_register since we are taking a branch
                std::cout << "Branch Taken. New PC: " << pc << std::endl;
            }
        } else {
            if (opcode == "jal" || opcode == "j") {
                // Handle jump instruction
                pc = labels[ID_EX_register[0][1]]; // Use the label to get the jump target
                IF_ID_register.clear(); // Clear the IF_ID_register since we are jumping
                std::cout << "Jumping. New PC: " << pc << std::endl;
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
        std::cout << "Memory Stage. Memory Address: " << MEM_WB_register[0][2] << std::endl;
    }
}

void Core::pipelineWriteBack(bool enableForwarding) {
    std::cout << "hello";
    if (!MEM_WB_register.empty() && !ID_EX_register.empty()) {
        std::cout << "Write Back Stage. Register: " << MEM_WB_register[0][1] << " | Value: " << MEM_WB_register[0][3] << std::endl;
        if (ID_EX_register[0][0] == "add" || ID_EX_register[0][0] == "sub") {
            std::string rd = ID_EX_register[0][1]; // Accessing the opcode of the instruction
            if (!EX_MEM_register.empty() && (EX_MEM_register[0][0] == "add" || EX_MEM_register[0][0] == "sub")) {
                std::string fwdReg = EX_MEM_register[0][1]; // Accessing the opcode of the forwarded instruction
                if (fwdReg == rd && enableForwarding) {
                    registers[rd] = std::stoi(EX_MEM_register[0][3]); // Assuming the fourth element is a string representation of an integer
                    std::cout << "Forwarding Result to Register " << rd << std::endl;
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
   Processor  processor;
    processor.setInitialMemory(0, 5);
    processor.setInitialMemory(4, 10);
    processor.setInitialMemory(8, 15);

    // Manually set instructions
    std::vector<std::vector<std::string>> program1 = {
        {"add", "sp", "ra", "ra"},
        {"add", "sp", "gp", "sp"}
    };

    std::map<std::string, int> labels1;

    processor.cores[0].setProgram({program1, labels1});
    processor.cores[1].setProgram({program1, labels1});

    // Set latencies for each instruction (default latencies if not specified)
    std::unordered_map<std::string, int> customLatencies = {
        {"add", 1}, {"sub", 1}, {"mul", 3}, {"beq", 1}, {"jal", 1}, {"j", 1}
    };


    

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

