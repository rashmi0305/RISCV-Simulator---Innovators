#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <map>
using namespace std;

class Core {
public:
    unordered_map<std::string, int> registers;
    int pc;

public:
    vector<string> program;

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

    const unordered_map<string, int>& getRegisters() const {
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
    
    return 0;
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

    const unordered_map<string, int>& getCoreRegisters(int coreIndex) const {
        if (coreIndex >= 0 && coreIndex < cores.size()) {
            return cores[coreIndex].getRegisters();
        } else {
            std::cerr << "Invalid core index." << std::endl;
            static unordered_map<string, int> emptyMap;
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
        // Increment clock after executing instructions for each core
        clock += 1;
    }
}

int main() {
    Processor sim;

    sim.cores[0].program.push_back("ADD ra t0 v0");
    sim.cores[1].program.push_back("LD t9 4(t8)");

    // Ensure reset is called for each core
    for (auto& core : sim.cores) {
        core.reset();
    }

    sim.cores[0].setRegister("v0", 5);
    sim.cores[0].setRegister("t0", 10);

    sim.cores[1].setRegister("t8", 1);

    sim.setMemory(268500996, 8);  // Set the value at memory address 268500996 to 8

    sim.run();

    // Display the state of the processor after execution
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