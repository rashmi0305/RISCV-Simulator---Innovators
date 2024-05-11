#ifndef Processor_h
#define Processor_h
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include<fstream>
#include<iomanip>
#include <map>
#include<algorithm>
#include <bitset>
#include<math.h>
#include<limits.h>
#include<sstream>
#include<random>
#include "Instruction.h"
#include "Cache.h"

bool predictBranch()//for now just static prediction
    {
        return false;
    }

class Core {
private:
    std::unordered_map<std::string, int> registers;
    std::unordered_map<std::string, std::vector<int>> vregisters;
    bool stall1=0; //stall in case of load word
    int stall=0;
    int numInst=0;
    int cycles=0;
   // bool initLat=false; 
    int stallCount=0;
    int memacess=1;
  
    std::map<std::string, int> labels;
    std::map<std::string, std::pair<int, int>> vectorInfo;
    Instruction IF_ID_register;//to store the instructions temporarily in between stages
    Instruction ID_EX_register;
    Instruction EX_MEM_register;
    Instruction MEM_WB_register;        
 public:
    int pc;
    bool dataforwarding;
    std::unordered_map<std::string, int> instLatencies;
    std::vector<std::vector<std::string>> program;
    Core():program(), labels(), pc(0), registers(),vregisters(), IF_ID_register(), ID_EX_register(), EX_MEM_register(), MEM_WB_register() {}
    void execute(std::vector<int>& memory,CacheSimulator &cache);
    int getRegister(const std::string& reg);
    std::vector<int> getvregisters(const std::string& reg);
    void setRegister(std::string reg, int num);
    void setvregister(std::string reg, std::vector<int >num);
    void setProgram(std::tuple<std::vector<std::vector<std::string>>, std::map<std::string, int>,std::map<std::string,std::pair<int,int>>> parsedProgram);
    const std::unordered_map<std::string, int>& getRegisters() const;
    const std::unordered_map<std::string, std::vector<int>>& getVectorRegisters() const;
    void reset();
    void setmem_acess(int x);
    void pipelineFetch(std::vector<int>& memory,CacheSimulator &cache);
    bool checkDependency(Instruction curr,Instruction prev,Instruction pprev);
    void pipelineDecode();
    //void decode_forwarding();
    void pipelineExecute(std::vector<int>& memory);
    void pipelineMemory(std::vector<int>& memory,CacheSimulator &cache);
    void pipelineWriteBack();
    void print();

        
     
};

void Core::print()
{
    std::cout << "Total Stall Count core : " <<stallCount<<std::endl; 
     std::cout << "Total Instruction Count core: " <<numInst<<std::endl; 
     std::cout << "Total Clock Cycles core : " <<cycles<<std::endl; 
     std::cout<<"IPC:"<<(double)numInst/cycles<<std::endl;
     std::cout<<"--------------------------------------------------------------"<<std::endl;
     
}
void Core::setmem_acess(int x){
     int memacess=x;
}
std::vector<int> Core::getvregisters(const std::string& reg) {
    // Implementation of getRegister function
    if (vregisters.find(reg) != vregisters.end()) {
            return vregisters[reg];
        } else {
            std::cerr << "vector Register " << reg << " not found." << std::endl;
            return {0};
        }
}
 void Core::setvregister(std::string reg,std:: vector<int> num){
      if (vregisters.find(reg) != vregisters.end() && reg!="zero") {
            vregisters[reg] = num;
        } else {
            std::cerr << "Register " << reg << " not found." << std::endl;
        }
 }

void Core::execute(std::vector<int> &memory,CacheSimulator &cache) {
    std::cout << "Start running ..." << std::endl;
    
    while (true) {
        // Increment clock cycles.
        
        this->pipelineWriteBack();
       
        this->pipelineMemory(memory,cache);
        
        this->pipelineExecute(memory);
        
        this->pipelineDecode();
       
        this->pipelineFetch(memory,cache);
        
        // Check if all pipeline stages are clear.
        if (pc >= program.size() && IF_ID_register.empty() && ID_EX_register.empty() && EX_MEM_register.empty() && MEM_WB_register.empty()) {
            break;
        }
        cycles++;
    }
    std::cout << "Done." << std::endl;
}

void Core::pipelineFetch(std::vector<int>&memory,CacheSimulator &cache) {
    std::cout<<"stalcount is"<<stallCount<<std::endl;
    if (pc < program.size() && stall==0) {
        while(true){
        if (program[pc][0] == "#" || program[pc][0].find(".") != std::string::npos) {
                pc = pc + 1;
   
        }
        else if (program[pc][0].find(":") != std::string::npos && program[pc].size() == 1) {
            pc = pc + 1;
        }
        else{
           
            int n = pc*4+3000;//to identify instruction by a number i.e pc which is stored in memory
            bool isHit=cache.prefetch(n);
            if(isHit!=true)
            {

                cycles+=memacess;//main mem access time-change to dynamic later change to add miss time
            }
            else {
                //can add hit time here(for latency) assumed to be 1 for now
            }
            break;
         }
        }
        if(!IF_ID_register.pred_taken && !EX_MEM_register.branch_taken)//checking for branch prediction and branch misprediction
        {
            Instruction inst(program[pc],pc,instLatencies);
            IF_ID_register =inst;
            pc++;
        }
        else
        {
            IF_ID_register=Instruction();
            stallCount++;
        }
       
     }
     else if(stall!=0)
     {
        // stall--;
        std::cout<<"Stall l"<<stall<<std::endl;//checking that no instruction fetched
     }
     else {
        IF_ID_register.clear();
    }
    std::cout<<"print if/id";
    this->IF_ID_register.printInst();
}
bool Core::checkDependency(Instruction curr,Instruction prev,Instruction pprev)//this function checks the data dependency between current,prev and pprev instructions and accordingly increments the stall
{   //if(!dataforwarding){
    if(stall==0 )
    {
        if((prev.opcode=="lw" )&& dataforwarding && (curr.rs1==prev.rd || curr.rs2==prev.rd) )//-down dep-prev done
    {
        stall++;
        stall1=true;
        stallCount++;
    }
    
      else if (!prev.rd.empty() && (prev.rd == curr.rs1 || prev.rd == curr.rs2)) {   //up for previous-done
        if(dataforwarding){
        if(!pprev.rd.empty() && (pprev.rd == curr.rs1 || pprev.rd == curr.rs2))
        {
            stall+=1;
            stallCount+=1;
            
        }
        return  true;
       }
        else
        {

            stall+=2;
            stallCount+=2;
        }

      }
      else if((pprev.opcode=="lw" ) && dataforwarding && (pprev.rd==curr.rs1 || pprev.rd==curr.rs2))//-down dep-pprev done
    {
        stall++;
        stallCount++;
    }
    else if (!pprev.rd.empty() && (pprev.rd == curr.rs1 || pprev.rd == curr.rs2)) {//up dependency for prev prev-done
            stall+=1;
            stallCount+=1;
            std::cout<<"I am";
        
    }
    
   
    //}
}
  return false; 
    
}
void Core::pipelineDecode() {
    
    if(!EX_MEM_register.branch_taken){
    if(EX_MEM_register.empty())
    {
        
    }
    if(stall!=0)
    {
        stall--;
    }
    if(stall==0){
    bool flag=checkDependency(IF_ID_register,EX_MEM_register,MEM_WB_register);
    if(stall!=0 && stall1)
    {

    }
    else if(stall==0)
    {
    
    ID_EX_register = IF_ID_register;
    //Register fetch for all possible instructions
    if(ID_EX_register.opcode=="add"||ID_EX_register.opcode=="sub")
    {
        ID_EX_register.rd_val=getRegister(ID_EX_register.rd);
        ID_EX_register.rs1_val=getRegister(ID_EX_register.rs1);
        ID_EX_register.rs2_val=getRegister(ID_EX_register.rs2);
    }
    else if(ID_EX_register.opcode=="vadd"||ID_EX_register.opcode=="vmul"||ID_EX_register.opcode=="vsub"){
        ID_EX_register.rd_vval=getvregisters(ID_EX_register.rd);
        ID_EX_register.rs1_vval=getvregisters(ID_EX_register.rs1);
        ID_EX_register.rs2_vval=getvregisters(ID_EX_register.rs2);
    }

    else if(ID_EX_register.opcode=="addi"||ID_EX_register.opcode=="srli"||ID_EX_register.opcode=="slli")
    {
        ID_EX_register.rd_val=getRegister(ID_EX_register.rd);
        ID_EX_register.rs1_val=getRegister(ID_EX_register.rs1);
    }
    else if(ID_EX_register.opcode=="vaddi"||ID_EX_register.opcode=="vmuli")
    {
        ID_EX_register.rd_vval=getvregisters(ID_EX_register.rd);
        ID_EX_register.rs1_vval=getvregisters(ID_EX_register.rs1);
    }
    
    else if (ID_EX_register.opcode == "sw") 
    {
        //sw rs2, imm(rs1) format
        ID_EX_register.rs2_val = getRegister(ID_EX_register.rs2);
        ID_EX_register.rs1_val = getRegister(ID_EX_register.rs1);
    }
    else if (ID_EX_register.opcode == "lw")
    {
    // Parse the instruction and extract rd, imm, and rs1
    ID_EX_register.rd_val = getRegister(ID_EX_register.rd);
    ID_EX_register.rs1_val = getRegister(ID_EX_register.rs1);
    }
    else if (ID_EX_register.opcode == "vlw")
    {//has two possible formats: vlw rd,vec_name which is rs1 i.e without imm and vlw rd, imm( rs1)
    ID_EX_register.rd_vval = getvregisters(ID_EX_register.rd);
    if(ID_EX_register.imm!=-1)
    {
        ID_EX_register.rs1_val = getRegister(ID_EX_register.rs1);
    }
    }

     else if (ID_EX_register.opcode == "vsw")
    {
    // Parse the instruction and extract rd, imm, and rs1
    ID_EX_register.rs2_vval = getvregisters(ID_EX_register.rs2);
    ID_EX_register.rs1_val = getRegister(ID_EX_register.rs1);
    }

    else if (ID_EX_register.opcode == "bge" || ID_EX_register.opcode == "bne" ||ID_EX_register.opcode == "beq" || ID_EX_register.opcode == "blt")
    {
    
        ID_EX_register.pred_taken=predictBranch();
        ID_EX_register.rs1_val = getRegister(ID_EX_register.rs1);
        ID_EX_register.rs2_val = getRegister(ID_EX_register.rs2);
        // Extract the immediate value and sign-extend it
        //ID_EX_register.imm = signExtend(ID_EX_register.imm);
    }
    else if (ID_EX_register.opcode == "li")
    {
        // Parse the instruction and extract rd and imm
        ID_EX_register.rd_val = ID_EX_register.imm;
    }
    else if (ID_EX_register.opcode == "j") {
     int addr=ID_EX_register.address = labels[ID_EX_register.label+":"];
    pc = addr; 
    IF_ID_register.clear();
    stallCount++;

 }
    }
    if(stall1 && stall==0){ //forwarding from mem stage
        if(MEM_WB_register.rd == ID_EX_register.rs1){

        ID_EX_register.rs1_val=MEM_WB_register.rd_val;
        }
        else if(MEM_WB_register.rd == ID_EX_register.rs2)
        {
            ID_EX_register.rs2_val=MEM_WB_register.rd_val;
        }
        stall1=false;
    }
    else if(flag && !stall1){//not lw and sw
        if(EX_MEM_register.rd == ID_EX_register.rs1)
        {
            ID_EX_register.rs1_val=EX_MEM_register.rd_val;
        }
        else if(EX_MEM_register.rd == ID_EX_register.rs2)
        {
            ID_EX_register.rs2_val=EX_MEM_register.rd_val;
        }

    }}
   
}
else{
        ID_EX_register.clear();
        stallCount++;
    }
     std::cout<<"print id/ex";
    ID_EX_register.printInst();
    //IF_ID_register.clear();
    

}

void Core::pipelineExecute(std::vector<int> &memory) {
    if(stall1){ 
        EX_MEM_register=Instruction();
    }
    else
{
     std::string opcode = ID_EX_register.opcode;
     if(ID_EX_register.latency>1)
     {
        ID_EX_register.latency--;
     }
     else{
    // Perform execution based on the opcode
    if (opcode == "addi") {
        ID_EX_register.rd_val = ID_EX_register.rs1_val + ID_EX_register.imm;
        // Update the EX_MEM_register
       // EX_MEM_register = {ID_EX_register.rd, ID_EX_register.rd_val};
    } 
    if (opcode == "vmuli") {//multiply vector by given scalar
        for(int i=0;i<ID_EX_register.rs1_vval.size();i++){
        ID_EX_register.rd_vval.push_back(ID_EX_register.rs1_vval[i] * ID_EX_register.imm);
        }
        // Update the EX_MEM_register
       // EX_MEM_register = {ID_EX_register.rd, ID_EX_register.rd_val};
    }
    if (opcode == "vaddi") {//add vector by given scalar
        for(int i=0;i<ID_EX_register.rs1_vval.size();i++){
        ID_EX_register.rd_vval.push_back( ID_EX_register.rs1_vval[i] + ID_EX_register.imm);
        }
        // Update the EX_MEM_register
       // EX_MEM_register = {ID_EX_register.rd, ID_EX_register.rd_val};
    } 
    else if (opcode == "add") {
        ID_EX_register.rd_val = ID_EX_register.rs1_val + ID_EX_register.rs2_val;
        // Update the EX_MEM_register
       // EX_MEM_register = {ID_EX_register.rd, ID_EX_register.rd_val};
    } 
    else  if (opcode == "vadd") {//add twoi vectors,if of different size then the bigger vector's remaining elements are retained next
        int i;
        for( i=0;i<std::min(ID_EX_register.rs1_vval.size(),ID_EX_register.rs2_vval.size());i++){
        ID_EX_register.rd_vval.push_back( ID_EX_register.rs1_vval[i] + ID_EX_register.rs2_vval[i]);
        }
        while(ID_EX_register.rs1_vval.size()>i)
        {
           ID_EX_register.rd_vval.push_back( ID_EX_register.rs1_vval[i]);
             i++;
        }
        while(ID_EX_register.rs2_vval.size()>i)
        {
           ID_EX_register.rd_vval.push_back( ID_EX_register.rs2_vval[i]);
            i++;
        }
    
        // Update the EX_MEM_register
       // EX_MEM_register = {ID_EX_register.rd, ID_EX_register.rd_val};
    } 
      if (opcode == "vmul") {//multiply elemenst of two vectors(only till same size)
        for(int i=0;i<std::min(ID_EX_register.rs1_vval.size(),ID_EX_register.rs2_vval.size());i++){
        ID_EX_register.rd_vval.push_back( ID_EX_register.rs1_vval[i] * ID_EX_register.rs2_vval[i]);
        }
        // Update the EX_MEM_register
       // EX_MEM_register = {ID_EX_register.rd, ID_EX_register.rd_val};
    } 
    
    else if (opcode == "sub") {
        ID_EX_register.rd_val = ID_EX_register.rs1_val - ID_EX_register.rs2_val;
        // Update the EX_MEM_register
       
    } 
    if (opcode == "vsub") {
        for(int i=0;i<std::min(ID_EX_register.rs1_vval.size(),ID_EX_register.rs2_vval.size());i++){
        ID_EX_register.rd_vval.push_back( ID_EX_register.rs1_vval[i] - ID_EX_register.rs2_vval[i]);
        }
        // Update the EX_MEM_register
       // EX_MEM_register = {ID_EX_register.rd, ID_EX_register.rd_val};
    } 
    else if ( opcode == "srli") {
         ID_EX_register.rd_val=ID_EX_register.rs1_val>>ID_EX_register.imm;
    } 
     else if ( opcode == "slli") {
         ID_EX_register.rd_val=ID_EX_register.rs1_val<<ID_EX_register.imm;
    } 
    else if (opcode == "lw"||opcode=="vlw") {
       
        ID_EX_register.address =ID_EX_register.rs1_val + ID_EX_register.imm;

    }

     else if (opcode == "sw"||opcode == "vsw") {
     
        ID_EX_register.address =ID_EX_register.rs1_val +ID_EX_register.imm;

     }
      else if (opcode == "vsw") {
     
        ID_EX_register.address =ID_EX_register.rs1_val +ID_EX_register.imm;

     }
     else if (opcode=="vlw") {
       if(ID_EX_register.imm!=-1)
       {
        ID_EX_register.address =ID_EX_register.rs1_val + ID_EX_register.imm;
       }
        
    }
    
    
       else if (opcode == "bge") {
       
                int a= ID_EX_register.rs1_val;
                int b= ID_EX_register.rs2_val;
               int addr= ID_EX_register.address=labels[ID_EX_register.label+":"];
                if(a>b||a==b){
                   pc=addr;
                   IF_ID_register=Instruction();
                   ID_EX_register.branch_taken=true;
                }                   
      } 
    else if (opcode == "blt") {
       
         int a = ID_EX_register.rs1_val;
         int b = ID_EX_register.rs2_val;
         int addr = ID_EX_register.address = labels[ID_EX_register.label + ":"];
         if (a < b) {
             pc = addr ;
             IF_ID_register=Instruction();
             ID_EX_register.branch_taken=true;
         } 
}
      else if (opcode == "bne") {
       
         int addr= ID_EX_register.address = labels[ID_EX_register.label + ":"];
         if (ID_EX_register.rs1_val != ID_EX_register.rs2_val) {
             pc =addr ;
             IF_ID_register=Instruction();
             ID_EX_register.branch_taken=true;
         }
     }
    
     else if (opcode == "beq") {
       
         int addr = ID_EX_register.address = labels[ID_EX_register.label + ":"];
         if (ID_EX_register.rs1_val == ID_EX_register.rs2_val) {
            pc = addr;
            IF_ID_register=Instruction();
            ID_EX_register.branch_taken=true;

         } 
     }
// 
    EX_MEM_register=ID_EX_register;
}
}
    std::cout<<"print EX/MEM";
    EX_MEM_register.printInst();
    // Clear the ID_EX_register after execution
   

        ID_EX_register.clear();
    
    

    // }
}


void Core::pipelineMemory(std::vector<int> &memory, CacheSimulator &cache) {
   // std::cout<<"Enter3e";

        std::string opcode = EX_MEM_register.opcode;
        // Check if the instruction is a load (lw) or store (sw)
        if (opcode == "lw") {
            // Perform memory read operation for load instruction
            
            bool isHit=cache.prefetch( EX_MEM_register.address);
            if(isHit==false)
            {
                cycles+=memacess;//change later based on miss time
            }
            int data = memory[EX_MEM_register.address]; // Access memory
            MEM_WB_register =EX_MEM_register; // Move instruction to the next pipeline register
            MEM_WB_register.rd_val = data; // Update instruction's destination register value
        } 
        else if (opcode == "sw") {//we have assumed cpu doesnt wait until entire write is complemeted,(waits only till written in some intermediate buffer)
            // Perform memory write operation for store instruction
            memory[EX_MEM_register.address] = EX_MEM_register.rs2_val; // Write data to memory
            MEM_WB_register = EX_MEM_register;// Move instruction to the next pipeline register
        
        }
        else if (opcode == "vlw") {
            std::pair<int, int>vectorValue;
            int vectorLength=4;
            // Perform memory read operation for load instruction
           std:: vector<int> data;
           if (EX_MEM_register.imm == -1) {
            // Check if the vector name exists in vectorInfo
            std::string vectorName = EX_MEM_register.rs1; // Assuming rs1 contains the vector name
            if (vectorInfo.find(vectorName) != vectorInfo.end()) {
                // Vector name found in vectorInfo, retrieve its value
                vectorValue = vectorInfo[vectorName];
                EX_MEM_register.address = vectorValue.first;
            }
          
            }
            vectorLength = vectorValue.second/4;
          
           for(int i=0;i<vectorLength;i++){
            data.push_back (memory[EX_MEM_register.address+i*4]); // Access memory
            EX_MEM_register.rd_vval.push_back( data[i]); // Update instruction's destination register value
           }
    
           for(int i=0;i<EX_MEM_register.rd_vval.size();i++)
           {
            std::cout<<EX_MEM_register.rd_vval[i]<<" ";
           }
           
           MEM_WB_register =EX_MEM_register;
        } 
        
        else if (opcode == "vsw") {//we have assumed cpu doesnt wait until entire write is complemeted,(waits only till written in some intermediate buffer)
            // Perform memory write operation for store instruction
            for(int i=0;i<EX_MEM_register.rs2_vval.size();i++){
            memory[EX_MEM_register.address+i*4] = EX_MEM_register.rs2_vval[i]; // Write data to memory
            }
             MEM_WB_register = EX_MEM_register;// Move instruction to the next pipeline register
        }
         else {
            // For other instructions, simply pass them to the next pipeline stage
            MEM_WB_register = EX_MEM_register;
        }
        std::cout<<"print MEM/WB";
        MEM_WB_register.printInst();
        // Clear the EX_MEM_register as the instruction has been processed
        EX_MEM_register.clear();
    }
    void Core::pipelineWriteBack() {
        // Check if the instruction has a destination register
         
        if (!MEM_WB_register.rd.empty()) {
            // Write back the result to the destination register
             
            
            if(MEM_WB_register.opcode=="vadd"||MEM_WB_register.opcode=="vsub"||MEM_WB_register.opcode=="vaddi"||MEM_WB_register.opcode=="vmul"||MEM_WB_register.opcode=="vmuli"|| MEM_WB_register.opcode=="vlw"||MEM_WB_register.opcode=="vsw")
            {
                 std::cout<<MEM_WB_register.rd<<" "<<" VAL IS ";
                for(int i=0;i<MEM_WB_register.rd_vval.size();i++){
                    std::cout<<MEM_WB_register.rd_vval[i]<<" ";
                }
                 copy(MEM_WB_register.rd_vval.begin(),MEM_WB_register.rd_vval.end(), back_inserter(vregisters[MEM_WB_register.rd]));  
            }
             
            else{
            std::cout<<MEM_WB_register.rd<<" "<<" VAL IS "<<MEM_WB_register.rd_val<<std::endl;
            {
            registers[MEM_WB_register.rd] = MEM_WB_register.rd_val; // Update the value of the destination register
            }
            numInst++;
             for (const auto& entry : getRegisters()) {
                std::cout << entry.first << ": " << entry.second<<std::endl;
             }
        }  
        
            // Mark the destination register as ready
            MEM_WB_register.rd_ready = true;
        }
        MEM_WB_register.clear();
    
}


void Core:: setProgram(std::tuple<std::vector<std::vector<std::string>>, std::map<std::string, int>,std::map<std::string,std::pair<int,int>>> parsedProg)
{
    program=std::get<0>(parsedProg);
    labels=std::get<1>(parsedProg);
    vectorInfo=std::get<2>(parsedProg);
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
const std::unordered_map<std::string, std::vector<int>>& Core::getVectorRegisters()const {
    return vregisters;}
    

void Core::reset() {
    // Implementation of reset function
     registers = {
             {"x0",0}, {"x1", 0}, {"x2", 0}, {"x3", 0}, {"x4", 0}, {"x5", 0},
    {"x6", 0}, {"x7", 0}, {"x8", 0}, {"x9", 0}, {"x10", 0},
    {"x11", 0}, {"x12", 0}, {"x13", 0}, {"x14", 0}, {"x15", 0},
    {"x16", 0}, {"x17", 0}, {"x18", 0}, {"x19", 0}, {"x20", 0},
    {"x21", 0}, {"x22", 0}, {"x23", 0}, {"x24", 0}, {"x25", 0},
    {"x26", 0}, {"x27", 0}, {"x28", 0}, {"x29", 0}, {"x30", 0},
    {"x31", 0}
        };
       vregisters={
    {"v0", {0}}, {"v1", {}}, {"v2", {}}, {"v3", {}},
    {"v4", {}}, {"v5", {}}, {"v6", {}}, {"v7", {}}
};

        std::cout << "Reset processor" << std::endl;
}

class Processor {
    public:
        CacheSimulator cache;
public:
    std::vector<int> memory;
    int clock;
    std::vector<Core> cores;

public:
    Processor();
    void run();
    void setCache(int cache_size,int block_size,int set_ass,int rep_policy,int mem_acess);
    void setInitialMemory(int wordAddress, int value);
    void setMemory(int wordAddress, int value);
    int getMemory(int wordAddress);
    const std::vector<int>& getMemoryContents() const;
    const std::unordered_map<std::string, int>& getCoreRegisters(int coreIndex) const;
    const std::unordered_map<std::string, std::vector<int>>& getCoreVectorRegisters(int coreIndex) const;

};
Processor::Processor() : memory(4096, 0), clock(0), cores(2),cache() {}
void Processor::setCache(int cache_size,int block_size,int set_ass,int rep_policy,int memacess)
{
    cache.setCache(cache_size,block_size,set_ass,rep_policy,memacess);
}

void Processor::run() {
    while (true) {
        bool allCoresCompleted = true;
        for (int i = 0; i < cores.size(); ++i) {
            if (cores[i].pc < cores[i].program.size()) {
                cores[i].execute(memory,cache);
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
    int index = (wordAddress) / 4;
        memory[index] = value;
}

void Processor::setMemory(int wordAddress, int value) {
    // Implementation of setMemory function
    int index = (wordAddress) / 4;
        memory[index] = value;
}

int Processor::getMemory(int wordAddress) {
    // Implementation of getMemory function
    int index = (wordAddress) / 4;
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
const std::unordered_map<std::string, std::vector<int>>& Processor::getCoreVectorRegisters(int coreIndex) const {
    // Implementation of getVectorRegisters function
    if (coreIndex >= 0 && coreIndex < cores.size()) {
        return cores[coreIndex].getVectorRegisters();
    } else {
        std::cerr << "Invalid core index." << std::endl;
        static std::unordered_map<std::string, std::vector<int>> emptyMap;
        return emptyMap;
    }
}
  
class Parser {
private:
    int index;
    
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
    std::tuple<std::vector<std::vector<std::string>>, std::map<std::string, int>, std::map<std::string, std::pair<int, int>>> parseFromFile(const std::string& filePath,std::vector<int>& memory) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filePath << std::endl;
            return {{}, {},{}};
        }

        std::vector<std::vector<std::string>> lineWiseSplit;
        std::map<std::string, int> labels;
        std::map<std::string, std::pair<int, int>> vectorInfo;
        std::string line;
        // int index =0; // Starting memory index

        while (std::getline(file, line)) {
            std::replace(line.begin(), line.end(), ',', ' ');
            std::vector<std::string> lineArr;
            std::stringstream lineStream(line);
            std::string token;
            while (lineStream >> token) {
               
                    lineArr.push_back(token);
                
            }
            lineWiseSplit.push_back(lineArr);
    
            
        }
        
        
        for (size_t i = 0; i < lineWiseSplit.size(); ++i) {
            if (lineWiseSplit[i][0].find(":") != std::string::npos) {
                labels[lineWiseSplit[i][0]] = i;
            }
        }
        for (size_t i = 0; i < lineWiseSplit.size(); ++i) {//example arr1: .word 1 2...
            if (lineWiseSplit[i][0].find(":") != std::string::npos && lineWiseSplit[i].size() > 1 && lineWiseSplit[i][1] == ".word") {
                std::vector<std::string>& line = lineWiseSplit[i];
                line.erase(line.begin());
                for (size_t j = 1; j < line.size(); ++j, index += 4) {
                    int value = std::stoi(line[j]);
                    memory[index] = value;

                }
                
            } else if (lineWiseSplit[i].size() > 0 && lineWiseSplit[i][0] == ".word") {//example .word 1 2 3..
                for (size_t j = 1; j < lineWiseSplit[i].size(); ++j, index += 4) {
                    int value = std::stoi(lineWiseSplit[i][j]);
                    memory[index] = value;
                }
            }
            //for vectors-store the vecName along with starting address and length in a map.
            else if (lineWiseSplit[i].size() > 0 && lineWiseSplit[i][0] == ".vec") {
                std::string vecName = lineWiseSplit[i][1]; // Extract the vector name
                vecName.pop_back(); // Remove the colon at the end
                int currentAddress = index; // Starting memory index
                for (size_t j = 2; j < lineWiseSplit[i].size(); ++j, index += 4) {
                
                    int value = std::stoi(lineWiseSplit[i][j]);
                    memory[index] = value;
    
                }

                int endAddress = index - 4; // Ending address
                vectorInfo[vecName] ={currentAddress,endAddress - currentAddress+4} ; // Store current addresses and length
}
        }
        std::tuple<std::vector<std::vector<std::string>>, std::map<std::string, int>, std::map<std::string, std::pair<int, int>>> parsedProgram = std::make_tuple(lineWiseSplit, labels,vectorInfo);
        int a=(int)lineWiseSplit.size();
        for(int i=0;i<a;i++)
        {
         memory[3000+i*4]=i;
        }
        return parsedProgram;
    }
};
#endif 