# RISC-V SIMULATOR 
-----
### Team Innovators 

## Minutes of Meetings:

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
    
### Phase -2
    How to check data dependency
    How to calculate IPC 
    How to find whether stall is present or not
    How to do execution based on latency
    How to initialize registers between IF,ID/RF,EXE,MEM and WB and take values from it
    Is there a need for another class-Instruction
    How to write branch prediction so that it can be extended later on
    
### Phase-1
    How to initialize registers and memory
    Should we use map to map register name to its value 
    How to access contents in registers and update the values-using functions or directlt
    Should execute have separate file or include inside processor(while pipelining then?)
    Should we consider using enums for register names,instruction types,opcodes or directly access them in string format
 
