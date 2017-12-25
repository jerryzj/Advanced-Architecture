#include "tomasulo.h"

int main(void){
    int i = 0;                      // loop index
    bool finish = FALSE;            // All instructions are finish or not
    bool allow_exec;                // Allow new inst. to execute
    int inst_count = 0;             // Count number of instructions
    int cycle_counter = 0;          // Count execution cycles, start from 1
    int program_counter = 0;        // Count issued instruction
    int RS_index = VALUE_EMPTY;     // Index of RS which will be use
    int reg_index = VALUE_EMPTY;

    INST_REG inst[MAX_INST_NUM];    // Declare instruction register
    DATA_REG fp_reg[8];             // Floating point data register
    RS_LS rs_load[NUM_LOAD_RS];     // Load entry
    RS_LS rs_store[NUM_STORE_RS];   // Store entry
    RS rs_add_sub[NUM_ADD_SUB_RS];  // Add./Sub. entry
    RS rs_mul_div[NUM_MUL_DIV_RS];  // Mul./Div. entry
    
    // Initialize reservation station 
    rs_ls_init(rs_load);
    rs_ls_init(rs_store);
    rs_add_init(rs_add_sub);
    rs_mul_init(rs_mul_div);
    
    // Initialize data reg
    reg_init(fp_reg);

    // Initialize instruction reg
    inst_reg_init(inst);

    // Read instructions from given file
    inst_count = read_inst(inst);
    
    while(finish == FALSE ){
// Cycle counter ++
        cycle_counter++;
// Decrement all executing RS's cycle_last
        for(i=0;i<NUM_LOAD_RS;i++){
            if(rs_load[i].state == exec){
                rs_load[i].cycle_last--;
            }
        }
        for(i=0;i<NUM_STORE_RS;i++){
            if(rs_store[i].state == exec){
                rs_store[i].cycle_last--;
            }
        }
        for(i=0;i<NUM_ADD_SUB_RS;i++){
            if(rs_add_sub[i].state == exec){
                rs_add_sub[i].cycle_last--;
            }
        }
        for(i=0;i<NUM_MUL_DIV_RS;i++){
            if(rs_mul_div[i].state == exec){
                rs_mul_div[i].cycle_last--;
            }
        }
// Check commit
        commit(inst,cycle_counter);
// Write back, broadcast result (update data reg)
        writeback(rs_load,rs_store,rs_add_sub,rs_mul_div,fp_reg,cycle_counter);
// Check instruction complete or not
        for (i=0;i<NUM_LOAD_RS;i++){
            if(rs_load[i].cycle_last == 0 && rs_load[i].state == exec){
                rs_load[i].inst_ptr->execute = cycle_counter;
                rs_load[i].state = done;
            }
        }
        for(i=0;i<NUM_STORE_RS;i++){
            if(rs_store[i].cycle_last == 0 && rs_store[i].state == exec){
                rs_store[i].inst_ptr->execute = cycle_counter;
                rs_store[i].state = done;
            }
        }
        for(i=0;i<NUM_ADD_SUB_RS;i++){
            if(rs_add_sub[i].cycle_last == 0 && rs_add_sub[i].state == exec){
                rs_add_sub[i].inst_ptr->execute = cycle_counter;
                rs_add_sub[i].state = done;
            }
        }
        for(i=0;i<NUM_MUL_DIV_RS;i++){
            if(rs_mul_div[i].cycle_last == 0 && rs_mul_div[i].state == exec){
                rs_mul_div[i].inst_ptr->execute = cycle_counter;
                rs_mul_div[i].state = done;
            }
        }
// Check register depedency
    for (i=0;i<NUM_LOAD_RS;i++){
            if(rs_load[i].state == exec){
                reg_index = find_reg_index(rs_load[i].dest_tag);
                fp_reg[reg_index].waiting = TRUE;
                fp_reg[reg_index].valid = TRUE;
                fp_reg[reg_index].inst_ptr = rs_load[i].inst_ptr;
            }
    }
    for(i=0;i<NUM_STORE_RS;i++){
        if(rs_store[i].state == issued || rs_store[i].state == exec){
                reg_index = find_reg_index(rs_store[i].dest_tag);
              //  fp_reg[reg_index].waiting = TRUE;
                fp_reg[reg_index].valid = TRUE;
                fp_reg[reg_index].inst_ptr = rs_store[i].inst_ptr;
            }
    }
    for(i=0;i<NUM_ADD_SUB_RS;i++){
        if(rs_add_sub[i].state == issued || rs_add_sub[i].state == exec){
            reg_index = find_reg_index(rs_add_sub[i].dest_tag);
            fp_reg[reg_index].waiting = TRUE;
            fp_reg[reg_index].valid = TRUE;
            fp_reg[reg_index].inst_ptr = rs_add_sub[i].inst_ptr;
        }
    }
    for(i=0;i<NUM_MUL_DIV_RS;i++){
        if(rs_mul_div[i].state == issued || rs_mul_div[i].state == exec){
            reg_index = find_reg_index(rs_mul_div[i].dest_tag);
            fp_reg[reg_index].waiting = TRUE;
            fp_reg[reg_index].valid = TRUE;
            fp_reg[reg_index].inst_ptr = rs_mul_div[i].inst_ptr;
        }
    }
// Add instruction into RS
        if (program_counter < inst_count){
            // Find available reservation station
            switch(inst[program_counter].type){
                case LW:
                    RS_index = find_idle_load(rs_load); 
                    break;
                case SW:
                    RS_index = find_idle_store(rs_store);
                    break;
                case ADD:
                case SUB:
                    RS_index = find_idle_add(rs_add_sub);
                    break;
                case MUL:
                case DIV:
                    RS_index = find_idle_mult(rs_mul_div);
                    break;
                default:
                    break;
            }
            if(RS_index != RS_NOT_FOUND){
                // add the current instruction to RS
                switch(inst[program_counter].type){
                    case LW:
                        set_idle_load(&rs_load[RS_index],&inst[program_counter],fp_reg,cycle_counter,RS_index);
                        break;
                    case SW:
                        set_idle_store(&rs_store[RS_index],&inst[program_counter],cycle_counter);
                        break;
                    case ADD:
                    case SUB:
                        set_idle_add(&rs_add_sub[RS_index],&inst[program_counter],fp_reg,cycle_counter,RS_index);
                        break;
                    case MUL:
                    case DIV:
                        set_idle_mult(&rs_mul_div[RS_index],&inst[program_counter],fp_reg,cycle_counter,RS_index);
                        break;
                    default:
                        break;
                }
                program_counter++;
            }
        }
// Check ready or not, allow execution
        for(i=0;i<NUM_LOAD_RS;i++){
            if(rs_load[i].inst_ptr !=  NULL){
                allow_exec = check_reg(inst,rs_load[i].inst_ptr->ID,rs_load[i].dest_tag);
                if(rs_load[i].state == issued && allow_exec == TRUE){
                    rs_load[i].state = exec;
                }
            }    
        }
        for(i=0;i<NUM_STORE_RS;i++){
            if(rs_store[i].inst_ptr != NULL){
                allow_exec = check_reg(inst,rs_store[i].inst_ptr->ID,rs_store[i].dest_tag);
                reg_index = find_reg_index(rs_store[i].dest_tag);
                if(rs_store[i].state == issued && fp_reg[reg_index].waiting == FALSE){
                    rs_store[i].state = exec;
                }
            }
        }
        for(i=0;i<NUM_ADD_SUB_RS;i++){
            if(rs_add_sub[i].inst_ptr !=  NULL){
                allow_exec = check_reg(inst,rs_add_sub[i].inst_ptr->ID,rs_add_sub[i].dest_tag);
                if(rs_add_sub[i].state == issued && rs_add_sub[i].Qj == VALUE_EMPTY && rs_add_sub[i].Qk == VALUE_EMPTY && allow_exec == TRUE){
                    rs_add_sub[i].state = exec;
                }
            }
        }
        for(i=0;i<NUM_MUL_DIV_RS;i++){
            if(rs_mul_div[i].inst_ptr !=  NULL){
                allow_exec = check_reg(inst,rs_mul_div[i].inst_ptr->ID,rs_mul_div[i].dest_tag);
                if(rs_mul_div[i].state == issued && rs_mul_div[i].Qj == VALUE_EMPTY && rs_mul_div[i].Qk == VALUE_EMPTY && allow_exec == TRUE){
                    rs_mul_div[i].state = exec;
                }
            }
        }

// Check finish
        finish = check_finish(inst,inst_count);
// Dump Cycle and current RS, reg status
        printf("*********************\n");
        printf("Cycle %d\n",cycle_counter);
        printf("Rservation station status\n");

        printf("LOAD |Base|Offset|Cycle last\n");
        for (i=0;i<NUM_LOAD_RS;i++){
            if(rs_load[i].inst_ptr != NULL){
                printf("LOAD%d  %s    %2d      %d\n",i+1,rs_load[i].inst_ptr->src_1,rs_load[i].offset,rs_load[i].cycle_last);
            }
        }
        printf("STORE | Dest |Cycle last\n");
        for (i=0;i<NUM_STORE_RS;i++){
            if(rs_store[i].inst_ptr != NULL){
                printf("STORE%d  %s+%d  %d\n",i+1,rs_store[i].inst_ptr->src_1,rs_store[i].offset,rs_store[i].cycle_last);
            }
        }
        printf("ADD/SUB | Vj | Vk | Qj | Qk | Cycle last\n");
        for (i=0;i<NUM_ADD_SUB_RS;i++){
            if(rs_add_sub[i].inst_ptr != NULL){
                switch(rs_add_sub[i].op){
                    case ADD:
                        printf("ADD%d    ",i+1);
                        break;
                    case SUB:
                        printf("SUB%d    ",i+1);
                        break;
                    default:
                        break;
                }
                printf("|");
                print_reg(rs_add_sub[i].Vj);
                printf("|");
                print_reg(rs_add_sub[i].Vk);
                printf("|");
                print_reg(rs_add_sub[i].Qj);
                printf("|");
                print_reg(rs_add_sub[i].Qk);
                printf("|");
                printf(" %d \n",rs_add_sub[i].cycle_last);
            }
        }
        printf("MUL/DIV | Vj | Vk | Qj | Qk | Cycle last\n");
        for (i=0;i<NUM_MUL_DIV_RS;i++){
            if(rs_mul_div[i].inst_ptr != NULL){
                switch(rs_mul_div[i].op){
                    case MUL:
                        printf("MUL%d    ",i+1);
                        break;
                    case DIV:
                        printf("DIV%d    ",i+1);
                        break;
                    default:
                        break;
                }
                printf("|");
                print_reg(rs_mul_div[i].Vj);
                printf("|");
                print_reg(rs_mul_div[i].Vk);
                printf("|");
                print_reg(rs_mul_div[i].Qj);
                printf("|");
                print_reg(rs_mul_div[i].Qk);
                printf("|");
                printf(" %d \n",rs_mul_div[i].cycle_last);
            }
        }
        printf("Data Register Status\n");
        printf("| F0 | F2 | F4 | F6 | F8 | F10| F12| F14|\n");
        for(i=0;i<8;i++){
            if(fp_reg[i].waiting == TRUE){      // Note that we only print valid bit == TRUE here
                printf("|wait");    
            }
            else{
                printf("|    ");
            }
        }
        printf("|\n");
    }
// Dump instruction table
    dump_inst(inst,inst_count);

    return 0;
}