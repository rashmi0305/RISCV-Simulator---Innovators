#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include<fstream>
#include <map>
#include<algorithm>
#include<sstream>


class Core {
public:
    std::unordered_map<std::string, int> registers;
    int pc;
   // std::pair<std::vector<std::vector<std::string>>, std::map<std::string, int>> parsedProgram;
    std::vector<std::vector<std::string>> program;
    std::map<std::string, int> labels;
    
    Core();
    int execute(std::vector<int>& memory);
    int getRegister(const std::string& reg);
    void setRegister(std::string reg, int num);
    void setProgram(std::pair<std::vector<std::vector<std::string>>, std::map<std::string, int>> parsedProgram);
    const std::unordered_map<std::string, int>& getRegisters() const;
    void reset();
};
Core::Core() : registers(), pc(0) {}

int Core::execute(std::vector<int>& memory) {
    std::cout<<"Hi";
      while (pc < program.size()) {
    
            const std::vector<std::string>& instruction = program[pc];
            if (instruction[0] == "" || instruction[0] == "#" || instruction[0].find(".") != std::string::npos) {
                pc = pc + 1;
   
            }
             else if (instruction[0].find(":") != std::string::npos && instruction.size() == 1) {
                pc = pc + 1;
   
            }

            if (!instruction.empty()) {
                std::string opcode = instruction[0];
                std::cout<<opcode;
                 if (opcode == "add") {
                    if (instruction.size() == 4) {
                        std::string rd = instruction[1];
                        std::string rs1 = instruction[2];
                        std::string rs2 = instruction[3];
                        registers[rs1] = 3;  // Set value for register at
                        registers[rs2] = 4; // Set value for register rs2
                        registers[rd] = registers[rs1] + registers[rs2];
                        std::cout << "Intermediate state:" << std::endl;
                        for (const auto& entry : registers) {
                         std::cout << entry.first << ": " << entry.second << std::endl;
                         }
                         pc += 1;
                         return getRegister(rd);
                        } else {
                        std::cerr << "Invalid 'add' instruction format at line " << pc << std::endl;
                        // Handle error: Invalid instruction format
                    }
                 }
                 else if (opcode == "lw") {
                    if (instruction.size() == 3) {
                        std::string rd = instruction[1];
                        std::string memOperand = instruction[2];
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
                            //std::cout << "Intermediate state:" << std::endl;
                            // for (const auto& entry : memory) {
                            //     std::cout << entry<< std::endl;
                            //     }
                        pc += 1;
                        return getRegister(rd);
                    } 
                    } else {
                         std::cerr << "Invalid 'ld' instruction format at line " << pc << std::endl;
                         }
                 }
                 else if (opcode == "srl") {
                    if (instruction.size() == 4) {
                    std::string rd = instruction[1];
                    std::string rs = instruction[2];
                    int immediate = std::stoi(instruction[3]);
                    if (registers.find(rs) != registers.end()) {
                    int rsValue = registers[rs];
                    registers[rd] = rsValue >> immediate;
                    std::cout << "Intermediate state:" << std::endl;
                    for (const auto& entry : registers) {
                        std::cout << entry.first << ": " << entry.second << std::endl;
                    }
                    pc += 1;
                 } else {
                    std::cerr << "Source register " << rs << " not found." << std::endl;
                 }
                 } else {
                     std::cerr << "Invalid 'srl' instruction format at line " << pc << std::endl;
                     // Handle error: Invalid instruction format
                }
}
else if (opcode == "sll") {
    if (instruction.size() == 4) {
        std::string rd = instruction[1];
        std::string rs = instruction[2];
        int immediate = std::stoi(instruction[3]);
        if (registers.find(rs) != registers.end()) {
            int rsValue = registers[rs];
            registers[rd] = rsValue << immediate;
            std::cout << "Intermediate state:" << std::endl;
            for (const auto& entry : registers) {
                std::cout << entry.first << ": " << entry.second << std::endl;
            }
            pc += 1;
        } else {
            std::cerr << "Source register " << rs << " not found." << std::endl;
        }
    } else {
        std::cerr << "Invalid 'sll' instruction format at line " << pc << std::endl;
        // Handle error: Invalid instruction format
    }
}
else if (opcode == "li") {
    if (instruction.size() == 3) {
        std::string rd = instruction[1];
        int immediate = std::stoi(instruction[2]);
        setRegister(rd, immediate);
        std::cout << "Intermediate state:" << std::endl;
        for (const auto& entry : getRegisters()) {
            std::cout << entry.first << ": " << entry.second << std::endl;
        }
        pc += 1;
        // Return value not specified in your original code, update as needed
    } else {
        std::cerr << "Invalid 'li' instruction format at line " << pc << std::endl;
        // Handle error: Invalid instruction format
    }
}
else if (opcode == "beq") {
    if (instruction.size() == 4) {
        std::string rs1 = instruction[1];
        std::string rs2 = instruction[2];
        std::string label = instruction[3];
        int addr = labels[label]; // Get the address corresponding to the label
        if (getRegister(rs1) == getRegister(rs2)) {
            pc = addr; // Update the program counter to the address if the registers are equal
        } else {
            pc += 1; // Increment the program counter otherwise
        }
        // No need to return a value in this case
    } else {
        std::cerr << "Invalid 'beq' instruction format at line " << pc << std::endl;
        // Handle error: Invalid instruction format
    }
}
else if (opcode == "blt") {
    if (instruction.size() == 4) {
        std::string rs1 = instruction[1];
        std::string rs2 = instruction[2];
        std::string label = instruction[3];
        int addr = labels[label]; // Get the address corresponding to the label
        if (getRegister(rs1) < getRegister(rs2)) {
            pc = addr; // Update the program counter to the address if rs1 < rs2
        } else {
            pc += 1; // Increment the program counter otherwise
        }
        // No need to return a value in this case
    } else {
        std::cerr << "Invalid 'blt' instruction format at line " << pc << std::endl;
        // Handle error: Invalid instruction format
    }
}
else if (opcode == "j") {
    if (instruction.size() == 2) {
        std::string label = instruction[1];
        int addr = labels[label];
        std::cout<<"adr"<<addr<<std::endl; // Get the address corresponding to the label
        pc = addr; // Update the program counter to the address
        // No need to return a value in this case
    } else {
        std::cerr << "Invalid 'j' instruction format at line " << pc << std::endl;
        // Handle error: Invalid instruction format
    }
}
else if (opcode == "bne") {
    if (instruction.size() == 4) {
        std::string rs1 = instruction[1];
        std::string rs2 = instruction[2];
        std::string label = instruction[3];
        if (registers.find(rs1) != registers.end() && registers.find(rs2) != registers.end()) {
            if (registers[rs1] != registers[rs2]) {
                int addr = labels[label]; // Get the address corresponding to the label
                pc = addr; // Update the program counter to the address
                // No need to return a value in this case
            } else {
                pc += 1; // Move to the next instruction if the condition is not met
            }
        } else {
            std::cerr << "Source register not found in 'bne' instruction at line " << pc << std::endl;
            // Handle error: Source register not found
        }
    } else {
        std::cerr << "Invalid 'bne' instruction format at line " << pc << std::endl;
        // Handle error: Invalid instruction format
    }
}
else if (opcode == "sub") {
    if (instruction.size() == 4) {
        std::string rd = instruction[1];
        std::string rs1 = instruction[2];
        std::string rs2 = instruction[3];
        if (registers.find(rs1) != registers.end() && registers.find(rs2) != registers.end()) {
            registers[rd] = registers[rs1] - registers[rs2];
            std::cout << "Intermediate state:" << std::endl;
            for (const auto& entry : registers) {
                std::cout << entry.first << ": " << entry.second << std::endl;
            }
            pc += 1;
            // Return value not specified in your original code, update as needed
        } else {
            std::cerr << "Source register not found in 'sub' instruction at line " << pc << std::endl;
            // Handle error: Source register not found
        }
    } else {
        std::cerr << "Invalid 'sub' instruction format at line " << pc << std::endl;
        // Handle error: Invalid instruction format
    }
}


                 else if (opcode == "addi") {
                    if (instruction.size() == 4) {
                        std::string rd = instruction[1];
                        std::string rs = instruction[2];
                        int immediate = std::stoi(instruction[3]);
                        if (registers.find(rs) != registers.end()) {
                            registers[rd] = registers[rs] + immediate;
                            std::cout << "Intermediate state:" << std::endl;
                            for (const auto& entry : registers) {
                                 std::cout << entry.first << ": " << entry.second << std::endl;
                            }
                        pc += 1;
                    } else {
                         std::cerr << "Source register " << rs << " not found." << std::endl;
                    }
                 } else {
                     std::cerr << "Invalid 'addi' instruction format at line " << pc << std::endl;
                    
                 }
                }
                else if (opcode == "sw") {
                    if (instruction.size() == 3) {
                        std::string srcReg = instruction[1];
                        std::string memOperand = instruction[2];
                        // Parse offset and destination register
                        size_t pos = memOperand.find('(');
                        size_t pos1 = memOperand.find(')');
                        if (pos != std::string::npos && pos1 != std::string::npos) {
                            std::string offsetStr = memOperand.substr(0, pos);
                            std::string destReg = memOperand.substr(pos + 1, pos1 - pos - 1);
                            int offset = std::stoi(offsetStr);
                            if (registers.find(destReg) != registers.end()) {
                            int destRegValue = registers[destReg];
                            int memoryAddress = destRegValue + offset;
                             memory[memoryAddress] = registers[srcReg];
    
                        pc += 1;
                // std::cout << "Final state after execution:" << std::endl;
                // for (const auto& entry : memory) {
                //     std::cout << entry << std::endl;
                // }
            } else {
                std::cerr << "Destination register " << destReg << " not found." << std::endl;
            }
        }
    } else {
        std::cerr << "Invalid 'sw' instruction format at line " << pc << std::endl;
        // Handle error: Invalid instruction format
    }
}


}

            }
            return 0;
        }
    

void Core:: setProgram(std::pair<std::vector<std::vector<std::string>>, std::map<std::string, int>> parsedProg)
{
    program=parsedProg.first;
    labels=parsedProg.second;
    std::cout<<"PROG SIZE IS"<<program.size();

}
int Core::getRegister(const std::string& reg) {
    // Implementation of getRegister function
    if (registers.find(reg) != registers.end()) {
            return registers[reg];
        } else {
            std::cerr << "Register " << reg << " not found." << std::endl;
            return 0;
        }
}

void Core::setRegister(std::string reg, int num) {
    // Implementation of setRegister function
     if (registers.find(reg) != registers.end() && reg!="zero") {
            registers[reg] = num;
        } else {
            std::cerr << "Register " << reg << " not found." << std::endl;
        }
}

const std::unordered_map<std::string, int>& Core::getRegisters() const {
    // Implementation of getRegisters function
     return registers;
}

void Core::reset() {
    // Implementation of reset function
     registers = {
            {"zero", 0}, {"a0", 0}, {"a1", 0},{"a2", 0},{"a3", 0}, {"t0", 0},
            {"t1", 0}, {"t2", 0}, {"t3", 0}, {"t4", 0}, {"t5", 0}, {"t6", 0},
            {"s0", 0}, {"s1", 0},{"s2", 0}, {"s3", 0}, {"s4", 0}, {"s5", 0}, 
            {"s6", 0},{"s7", 0}, {"gp", 0}, {"sp", 0}, {"s8", 0}, {"ra", 0},
            {"tp",0},{"a4",0},{"a5",0},{"a6",0},{"a7",0},{"s9",0},
            {"s10",0},{"s11",0}
        };

        std::cout << "Reset processor" << std::endl;
}


class Processor {
public:
    std::vector<int> memory;
    int clock;
    std::vector<Core> cores;

public:
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
   // std::cout<<"prog size is"<<cores[0].program.size()<<std::endl;
    //std::cout<<"prog size is"<<cores[1].program.size()<<std::endl;
    while (clock < std::max(cores[0].program.size(), cores[1].program.size())) {
        for (int i = 0; i < 2; ++i) {
            if (clock < cores[i].program.size()) {
                cores[i].execute(memory);
            }
        }
        clock += 1;
    }
}

void Processor::setInitialMemory(int wordAddress, int value) {
    // Implementation of setInitialMemory function
    int index = (wordAddress - 268500992) / 4;
        memory[index] = value;
}

void Processor::setMemory(int wordAddress, int value) {
    // Implementation of setMemory function
    int index = (wordAddress - 268500992) / 4;
        memory[index] = value;
}

int Processor::getMemory(int wordAddress) {
    // Implementation of getMemory function
    int index = (wordAddress - 268500992) / 4;
        return memory[index];
}

const std::vector<int>& Processor::getMemoryContents() const {
    // Implementation of getMemoryContents function
    return memory;
}

const std::unordered_map<std::string, int>& Processor::getCoreRegisters(int coreIndex) const {
    // Implementation of getCoreRegisters function
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
    Processor processor; // Member variable of type Processor
    std::unordered_map<std::string, std::string> registerAliases = {
        {"x0","zero"},{"x1", "ra"}, {"x2", "sp"}, {"x3", "gp"}, {"x4", "tp"}, {"x5", "t0"},{"x6", "t1"}, {"x7", "t2"}, {"x8", "s0"}, {"x9", "s1"}, {"x10", "a0"},
        {"x11", "a1"}, {"x12", "a2"}, {"x13", "a3"}, {"x14", "a4"}, {"x15", "a5"},{"x16", "a6"}, {"x17", "a7"}, {"x18", "s2"}, {"x19", "s3"}, {"x20", "s4"},
        {"x21", "s5"}, {"x22", "s6"}, {"x23", "s7"}, {"x24", "s8"}, {"x25", "s9"},{"x26", "s10"}, {"x27", "s11"}, {"x28", "t3"}, {"x29", "t4"}, {"x30", "t5"},{"x31","t6"}
    };

public:
    std::pair<std::vector<std::vector<std::string>>, std::map<std::string, int>> parseFromFile(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filePath << std::endl;
            return {{}, {}};
        }

        std::vector<std::vector<std::string>> lineWiseSplit;
        std::map<std::string, int> labels;
        std::string line;
        int index = 268500992; // Starting memory index

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
            // for(auto tokens:lineArr)
            // {
            //     std::cout<<tokens<<std::endl;
            // }
            
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
                    processor.setInitialMemory(index, value);
                }
            } else if (lineWiseSplit[i].size() > 0 && lineWiseSplit[i][0] == ".word") {
                for (size_t j = 1; j < lineWiseSplit[i].size(); ++j, index += 4) {
                    int value = std::stoi(lineWiseSplit[i][j]);
                    processor.setInitialMemory(index, value);
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
    auto parsedProgram1 = parser.parseFromFile("code1.txt"); // Parse the program from a file
    auto parsedProgram2 = parser.parseFromFile("code2.txt");//2nd file
    sim.cores[0].setProgram(parsedProgram1);
    sim.cores[1].setProgram(parsedProgram2);

    for (auto& core : sim.cores) {
        core.reset();
    }

    sim.cores[0].setRegister("v0", 7);
    sim.cores[0].setRegister("t0", 10);
    sim.setMemory(268500992+20, 8); 
    sim.cores[1].setRegister("t8", 4);


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



