#include "tomasulo.h"

int read_inst(INST_REG* inst){
    char type[5],test[5]; 
    char dest[5];
    int offset;
    char source_1[5];
    char source_2[5];
    const char *delimiter = "()";  //Set delimiter for input string
    char *p;
    int len;            
    int inst_count = 0;

    // Read instruction from file
    while(scanf("%s",test) != EOF && inst_count < 15){
        strcpy(type,test);
        scanf(" %s",dest);

        //store type and destination to INST 
        if(strcmp(type,"LW")==0){
            inst[inst_count].type = LW;
        }
        else if(strcmp(type,"SW")==0){
            inst[inst_count].type = SW;
        }
        else if(strcmp(type,"MUL")==0){
            inst[inst_count].type = MUL;
        }    
        else if(strcmp(type,"ADD")==0){
            inst[inst_count].type = ADD;
        }            
        else if(strcmp(type,"SUB")==0){
            inst[inst_count].type = SUB; 
        }
        else{
            inst[inst_count].type = DIV;
        }
        strcpy(inst[inst_count].dest,dest);
        inst[inst_count].dest_tag = reg_to_reg_tag(dest);
        if((strcmp(type,"LW") == 0) || (strcmp(type,"SW") == 0)){   //read I-type instruction
            scanf("%d %s",&offset,source_1);
            len = strlen(source_1);
            p = strtok(source_1, delimiter);
            memcpy(source_1,p,(len-1));        
            p = strtok(NULL, delimiter);        //reset pointer
            inst[inst_count].offset = offset;
            strcpy(inst[inst_count].src_1,source_1);
            inst[inst_count].src_1_tag = reg_to_reg_tag(source_1);
        }
        else{                                                       //read R-type instruction
            scanf("%s %s",source_1,source_2);            
            strcpy(inst[inst_count].src_1,source_1);
            strcpy(inst[inst_count].src_2,source_2);
            inst[inst_count].src_1_tag = reg_to_reg_tag(source_1);
            inst[inst_count].src_2_tag = reg_to_reg_tag(source_2);
        }
        inst[inst_count].ID = inst_count;         // Set identifier 
        inst_count++;
    }
    return inst_count;
}

void dump_inst(INST_REG* inst,int inst_count){

    printf("  Instruction  |Is|Ex|Wb|Cm\n");
    for(int i=0;i<inst_count;i++){
        switch(inst[i].type){
            case LW:
                printf("LW  %3s %3d %3s",inst[i].dest,inst[i].offset,inst[i].src_1);
                printf(" %2d %2d %2d %2d\n",inst[i].issue,inst[i].execute,inst[i].writeback,inst[i].commit);
                break;
            case SW:
                printf("SW  %3s %3d %3s",inst[i].dest,inst[i].offset,inst[i].src_1);
                printf(" %2d %2d %2d %2d\n",inst[i].issue,inst[i].execute,inst[i].writeback,inst[i].commit);
                break;
            case ADD:
                printf("ADD %3s %3s %3s",inst[i].dest,inst[i].src_1,inst[i].src_2);
                printf(" %2d %2d %2d %2d\n",inst[i].issue,inst[i].execute,inst[i].writeback,inst[i].commit);
                break;
            case MUL:
                printf("MUL %3s %3s %3s",inst[i].dest,inst[i].src_1,inst[i].src_2);
                printf(" %2d %2d %2d %2d\n",inst[i].issue,inst[i].execute,inst[i].writeback,inst[i].commit);
                break;
            case SUB:
                printf("SUB %3s %3s %3s",inst[i].dest,inst[i].src_1,inst[i].src_2);
                printf(" %2d %2d %2d %2d\n",inst[i].issue,inst[i].execute,inst[i].writeback,inst[i].commit);
                break;
            case DIV:
                printf("DIV %3s %3s %3s",inst[i].dest,inst[i].src_1,inst[i].src_2);
                printf(" %2d %2d %2d %2d\n",inst[i].issue,inst[i].execute,inst[i].writeback,inst[i].commit);
                break;
            default:
                break;
        }
        
    }
}

void rs_ls_init(RS_LS* load){
	int i;
	for(i=0;i<NUM_LOAD_RS;i++){
		load[i].inst_ptr = NULL;
        load[i].op = NOP;
        load[i].base_tag = NO_REG;
        load[i].dest_tag = NO_REG;
        load[i].offset = VALUE_EMPTY;
        load[i].cycle_last = VALUE_EMPTY;
        load[i].state = unused;
	}
}

void rs_add_init(RS* add){
	int i;
	for(i=0;i<NUM_ADD_SUB_RS;i++){
		add[i].inst_ptr = NULL;
        add[i].op = NOP;
        add[i].dest_tag = NO_REG;
        add[i].Vj = VALUE_EMPTY;
        add[i].Vk = VALUE_EMPTY;
        add[i].Qj = VALUE_EMPTY;
        add[i].Qk = VALUE_EMPTY;
        add[i].cycle_last = VALUE_EMPTY;
        add[i].state = unused;
	}
}

void rs_mul_init(RS* mul){
	int i;
	for(i=0;i<NUM_MUL_DIV_RS;i++){
		mul[i].inst_ptr = NULL;
        mul[i].op = NOP;
        mul[i].dest_tag = NO_REG;
        mul[i].Vj = VALUE_EMPTY;
        mul[i].Vk = VALUE_EMPTY;
        mul[i].Qj = VALUE_EMPTY;
        mul[i].Qk = VALUE_EMPTY;
        mul[i].cycle_last = VALUE_EMPTY;
        mul[i].state = unused;
	}
}
// Set LOAD reservation for its instruction and register field
void set_idle_load(RS_LS* load, INST_REG* inst, DATA_REG* data,int cycle_count,int RS_index){
	int reg_index;

    // Set inst reg
    inst->issue = cycle_count;
    load->state = issued;
    // Set reservation station
	load->inst_ptr = inst;
    load->op = inst->type;
	load->base_tag = inst->src_1_tag;
    load->dest_tag = inst->dest_tag;
	load->offset = inst->offset;
	load->cycle_last = LW_CYCLE;
    // Set data register
    reg_index = find_reg_index(load->dest_tag);
    data[reg_index].valid = TRUE;
    data[reg_index].waiting = TRUE;
    data[reg_index].inst_ptr = inst;
    data[reg_index].value = LOAD_OFFSET + RS_index;
}

void set_idle_store(RS_LS* store, INST_REG* inst,int cycle_count){

    // Set inst reg
    inst->issue = cycle_count;
    store->state = issued;
	store->inst_ptr = inst;
    store->op = inst->type;
	store->base_tag = inst->src_1_tag;
    store->dest_tag = inst->dest_tag;
	store->offset = inst->offset;
	store->cycle_last = SW_CYCLE;
}

// Set ADD/SUB reservation for its instruction and register field
void set_idle_add(RS* add, INST_REG* inst, DATA_REG* data,int cycle_count,int RS_index){
	int reg_index;

    // Set inst reg
    inst->issue = cycle_count;

    add->state = issued;
	add->inst_ptr = inst;
    add->op = inst->type;
    add->dest_tag = inst->dest_tag;    
	add->cycle_last = ADD_SUB_CYCLE;

	// Find target register
	int src_index;
    src_index = find_reg_index(inst->src_1_tag);
    // Set Qj or Vj
	if(data[src_index].waiting == TRUE){
		add->Qj = src_index;
		add->Vj = VALUE_EMPTY;
	}
	else {
		add->Qj = VALUE_EMPTY;
		add->Vj = src_index;
	}
	// Set Qk or Vk
    src_index = find_reg_index(inst->src_2_tag);
	if(data[src_index].waiting == TRUE){
		add->Qk = src_index;
		add->Vk = VALUE_EMPTY;
	}
	else {
		add->Qk = VALUE_EMPTY;
		add->Vk = src_index;
	}
    // Set ready bit
    if(add->Qj == VALUE_EMPTY && add->Qk == VALUE_EMPTY){
        add->state = exec;
    }
    else{
        add->state = issued;
    }

    reg_index = find_reg_index(add->inst_ptr->dest_tag);
    data[reg_index].valid = TRUE;
    data[reg_index].waiting = TRUE;        //when the data reg is used as a source reg, set waiting bit FALSE
    data[reg_index].inst_ptr = inst;
    data[reg_index].value = ADD_OFFSET + RS_index;
}

// Set MULT/DIV reservation for its instruction and register field
void set_idle_mult(RS* mult, INST_REG* inst, DATA_REG* data,int cycle_count,int RS_index){
    int reg_index;
    
    // Set inst reg
    inst->issue = cycle_count;

    mult->state = issued;
	mult->inst_ptr = inst;
    mult->op = inst->type;
    mult->dest_tag = inst->dest_tag;
	switch(mult->op){
		case MUL: mult->cycle_last = MUL_CYCLE; break;
		case DIV:  mult->cycle_last = DIV_CYCLE; break;
        default: break;
	}
	// Find target register
	int src_index;
    src_index = find_reg_index(inst->src_1_tag);
    // Set Qj or Vj
	if(data[src_index].waiting == TRUE){
		mult->Qj = src_index;
		mult->Vj = VALUE_EMPTY;
	}
	else {
		mult->Qj = VALUE_EMPTY;
		mult->Vj = src_index;
	}
	// Set Qk or Vk
    src_index = find_reg_index(inst->src_2_tag);
	if(data[src_index].waiting == TRUE){
		mult->Qk = src_index;
		mult->Vk = VALUE_EMPTY;
	}
	else {
		mult->Qk = VALUE_EMPTY;
		mult->Vk = src_index;
	}
    // Set ready bit
    if(mult->Qj == VALUE_EMPTY && mult->Qk == VALUE_EMPTY){
        mult->state = exec;
    }
    else{
        mult->state = issued;
    }
    // Set dest reg
    reg_index = find_reg_index(mult->inst_ptr->dest_tag);
    data[reg_index].valid = TRUE;
    data[reg_index].waiting = TRUE;        //when the data reg is used as a source reg, set waiting bit FALSE
    data[reg_index].inst_ptr = inst;
    data[reg_index].value = MULT_OFFSET + RS_index;
}

int find_reg_index(DATA_REG_TAG data){
    switch(data){
        case F0:
            return 0;
        case F2:
            return 1;
        case F4:
            return 2;
        case F6: 
            return 3;
        case F8:
            return 4;
        case F10:
            return 5;
        case F12:
            return 6;
        case F14: 
            return 7; 
        default:
            return VALUE_EMPTY;  
    }
}

void reg_init(DATA_REG* reg){
    int i;
    for(i=0;i<8;i++){
        reg[i].valid = FALSE;
        reg[i].waiting = FALSE;
        reg[i].inst_ptr = NULL; 
        reg[i].value = VALUE_EMPTY; 
    }
}

void inst_reg_init(INST_REG* inst){
    int i;
    for(i=0;i<MAX_INST_NUM;i++){
        inst[i].issue = VALUE_EMPTY;
        inst[i].execute = VALUE_EMPTY;
        inst[i].writeback = VALUE_EMPTY;
        inst[i].commit = VALUE_EMPTY;
        inst[i].type = NOP;
        inst[i].offset = VALUE_EMPTY;
        inst[i].ID = VALUE_EMPTY;
    }
}

DATA_REG_TAG reg_to_reg_tag(char input[]){
    
    if(strcmp(input,"F0") == 0){
        return F0;
    }
    else if(strcmp(input,"F2") == 0){
        return F2;
    }
    else if(strcmp(input,"F4") == 0){
        return F4;
    }
    else if(strcmp(input,"F6") == 0){
        return F6;
    }
    else if(strcmp(input,"F8") == 0){
        return F8;
    }
    else if(strcmp(input,"F10") == 0){
        return F10;
    }
    else if(strcmp(input,"F12") == 0){
        return F12;
    }
    else{
        return F14;
    }
}

// Find idle LOAD reservation station
int find_idle_load(RS_LS* rs_load){
	int i=0;
	for(i=0;i<NUM_LOAD_RS;i++){
        if(rs_load[i].state == unused){ 
            return i; 
        }    
    }
	return RS_NOT_FOUND;
}

// Find idle STORE reservation station
int find_idle_store(RS_LS* rs_store){
	int i=0;
	for(i=0;i<NUM_STORE_RS;i++){
        if(rs_store[i].state == unused){
            return i; 
        }    
    }
	return RS_NOT_FOUND;
}

// Find idle ADD/SUB reservation station
int find_idle_add(RS* rs_add_sub){
	int i=0;
	for(i=0;i<NUM_ADD_SUB_RS;i++){
        if(rs_add_sub[i].state == unused){
            return i;
        } 
    }
	return RS_NOT_FOUND;
}

// Find idle MULT/DIV reservation station
int find_idle_mult(RS* rs_mul_div){
	int i=0;
	for(i=0;i<NUM_MUL_DIV_RS;i++){ 
        if(rs_mul_div[i].state == unused){ 
            return i; 
        }
    }
	return RS_NOT_FOUND;
}

bool check_reg(const INST_REG* inst,int inst_count,DATA_REG_TAG tag){
    int i;

    for(i=0;i<inst_count;i++){
        if(inst[i].commit == VALUE_EMPTY && inst[i].dest_tag == tag){
            return FALSE;
        }
    }
    return TRUE;
}

bool check_finish(INST_REG* inst, int inst_count){
    int i;
    for(i=0;i<inst_count;i++){
        if(inst[i].commit == VALUE_EMPTY){
            return FALSE;
        }
    }
    return TRUE;
}

void writeback(RS_LS* rs_load, RS_LS* rs_store, RS* rs_add_sub, RS* rs_mul_div, DATA_REG* reg,const int cycle_counter){
    int i;
    int reg_index;
    for(i=0;i<NUM_LOAD_RS;i++){
        if(rs_load[i].state == done){
            // set writeback cycle 
            reg_index = find_reg_index(rs_load[i].dest_tag);
            rs_load[i].inst_ptr->writeback = cycle_counter;
            reg[reg_index].value = MEM_OFFSET + rs_load[i].inst_ptr->ID;
            update_RS(rs_add_sub,rs_mul_div,reg_index);
            reset_RS_LS(&rs_load[i]);           // clean the RS entry
            reset_REG(&reg[reg_index]);
        }
    }
    for(i=0;i<NUM_STORE_RS;i++){
        if(rs_store[i].state == done){
            reg_index = find_reg_index(rs_store[i].dest_tag);
            rs_store[i].inst_ptr->writeback = cycle_counter;
            reset_RS_LS(&rs_store[i]);
            reset_REG(&reg[reg_index]);
        }
    }
    for(i=0;i<NUM_ADD_SUB_RS;i++){
        if(rs_add_sub[i].state == done){
            reg_index = find_reg_index(rs_add_sub[i].dest_tag);
            rs_add_sub[i].inst_ptr->writeback = cycle_counter;
            reg[reg_index].value = MEM_OFFSET + rs_add_sub[i].inst_ptr->ID;
            update_RS(rs_add_sub,rs_mul_div,reg_index);
            reset_RS(&rs_add_sub[i]);
            reset_REG(&reg[reg_index]);
        }
    }
    for(i=0;i<NUM_MUL_DIV_RS;i++){
        if(rs_mul_div[i].state == done){
            reg_index = find_reg_index(rs_mul_div[i].dest_tag);
            rs_mul_div[i].inst_ptr->writeback = cycle_counter;
            reg[reg_index].value = MEM_OFFSET + rs_mul_div[i].inst_ptr->ID;
            update_RS(rs_add_sub,rs_mul_div,reg_index);
            reset_RS(&rs_mul_div[i]);
            reset_REG(&reg[reg_index]);
        }
    }
}

void commit(INST_REG* inst,const int cycle_count){
    bool check;
    int i;
    static int commit_count = 0;

    if(commit_count==0){
        check = TRUE;
    }
    else{
        check = FALSE;
    }
    for(i=0;i<commit_count;i++){
        if(inst[i].writeback != VALUE_EMPTY && inst[i].commit != VALUE_EMPTY){
            check = TRUE;
        }
        else{
            check = FALSE;
        }
    }
    if(inst[commit_count].writeback!=VALUE_EMPTY&&inst[commit_count].commit==VALUE_EMPTY&&check==TRUE){
        inst[commit_count].commit = cycle_count;
        commit_count++;
    }
}

void update_RS(RS* rs_add_sub,RS* rs_mul_div, int reg_index){
    int i;
    for(i=0;i<NUM_ADD_SUB_RS;i++){
        if(rs_add_sub[i].state == issued){
            if(rs_add_sub[i].Qj == reg_index){
                rs_add_sub[i].Vj = reg_index;
                rs_add_sub[i].Qj = VALUE_EMPTY;
            }
            if(rs_add_sub[i].Qk == reg_index){
                rs_add_sub[i].Vk = reg_index;
                rs_add_sub[i].Qk = VALUE_EMPTY;
            }
        }    
    }
    for(i=0;i<NUM_MUL_DIV_RS;i++){
        if(rs_mul_div[i].state == issued){
            if(rs_mul_div[i].Qj == reg_index){
                rs_mul_div[i].Vj = reg_index;
                rs_mul_div[i].Qj = VALUE_EMPTY;
            }
            if(rs_mul_div[i].Qk == reg_index){
                rs_mul_div[i].Vk = reg_index;
                rs_mul_div[i].Qk = VALUE_EMPTY;
            }
        }
    }
}

void reset_RS_LS(RS_LS* rs){
    rs->inst_ptr = NULL;
    rs->op = NOP;
    rs->base_tag = NO_REG;
    rs->dest_tag = NO_REG;
    rs->offset = VALUE_EMPTY;
    rs->cycle_last = VALUE_EMPTY;
    rs->state = unused;
}

void reset_RS(RS* rs){
    rs->inst_ptr = NULL;
    rs->op = NOP;
    rs->dest_tag = NO_REG;
    rs->Vj = VALUE_EMPTY;
    rs->Vk = VALUE_EMPTY;
    rs->Qj = VALUE_EMPTY;
    rs->Qk = VALUE_EMPTY;
    rs->cycle_last = VALUE_EMPTY;
    rs->state = unused;
}

void reset_REG(DATA_REG* reg){
    reg->valid = FALSE;
    reg->waiting = FALSE;
    reg->inst_ptr = NULL;
}

void print_reg(int reg_index){
    switch(reg_index){
        case 0:
            printf(" F0 ");
            break;
        case 1:
            printf(" F2 ");
            break;
        case 2:
            printf(" F4 ");
            break;
        case 3:
            printf(" F6 ");
            break;
        case 4:
            printf(" F8 ");
            break;
        case 5:
            printf(" F10");
            break;
        case 6:
            printf(" F12");
            break;
        case 7:
            printf(" F14");
            break;
        default:
            printf("XXXX");
            break;
    }
}