# Riscv-innovators
# CO project

    MinutesofMeetings:
    Date: 01-Mar-2024
    Members:Rashmitha,Sahiti
    Minutes:1.Understood how to implement data dependency,latency
            2.Decided to restructure code to write Instruction class
    ======================================================================================
    MinutesofMeetings:
    Date: 25-Feb-2024
    Members:Rashmitha,Sahiti
    Minutes:1.Updated code 1 for BUBBLE SORT and code 2 for selection sort
            2.Updated Memory for two codes code 1 and code 2
            3.updated execute function for "ecall" opcode
            4.Generated LOGII file for explanation of Memory Usage,Core Registers,parsing of files,shared Memory to Core
            5.Displayed output of processor.cpp file in "LOGII" file for clear output format.
    ======================================================================================
    MinutesofMeetings:
    Date: 24-Feb-2024
    Members:Rashmitha,Sahiti
    Minutes:1.Updated parser to store labels for branch instructions
            2.Worked on execute function-branch instructions.
            3.Added bubblesort code as input file
            4.Mapping of registers from x1,x2 format to their aliases done
    ======================================================================================
    Date: 23-Feb-2024
    Members:Rashmitha,Sahiti
    Minutes:1.Worked for sub,addi,srl,sll,li
            2.Finished parser part (for these instructions)
    ======================================================================================
    Date : 22-Feb-2024
    Members:Rashmitha,Sahiti
    Minutes:1.Created Github repository and pushed initial files.
            2.Worked on execute function-add lw instructions
            3.Started working on parser to read from file instead of string
    ======================================================================================
    
    Date : 20 -Feb -2024
    Members:Rashmitha,Sahiti
    Decisions:  1.Initial idea for parser and processor decided.
                2.Divided work-Parser to Sahiti and Processor to Rashmitha
                3.Datatypes for register and memory decided as maps and array respectively.
    ======================================================================================
    Date: 16-Feb-2024
    Members:Sahiti,Rashmitha
    Decisions:1.Went through RISCV instruction manual
            2.Looked through and understood sim.py code in google classroom
    ======================================================================================
    Date : 12 -Feb -2024
    Members:Rashmitha,Sahiti
    Decisions:  1.Language-C++
                2.Looked through references for RISCV and ideas
                
    ======================================================================================
    Questions we had in initial stages:
    Phase -2
    How to check data dependency
    how to enable and disable data forwarding 
    how to calculate IPC ,what is the meaning of cycle 
    how to find whether stall is present or not
    Phase-1
    how to initialize registers and memory
    should we use map to map register name to its value 
    how to access contents in registers and update the values
    Should execute have separate file or include inside processor(while pipelining then?)
    Should we consider using enums for register names,instruction types,opcodes or directly access them in string format
 
    ======================================================================================
