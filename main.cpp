#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include<fstream>
#include<iomanip>
#include<algorithm>
#include<sstream>
#include "Processor.h"
int main() {
       Processor sim;
    Parser parser;
    
    auto parsedProgram1 = parser.parseFromFile("code1.txt",sim.memory); // Parse the program from a file
    auto parsedProgram2 = parser.parseFromFile("vcode1.txt",sim.memory);//2nd file
    sim.cores[0].setProgram(parsedProgram1);
    sim.cores[1].setProgram(parsedProgram2);
   //code for parsing cache-the parameters are to be changed in the input file cache_params.txt

    std::ifstream file("cache_params.txt");
    
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file." << std::endl;
        return 1;
    }

    uint64_t cache_size, block_size, set_ass;
    int rep_policy,memacess;

  
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);

        if (!(iss >> cache_size >> block_size >> set_ass >> rep_policy>>memacess)) {
            std::cerr << "Error: Invalid input format." << std::endl;
            return 1;
        }

        
        sim.setCache(cache_size, block_size, set_ass, rep_policy,memacess);//should also take main mem access time(update it)
        sim.cores[0].setmem_acess(memacess);
        sim.cores[1].setmem_acess(memacess);
    }

    
    file.close();
    std::cout<<cache_size<<" "<<block_size<<" "<< set_ass<<" "<< rep_policy<<" "<<memacess;
    sim.setCache(cache_size , block_size, set_ass,rep_policy,memacess);

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
    const auto& vecRegisters0=sim.getCoreVectorRegisters(0);
     const auto& vecRegisters1=sim.getCoreVectorRegisters(1);

  
    std::cout << "+============ After Run ============:" << std::endl;
    std::cout << "Core 0 Registers:" << std::endl;
    for (const auto& entry : coreRegisters0) {
        std::cout << entry.first << ": " << entry.second<<std::endl;
    }
    std::cout << "Core 0 Vector Registers:" << std::endl;
for (const auto& entry : vecRegisters0) {
    std::cout << entry.first << ": ";
    const auto& vec = entry.second;
    for (int value : vec) {
        std::cout << value << " ";
    }
    std::cout << std::endl;
}
    std::cout << "Core 1 Registers:" << std::endl;
    for (const auto& entry : coreRegisters1) {
        std::cout << entry.first << ": " << entry.second << std::endl;
    }
    std::cout << "Core 1 Vector Registers:" << std::endl;
for (const auto& entry : vecRegisters1) {
    std::cout << entry.first << ": ";
    const auto& vec = entry.second;
    for (int value : vec) {
        std::cout << value << " ";
    }
    std::cout << std::endl;
}

    std::cout<<"FINAL MEMORY"<<std::endl;
    for (const auto& entry : sim.getMemoryContents()) {
        std::cout << entry<<" ";
    }
    sim.cores[0].print();
    sim.cores[1].print();
    std::cout<<" Acess Time in Cycles "<<sim.cache.getAcessRate()<<std::endl;
    std::cout<<" Hit rate  "<<sim.cache.getHitRate()<<std::endl;
    std::cout<<" Miss rate  "<<sim.cache.getMissRate()<<std::endl;

    return 0;
}