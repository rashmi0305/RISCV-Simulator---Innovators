#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include<fstream>
#include<iomanip>
#include <map>
#include<algorithm>
#include<sstream>

class Instruction{
public:

    std::string opcode;
    std::string rd;
    std::string rs1;
    std::string rs2;
    int rd_val=0;
    int rs1_val=0;
    int rs2_val=0;
    int imm=0;
    //int memory_addr=0;
    int latency=1;
    bool rd_ready=false;
    int branch_target=0;
    int pred_target=0;
    bool branch_taken=false;
    bool pred_taken=false;
    Instruction(){};
    Instruction::Instruction(std::vector<std::string> data){
        opcode=data[0];
    }
};



class Core {
public:
    std::unordered_map<std::string, int> registers;
    int pc;
    int ticks;
    int cycleCount;
    int stallCount;
    std::vector<std::vector<std::string>> program;
    std::map<std::string, int> labels;
    Instruction IF_ID_register;//to store the instructions temporarily in between stages
    Instruction ID_EX_register;
    Instruction EX_MEM_register;
    Instruction MEM_WB_register;        


 Core();
    void execute(std::vector<int>& memory);
    int getRegister(const std::string& reg);
    void setRegister(std::string reg, int num);
    void setProgram(std::pair<std::vector<std::vector<std::string>>, std::map<std::string, int>> parsedProgram);
    const std::unordered_map<std::string, int>& getRegisters() const;
    void reset();
    void pipelineFetch();
    void pipelineDecode();
    void pipelineExecute(std::vector<int>& memory);
    void pipelineMemory(std::vector<int>& memory);
    void pipelineWriteBack();
};


void Core::execute(std::vector<int> &memory) {
    std::cout << "Start running ..." << std::endl;
    
    while (true) {
        // Increment clock ticks.
        ticks++;

        // Process pipeline stages backwards.
        pipelineWriteBack();
        pipelineMemory(memory);
        pipelineExecute(memory);
        pipelineDecode();
        pipelineFetch();

        // Check if all pipeline stages are clear.
        if (pc >= program.size()){ //&& IF_ID_register.empty() && ID_EX_register.empty() && EX_MEM_register.empty() && MEM_WB_register.empty()) {
            break;
        }
    }

    std::cout << "Done." << std::endl;
}

void Core::pipelineFetch() {
    if (pc < program.size()) {
        Instruction inst(program[pc]);
        IF_ID_register =inst;//check if gets copied entirely-maybe problem
        pc++;
     }
     // else {
    //     IF_ID_register.clear();
    // }
}

void Core::pipelineDecode() {
   // if (!IF_ID_register.empty()) {
        ID_EX_register = IF_ID_register;
        //..IF_ID_register.clear();
    //}
}

void Core::pipelineExecute(std::vector<int> &memory) {
    // if (!ID_EX_register.empty()) {
        // Execute the instruction (not implemented in this example)
        EX_MEM_register = ID_EX_register;
        //ID_EX_register.clear();
    //}
}

void Core::pipelineMemory(std::vector<int> &memory) {
    //if (!EX_MEM_register.empty()) {
        // Access memory if needed (not implemented in this example)
        MEM_WB_register = EX_MEM_register;
       // EX_MEM_register.clear();
   // }
}

// void Core::pipelineWriteBack() {
//     if (!MEM_WB_register.empty()) {
//         // Write back the result (not implemented in this example)
//         MEM_WB_register.clear();
//     }
// }



//Core::Core() : registers(), pc(0) {}
Core:: Core():program(), labels(), pc(0), registers(), IF_ID_register(), ID_EX_register(), EX_MEM_register(), MEM_WB_register() {}
void Core::execute(std::vector<int>& memory) {
    //std::cout<<"Hi";
       while (pc < program.size()) {
    
            const std::vector<std::string>& instruction = program[pc];
            if (instruction[0] == "#" || instruction[0].find(".") != std::string::npos) {
                pc = pc + 1;
   
            }
             else if (instruction[0].find(":") != std::string::npos && instruction.size() == 1) {
                pc = pc + 1;
   
            }

            if (!instruction.empty()) {
                std::string opcode = instruction[0];
                std::cout<<"EXECUTING: "<<opcode<<" ";
                if (opcode == "add") {
                    if (instruction.size() == 4) {
                        std::string rd = instruction[1];
                        std::string rs1 = instruction[2];
                        std::string rs2 = instruction[3];
                        registers[rd] = registers[rs1] + registers[rs2];
                        std::cout << "Register values:" << std::endl;
                         for (int i = 0; i < 32; ++i) {
                            std::string regName = "x" + std::to_string(i);
                            std::cout << regName << ": "<< std::right << registers[regName] << "  ";
                            if ((i + 1) % 8 == 0) {
                                std::cout << std::endl;
                            }
                        }
                        std::cout << std::endl;


                         pc += 1;
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
                             for (int i = 0; i < 32; ++i) {
                            std::string regName = "x" + std::to_string(i);
                            std::cout << regName << ": "<< std::right << registers[regName] << "  ";
                            if ((i + 1) % 8 == 0) {
                                std::cout << std::endl;
                            }
                        }
                        pc += 1;
                    } 
                    } else {
                         std::cerr << "Invalid 'lw' instruction format at line " << pc << std::endl;
                         }
                 }
                 else if (opcode == "bge") {
                    if (instruction.size() == 4) {
                        std::string rs1 = instruction[1];
                        std::string rs2 = instruction[2];
                        std::string label = instruction[3];
                        int addr = labels[label+":"];
                        if (registers[rs1] >= registers[rs2]) {
                            pc = addr; 
                        }else{
                            pc += 1; 
                        }
                       
                    } else {
                        std::cerr << "Invalid 'bge' instruction format at line " << pc << std::endl;
                        
                    }
}

                 else if (opcode == "srli") {
                    if (instruction.size() == 4) {
                    std::string rd = instruction[1];
                    std::string rs = instruction[2];
                    int immediate = std::stoi(instruction[3]);
                    if (registers.find(rs) != registers.end()) {
                    int rsValue = registers[rs];
                    registers[rd] = rsValue >> immediate;
                      std::cout << "Register values:" << std::endl;
                         for (int i = 0; i < 32; ++i) {
                            std::string regName = "x" + std::to_string(i);
                            std::cout << regName << ": "<< std::right << registers[regName] << "  ";
                            if ((i + 1) % 8 == 0) {
                                std::cout << std::endl;
                            }
                        }
                    pc += 1;
                 } else {
                    std::cerr << "Source register " << rs << " not found." << std::endl;
                 }
                 } else {
                     std::cerr << "Invalid 'srl' instruction format at line " << pc << std::endl;
                }
}
                else if (opcode == "slli") {
                    if (instruction.size() == 4) {
                        std::string rd = instruction[1];
                        std::string rs = instruction[2];
                        int immediate = std::stoi(instruction[3]);
                        if (registers.find(rs) != registers.end()) {
                            int rsValue = registers[rs];
                            registers[rd] = rsValue << immediate;
                             for (int i = 0; i < 32; ++i) {
                            std::string regName = "x" + std::to_string(i);
                            std::cout << regName << ": "<< std::right << registers[regName] << "  ";
                            if ((i + 1) % 8 == 0) {
                                std::cout << std::endl;
                            }
                        }
                            pc += 1;
                        } else {
                            std::cerr << "Source register " << rs << " not found." << std::endl;
                        }
                    } else {
                        std::cerr << "Invalid 'slli' instruction format at line " << pc << std::endl;
                       
                    }
                }
                else if (opcode == "li") {
                    if (instruction.size() == 3) {
                        std::string rd = instruction[1];
                        int immediate = std::stoi(instruction[2]);
                        setRegister(rd, immediate);
                        for (int i = 0; i < 32; ++i) {
                            std::string regName = "x" + std::to_string(i);
                            std::cout << regName << ": "<< std::right << registers[regName] << "  ";
                            if ((i + 1) % 8 == 0) {
                                std::cout << std::endl;
                            }
                        }
                        pc += 1;
                        
                    } else {
                        std::cerr << "Invalid 'li' instruction format at line " << pc << std::endl;
                      
                    }
}
                else if (opcode == "beq") {
                    if (instruction.size() == 4) {
                        std::string rs1 = instruction[1];
                        std::string rs2 = instruction[2];
                        std::string label = instruction[3];
                        int addr = labels[label+":"]; 
                        if (getRegister(rs1) == getRegister(rs2)) {
                            pc = addr; 
                        } else {
                            pc += 1; 
                        }
                       
                    } else {
                        std::cerr << "Invalid 'beq' instruction format at line " << pc << std::endl;
                       
                    }
                }
                else if (opcode == "blt") {
                    if (instruction.size() == 4) {
                        std::string rs1 = instruction[1];
                        std::string rs2 = instruction[2];
                        std::string label = instruction[3];
                        int addr = labels[label+":"];
                        if (getRegister(rs1) < getRegister(rs2)) {
                            pc = addr; 
                        } else {
                            pc += 1; 
                        }
                        
                    } else {
                        std::cerr << "Invalid 'blt' instruction format at line " << pc << std::endl;
                        
                    }
                }
                else if (opcode == "j") {
                    if (instruction.size() == 2) {
                        std::string label = instruction[1];
                        int addr = labels[label+":"];
                        std::cout<<"adr"<<addr<<std::endl;
                        pc = addr; 
                       
                    } else {
                        std::cerr << "Invalid 'j' instruction format at line " << pc << std::endl;
                       
                    }
}
                else if (opcode == "bne") {
                   if (instruction.size() == 4) {
                       std::string rs1 = instruction[1];
                       std::string rs2 = instruction[2];
                       std::string label = instruction[3];
                       if (registers.find(rs1) != registers.end() && registers.find(rs2) != registers.end()) {
                           if (registers[rs1] != registers[rs2]) {
                               int addr = labels[label+":"]; 
                               pc = addr; 
                           } else {
                               pc += 1; 
                           }
                       } else {
                           std::cerr << "Source register not found in 'bne' instruction at line " << pc << std::endl;
                       }
                   } else {
                       std::cerr << "Invalid 'bne' instruction format at line " << pc << std::endl;
    
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
                            std::cout << "Register values:" << std::endl;
                         for (int i = 0; i < 32; ++i) {
                            std::string regName = "x" + std::to_string(i);
                            std::cout << regName << ": "<< std::right << registers[regName] << "  ";
                            if ((i + 1) % 8 == 0) {
                                std::cout << std::endl;
                            }
                        }
                            pc += 1;
                           
                        } else {
                            std::cerr << "Source register not found in 'sub' instruction at line " << pc << std::endl;

                        }
                    } else {
                        std::cerr << "Invalid 'sub' instruction format at line " << pc << std::endl;

                    }
}

                else if (opcode == "ecall") {
                   if (instruction.size() == 1) {
                       // Retrieve the system call number from register a7
                       int syscall_number = getRegister("a7");
                       switch (syscall_number) {
                           case 1: // print_int
                               std::cout << getRegister("a0") << std::endl;
                               break;
                           case 4: // print space
                               
                               for (int i = getRegister("a0"); memory[i] != '\0'; ++i) {
                                   std::cout << static_cast<char>(memory[i]);
                               }
                               std::cout << std::endl;
                               break;

                           default:
                               std::cerr << "Unsupported system call number: " << syscall_number << std::endl;
                               exit(1); // Exit with error code
                               break;
                       }


                       pc += 1;
                   } else {
                       std::cerr << "Invalid 'ecall' instruction format at line " << pc << std::endl;

                   }
}

                 else if (opcode == "addi") {
                    if (instruction.size() == 4) {
                        std::string rd = instruction[1];
                        std::string rs = instruction[2];
                        int immediate = std::stoi(instruction[3]);
                        if (registers.find(rs) != registers.end()) {
                            registers[rd] = registers[rs] + immediate;
                             std::cout << "Register values:" << std::endl;
                        for (int i = 0; i < 32; ++i) {
                            std::string regName = "x" + std::to_string(i);
                            std::cout << regName << ": "<< std::right << registers[regName] << "  ";
                            if ((i + 1) % 8 == 0) {
                                std::cout << std::endl;
                            }
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
                std::cout << "Final state after sw execution(ONLY FEW):" << std::endl;
                for(int i=0;i<55;i++)
                            {
                                std::cout<<memory[i]<<" ";
                            }
                            std::cout<<std::endl;
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
        if (allCoresCompleted) {
            break; // All cores have completed execution
        }
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
    int index;
    std::unordered_map<std::string, std::string> registerAliases = {
        {"x0","zero"},{"x1", "ra"}, {"x2", "sp"}, {"x3", "gp"}, {"x4", "tp"}, {"x5", "t0"},{"x6", "t1"}, {"x7", "t2"}, {"x8", "s0"}, {"x9", "s1"}, {"x10", "a0"},
        {"x11", "a1"}, {"x12", "a2"}, {"x13", "a3"}, {"x14", "a4"}, {"x15", "a5"},{"x16", "a6"}, {"x17", "a7"}, {"x18", "s2"}, {"x19", "s3"}, {"x20", "s4"},
        {"x21", "s5"}, {"x22", "s6"}, {"x23", "s7"}, {"x24", "s8"}, {"x25", "s9"},{"x26", "s10"}, {"x27", "s11"}, {"x28", "t3"}, {"x29", "t4"}, {"x30", "t5"},{"x31","t6"}
    };

public:
    Parser(){index=0;}
    int getIndex()
    {
        return index;
    }
    void setIndex(int a)
    {
        index=a;
    }

    std::pair<std::vector<std::vector<std::string>>, std::map<std::string, int>> parseFromFile(const std::string& filePath,std::vector<int>& memory) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filePath << std::endl;
            return {{}, {}};
        }

        std::vector<std::vector<std::string>> lineWiseSplit;
        std::map<std::string, int> labels;
        std::string line;
        // int index =0; // Starting memory index

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
                    memory[index] = value;
                    //processor.setInitialMemory(index, value);
                }
                //  std::cout << "Core 0 Memory:" << std::endl;
                // for (const auto& entry : processor.getMemoryContents()) {
                //     std::cout << entry<<" ";
                // }
                
            } else if (lineWiseSplit[i].size() > 0 && lineWiseSplit[i][0] == ".word") {
                for (size_t j = 1; j < lineWiseSplit[i].size(); ++j, index += 4) {
                    int value = std::stoi(lineWiseSplit[i][j]);
                    memory[index] = value;
                    //processor.setInitialMemory(index, value);
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
    auto parsedProgram1 = parser.parseFromFile("code1.txt",sim.memory); // Parse the program from a file
    auto parsedProgram2 = parser.parseFromFile("code2.txt",sim.memory);//2nd file
    sim.cores[0].setProgram(parsedProgram1);
    sim.cores[1].setProgram(parsedProgram2);

    for (auto& core : sim.cores) {
        core.reset();
    }
    std::cout << " Memory:" << std::endl;
    for (const auto& entry : sim.getMemoryContents()) {
        std::cout << entry<<" ";
    }
    std::cout<<std::endl;
    sim.run();    
    const auto& coreRegisters0 = sim.getCoreRegisters(0);
    const auto& coreRegisters1 = sim.getCoreRegisters(1);
  
    std::cout << "+============ After Run ============:" << std::endl;
    std::cout << "Core 0 Registers:" << std::endl;
    for (const auto& entry : coreRegisters0) {
        std::cout << entry.first << ": " << entry.second<<std::endl;
    }

    std::cout << "Core 1 Registers:" << std::endl;
    for (const auto& entry : coreRegisters1) {
        std::cout << entry.first << ": " << entry.second << std::endl;
    }
    std::cout<<"FINAL MEMORY"<<std::endl;
    for (const auto& entry : sim.getMemoryContents()) {
        std::cout << entry<<" ";
    }

    return 0;
}



