#ifndef TOMASULO_H
#define TOMASULO_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INST_NUM 15

// Reservation station parameter
#define NUM_LOAD_RS 3
#define NUM_STORE_RS 3
#define NUM_ADD_SUB_RS 3
#define NUM_MUL_DIV_RS 3
#define RS_NOT_FOUND -1

// Cycle count of each instruction
#define LW_CYCLE 2
#define SW_CYCLE 2
#define ADD_SUB_CYCLE 2
#define MUL_CYCLE 10
#define DIV_CYCLE 40

// Reservation ID group for dumping code
#define LOAD_OFFSET 1000
#define STORE_OFFSET 2000
#define ADD_OFFSET 3000
#define MULT_OFFSET 4000
#define MEM_OFFSET 5000
#define VALUE_EMPTY -1

typedef enum boolean{
   FALSE = 0,
   TRUE = 1
}bool;

typedef enum ins_op{
	NOP, LW, SW, MUL, SUB, DIV, ADD
}INST_OP;

typedef enum Register{
    F0,F2,F4,F6,F8,F10,F12,F14,NO_REG
}DATA_REG_TAG;

typedef enum rs_state{
    unused,issued,exec,done
}RS_state;

typedef struct instruction_reg{
    // Instruction status //
    int issue;              // default = 0
    int execute;            // Record execute order
    int writeback;          // Record writeback order
    int commit;             // Record commit order
    int ID;                 // Instruction identifier

    // Instruction parameters
    INST_OP type;           // Instruction type
    int offset;             // Offset (I-type)
    char src_1[5];          // Source 1 (R-type) or base (I-type)
    DATA_REG_TAG src_1_tag; // Source 1 enum
    char src_2[5];          // Source 2 (R-type)
    DATA_REG_TAG src_2_tag; // Source 2 enum
    char dest[5];           // Destination
    DATA_REG_TAG dest_tag;  // Destination enum
}INST_REG;

typedef struct data_reg{
    bool valid;             // Reg is valid or not
    bool waiting;           // All source data ready
    INST_REG *inst_ptr;     // point to the instruction reg, default should be NULL
    int value;              // register value
}DATA_REG;

typedef struct rs_operation{
    RS_state state;         // reservation station state 
	INST_REG *inst_ptr;     // point to user(instruction)
	INST_OP op;             // user inst. type
    DATA_REG_TAG dest_tag;  // destination reg
	int Vj, Vk, Qj, Qk;     // source reg (index)
	int cycle_last;         // -1 everytime during execution
}RS;

typedef struct rs_load_store {
    RS_state state;         // reservation station state         
	INST_REG *inst_ptr;     // point to user(instruction)
	INST_OP op;             // user inst. type
    DATA_REG_TAG base_tag;  // base register
    DATA_REG_TAG dest_tag;  // destination reg
    int offset;             // offset
	int cycle_last;         // -1 everytime during execution
}RS_LS;

// Read instruction from file
int read_inst(INST_REG* inst);

// Dump instruction list after execution                      
void dump_inst(INST_REG* inst,int inst_count);

// Initialize reservation station
void rs_ls_init(RS_LS* load);
void rs_add_init(RS* add);
void rs_mul_init(RS* mul);

// Initialize data register
void reg_init(DATA_REG* reg);
void inst_reg_init(INST_REG* inst);

// Find idle reservation station
int find_idle_load(RS_LS* rs_load);
int find_idle_store(RS_LS* rs_store);
int find_idle_add(RS* rs_add_sub);
int find_idle_mult(RS* rs_mul_div);

// Put instruction into reservation station
void set_idle_load(RS_LS* load, INST_REG* inst, DATA_REG* data,int cycle_count,int RS_index);
void set_idle_store(RS_LS* store, INST_REG* inst,int cycle_count);
void set_idle_add(RS* add, INST_REG* inst, DATA_REG* data,int cycle_count,int RS_index);
void set_idle_mult(RS *mult, INST_REG* inst, DATA_REG* data,int cycle_count,int RS_index);

// Write back function 
void writeback(RS_LS* rs_load, RS_LS* rs_store, RS* rs_add_sub, RS* rs_mul_div, DATA_REG* reg,const int cycle_counter);

// Commit 
void commit(INST_REG* inst,const int cycle_count);

// Decide terminate or not
bool check_finish(INST_REG* inst, int inst_count);

// Convert register tag to integer
int find_reg_index(DATA_REG_TAG data);

// Check register is not used by previous instruction
bool check_reg(const INST_REG* inst,int inst_count,DATA_REG_TAG tag);

// reset reservation station entry
void reset_RS(RS* rs);
void reset_RS_LS(RS_LS* rs);

// reset data register
void reset_REG(DATA_REG* reg);

// Update Reservation station
void update_RS(RS* rs_add_sub,RS* rs_mul_div, int reg_index);

// Convert register name to enum            
DATA_REG_TAG reg_to_reg_tag(char input[]);          

// print register
void print_reg(int reg_index);

#endif