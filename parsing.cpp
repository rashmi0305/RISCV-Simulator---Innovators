    #include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>

    class core{
        public:
    std:: unordered_map <std::string, int> registers;
    int pc;
    std::vector<std::string> program;
    int getRegister(const std::string& reg);
    void setRegister(std::string reg, int num);
     void reset() ;
      void execute(std::vector<int>& memory);
        void readProgramFromFile(const std::string& filename);
        };
        /// @brief /
        /// @param filename 
        ////parsing assembly code from file
        void core::readProgramFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Unable to open file: " << filename << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            program.push_back(line);
        }

        file.close();
    }
    class Processor {
        //////////
        public:
    std::vector<int> memory;
    int clock;
    std::vector<core> cores;
    };
    int main() {
    Processor sim;

    sim.cores[0].setRegister("t0", 10);
    sim.cores[1].setRegister("t8", 268500992); // Set t8 to a valid memory address
    sim.cores[0].readProgramFromFile("code1.txt");
    sim.cores[1].readProgramFromFile("code2.txt");
    for (auto& core : sim.cores) {
        core.reset();
    }
    }