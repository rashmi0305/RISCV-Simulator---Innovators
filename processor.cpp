#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <map>
class Core {
public:
    std::unordered_map<std::string, int> registers;
    int pc;

public:
    std::vector<std::string> program;

    Core() : registers(), pc(0) {}

    int execute(std::vector<int>& memory);
    int getRegister(const std::string& reg) {
        if (registers.find(reg) != registers.end()) {
            return registers[reg];
        } else {
            std::cerr << "Register " << reg << " not found." << std::endl;
            return 0;
        }
    }
    void setRegister(std::string reg, int num) {
        if (registers.find(reg) != registers.end()) {
            registers[reg] = num;
        } else {
            std::cerr << "Register " << reg << " not found." << std::endl;
        }
    }

    const std::unordered_map<std::string, int>& getRegisters() const {
        return registers;
    }
    void reset() {
        registers = {
            {"r0", 0}, {"at", 0}, {"v0", 0}, {"v1", 0}, {"a0", 0}, {"a1", 0},
            {"a2", 0}, {"a3", 0}, {"t0", 0}, {"t1", 0}, {"t2", 0}, {"t3", 0},
            {"t4", 0}, {"t5", 0}, {"t6", 0}, {"t7", 0}, {"s0", 0}, {"s1", 0},
            {"s2", 0}, {"s3", 0}, {"s4", 0}, {"s5", 0}, {"s6", 0}, {"s7", 0},
            {"t8", 0}, {"t9", 0}, {"k0", 0}, {"k1", 0}, {"gp", 0}, {"sp", 0},
            {"s8", 0}, {"ra", 0}
        };

        std::cout << "Reset processor" << std::endl;
    }
};

int Core::execute(std::vector<int>& memory) {
    if (pc < program.size()) {
        std::string instruction = program[pc];
       

        size_t pos = instruction.find(' ');
        if (pos != std::string::npos) {
            std::string opcode = instruction.substr(0, pos);
            std::string rest = instruction.substr(pos + 1);

            std::vector<std::string> parts;
            size_t start = 0, end = rest.find(',');

            while (end != std::string::npos) {
                parts.push_back(rest.substr(start, end - start));
                start = end + 1;
                end = rest.find(',', start);
            }
            parts.push_back(rest.substr(start, end));

            std::cout << "Executing opcode: " << opcode << std::endl;

            if (opcode == "add") {
                std::string rd = parts[0];
                std::string rs1 = parts[1];
                std::string rs2 = parts[2];
                registers[rs1] = 3;  // Set value for register at
                registers[rs2] = 4; // Set value for register rs2
                registers[rd] = registers[rs1] + registers[rs2];
                std::cout << "Intermediate state:" << std::endl;
                for (const auto& entry : registers) {
                    std::cout << entry.first << ": " << entry.second << std::endl;
                }
                pc += 1;
                return getRegister(rd);
            } else if (opcode == "ld") {
                
                std::string rd = parts[0];
                std::string memOperand = parts[1];
                // Parse offset and source register
                size_t pos = memOperand.find('(');
                size_t pos1 = memOperand.find(')');

                if (pos != std::string::npos && pos1 != std::string::npos) {
                    std::string offsetStr = memOperand.substr(0, pos);
                    std::string srcReg = memOperand.substr(pos + 1, pos1 - pos - 1);
                    int offset = std::stoi(offsetStr);
                    int srcRegValue = getRegister(srcReg);
                    int memoryAddress = srcRegValue + offset;
                    registers[rd] = memory[memoryAddress];
                    std::cout << "Intermediate state:" << std::endl;
                    for (const auto& entry : registers) {
                        std::cout << entry.first << ": " << entry.second << std::endl;
                    }
                    pc += 1;
                    return getRegister(rd);
                } else {
                    std::cerr << "Error parsing LD instruction: " << instruction << std::endl;
                }
            }
            else if (opcode == "addi") {
                std::string rd = parts[0];
                std::string rs = parts[1];
                int immediate = std::stoi(parts[2]);
                if (registers.find(rs) != registers.end()) {
                    registers[rs] = 3;  // Set value for register rs
                    registers[rd] = registers[rs] + immediate;
                    std::cout << "Intermediate state:" << std::endl;
                    for (const auto& entry : registers) {
                        std::cout << entry.first << ": " << entry.second << std::endl;
                    }
                    pc += 1;
                } else {
                    std::cerr << "Source register " << rs << " not found." << std::endl;
                }}
                
             else if (opcode == "sw") {
                std::string srcReg = parts[0];
                std::cout<<parts[0]<<std::endl;
                std::string memOperand = parts[1];
                // Parse offset and destination register
                size_t pos = memOperand.find('(');
                size_t pos1 = memOperand.find(')');
                if (pos != std::string::npos && pos1 != std::string::npos) {
                    std::string offsetStr = memOperand.substr(0, pos);
                    std::string destReg = memOperand.substr(pos + 1, pos1 - pos - 1);
                    int offset = std::stoi(offsetStr);
                    int destRegValue = getRegister(destReg);
                    int memoryAddress = destRegValue + offset;
                    memory[memoryAddress] = getRegister(srcReg);
                    std::cout << "Intermediate state:" << std::endl;
                    for (const auto& entry : memory) {
                        std::cout << entry << std::endl;
                    }
                    pc += 1;
                    std::cout << "Final state after execution:" << std::endl;
                    for (const auto& entry : memory) {
                        std::cout << entry<<std::endl;
                    }
    
                } else {
                    std::cerr << "Error parsing SW instruction: " << instruction << std::endl;
                }
        } 
        else {
                std::cerr << "Unsupported opcode: " << opcode << std::endl;
            }
        }
    }

    
}

class Processor {
public:
    std::vector<int> memory;
    int clock;
    std::vector<Core> cores;

public:
    Processor() : memory(4096, 0), clock(0), cores(2) {}

    void run();

    void setInitialMemory(int wordAddress, int value) {
        int index = (wordAddress - 268500992) / 4;
        memory[index] = value;
    }

    void setMemory(int wordAddress, int value) {
        int index = (wordAddress - 268500992) / 4;
        memory[index] = value;
    }

    int getMemory(int wordAddress) {
        int index = (wordAddress - 268500992) / 4;
        return memory[index];
    }

    const std::vector<int>& getMemoryContents() const {
        return memory;
    }

    const std::unordered_map<std::string, int>& getCoreRegisters(int coreIndex) const {
        if (coreIndex >= 0 && coreIndex < cores.size()) {
            return cores[coreIndex].getRegisters();
        } else {
            std::cerr << "Invalid core index." << std::endl;
            static std::unordered_map<std::string, int> emptyMap;
            return emptyMap;
        }
    }
};

void Processor::run() {
    while (clock < std::max(cores[0].program.size(), cores[1].program.size())) {
        for (int i = 0; i < 2; ++i) {
            if (clock < cores[i].program.size()) {
                cores[i].execute(memory);
            }
        }
        clock += 1;
    }
}

int main() {
    Processor sim;

    sim.cores[0].program.push_back("addi ra,t0,12");
    sim.cores[1].program.push_back("sw t9,4(t8)");
    for (auto& core : sim.cores) {
        core.reset();
    }

   // sim.cores[0].setRegister("v0", 7);
    sim.cores[0].setRegister("t0", 10);
    sim.setMemory(268500992+20, 8); 
    sim.cores[1].setRegister("t8", 2);

   
    sim.run();

    
    const auto& coreRegisters0 = sim.getCoreRegisters(0);
    const auto& coreRegisters1 = sim.getCoreRegisters(1);

    std::cout << "+============ After Run ============:" << std::endl;
    std::cout << "Core 0 Registers:" << std::endl;
    for (const auto& entry : coreRegisters0) {
        std::cout << entry.first << ": " << entry.second << std::endl;
    }

    std::cout << "Core 1 Registers:" << std::endl;
    for (const auto& entry : coreRegisters1) {
        std::cout << entry.first << ": " << entry.second << std::endl;
    }

    return 0;
}
