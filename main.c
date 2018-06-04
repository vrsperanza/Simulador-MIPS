#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>

// ************************** HELPER FUNCTIONS ***************************
void sem_guarantee(sem_t * sem){
	sem_wait(sem);
	sem_post(sem);
}

char pc_write(int UCOut, char ALUZero){
	return (((((UCOut >> 15) & 1) & !ALUZero) | (ALUZero & !((UCOut >> 15) & 1))) & ((UCOut >> 10) & 1)) | ((UCOut >> 11) & 1);
	//return ((UCOut >> 11) & 1) | ( ((UCOut >> 10) & 1) & ( (ALUZero & ((UCOut >> 10) & 1)) | (!ALUZero & !((UCOut >> 10) & 1)) ) );
}

char ir_write(int UCOut){
	return (UCOut >> 16) & 1;
}

// ****************************** SIGN SPLIT *****************************
typedef struct SIGN_SPLIT{
	pthread_t * thread;
	sem_t * begin;
	sem_t * done;
	sem_t * dependency;
	int * input;
	char * output;

	int splitBegin;
	int splitEnd;
	int splitMask;

} SIGN_SPLIT;

void* SIGN_SPLIT_main(void * args){
	SIGN_SPLIT * signals = (SIGN_SPLIT*)args;
	while(1){
		sem_wait(signals->begin);

		sem_guarantee(signals->dependency);

		*(signals->output) = (*(signals->input) >> signals->splitBegin) & signals->splitMask;

		sem_post(signals->done);
	}
}

SIGN_SPLIT * SIGN_SPLIT_init(int * input, char * output, int splitBegin, int splitEnd, sem_t * dependency){
	SIGN_SPLIT * sign = (SIGN_SPLIT*)malloc(sizeof(SIGN_SPLIT));

    sign->thread = (pthread_t*)malloc(sizeof(pthread_t));

	sign->begin = (sem_t*)malloc(sizeof(sem_t));
	sign->done = (sem_t*)malloc(sizeof(sem_t));

	sign->dependency = dependency;

    sem_init(sign->begin, 0, 0);
    sem_init(sign->done, 0, 0);

	sign->input = input;
	sign->output = output;

	sign->splitBegin = splitBegin;
	sign->splitEnd = splitEnd;
	sign->splitMask = 0;
	for(int i = splitBegin; i <= splitEnd; i++)
		sign->splitMask |= 1 << (i-splitBegin);

    pthread_create(sign->thread,NULL,SIGN_SPLIT_main,(void*)sign);

	return sign;
}

// **************************** SIGN BIGSPLIT ****************************
typedef struct SIGN_BIGSPLIT{
	pthread_t * thread;
	sem_t * begin;
	sem_t * done;
	sem_t * dependency;
	int * input;
	int * output;

	int splitBegin;
	int splitEnd;
	int splitMask;

} SIGN_BIGSPLIT;

void* SIGN_BIGSPLIT_main(void * args){
	SIGN_BIGSPLIT * signals = (SIGN_BIGSPLIT*)args;
	while(1){
		sem_wait(signals->begin);

		sem_guarantee(signals->dependency);
		*(signals->output) = (*(signals->input) >> signals->splitBegin) & signals->splitMask;

		sem_post(signals->done);
	}
}

SIGN_BIGSPLIT * SIGN_BIGSPLIT_init(int * input, int * output, int splitBegin, int splitEnd, sem_t * dependency){
	SIGN_BIGSPLIT * sign = (SIGN_BIGSPLIT*)malloc(sizeof(SIGN_BIGSPLIT));

    sign->thread = (pthread_t*)malloc(sizeof(pthread_t));

	sign->begin = (sem_t*)malloc(sizeof(sem_t));
	sign->done = (sem_t*)malloc(sizeof(sem_t));

	sign->dependency = dependency;

    sem_init(sign->begin, 0, 0);
    sem_init(sign->done, 0, 0);

	sign->input = input;
	sign->output = output;

	sign->splitBegin = splitBegin;
	sign->splitEnd = splitEnd;
	sign->splitMask = 0;
	for(int i = splitBegin; i <= splitEnd; i++)
		sign->splitMask |= 1 << (i-splitBegin);

    pthread_create(sign->thread,NULL,SIGN_BIGSPLIT_main,(void*)sign);

	return sign;
}

// ****************************** SIGN EXTEND ******************************
typedef struct SIGN_EXTEND{
	pthread_t * thread;
	sem_t * begin;
	sem_t * done;
	sem_t * dependency;
	int * input;
	int * output;
} SIGN_EXTEND;

void* SIGN_EXTEND_main(void * args){
	SIGN_EXTEND * signals = (SIGN_EXTEND*)args;
	while(1){
		sem_wait(signals->begin);

		sem_guarantee(signals->dependency);

		*(signals->output) = (int)(short)*(signals->input);

		sem_post(signals->done);
	}
}

SIGN_EXTEND * SIGN_EXTEND_init(int * input, int * output, sem_t * dependency){
	SIGN_EXTEND * sign = (SIGN_EXTEND*)malloc(sizeof(SIGN_EXTEND));

    sign->thread = (pthread_t*)malloc(sizeof(pthread_t));

	sign->begin = (sem_t*)malloc(sizeof(sem_t));
	sign->done = (sem_t*)malloc(sizeof(sem_t));

	sign->dependency = dependency;

    sem_init(sign->begin, 0, 0);
    sem_init(sign->done, 0, 0);

	sign->input = input;
	sign->output = output;

    pthread_create(sign->thread,NULL,SIGN_EXTEND_main,(void*)sign);

	return sign;
}

// ****************************** SHIFT LEFT ******************************
typedef struct SHIFT_LEFT{
	pthread_t * thread;
	sem_t * begin;
	sem_t * done;
	sem_t * dependency;
	int * input;
	int * output;
	int amount;
} SHIFT_LEFT;

void* SHIFT_LEFT_main(void * args){
	SHIFT_LEFT * signals = (SHIFT_LEFT*)args;
	while(1){
		sem_wait(signals->begin);

		sem_guarantee(signals->dependency);

		*(signals->output) = *(signals->input) << signals->amount;

		sem_post(signals->done);
	}
}

SHIFT_LEFT * SHIFT_LEFT_init(int * input, int * output, int amount, sem_t * dependency){
	SHIFT_LEFT * sign = (SHIFT_LEFT*)malloc(sizeof(SHIFT_LEFT));

    sign->thread = (pthread_t*)malloc(sizeof(pthread_t));

	sign->begin = (sem_t*)malloc(sizeof(sem_t));
	sign->done = (sem_t*)malloc(sizeof(sem_t));

	sign->dependency = dependency;

    sem_init(sign->begin, 0, 0);
    sem_init(sign->done, 0, 0);

	sign->input = input;
	sign->output = output;

	sign->amount = amount;

    pthread_create(sign->thread,NULL,SHIFT_LEFT_main,(void*)sign);

	return sign;
}

// ****************************** MUX 1 BIT ******************************
typedef struct MUX_1bit{
	pthread_t * thread;
	sem_t * begin;
	sem_t * done;
	sem_t * dependency0;
	sem_t * dependency1;
	sem_t * dependency2;
	char * option;
	int * input0;
	int * input1;
	int * result;
} MUX_1bit;

void* MUX_1bit_main(void * args){
	MUX_1bit * signals = (MUX_1bit*)args;
	while(1){
		sem_wait(signals->begin);

		sem_guarantee(signals->dependency0);
		sem_guarantee(signals->dependency1);
		sem_guarantee(signals->dependency2);

		switch(*(signals->option)){
			case 0:
				*(signals->result) = *(signals->input0);
				break;
			case 1:
				*(signals->result) = *(signals->input1);
				break;
		}

		sem_post(signals->done);
	}
}

MUX_1bit * MUX_1bit_init(char * option, int * result, int * input0, int * input1, sem_t * dependency0, sem_t * dependency1, sem_t * dependency2){
	MUX_1bit * mux = (MUX_1bit*)malloc(sizeof(MUX_1bit));

    mux->thread = (pthread_t*)malloc(sizeof(pthread_t));

	mux->begin = (sem_t*)malloc(sizeof(sem_t));
	mux->done = (sem_t*)malloc(sizeof(sem_t));
	mux->dependency0 = dependency0;
	mux->dependency1 = dependency1;
	mux->dependency2 = dependency2;

    sem_init(mux->begin, 0, 0);
    sem_init(mux->done, 0, 0);

	mux->option = option;
	mux->input0 = input0;
	mux->input1 = input1;
	mux->result = result;

    pthread_create(mux->thread,NULL,MUX_1bit_main,(void*)mux);

	return mux;
}

// ****************************** MUX 2 BITS ******************************
typedef struct MUX_2bits{
	pthread_t * thread;
	sem_t * begin;
	sem_t * done;
	sem_t * dependency0;
	sem_t * dependency1;
	sem_t * dependency2;
	sem_t * dependency3;
	sem_t * dependency4;
	char * option;
	int * input0;
	int * input1;
	int * input2;
	int * input3;
	int * result;
} MUX_2bits;

void* MUX_2bits_main(void * args){
	MUX_2bits * signals = (MUX_2bits*)args;
	while(1){
		sem_wait(signals->begin);

		sem_guarantee(signals->dependency0);
		sem_guarantee(signals->dependency1);
		sem_guarantee(signals->dependency2);
		sem_guarantee(signals->dependency3);
		sem_guarantee(signals->dependency4);

		int op = *(signals->option);

		switch(*(signals->option)){
			case 0:
				*(signals->result) = *(signals->input0);
				break;
			case 1:
				*(signals->result) = *(signals->input1);
				break;
			case 2:
				*(signals->result) = *(signals->input2);
				break;
			case 3:
				*(signals->result) = *(signals->input3);
				break;
		}

		sem_post(signals->done);
	}
}

MUX_2bits * MUX_2bits_init(char * option, int * result, int * input0, int * input1, int * input2, int * input3,
							sem_t * dependency0, sem_t * dependency1, sem_t * dependency2, sem_t * dependency3, sem_t * dependency4){
	MUX_2bits * mux = (MUX_2bits*)malloc(sizeof(MUX_2bits));

    mux->thread = (pthread_t*)malloc(sizeof(pthread_t));

	mux->begin = (sem_t*)malloc(sizeof(sem_t));
	mux->done = (sem_t*)malloc(sizeof(sem_t));

    sem_init(mux->begin, 0, 0);
    sem_init(mux->done, 0, 0);

	mux->dependency0 = dependency0;
	mux->dependency1 = dependency1;
	mux->dependency2 = dependency2;
	mux->dependency3 = dependency3;
	mux->dependency4 = dependency4;

	mux->option = option;
	mux->input0 = input0;
	mux->input1 = input1;
	mux->input2 = input2;
	mux->input3 = input3;
	mux->result = result;

    pthread_create(mux->thread,NULL,MUX_2bits_main,(void*)mux);

	return mux;
}

// ******************************* REGISTER *******************************
typedef struct REGISTER{
	pthread_t * thread;
	sem_t * begin;
	sem_t * done;
	int * input;
	int * output;
} REGISTER;

void* REGISTER_main(void * args){
	REGISTER * signals = (REGISTER*)args;
	while(1){
		sem_wait(signals->begin);

		*(signals->output) = *(signals->input);

		sem_post(signals->done);
	}
}

REGISTER * REGISTER_init(int * input, int * output){
	REGISTER * reg = (REGISTER*)malloc(sizeof(REGISTER));

    reg->thread = (pthread_t*)malloc(sizeof(pthread_t));

	reg->begin = (sem_t*)malloc(sizeof(sem_t));
	reg->done = (sem_t*)malloc(sizeof(sem_t));

    sem_init(reg->begin, 0, 0);
    sem_init(reg->done, 0, 1);

	reg->input = input;
	reg->output = output;

    pthread_create(reg->thread,NULL,REGISTER_main,(void*)reg);

	return reg;
}

// ****************************** REGISTERS *******************************
typedef struct REGISTERS{
	pthread_t * thread;
	sem_t * begin;
	sem_t * done;
	sem_t * dependency0;
	sem_t * dependency1;
	sem_t * dependency2;
	sem_t * dependency3;
	sem_t * dependency4;

	char * regWrite;

	int * readRegister1;
	int * readRegister2;
	int * writeRegister;
	int * writeData;

	int * readData1;
	int * readData2;

	int * registers;
} REGISTERS;

void * REGISTERS_main(void * args){
	REGISTERS * registers = (REGISTERS*)args;
	while(1){
		sem_wait(registers->begin);

		sem_guarantee(registers->dependency0);
		sem_guarantee(registers->dependency1);
		sem_guarantee(registers->dependency2);
		sem_guarantee(registers->dependency3);
		sem_guarantee(registers->dependency4);

		if(*(registers->regWrite)){
			registers->registers[*(registers->writeRegister)] = *(registers->writeData);
		}
		else{
			*(registers->readData1) = registers->registers[*(registers->readRegister1)];
			*(registers->readData2) = registers->registers[*(registers->readRegister2)];
		}

		sem_post(registers->done);
	}
}

REGISTERS * REGISTERS_init(int * readRegister1, int * readRegister2, int * writeRegister,
						   int * writeData, int * readData1, int * readData2, char * regWrite,
						   sem_t * dependency0, sem_t * dependency1, sem_t * dependency2, sem_t * dependency3, sem_t * dependency4){

	REGISTERS * registers = (REGISTERS*)malloc(sizeof(REGISTERS));
	registers->readRegister1 = readRegister1;
	registers->readRegister2 = readRegister2;
	registers->writeRegister = writeRegister;
	registers->writeData = writeData;
	registers->readData1 = readData1;
	registers->readData2 = readData2;
	registers->regWrite = regWrite;
	registers->dependency0 = dependency0;
	registers->dependency1 = dependency1;
	registers->dependency2 = dependency2;
	registers->dependency3 = dependency3;
	registers->dependency4 = dependency4;

	registers->registers = (int*)malloc(sizeof(int) * 32);
	registers->registers[0] = 0;

	registers->begin = (sem_t*)malloc(sizeof(sem_t));
	registers->done = (sem_t*)malloc(sizeof(sem_t));
    sem_init(registers->begin, 0, 0);
    sem_init(registers->done, 0, 1);

	registers->thread = (pthread_t*)malloc(sizeof(pthread_t));
    pthread_create(registers->thread,NULL,REGISTERS_main,(void*)registers);

	return registers;
}

// ********************************* ALU **********************************
typedef struct ALU{
	pthread_t * thread;
	sem_t * begin;
	sem_t * done;
	sem_t * dependency0;
	sem_t * dependency1;
	sem_t * dependency2;

	char * option;
	int * result;
	char * zero;
	int * input0;
	int * input1;
} ALU;

void * ALU_main(void * args){
	ALU * signals = (ALU*)args;
	while(1){
		sem_wait(signals->begin);

		sem_guarantee(signals->dependency0);
		sem_guarantee(signals->dependency1);
		sem_guarantee(signals->dependency2);

		switch(*(signals->option)){
			case 0:
				*(signals->result) = *(signals->input0) & *(signals->input1);
				printf("%d = %d & %d\n", *(signals->result), *(signals->input0), *(signals->input1));
			break;

			case 1:
				*(signals->result) = *(signals->input0) | *(signals->input1);
				printf("%d = %d | %d\n", *(signals->result), *(signals->input0), *(signals->input1));
			break;

			case 2:
				*(signals->result) = *(signals->input0) + *(signals->input1);
				printf("%d = %d + %d\n", *(signals->result), *(signals->input0), *(signals->input1));
			break;

			case 6:
				*(signals->result) = *(signals->input0) - *(signals->input1);
				printf("%d = %d - %d\n", *(signals->result), *(signals->input0), *(signals->input1));
			break;

			case 7:
				*(signals->result) = ( *(signals->input0) < *(signals->input1) );
				printf("%d = %d < %d\n", *(signals->result), *(signals->input0), *(signals->input1));
			break;

			case 12:
				*(signals->result) = !(*(signals->input0) | *(signals->input1));
				printf("%d = !%d | %d\n", *(signals->result), *(signals->input0), *(signals->input1));
			break;
		}

		*(signals->zero) = (*(signals->result) == 0);

		sem_post(signals->done);
	}
}

ALU * ALU_init(char * option, int * result, char * zero, int * input0, int * input1, sem_t * dependency0, sem_t * dependency1, sem_t * dependency2){
	ALU * alu = (ALU*)malloc(sizeof(ALU));

    alu->thread = (pthread_t*)malloc(sizeof(pthread_t));

	alu->begin = (sem_t*)malloc(sizeof(sem_t));
	alu->done = (sem_t*)malloc(sizeof(sem_t));

    sem_init(alu->begin, 0, 0);
    sem_init(alu->done, 0, 0);

	alu->option = option;
	alu->result = result;
	alu->zero = zero;
	alu->input0 = input0;
	alu->input1 = input1;
	alu->dependency0 = dependency0;
	alu->dependency1 = dependency1;
	alu->dependency2 = dependency2;

    pthread_create(alu->thread,NULL,ALU_main,(void*)alu);

	return alu;
}

// ******************************* MEMORY *********************************
typedef struct MEMORY{
	pthread_t * thread;
	sem_t * begin;
	sem_t * done;
	sem_t * dependency0;
	sem_t * dependency1;
	sem_t * dependency2;
	sem_t * dependency3;

	char * memRead;
	char * memWrite;
	int * address;
	int * writeData;
	int * output;
	unsigned char * memory;
} MEMORY;

void * MEMORY_main(void * args){
	MEMORY * memory = (MEMORY*)args;
	while(1){
		sem_wait(memory->begin);

		sem_guarantee(memory->dependency0);
		sem_guarantee(memory->dependency1);
		sem_guarantee(memory->dependency2);
		sem_guarantee(memory->dependency3);

		if(*(memory->memRead)){
			*(memory->output) = memory->memory[*(memory->address) + 3] << 24;
			*(memory->output) |= memory->memory[*(memory->address) + 2] << 16;
			*(memory->output) |= memory->memory[*(memory->address) + 1] << 8;
			*(memory->output) |= memory->memory[*(memory->address)];

			printf("MEM READ %d = %d\n", *(memory->address), *(memory->output));
		}
		else if(*(memory->memWrite)){
			memory->memory[*(memory->address) + 3] = (*(memory->writeData) >> 24) & 0xff;
			memory->memory[*(memory->address) + 2] = (*(memory->writeData) >> 16) & 0xff;
			memory->memory[*(memory->address) + 1] = (*(memory->writeData) >> 8) & 0xff;
			memory->memory[*(memory->address)] = (*(memory->writeData)) & 0xff;

			printf("MEM WRITE %d = %d\n", *(memory->address), *(memory->writeData));
		}

		sem_post(memory->done);
	}
}

MEMORY * MEMORY_init(char * memRead, char * memWrite, int * address, int * writeData,
					 int * output, sem_t * dependency0, sem_t * dependency1, sem_t * dependency2, sem_t * dependency3, int byteCapacity){

	MEMORY * memory = (MEMORY*)malloc(sizeof(MEMORY));
	memory->memRead = memRead;
	memory->memWrite = memWrite;
	memory->address = address;
	memory->writeData = writeData;
	memory->output = output;

	memory->begin = (sem_t*)malloc(sizeof(sem_t));
	memory->done = (sem_t*)malloc(sizeof(sem_t));

    sem_init(memory->begin, 0, 0);
    sem_init(memory->done, 0, 0);

	memory->dependency0 = dependency0;
	memory->dependency1 = dependency1;
	memory->dependency2 = dependency2;
	memory->dependency3 = dependency3;

	memory->memory = (unsigned char*)malloc(sizeof(unsigned char) * byteCapacity);

	memory->thread = (pthread_t*)malloc(sizeof(pthread_t));
    pthread_create(memory->thread,NULL,MEMORY_main,(void*)memory);

	return memory;
}

// ***************************** ALU CONTROL *******************************
typedef struct ALU_CONTROL{
	pthread_t * thread;
	sem_t * begin;
	sem_t * done;
	sem_t * dependency0;
	sem_t * dependency1;
	char * input;
	char * output;
	char * option;
} ALU_CONTROL;

void* ALU_CONTROL_main(void * args){
	ALU_CONTROL * signals = (ALU_CONTROL*)args;
	while(1){
		sem_wait(signals->begin);

		sem_guarantee(signals->dependency0);
		sem_guarantee(signals->dependency1);
		
		char op0 = *(signals->option) & 0b01;
		char op1 = *(signals->option) & 0b10;
		char func = *(signals->input) & 0b001111;
		
        	// Inicializa como 0
		*(signals->output) = 0;
		
		// Primeiro bit
		*(signals->output) |= ( (op1 == 0b10) & ( (func == 0b000101) | (func == 0b001010) ) );

		// Segundo bit
		*(signals->output) |= ( ((op0 == 0b00) & (op1 == 0b00)) | (op0 == 0b01) | ((op1 == 0b10) & 
		((func == 0b000000) | (func == 0b000010) | (func == 0b001010)))) << 1;

		// Terceiro bit
		*(signals->output) |= ( (op0 == 0b01) | ( (op1 == 0b10) & ( (func == 0b000010) | (func == 0b001010)))) << 2;
        

		/*
		if(*(signals->option) == 0) {
			//add
			*(signals->output) = 2;
		} else if(*(signals->option) == 1) {
			//subtract
			*(signals->output) = 6;
		} else {
			//Use function field
			switch (*(signals->input)) {

				case 12:	//AND
					*(signals->output) = 0;
				break;

				case 32:	//add
					*(signals->output) = 2;
				break;

				case 34:	//subtract
					*(signals->output) = 6;
				break;

				case 36:	//AND
					*(signals->output) = 0;
				break;

				case 37:	//OR
					*(signals->output) = 1;
				break;

				case 42:	//set on less than
					*(signals->output) = 7;
				break;
			}
		}
		*/

		printf("ALU_CONTROL: %d %d %d\n", *(signals->option), *(signals->input), *(signals->output));

		sem_post(signals->done);
	}
}

ALU_CONTROL * ALU_CONTROL_init(char * input, char * output, char * option, sem_t * dependency0, sem_t * dependency1){
	ALU_CONTROL * sign = (ALU_CONTROL*)malloc(sizeof(ALU_CONTROL));

    sign->thread = (pthread_t*)malloc(sizeof(pthread_t));

	sign->begin = (sem_t*)malloc(sizeof(sem_t));
	sign->done = (sem_t*)malloc(sizeof(sem_t));

    sem_init(sign->begin, 0, 0);
    sem_init(sign->done, 0, 0);

	sign->input = input;
	sign->output = output;

	sign->option = option;

	sign->dependency0 = dependency0;
	sign->dependency1 = dependency1;

    pthread_create(sign->thread,NULL,ALU_CONTROL_main,(void*)sign);

	return sign;
}

// ******************************* CONTROL *********************************
typedef struct CONTROL{
	pthread_t * thread;
	sem_t * begin;
	sem_t * done;
	char currentState;
	char * option;
	int * output;
} CONTROL;

void* CONTROL_main(void * args){
	CONTROL * signals = (CONTROL*)args;
	while(1){
		sem_wait(signals->begin);

		//"s" e "op" são variáveis utilizadas com o único propósito de facilitar a visualização e, consequentemente, a correção do trabalho
		char s = signals->currentState;
		char op = *(signals->option);
		
		printf("OP IN: %d ", (int)op);
		for(int i = 7; i >= 0; i--)
			printf("%d", (int)(op >> i & 1));
		printf("\n");
		
		printf("STATE IN: %d\n", (int)signals->currentState);
		
		signals->currentState = 0;

		#define	JUMP 0b000010
		#define JR 0b010100
		#define JAL 0b000011
		#define JALR 0b010101
		#define LW 0b100011
		#define SW 0b101011
		#define ADDI 0b001000
		#define RTYPE 0b000000
		#define ANDI 0b001100
		#define BEQ 0b000100
		#define BNE 0b000101

		signals->currentState |= ( (s==0) | ((s==1) & ( (op == JUMP)|(op == JR)|(op==JALR) )) | (s==2) | (s==6) | (s==10) );

		signals->currentState |= ( ( (s==1) & ( (op==LW)|(op==SW)|(op==ADDI)|(op==RTYPE)|(op==ANDI)|(op==JAL)|(op==JALR) ) ) |
															 ( (s==2) & (op!=SW) ) | (s==6) | (s==10)	) << 1;

		signals->currentState |= ( ( (s==1) & ( (op==RTYPE)|(op==BNE)|(op==JR)|(op==JAL)|(op==JALR) ) ) |
			 													 (s==3) | ( (s==2) & (op==SW) ) | (s==6) ) << 2;

		signals->currentState |= ( ( (s==1) & ( (op==BEQ)|(op==JUMP)|(op==ANDI)|(op==BNE)|(op==JR)|(op==JAL)|(op==JALR) ) ) |
	 															((s==2) & (op==ADDI)) | (s==10) ) << 3;
		s = signals->currentState;

		*(signals->output) = 0;
		*(signals->output) |= (s==7);	//regDST0
		*(signals->output) |= (s==14) << 1; //regDST1
		*(signals->output) |= ((s==4) | (s==7) | (s==11) | (s==14) | (s==15)) << 2;	//regWrite
		*(signals->output) |= ((s==2) | (s==6) | (s==8) | (s==10) | (s==12)) << 3;	//ALUSrcA
		*(signals->output) |= ((s==0) | (s==1)) << 4; //ALUSrcB0
		*(signals->output) |= ((s==1) | (s==2) | (s==10)) << 5;	//ALUSrcB1
		*(signals->output) |= ((s==8) | (s==12)) << 6;	//ALUOp0
		*(signals->output) |= ((s==6) | (s==10)) << 7;	//ALUOp1
		*(signals->output) |= ((s==8) | (s==12) | (s==13) | (s==15)) << 8;	//PCSource0
		*(signals->output) |= ((s==9) | (s==13) | (s==14) | (s==15)) << 9;	//PCSource1
		*(signals->output) |= ((s==8) | (s==12)) << 10;	//PCWriteCond
		*(signals->output) |= ((s==0) | (s==9) | (s==13) | (s==14) | (s==15)) << 11;	//PCWrite
		*(signals->output) |= ((s==3) | (s==5)) << 12;	//IorD
		*(signals->output) |= ((s==0) | (s==3)) << 13;	//MemRead
		*(signals->output) |= (s==5) << 14;	//MemWrite
		*(signals->output) |= (s==12) << 15;	//BNE
		*(signals->output) |= (s==0) << 16;	//IRWrite
		*(signals->output) |= (s==4) << 17;	//MemtoReg0
		*(signals->output) |= ((s==14) | (s==15)) << 18;	//MemtoReg1

		printf("OP: %d, STATE: %d\n", (int)op, (int)signals->currentState);

		sem_post(signals->done);
	}
}

CONTROL * CONTROL_init(char * option, int * output){
	CONTROL * sign = (CONTROL*)malloc(sizeof(CONTROL));

    sign->thread = (pthread_t*)malloc(sizeof(pthread_t));

	sign->begin = (sem_t*)malloc(sizeof(sem_t));
	sign->done = (sem_t*)malloc(sizeof(sem_t));

    sem_init(sign->begin, 0, 0);
    sem_init(sign->done, 0, 0);

	sign->output = output;

	sign->option = option;

	sign->currentState = -1;

    pthread_create(sign->thread,NULL,CONTROL_main,(void*)sign);

	return sign;
}

// ******************************* PRINTS **********************************

void print_binary(int val){
	for(int i = 31; i >= 0; i--)
		printf("%d", (val >> i) & 1);
}

void print_state(int pc, int ir, int mdr, int a, int b, int aluOut, int control, REGISTERS * registers, MEMORY * memory){
	printf("PC=%d\tIR=%u\tMDR=%d\n", pc, (unsigned int)ir, mdr);
	printf("A=%d\tB=%d\tAluOut=%d\n", a, b, aluOut);
	printf("Controle=");
	print_binary(control);
	printf("\n\n");
	printf("Banco de registradores\n");
	printf("R00(r0)=%d\tR08(t0)=%d\tR16(s0)=%d\tR24(t8)=%d\n", registers->registers[0], registers->registers[8], registers->registers[16], registers->registers[24]);
	printf("R01(at)=%d\tR09(t1)=%d\tR17(s1)=%d\tR25(t9)=%d\n", registers->registers[1], registers->registers[9], registers->registers[17], registers->registers[25]);
	printf("R02(v0)=%d\tR10(t2)=%d\tR18(s2)=%d\tR26(k0)=%d\n", registers->registers[2], registers->registers[10], registers->registers[18], registers->registers[26]);
	printf("R03(v1)=%d\tR11(t3)=%d\tR19(s3)=%d\tR27(k1)=%d\n", registers->registers[3], registers->registers[11], registers->registers[19], registers->registers[27]);
	printf("R04(a0)=%d\tR12(t4)=%d\tR20(s4)=%d\tR28(gp)=%d\n", registers->registers[4], registers->registers[12], registers->registers[20], registers->registers[28]);
	printf("R05(a1)=%d\tR13(t5)=%d\tR21(s5)=%d\tR29(sp)=%d\n", registers->registers[5], registers->registers[13], registers->registers[21], registers->registers[29]);
	printf("R06(a2)=%d\tR14(t6)=%d\tR22(s6)=%d\tR30(s8)=%d\n", registers->registers[6], registers->registers[14], registers->registers[22], registers->registers[30]);
	printf("R07(a3)=%d\tR15(t7)=%d\tR23(s7)=%d\tR31(ra)=%d\n", registers->registers[7], registers->registers[15], registers->registers[23], registers->registers[31]);

	printf("\n");
	printf("Memoria (enderecos a byte)\n");
	for(int i = 0; i < 32; i++){
		for(int j = 0; j < 4; j++){
			printf("[%02d]=%05d\t", i+32*j, memory->memory[i+32*j]);
		}
		printf("\n");
	}
	printf("*********************************************************************************\n\n");
}

// ************************** LOAD INSTRUCTIONS ****************************

void load_instructions(unsigned char * memory){
	unsigned int c;
	FILE *file;
	int currPos = 0;
	file = fopen("code.bin", "r");
	while(fscanf(file, "%u", &c) != EOF){
		memory[currPos+3] = (c >> 24) & 0xFF;
		memory[currPos+2] = (c >> 16) & 0xFF;
		memory[currPos+1] = (c >> 8) & 0xFF;
		memory[currPos] = c & 0xFF;

		printf("Load %d: %d | %d %d %d %d\n", currPos, c, memory[currPos], memory[currPos+1], memory[currPos+2], memory[currPos+3]);

		currPos += 4;
	}
	fclose(file);

	memory[currPos+3] = -1;
	memory[currPos+2] = -1;
	memory[currPos+1] = -1;
	memory[currPos] = -1;

	return;
}

// ******************************** MAIN **********************************
int main()
{
	// Declare signals
	int i0 = 0;
	int i4 = 4;
	int i31 = 31;
	sem_t sem_done;
	sem_init(&sem_done, 0, 1);

	int PCSource = 0;
	int PCOut = 0;

	int UCOut = 0;

	int IROut = 0;

	int MEMOut = 0;
	int MEMOutReg = 0;

	char IROut_26_31 = 0;
	char IROut_0_5 = 0;

	int IROut_0_15 = 0;
	int IROut_0_25 = 0;
	int IROut_11_15 = 0;
	int IROut_16_20 = 0;
	int IROut_21_25 = 0;

	char UCOut_IorD = 0;
	char UCOut_regWrite = 0;
	char UCOut_ALUSrcA = 0;
	char UCOut_ALUSrcB = 0;
	char UCOut_ALUop = 0;
	char UCOut_PCSrc = 0;
	char UCOut_memRead = 0;
	char UCOut_memWrite = 0;
	char UCOut_regDst = 0;
	char UCOut_memToReg = 0;

	int ALUOut = 0;
	char ALUOutZero = 0;
	int ALURegOut = 0;

	int MEMAddress = 0;

	int REGISTERSWriteRegister = 0;
	int REGISTERSWriteData = 0;

	int REGISTERSOutA = 0;
	int REGISTERSOutB = 0;

	int RegAOut = 0;
	int RegBOut = 0;

	int signExtendOut = 0;
	int toALUShiftLeftOut = 0;
	int toPCShiftLeftOut = 0;

	int ALUMuxAOut = 0;
	int ALUMuxBOut = 0;

	char ALUControlOut = 0;

	// Initiate blocks
	REGISTER * PC = REGISTER_init(&PCSource, &PCOut);
	REGISTER * IR = REGISTER_init(&MEMOut, &IROut);
	REGISTER * ALUOutReg = REGISTER_init(&ALUOut, &ALURegOut);

	REGISTER * RegA = REGISTER_init(&REGISTERSOutA, &RegAOut);
	REGISTER * RegB	= REGISTER_init(&REGISTERSOutB, &RegBOut);

	REGISTER * MEMDataReg = REGISTER_init(&MEMOut, &MEMOutReg);

	CONTROL * UC = CONTROL_init(&IROut_26_31, &UCOut);

	SIGN_SPLIT * IRSplit_26_31 = SIGN_SPLIT_init(&IROut, &IROut_26_31, 26, 31, IR->done);
	SIGN_SPLIT * IRSplit_0_5 = SIGN_SPLIT_init(&IROut, &IROut_0_5, 0, 5, IR->done);

	SIGN_BIGSPLIT * IRSplit_0_15 = SIGN_BIGSPLIT_init(&IROut, &IROut_0_15, 0, 15, IR->done);
	SIGN_BIGSPLIT * IRSplit_0_25 = SIGN_BIGSPLIT_init(&IROut, &IROut_0_25, 0, 25, IR->done);
	SIGN_BIGSPLIT * IRSplit_11_15 = SIGN_BIGSPLIT_init(&IROut, &IROut_11_15, 11, 15, IR->done);
	SIGN_BIGSPLIT * IRSplit_16_20 = SIGN_BIGSPLIT_init(&IROut, &IROut_16_20, 16, 20, IR->done);
	SIGN_BIGSPLIT * IRSplit_21_25 = SIGN_BIGSPLIT_init(&IROut, &IROut_21_25, 21, 25, IR->done);

	SIGN_SPLIT * UCOutSplit_regDst = SIGN_SPLIT_init(&UCOut, &UCOut_regDst, 0, 1, UC->done);
	SIGN_SPLIT * UCOutSplit_regWrite = SIGN_SPLIT_init(&UCOut, &UCOut_regWrite, 2, 2, UC->done);
	SIGN_SPLIT * UCOutSplit_ALUSrcA = SIGN_SPLIT_init(&UCOut, &UCOut_ALUSrcA, 3, 3, UC->done);
	SIGN_SPLIT * UCOutSplit_ALUSrcB = SIGN_SPLIT_init(&UCOut, &UCOut_ALUSrcB, 4, 5, UC->done);
	SIGN_SPLIT * UCOutSplit_ALUop = SIGN_SPLIT_init(&UCOut, &UCOut_ALUop, 6, 7, UC->done);
	SIGN_SPLIT * UCOutSplit_PCSrc = SIGN_SPLIT_init(&UCOut, &UCOut_PCSrc, 8, 9, UC->done);
	SIGN_SPLIT * UCOutSplit_IorD = SIGN_SPLIT_init(&UCOut, &UCOut_IorD, 12, 12, UC->done);
	SIGN_SPLIT * UCOutSplit_memRead = SIGN_SPLIT_init(&UCOut, &UCOut_memRead, 13, 13, UC->done);
	SIGN_SPLIT * UCOutSplit_memWrite = SIGN_SPLIT_init(&UCOut, &UCOut_memWrite, 14, 14, UC->done);
	SIGN_SPLIT * UCOutSplit_memToReg = SIGN_SPLIT_init(&UCOut, &UCOut_memToReg, 17, 18, UC->done);


	MUX_1bit * memoryAddressMux = MUX_1bit_init(&UCOut_IorD, &MEMAddress, &PCOut, &ALURegOut, PC->done, ALUOutReg->done, UCOutSplit_IorD->done);


	MEMORY * memory = MEMORY_init(&UCOut_memRead, &UCOut_memWrite, &MEMAddress, &RegBOut, &MEMOut,
									memoryAddressMux->done, RegB->done, UCOutSplit_memWrite->done, UCOutSplit_memRead->done, 128);


	MUX_2bits * registersWriteRegisterMux = MUX_2bits_init(&UCOut_regDst, &REGISTERSWriteRegister, &IROut_16_20, &IROut_11_15, &i31, &i0,
														   IRSplit_16_20->done, IRSplit_11_15->done, &sem_done, &sem_done, UCOutSplit_regDst->done);


	MUX_2bits * registersWriteDataMux = MUX_2bits_init(&UCOut_memToReg, &REGISTERSWriteData, &ALURegOut, &MEMOutReg, &PCOut, &i0,
														   ALUOutReg->done, MEMDataReg->done, PC->done, &sem_done, UCOutSplit_memToReg->done);

	SIGN_EXTEND * signExtend = SIGN_EXTEND_init(&IROut_0_15, &signExtendOut, IRSplit_0_15->done);
	SHIFT_LEFT * toALUShiftLeft = SHIFT_LEFT_init(&signExtendOut, &toALUShiftLeftOut, 2, signExtend->done);
	SHIFT_LEFT * toPCShiftLeft = SHIFT_LEFT_init(&IROut_0_25, &toPCShiftLeftOut, 2, IRSplit_0_25->done);

	MUX_1bit * ALUMuxA = MUX_1bit_init(&UCOut_ALUSrcA, &ALUMuxAOut, &PCOut, &RegAOut, PC->done, RegA->done, UCOutSplit_ALUSrcA->done);
	MUX_2bits * ALUMuxB = MUX_2bits_init(&UCOut_ALUSrcB, &ALUMuxBOut, &RegBOut, &i4, &signExtendOut, &toALUShiftLeftOut,
							RegB->done, &sem_done, signExtend->done, toALUShiftLeft->done, UCOutSplit_ALUSrcB->done);

	ALU_CONTROL * ALUControl = ALU_CONTROL_init(&IROut_0_5, &ALUControlOut, &UCOut_ALUop, IRSplit_0_15->done, UCOutSplit_ALUop->done);
	ALU * ALU = ALU_init(&ALUControlOut, &ALUOut, &ALUOutZero, &ALUMuxAOut, &ALUMuxBOut, ALUMuxA->done, ALUMuxB->done, ALUControl->done);

	MUX_2bits * toPCMux = MUX_2bits_init(&UCOut_PCSrc, &PCSource, &ALUOut, &ALURegOut, &toPCShiftLeftOut, &RegAOut,
											ALU->done, ALUOutReg->done, toPCShiftLeft->done, UCOutSplit_PCSrc->done, RegA->done);

	REGISTERS * registers = REGISTERS_init(&IROut_21_25, &IROut_16_20, &REGISTERSWriteRegister, &REGISTERSWriteData,
											&REGISTERSOutA, &REGISTERSOutB, &UCOut_regWrite,
											IRSplit_21_25->done, IRSplit_16_20->done, registersWriteRegisterMux->done, registersWriteDataMux->done, UCOutSplit_regWrite->done);


	// Load instructions
	load_instructions(memory->memory);

	int oldA2 = registers->registers[6];

	// Execution loop
	while(1){
		print_state(PCOut, IROut, MEMOutReg, RegAOut, RegBOut, ALUOut, UCOut, registers, memory);
		

		// Compute logic
		sem_post(UC->begin);
		sem_post(IRSplit_26_31->begin);
		sem_post(IRSplit_0_5->begin);
		sem_post(IRSplit_0_15->begin);
		sem_post(IRSplit_0_25->begin);
		sem_post(IRSplit_11_15->begin);
		sem_post(IRSplit_16_20->begin);
		sem_post(IRSplit_21_25->begin);
		sem_post(UCOutSplit_regDst->begin);
		sem_post(UCOutSplit_regWrite->begin);
		sem_post(UCOutSplit_ALUSrcA->begin);
		sem_post(UCOutSplit_ALUSrcB->begin);
		sem_post(UCOutSplit_ALUop->begin);
		sem_post(UCOutSplit_PCSrc->begin);
		sem_post(UCOutSplit_IorD->begin);
		sem_post(UCOutSplit_memRead->begin);
		sem_post(UCOutSplit_memWrite->begin);
		sem_post(UCOutSplit_memToReg->begin);
		sem_post(memoryAddressMux->begin);
		sem_post(memory->begin);
		sem_post(registersWriteRegisterMux->begin);
		sem_post(registersWriteDataMux->begin);
		sem_post(signExtend->begin);
		sem_post(toALUShiftLeft->begin);
		sem_post(toPCShiftLeft->begin);
		sem_post(ALUMuxA->begin);
		sem_post(ALUMuxB->begin);
		sem_post(ALUControl->begin);
		sem_post(ALU->begin);
		sem_post(toPCMux->begin);
		sem_post(registers->begin);

		sem_guarantee(UC->done);
		sem_guarantee(IRSplit_26_31->done);
		sem_guarantee(IRSplit_0_5->done);
		sem_guarantee(IRSplit_0_15->done);
		sem_guarantee(IRSplit_0_25->done);
		sem_guarantee(IRSplit_11_15->done);
		sem_guarantee(IRSplit_16_20->done);
		sem_guarantee(IRSplit_21_25->done);
		sem_guarantee(UCOutSplit_regDst->done);
		sem_guarantee(UCOutSplit_regWrite->done);
		sem_guarantee(UCOutSplit_ALUSrcA->done);
		sem_guarantee(UCOutSplit_ALUSrcB->done);
		sem_guarantee(UCOutSplit_ALUop->done);
		sem_guarantee(UCOutSplit_PCSrc->done);
		sem_guarantee(UCOutSplit_IorD->done);
		sem_guarantee(UCOutSplit_memRead->done);
		sem_guarantee(UCOutSplit_memWrite->done);
		sem_guarantee(UCOutSplit_memToReg->done);
		sem_guarantee(memoryAddressMux->done);
		sem_guarantee(memory->done);
		sem_guarantee(registersWriteRegisterMux->done);
		sem_guarantee(registersWriteDataMux->done);
		sem_guarantee(signExtend->done);
		sem_guarantee(toALUShiftLeft->done);
		sem_guarantee(toPCShiftLeft->done);
		sem_guarantee(ALUMuxA->done);
		sem_guarantee(ALUMuxB->done);
		sem_guarantee(ALUControl->done);
		sem_guarantee(ALU->done);
		sem_guarantee(toPCMux->done);
		sem_guarantee(registers->done);

		
		if(IROut == -1)
			break;
		
		// Write registers
		if(pc_write(UCOut, ALUOutZero)){
			sem_wait(PC->done);
			sem_post(PC->begin);
		}
		if(ir_write(UCOut)){
			sem_wait(IR->done);
			sem_post(IR->begin);
		}

		sem_wait(ALUOutReg->done);
		sem_wait(RegA->done);
		sem_wait(RegB->done);
		sem_wait(MEMDataReg->done);

		sem_post(ALUOutReg->begin);
		sem_post(RegA->begin);
		sem_post(RegB->begin);
		sem_post(MEMDataReg->begin);

		sem_guarantee(ALUOutReg->done);
		sem_guarantee(RegA->done);
		sem_guarantee(RegB->done);
		sem_guarantee(MEMDataReg->done);

		sem_guarantee(PC->done);
		sem_guarantee(IR->done);
		
		// Reset completion status
		sem_wait(UC->done);
		sem_wait(IRSplit_26_31->done);
		sem_wait(IRSplit_0_5->done);
		sem_wait(IRSplit_0_15->done);
		sem_wait(IRSplit_0_25->done);
		sem_wait(IRSplit_11_15->done);
		sem_wait(IRSplit_16_20->done);
		sem_wait(IRSplit_21_25->done);
		sem_wait(UCOutSplit_regDst->done);
		sem_wait(UCOutSplit_regWrite->done);
		sem_wait(UCOutSplit_ALUSrcA->done);
		sem_wait(UCOutSplit_ALUSrcB->done);
		sem_wait(UCOutSplit_ALUop->done);
		sem_wait(UCOutSplit_PCSrc->done);
		sem_wait(UCOutSplit_IorD->done);
		sem_wait(UCOutSplit_memRead->done);
		sem_wait(UCOutSplit_memWrite->done);
		sem_wait(UCOutSplit_memToReg->done);
		sem_wait(memoryAddressMux->done);
		sem_wait(memory->done);
		sem_wait(registersWriteRegisterMux->done);
		sem_wait(registersWriteDataMux->done);
		sem_wait(signExtend->done);
		sem_wait(toALUShiftLeft->done);
		sem_wait(toPCShiftLeft->done);
		sem_wait(ALUMuxA->done);
		sem_wait(ALUMuxB->done);
		sem_wait(ALUControl->done);
		sem_wait(ALU->done);
		sem_wait(toPCMux->done);
		sem_wait(registers->done);
	}

    return 0;
}
