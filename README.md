# RISC-V SIMULATOR 
-----
## Team Innovators

## Building and Running the Program

To build and run the program, follow these steps:

1. **Clone the Repository**: Clone the GitHub repository containing the C++ code and the Makefile.

    ```bash
    git clone https://github.com/rashmi0305/RISCV-Simulator---Innovators.git
    ```

2. **Navigate to the Repository Directory**: Open a terminal and navigate to the directory where the repository was cloned.

    ```bash
    cd RISCV-Simulator---Innovators
    ```

3. **Build the Program**: Run the following command to compile the program. This will generate an executable named `simulator`.

    ```bash
    make
    ```

4. **Run the Program**: Once the compilation is successful, execute the generated executable.

    ```bash
    ./processor
    ```

5. **Clean Up (Optional)**: To remove the generated object files and executable, run the following command.

    ```bash
    make clean
    ```

---

## Minutes of Meetings:
#### Date: 9-may-2024
    Members:Rashmitha,Sahiti
    Minutes:1.Decided to integrate simd unit into the processor itself.
            2.Decided on using special vector registers and instructions-vadd vaddi vmul vmuli vlw vsw 
#### Date: 8-may-2024
    Members:Rashmitha,Sahiti
    Minutes:1.Decided to check previous 2 addresses logic in stride prefetcher.
            2.Tested prefetching-got better values for hit rate etc
            3.Decided to do SIMD
#### Date: 7-may-2024
    Members:Rashmitha,Sahiti
    Minutes:1.Decided to do prefetching using stride prefetcher
            2.Thought about how to store pre-fetch data into cache-use access function written before
#### Date: 6-may-2024
    Members:Rashmitha,Sahiti
    Minutes:1.Decided to implement pre-fetch or another level of cache
#### Date: 10-Apr-2024
    Members:Rashmitha,Sahiti
    Minutes:1.Update code for LRU Replacement policy and succesfully ran program.
            2.Updated code for to find Cache Access time.
#### Date: 8-Apr-2024
    Members:Rashmitha,Sahiti
    Minutes:1.Updated code for Replacement policies like LFU and Random and checked if programruns correctly.
            2.Updated function to get MissRate,HitRate.
#### Date: 6-Apr-2024
    Members:Rashmitha,Sahiti
    Minutes:1.Update splitaddress function to get Tag ,Index ,Offset Bits correctly.
            2.Discussed and Update code for Replacement policies like LFU and Random.
#### Date: 3-Apr-2024
    Members:Rashmitha,Sahiti
    Minutes:1.Discarded idea of storing dtata in cache
            2.Implementation of code for class CacheSimulator
            3..Updated Code to take input Cache_Size, Block_size, Set_associativity,Replacement_policy, Memory_Acess from file.
#### Date: 30-Mar-2024
    Members:Rashmitha,Sahiti
    Minutes:1.Checked references in net to understand cache briefly and how to access and store data in cache.
            2.Finally decided to take Cache as 2-D Array, and to store data and pc values in it for better implimentation. 
#### Date: 27-Mar-2024
    Members:Rashmitha,Sahiti
    Minutes:1.Discussed on how to take cache data structure as we have to store both instructions and data where instructions are vectors and data is int.
            2.Discussed ways to store instruction in main memory.
#### Date: 10-Mar-2024
    Members:Rashmitha,Sahiti
    Minutes:1.Updated code for with forwarding and Latency
            2.Total number of stalls and cycles for each core added
            3.Ipc for each core and number of Instructions added
#### Date: 09-Mar-2024
    Members:Rashmitha,Sahiti
    Minutes:1.Updated code for without forwarding
            2.Discussion for Data with forwarding
#### Date: 07-Mar-2024
    Members:Rashmitha,Sahiti
    Minutes:1.Discussion for data dependency and forwarding
   
#### Date: 06-Mar-2024
    Members:Rashmitha,Sahiti
    Minutes:1.Updated branch prediction 
#### Date: 04-Mar-2024
    Members:Rashmitha,Sahiti
    Minutes:1.Discussion and implemented pipeline stages       

#### Date: 01-Mar-2024
    Members:Rashmitha,Sahiti
    Minutes:1.Discussed how to implement data dependency,latency
            2.Decided to restructure code to write Instruction class
            3.Discussed about empty function for instruction
    
#### Date: 25-Feb-2024
    Members:Rashmitha,Sahiti
    Minutes:1.Updated code 1 for BUBBLE SORT and code 2 for selection sort
            2.Updated Memory for two codes code 1 and code 2
            3.updated execute function for "ecall" opcode
            4.Generated LOGII file for explanation of Memory Usage,Core Registers,parsing of files,shared Memory to Core
            5.Displayed output of processor.cpp file in "LOGII" file for clear output format.

#### Date: 24-Feb-2024
    Members:Rashmitha,Sahiti
    Minutes:1.Updated parser to store labels for branch instructions
            2.Worked on execute function-branch instructions.
            3.Added bubblesort code as input file
            4.Mapping of registers from x1,x2 format to their aliases done
#### Date: 23-Feb-2024
    Members:Rashmitha,Sahiti
    Minutes:1.Worked for sub,addi,srl,sll,li
            2.Finished parser part (for these instructions)
   
#### Date : 22-Feb-2024
    Members:Rashmitha,Sahiti
    Minutes:1.Created Github repository and pushed initial files.
            2.Worked on execute function-add lw instructions
            3.Started working on parser to read from file instead of string
    
 ####   Date : 20 -Feb -2024
    Members:Rashmitha,Sahiti
    Decisions:  1.Initial idea for parser and processor decided.
                2.Divided work-Parser to Sahiti and Processor to Rashmitha
                3.Datatypes for register and memory decided as maps and array respectively.
   
####    Date: 16-Feb-2024
    Members:Sahiti,Rashmitha
    Decisions:1.Went through RISCV instruction manual
            2.Looked through and understood sim.py code in google classroom
    
####    Date : 12 -Feb -2024
    Members:Rashmitha,Sahiti
    Decisions:  1.Language-C++
                2.Looked through references for RISCV and ideas
-----
                
## Questions we had in initial stages:
### Phase -3
    What to take Cache Data datastructure
    How to put both instructions and data in same cache
    Should we store data, or should store only tag and index bits
    How to do execution based on variable latency
    Which replacement policy is better other than LRU
### Phase -2
    How to check data dependency
    How to calculate IPC 
    How to find whether stall is present or not
    How to initialize registers between IF,ID/RF,EXE,MEM and WB and take values from it
    Is there a need for another class-Instruction
    How to write branch prediction so that it can be extended later on
    
### Phase-1
    How to initialize registers and memory
    Should we use map to map register name to its value 
    How to access contents in registers and update the values-using functions or directly
    Should execute have separate file or include inside processor(while pipelining then?)
    Should we consider using enums for register names,instruction types,opcodes or directly access them in string format
 
