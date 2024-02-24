# Riscv-innovators
# CO project

    MinutesofMeetings:
    Date : 18 -Feb -2024
    Members:Rashmitha,Sahiti
    Decisions:  1.Language-C++
                2.Looked through references for RISCV and ideas
        
    Date : 20 -Feb -2024
    Members:Rashmitha,Sahiti
    Decisions:  1.Initial idea for parser and processor decided.
                2.Divided work-Parser to Sahiti and Processor to Rashmitha
                3.Datatypes for register and memory decided as maps and array respectively.
    Date : 22-Feb-2024
    Members:Rashmitha,Sahiti
    Minutes:1.Created Github repository and pushed initial files.
            2.Worked on execute function-add lw instructions
            3.Started working on parser to read from file instead of string
    Date: 23-Feb-2024
    Members:Rashmitha,Sahiti
    Minutes:1.Worked for sub,addi,srl,sll,li
            2.Finished parser part (for these instructions)
    Date: 24-Feb-2024
    Members:Rashmitha,Sahiti
    Minutes:1.Updated parser to store labels for branch instructions
            2.Worked on execute function-branch instructions.
            3.Added bubblesort code as input file
            4.Mapping of registers from x1,x2 format to their aliases done
    
    Questions in initial state:
    how to initialize registers and memory
    should we use map to map register name to its value 
    how to access contents in registers and update the values
    Questions in intermediate state:
    Should execute have separate file or include inside processor(while pipelining then?)
    Should we consider using enums for register names,instruction types,opcodes or directly access them in string format
    Should the parser parse from a file containing the instructions or otherwise
