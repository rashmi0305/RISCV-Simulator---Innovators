#ifndef INSTRUCTION_H
#define INSTRUCTION_H
#include<algorithm>
#include <unordered_map>
#include <iostream>
#include <vector>
#include <string>

class Instruction{
private:
    bool cleared=false;//checks if instruction is empty/has been cleared
public:

    std::string opcode;
    std::string rd;
    std::string rs1;
    std::string rs2;
    std::string label;
    int pc;
    int rd_val=0;
    int rs1_val=0;
    int rs2_val=0;
    std::vector<int> rd_vval={0};
    std::vector<int> rs1_vval={0};
    std::vector<int> rs2_vval={0};
    int imm=0;
    int latency=1;
    int address=0;
    bool rd_ready=false;
    int branch_target=0;
    int pred_target=0;
    bool branch_taken;
    bool pred_taken; //prediction-for now always false
    Instruction() {
        clear(); // Call clear() in the constructor to ensure initial state
    }

    // Clear function to reset all member variables
    void clear() {
        opcode.clear();
        rd.clear();
        rs1.clear();
        rs2.clear();
        label.clear();
        latency=1;
        pc = 0;
        rd_val = 0;
        rs1_val = 0;
        rs2_val = 0;
        rd_vval = {0};
        rs1_vval = {0};
        rs2_vval = {0};
        imm = 0;
        address = 0;
        rd_ready = false;
        branch_target = 0;
        pred_target = 0;
        branch_taken = false;
        pred_taken = false;
        cleared=true;
    }
    
    Instruction(std::vector<std::string> data,int pc,std::unordered_map<std::string, int> instLatencies)
    {
        this->pc=pc;
        opcode=data[0];
        latency=instLatencies[opcode];
        if(opcode=="add"||opcode=="sub" || opcode=="vadd" || opcode=="vsub")//add rd, rs1, rs2;sub rd, rs1, rs2
        {
            rd = data[1];
            rs1 = data[2];
            rs2 = data[3];
        }
        else if(opcode=="addi" || opcode=="srli"|| opcode=="slli" || opcode=="vaddi")
        {
            rd = data[1];
            rs1 = data[2];
            imm = std::stoi(data[3]);
        }
        else if(opcode=="lw")//ld rd, imm(rs1)
        {
             rd = data[1];
            std::string memOperand = data[2];
            // Parse offset and source register
            size_t pos = memOperand.find('(');
            size_t pos1 = memOperand.find(')');
            if (pos != std::string::npos && pos1 != std::string::npos) 
            {
                std::string offsetStr = memOperand.substr(0, pos);
                std::string srcReg = memOperand.substr(pos + 1, pos1 - pos - 1);
                imm = std::stoi(offsetStr);
                rs1 =srcReg;
            }
            else
            {
                rs1=memOperand;
            }
        }
        else if (opcode == "sw" || opcode=="vsw") //sw rs2, imm(rs1)
        {
            if (data.size() == 3)
            {
               rs2 = data[1];//srcReg
               std::string memOperand = data[2];
               // Parse offset and destination register
               size_t pos = memOperand.find('(');
               size_t pos1 = memOperand.find(')');
               if (pos != std::string::npos && pos1 != std::string::npos) {
                   std::string offsetStr = memOperand.substr(0, pos);
                   std::string destReg = memOperand.substr(pos + 1, pos1 - pos - 1);
                   imm = std::stoi(offsetStr);
                   rs1=destReg;
               }
               else
               {
                   //print error
               }
            }
            else
            {
               std::cerr << "Invalid instruction format at line " << pc << std::endl; 
            }
        }
        else if (opcode == "bge" ||opcode=="bne"||opcode=="beq" ||opcode=="blt") 
        {
            if (data.size() == 4) 
            {
                rs1 = data[1];
                rs2 = data[2];
                label = data[3];
               
            }
            else 
            {
                std::cerr << "Invalid instruction format at line " << pc << std::endl; 
            }
        }
         else if (opcode == "li")
        {
            if (data.size() == 3)
            {
                rd = data[1];
                imm = std::stoi(data[2]);
            }
            else 
            {
                std::cerr << "Invalid 'li' instruction format at line " << pc << std::endl;
            }
        }
        else if (opcode == "j") 
        {
            if (data.size() == 2)
            {
                label = data[1];
            } 
            else 
            {
                std::cerr << "Invalid 'j' instruction format at line " << pc << std::endl;  
            }
        }
       


    }
    void printInst()//printing instructions to check if correctly flowing through pipeline
    {
        std::cout<<"Op"<<opcode<<" "<<" dest"<<rd<<" "<<" rs1"<<rs1<<" "<<" rs2"<<rs2<<" "<<"L"<<label<<" "<<" pc"<<pc<<" "<<" Values:"<<rd_val<<" "<<rs1_val<<" "<<rs2_val<<" "<<"Imm"<<imm<<" "<<address<<std::endl;
    }
    bool empty()
    {
        return cleared;
    }
    

};
#endif 