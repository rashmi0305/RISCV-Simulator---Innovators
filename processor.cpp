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
#include<sstream>
#include<random>
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
        if(opcode=="add"||opcode=="sub")//add rd, rs1, rs2;sub rd, rs1, rs2
        {
            rd = data[1];
            rs1 = data[2];
            rs2 = data[3];
        }
        else if(opcode=="addi" || opcode=="srli"|| opcode=="slli")
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
        else if (opcode == "sw") //sw rs2, imm(rs1)
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
        std::cout<<"O"<<opcode<<" "<<"d"<<rd<<" "<<"rs1"<<rs1<<" "<<"rs2"<<rs2<<" "<<"L"<<label<<" "<<"pc"<<pc<<" "<<"rdv"<<rd_val<<" "<<rs1_val<<" "<<rs2_val<<" "<<imm<<" "<<address<<std::endl;
    }
    bool empty()
    {
        return cleared;
    }
    

};

bool predictBranch()
    {
        return false;
    }
#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>

struct CacheBlock {
    int tag;
    bool valid;
    int priority;
};

class CacheSimulator {
private:

    std::vector<std::vector<CacheBlock>> cache; // 2D vector representing sets and blocks
    int cacheSize=0;
    int choice=0;//LRU or Random
    int blockSize=0;
    int numSets=0;
    int numBlocksPerSet=0;//set associativity
    double cacheHits=0;
    double cacheMisses=0;
std::pair<int,int> splitAddress(int address) {
    int num_bits_offset = static_cast<int>(std::log2(blockSize));
        std::cout<<num_bits_offset<<std::endl;
        address >>= num_bits_offset;
        int num_bits=static_cast<int>(std::log2(numSets));
        int index = address & ((1 << num_bits - 1));
        int num_bits_index =static_cast<int> (std::log2(index));
        int tag = address >>(num_bits);
        std::cout<<"Address:"<<address<<std::endl;//for checking values
        std::cout<<"num bits Offset:"<<num_bits_offset<<std::endl;
        std::cout<<"tag:"<<tag<<std::endl;
        std::cout<<"index"<<index<<std::endl;

    return std::make_pair(tag, index);
}


public:
     CacheSimulator(){}
    void setCache  (int _cacheSize, int _blockSize, int _associativity,int _choice)
        {
         cacheSize=_cacheSize; 
         blockSize=_blockSize;
        assert(cacheSize % blockSize == 0);
        choice=_choice;
        numBlocksPerSet = _associativity;
        numSets = cacheSize / (blockSize * numBlocksPerSet);
        cache.resize(numSets, std::vector<CacheBlock>(numBlocksPerSet));
        for (auto& set : cache) {
            for (auto& block : set) {
                block.valid = false;
            }
        }
    }
    double getHitRate(){
        int totalAccesses = cacheHits + cacheMisses;
        if (totalAccesses == 0) {
            return 0.0; // Avoid division by zero
        }
        return static_cast<double>(cacheHits) / totalAccesses;
    }
double getMissRate()  {
    int totalAccesses = cacheHits + cacheMisses;
     if (totalAccesses == 0) {
            return 0.0; // Avoid division by zero
        }
    return static_cast<double>(cacheMisses) / totalAccesses; // Miss rate is 1 - hit rate
    }

    bool access(int address) {
        std::cout<<"init address"<<address<<std::endl;
        int max=0;
        int max_block_tag=-1;//to keep track of lru block
        std::pair<int,int> a=splitAddress(address);
        int tag=a.first;
        int index=a.second;
        for (int i=0;i<numBlocksPerSet;i++) {
            
        
            if (cache[index][i].valid==true && cache[index][i].tag ==tag ) {
                for(auto & c:cache[index] )
                    if(c.tag!=cache[index][i].tag && c.valid==true)
                    {
                        //check this logic**
                        c.priority++;//MRU will have least number for priority variable and lru will have the highest number.
                        if(c.priority>max)
                        {
                            max=c.priority;
                            max_block_tag=c.tag;//this is to be replaced if miss occurs
                        }
                        // Cache hit
                    }
                        cacheHits++;
                        return true;
                    
                }
        }
            cacheMisses++;
             for (int i=0;i<numBlocksPerSet;i++) {
            //checks if set is not full ,directly adds block
            std::cout<<cache[index][i].valid;
            if(cache[index][i].valid==false)
            {
                cache[index][i].tag=tag;
                cache[index][i].valid=true;
                return false;
            }
         }
        //LRU
        if(choice==1)
        {
        // Cache miss, replace the least recently used block in the set
        for(int i=0;i<numBlocksPerSet;i++)
        {
            if(cache[index][i].tag==max_block_tag)
            {
                cache[index][i].tag=tag;
                cache[index][i].valid= true;
            }
        }
        
        }
        else
        {//Random-(update policy if possible)
        int lowerLimit=0;
        int upperLimit=numBlocksPerSet-1;// Generate a random number within the range [lowerLimit, upperLimit]
        int random=lowerLimit + rand() % (upperLimit - lowerLimit + 1);
        cache[index][random].tag=tag;
        cache[index][random].valid=true;
        }
        return false;
    }
};

 

class Core {
private:
    std::unordered_map<std::string, int> registers;
    bool stall1=0; //stall in case of load word
    int stall=0;
    int numInst=0;
    int cycles=0;
    bool initLat=false; 
    int stallCount=0;
  
    std::map<std::string, int> labels;
    
    Instruction IF_ID_register;//to store the instructions temporarily in between stages
    Instruction ID_EX_register;
    Instruction EX_MEM_register;
    Instruction MEM_WB_register;        
 public:
    int pc;
    bool dataforwarding;
    std::unordered_map<std::string, int> instLatencies;
    std::vector<std::vector<std::string>> program;
    Core():program(), labels(), pc(0), registers(), IF_ID_register(), ID_EX_register(), EX_MEM_register(), MEM_WB_register() {}
    void execute(std::vector<int>& memory,CacheSimulator &cache);
    int getRegister(const std::string& reg);
    void setRegister(std::string reg, int num);
    void setProgram(std::pair<std::vector<std::vector<std::string>>, std::map<std::string, int>> parsedProgram);
    const std::unordered_map<std::string, int>& getRegisters() const;
    void reset();
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
// unsigned int binary_to_uint(std:: string b)
// {
    
// }
void Core::pipelineFetch(std::vector<int>&memory,CacheSimulator &cache) {
    std::cout<<"stalcount is"<<stallCount<<std::endl;
    if (pc < program.size() && stall==0) {
        while(true){
        if (program[pc][0] == "#" || program[pc][0].find(".") != std::string::npos) {
                pc = pc + 1;
                // numInst++;
                // cycles++;
   
        }
        else if (program[pc][0].find(":") != std::string::npos && program[pc].size() == 1) {
            pc = pc + 1;
            // numInst++;
            // cycles++;

        }
        else{
           
            int n = pc+4000;
            // std::string add = std::bitset<32>(n).to_string();
            bool isHit=cache.access(n);
            if(isHit!=true)
            {

                cycles+=2;//main mem access time-change to dynamic later change to add miss time
            }
            else {
                //add hit time here?(for latency)
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
bool Core::checkDependency(Instruction curr,Instruction prev,Instruction pprev)
{   //if(!dataforwarding){
    if(stall==0 )
    {
        if(prev.opcode=="lw" && dataforwarding && (curr.rs1==prev.rd || curr.rs2==prev.rd) )//-down dep-prev done
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
      else if(pprev.opcode=="lw" && dataforwarding && (pprev.rd==curr.rs1 || pprev.rd==curr.rs2))//-down dep-pprev done
    {
        stall++;
        stallCount++;
    }
    else if (!pprev.rd.empty() && (pprev.rd == curr.rs1 || pprev.rd == curr.rs2)) {//up dependency for prev prev-done
            stall+=1;
            stallCount+=1;
        
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
    else if(initLat==false && EX_MEM_register.latency!=1){
    initLat=true;
    stall+=EX_MEM_register.latency;
    stallCount+=EX_MEM_register.latency-1;}
    if(stall!=0)
    {
        stall--;
    }
   // if (!IF_ID_register.empty()) {
    if(stall==0){
    bool flag=checkDependency(IF_ID_register,EX_MEM_register,MEM_WB_register);
    if(stall!=0 && stall1)
    {

    }
    else if(stall==0)
    {
    
    ID_EX_register = IF_ID_register;
    if(ID_EX_register.opcode=="add"||ID_EX_register.opcode=="sub")
    {
        ID_EX_register.rd_val=getRegister(ID_EX_register.rd);
        ID_EX_register.rs1_val=getRegister(ID_EX_register.rs1);
        ID_EX_register.rs2_val=getRegister(ID_EX_register.rs2);
    }
    if(ID_EX_register.opcode=="addi"||ID_EX_register.opcode=="srli"||ID_EX_register.opcode=="slli")
    {
        ID_EX_register.rd_val=getRegister(ID_EX_register.rd);
        ID_EX_register.rs1_val=getRegister(ID_EX_register.rs1);
    }
    else if (ID_EX_register.opcode == "lw")
    {
    // Parse the instruction and extract rd, imm, and rs1
    ID_EX_register.rd_val = getRegister(ID_EX_register.rd);
    ID_EX_register.rs1_val = getRegister(ID_EX_register.rs1);
    }

    else if (ID_EX_register.opcode == "sw") 
    {
        //sw rs2, imm(rs1)
        ID_EX_register.rs2_val = getRegister(ID_EX_register.rs2);
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
    else if (ID_EX_register.opcode == "li")//HERE OR EXECUTE ?
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

void Core::pipelineExecute(std::vector<int> &memory) {//write logic for latency here
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
    if (opcode == "add") {
        ID_EX_register.rd_val = ID_EX_register.rs1_val + ID_EX_register.rs2_val;
        // Update the EX_MEM_register
       // EX_MEM_register = {ID_EX_register.rd, ID_EX_register.rd_val};
    } 
    
    else if (opcode == "sub") {
        ID_EX_register.rd_val = ID_EX_register.rs1_val - ID_EX_register.rs2_val;
        // Update the EX_MEM_register
       
    } 
    else if ( opcode == "srli") {
         ID_EX_register.rd_val=ID_EX_register.rs1_val>>ID_EX_register.imm;
    } 
     else if ( opcode == "slli") {
         ID_EX_register.rd_val=ID_EX_register.rs1_val<<ID_EX_register.imm;
    } 
    else if (opcode == "lw") {
       
        ID_EX_register.address =ID_EX_register.rs1_val + ID_EX_register.imm;

    }
    
     else if (opcode == "sw") {
     
        ID_EX_register.address =ID_EX_register.rs1_val +ID_EX_register.imm;

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

        std::string opcode = EX_MEM_register.opcode;
        // Check if the instruction is a load (lw) or store (sw)
        if (opcode == "lw") {
            // Perform memory read operation for load instruction
            bool isHit=cache.access( EX_MEM_register.address);
            if(isHit==false)
            {
                cycles+=2;//change later based on miss time
            }
            int data = memory[EX_MEM_register.address]; // Access memory
            MEM_WB_register =EX_MEM_register; // Move instruction to the next pipeline register
            MEM_WB_register.rd_val = data; // Update instruction's destination register value
        } 
        else if (opcode == "sw") {//we have assumed cpu doesnt wait until entire write is complemeted,(waits only till written in some intermediate buffer)
            // Perform memory write operation for store instruction
            memory[EX_MEM_register.address] = EX_MEM_register.rs2_val; // Write data to memory
            MEM_WB_register = EX_MEM_register;// Move instruction to the next pipeline register
        } else {
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
             std::cout<<MEM_WB_register.rd<<" "<<" VAL IS "<<MEM_WB_register.rd_val<<std::endl;
            registers[MEM_WB_register.rd] = MEM_WB_register.rd_val; // Update the value of the destination register
            numInst++;
             for (const auto& entry : getRegisters()) {
                std::cout << entry.first << ": " << entry.second<<std::endl;
        }  
        
            // Mark the destination register as ready
            MEM_WB_register.rd_ready = true;
        }
        MEM_WB_register.clear();
    
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
             {"x0",0}, {"x1", 0}, {"x2", 0}, {"x3", 0}, {"x4", 0}, {"x5", 0},
    {"x6", 0}, {"x7", 0}, {"x8", 0}, {"x9", 0}, {"x10", 0},
    {"x11", 0}, {"x12", 0}, {"x13", 0}, {"x14", 0}, {"x15", 0},
    {"x16", 0}, {"x17", 0}, {"x18", 0}, {"x19", 0}, {"x20", 0},
    {"x21", 0}, {"x22", 0}, {"x23", 0}, {"x24", 0}, {"x25", 0},
    {"x26", 0}, {"x27", 0}, {"x28", 0}, {"x29", 0}, {"x30", 0},
    {"x31", 0}
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
    void setCache(int cache_size,int block_size,int set_ass,int rep_policy);
    void setInitialMemory(int wordAddress, int value);
    void setMemory(int wordAddress, int value);
    int getMemory(int wordAddress);
    const std::vector<int>& getMemoryContents() const;
    const std::unordered_map<std::string, int>& getCoreRegisters(int coreIndex) const;
};
Processor::Processor() : memory(4096, 0), clock(0), cores(2),cache() {}
void Processor::setCache(int cache_size,int block_size,int set_ass,int rep_policy)
{
    cache.setCache(cache_size,block_size,set_ass,rep_policy);
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
               
                    lineArr.push_back(token);
                
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
        int a=(int)lineWiseSplit.size();
        for(int i=0;i<a;i++)
        {
            memory[4000+i]=i;
        }
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
    //code for parsing cache

    std::ifstream file("cache_params.txt");
    
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file." << std::endl;
        return 1;
    }

    uint64_t cache_size, block_size, set_ass;
    int rep_policy;

  
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);

        if (!(iss >> cache_size >> block_size >> set_ass >> rep_policy)) {
            std::cerr << "Error: Invalid input format." << std::endl;
            return 1;
        }

        
        sim.setCache(cache_size, block_size, set_ass, rep_policy);//should also take main mem access time(update it)
    }

    
    file.close();
    std::cout<<cache_size<<" "<<block_size<<" "<< set_ass<<" "<< rep_policy;
    sim.setCache(cache_size , block_size, set_ass,rep_policy);
     for(int j=0;j<=1;j++){
     std::cout<<"Core"<<j+1<<":FORWARDING:Click 1 to enable,else disable";
     int i=1;
    std::cin>>i;
     if(i==1){
         sim.cores[j].dataforwarding=true;
     }
     else{
         sim.cores[j].dataforwarding=false;
     }
      }
  
    std::unordered_map<std::string, int> instLatencies= {
        {"add",1}, {"sub", 1}, {"addi" ,1},{"srli",1},{"slli",1},{"li",1},
        {"bge",1},{"bne",1},{"blt",1},{"bgt",1},{"lw",1},{"sw",1},{"beq",1}
    };
    sim.cores[0].instLatencies=instLatencies;
    sim.cores[1].instLatencies=instLatencies;
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
    sim.cores[0].print();
    sim.cores[1].print();
    std::cout<<" Hit rate  "<<sim.cache.getHitRate()<<std::endl;
    std::cout<<" Miss rate  "<<sim.cache.getMissRate()<<std::endl;

    return 0;
}