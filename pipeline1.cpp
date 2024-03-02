#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include <iomanip>
#include <map>
#include <algorithm>
#include <sstream>

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

    void execute(std::vector<int>& memory);
    int getRegister(const std::string& reg);
    void setRegister(std::string reg, int num);
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

    const int latency[NUM_STAGES] = {1, 1, 3, 1, 1}; // Latency values for each stage
    int stageCounter;

    PipelineStage currentStage;

    void pipelineFetch();
    void pipelineDecode();
    void pipelineExecute(std::vector<int>& memory);
    void pipelineMemory(std::vector<int>& memory);
    void pipelineWriteBack();
    void advancePipeline();
    void executeAdd();
    void executeLw(std::vector<int>& memory);
    void executeSw(std::vector<int>& memory);
    void executeBge();
};

void Core::advancePipeline() {
    currentStage = static_cast<PipelineStage>((currentStage + 1) % NUM_STAGES);
    stageCounter = 0; // Reset the stage counter when moving to the next stage
}

void Core::execute(std::vector<int>& memory) {
    while (pc < program.size() || currentStage != WRITEBACK) {
        switch (currentStage) {
            case FETCH:
                pipelineFetch();
                break;
            case DECODE:
                pipelineDecode();
                break;
            case EXECUTE:
                pipelineExecute(memory);
                break;
            case MEMORY:
                pipelineMemory(memory);
                break;
            case WRITEBACK:
                pipelineWriteBack();
                break;
            default:
                break;
        }


        if (currentStage == WRITEBACK && pc >= program.size()) {
            // Reached the end of the program
            break;
        }

        // Increment cycle count
        cycleCount++;

        // Introduce stall if needed
        if (currentStage != WRITEBACK && stageCounter < latency[currentStage]) {
            // Stall
            stallCount++;
        } 
        if (stageCounter < latency[currentStage]) {
            ++stageCounter;
        } else {
            advancePipeline();
        }
    }
}

void Core::pipelineFetch() {
    if (pc < program.size()) {
        IF_ID_register.push_back(program[pc]); // Push a vector of strings
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

void Core::pipelineExecute(std::vector<int>& memory) {
    if (!ID_EX_register.empty()) {
        std::string opcode = ID_EX_register[0][0]; // Access the opcode in the first vector
        if (opcode == "add") {
            executeAdd();
        } else if (opcode == "lw") {
            executeLw(memory);
        } else if (opcode == "sw") {
            executeSw(memory);
        } else if (opcode == "bge") {
            executeBge();
        }
    }
}

void Core::pipelineMemory(std::vector<int>& memory) {
    if (!EX_MEM_register.empty()) {
        MEM_WB_register = EX_MEM_register;
        EX_MEM_register.clear(); // Clear the EX_MEM_register after using it
    }
}

void Core::pipelineWriteBack() {
    if (!MEM_WB_register.empty()) {
        if (ID_EX_register[0][0] == "add" || ID_EX_register[0][0] == "lw" || ID_EX_register[0][0] == "sw") {
            std::string rd = ID_EX_register[0][1]; // Accessing the opcode of the instruction
            if (MEM_WB_register[0][0] == "add" || MEM_WB_register[0][0] == "lw" || MEM_WB_register[0][0] == "sw") {
                std::string fwdReg = MEM_WB_register[0][1]; // Accessing the opcode of the forwarded instruction
                if (fwdReg == rd) {
                    registers[rd] = std::stoi(MEM_WB_register[0][3]); // Assuming the fourth element is a string representation of an integer
                }
            }
        }
    }
}

void Core::executeAdd() {
    if (ID_EX_register.size() >= 4) {
        std::string rd = ID_EX_register[0][1];
        std::string rs1 = ID_EX_register[0][2];
        std::string rs2 = ID_EX_register[0][3];
        registers[rd] = registers[rs1] + registers[rs2];
    }
}

void Core::executeLw(std::vector<int>& memory) {
    if (ID_EX_register.size() >= 3) {
        std::string rd = ID_EX_register[0][1];
        std::string memOperand = ID_EX_register[0][2];
        size_t pos = memOperand.find('(');
        size_t pos1 = memOperand.find(')');
        if (pos != std::string::npos && pos1 != std::string::npos) {
            std::string offsetStr = memOperand.substr(0, pos);
            std::string srcReg = memOperand.substr(pos + 1, pos1 - pos - 1);
            int offset = std::stoi(offsetStr);
            int srcRegValue = getRegister(srcReg);
            int memoryAddress = srcRegValue + offset;
            registers[rd] = memory[memoryAddress];
        }
    }
}

void Core::executeSw(std::vector<int>& memory) {
    if (ID_EX_register.size() >= 3) {
        std::string rs = ID_EX_register[0][1];
        std::string memOperand = ID_EX_register[0][2];
        size_t pos = memOperand.find('(');
        size_t pos1 = memOperand.find(')');
        if (pos != std::string::npos && pos1 != std::string::npos) {
            std::string offsetStr = memOperand.substr(0, pos);
            std::string destReg = memOperand.substr(pos + 1, pos1 - pos - 1);
            int offset = std::stoi(offsetStr);
            int destRegValue = getRegister(destReg);
            int memoryAddress = destRegValue + offset;
            memory[memoryAddress] = registers[rs];
        }
    }
}

void Core::executeBge() {
    if (ID_EX_register.size() >= 4) {
        std::string rs1 = ID_EX_register[0][1];
        std::string rs2 = ID_EX_register[0][2];
        std::string label = ID_EX_register[0][3];
        int addr = labels[label + ":"];
        if (registers[rs1] >= registers[rs2]) {
            pc = addr;
        }
    }
}

void Core::setProgram(std::pair<std::vector<std::vector<std::string>>, std::map<std::string, int>> parsedProg) {
    program = parsedProg.first;
    labels = parsedProg.second;
    std::cout << "PROG SIZE IS" << program.size();
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
        {"zero", 0}, {"ra", 0}, {"sp", 0}, {"gp", 0}, {"tp", 0}, {"t0", 0}, {"t1", 0}, {"t2", 0}, {"s0", 0}, {"s1", 0}, {"a0", 0}, {"a1", 0}, {"a2", 0}, {"a3", 0}, {"a4", 0}, {"a5", 0}, {"a6", 0}, {"a7", 0}, {"s2", 0}, {"s3", 0}, {"s4", 0}, {"s5", 0}, {"s6", 0}, {"s7", 0}, {"s8", 0}, {"s9", 0}, {"s10", 0}, {"s11", 0}, {"t3", 0}, {"t4", 0}, {"t5", 0}, {"t6", 0}};
    std::cout << "Reset processor" << std::endl;
}

class Processor {
public:
    std::vector<int> memory;
    int clock;
    std::vector<Core> cores;

    Processor();
    void run();
    void setInitialMemory(int wordAddress, int value);
    void setMemory(int wordAddress, int value);
    int getMemory(int wordAddress);
    const std::vector<int>& getMemoryContents() const;
    const std::unordered_map<std::string, int>& getCoreRegisters(int coreIndex) const;
};

Processor::Processor() : memory(4096, 0), clock(0), cores(2) {}
void Processor::run() {
    int totalCycles = 0;
    int totalStalls = 0;

    while (true) {
        bool allCoresCompleted = true;
        for (int i = 0; i < cores.size(); ++i) {
            if (cores[i].pc < cores[i].program.size()) {
                cores[i].execute(memory);
                if (cores[i].pc < cores[i].program.size()) {
                    allCoresCompleted = false;
                }
            }
        }

        // Break if all cores completed
        if (allCoresCompleted) {
            std::cout << "All cores completed." << std::endl;
            break;
        }

        // Increment total cycles
        totalCycles++;

        // Calculate total stalls
        int maxStalls = 0;
        for (const auto& core : cores) {
            if (core.stallCount > maxStalls) {
                maxStalls = core.stallCount;
            }
        }
        totalStalls += maxStalls;

        // Reset stall counters for the next cycle
        for (auto& core : cores) {
            core.stallCount = 0;
        }

        std::cout << "Cycle: " << totalCycles << ", Stalls: " << maxStalls << std::endl;

        // Print pipeline state for each core
        for (int i = 0; i < cores.size(); ++i) {
            std::cout << "Core " << i << " - PC: " << cores[i].pc << ", Current Stage: " << cores[i].currentStage << std::endl;
        }
        
        // Print registers for each core
        for (int i = 0; i < cores.size(); ++i) {
            const auto& coreRegisters = cores[i].getRegisters();
            std::cout << "Core " << i << " Registers:" << std::endl;
            for (const auto& entry : coreRegisters) {
                std::cout << entry.first << ": " << entry.second << std::endl;
            }
        }

        // Print memory contents
        std::cout << "Memory:" << std::endl;
        for (const auto& entry : getMemoryContents()) {
            std::cout << entry << " ";
        }
        std::cout << std::endl;
    }

    // Print results
    std::cout << "Total Cycles: " << totalCycles << std::endl;
    std::cout << "Total Stalls: " << totalStalls << std::endl;
}



void Processor::setInitialMemory(int wordAddress, int value) {
    int index = (wordAddress - 268500992) / 4;
    memory[index] = value;
}

void Processor::setMemory(int wordAddress, int value) {
    int index = (wordAddress - 268500992) / 4;
    memory[index] = value;
}

int Processor::getMemory(int wordAddress) {
    int index = (wordAddress - 268500992) / 4;
    return memory[index];
}

const std::vector<int>& Processor::getMemoryContents() const {
    return memory;
}

const std::unordered_map<std::string, int>& Processor::getCoreRegisters(int coreIndex) const {
    if (coreIndex >= 0 && coreIndex < cores.size()) {
        return cores[coreIndex].getRegisters();
    } else {
        std::cerr << "Invalid core index." << std::endl;
        static std::unordered_map<std::string, int> emptyMap;
        return emptyMap;
    }
}

class Parser {
private:
    int index;
    std::unordered_map<std::string, std::string> registerAliases = {
        {"x0", "zero"}, {"x1", "ra"}, {"x2", "sp"}, {"x3", "gp"}, {"x4", "tp"}, {"x5", "t0"}, {"x6", "t1"}, {"x7", "t2"}, {"x8", "s0"}, {"x9", "s1"}, {"x10", "a0"}, {"x11", "a1"}, {"x12", "a2"}, {"x13", "a3"}, {"x14", "a4"}, {"x15", "a5"}, {"x16", "a6"}, {"x17", "a7"}, {"x18", "s2"}, {"x19", "s3"}, {"x20", "s4"}, {"x21", "s5"}, {"x22", "s6"}, {"x23", "s7"}, {"x24", "s8"}, {"x25", "s9"}, {"x26", "s10"}, {"x27", "s11"}, {"x28", "t3"}, {"x29", "t4"}, {"x30", "t5"}, {"x31", "t6"}};

public:
    Parser() { index = 0; }
    int getIndex() { return index; }
    void setIndex(int a) { index = a; }

    std::pair<std::vector<std::vector<std::string>>, std::map<std::string, int>> parseFromFile(const std::string& filePath, std::vector<int>& memory) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filePath << std::endl;
            return {{}, {}};
        }

        std::vector<std::vector<std::string>> lineWiseSplit;
        std::map<std::string, int> labels;
        std::string line;

        while (std::getline(file, line)) {
            std::replace(line.begin(), line.end(), ',', ' ');
            std::vector<std::string> lineArr;
            std::stringstream lineStream(line);
            std::string token;
            while (lineStream >> token) {
                if (registerAliases.find(token) != registerAliases.end()) {
                    lineArr.push_back(registerAliases[token]);
                } else {
                    lineArr.push_back(token);
                }
            }
            lineWiseSplit.push_back(lineArr);
        }

        for (size_t i = 0; i < lineWiseSplit.size(); ++i) {
            if (lineWiseSplit[i][0].find(":") != std::string::npos) {
                labels[lineWiseSplit[i][0]] = i;
            }
        }

        for (size_t i = 0; i < lineWiseSplit.size(); ++i) {
            if (lineWiseSplit[i][0].find(":") != std::string::npos && lineWiseSplit[i].size() > 1 && lineWiseSplit[i][1] == ".word") {
                std::vector<std::string>& line = lineWiseSplit[i];
                line.erase(line.begin());
                for (size_t j = 1; j < line.size(); ++j, index += 4) {
                    int value = std::stoi(line[j]);
                    memory[index] = value;
                }
            } else if (lineWiseSplit[i].size() > 0 && lineWiseSplit[i][0] == ".word") {
                for (size_t j = 1; j < lineWiseSplit[i].size(); ++j, index += 4) {
                    int value = std::stoi(lineWiseSplit[i][j]);
                    memory[index] = value;
                }
            }
        }
        std::pair<std::vector<std::vector<std::string>>, std::map<std::string, int>> parsedProgram = std::make_pair(lineWiseSplit, labels);
        return parsedProgram;
    }
};

int main() {
    Processor sim;
    Parser parser;

    auto parsedProgram1 = parser.parseFromFile("code1.txt", sim.memory);
    auto parsedProgram2 = parser.parseFromFile("code2.txt", sim.memory);

    sim.cores[0].setProgram(parsedProgram1);
    sim.cores[1].setProgram(parsedProgram2);

    for (auto& core : sim.cores) {
        core.reset();
    }

    std::cout << "Memory:" << std::endl;
    for (const auto& entry : sim.getMemoryContents()) {
        std::cout << entry << " ";
    }
    std::cout << std::endl;

    sim.run();

    for (int i = 0; i < sim.cores.size(); ++i) {
        const auto& coreRegisters = sim.getCoreRegisters(i);

        std::cout << "After Run - Core " << i << " Registers:" << std::endl;
        for (const auto& entry : coreRegisters) {
            std::cout << entry.first << ": " << entry.second << std::endl;
        }
    }

    std::cout << "FINAL MEMORY" << std::endl;
    for (const auto& entry : sim.getMemoryContents()) {
        std::cout << entry << " ";
    }

    return 0;
}


