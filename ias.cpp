#include<bits/stdc++.h>
#include<unordered_map>
#include<string>

using namespace std;

unordered_map<int, string> M;

// AC holds the results of ALU operation
// the size of AC is 40 bits
string AC="0000000000000000000000000000000000000000";

// size of MQ is 40 bits
string MQ="0000000000000000000000000000000000000000";

string decTobin(int x, int n)
{
	char temp[n];
	string data;
	char str[n];
	int i=0;
	while(x>0)
	{
		if(x%2==1)
			str[i]='1';
		else
			str[i]='0';
		x=x/2;
		i++;
	}
	for(;i<n;i++)
		str[i]='0';
	i=0;
	int j=n-1;
	while(i<n)
	{
		temp[j]=str[i];
		i++;
		j--;
	}
	data=temp;
	return data;
}

int binTodec(string str, int n)
{
	int decimal=0;
	for(int i=0;i<n;i++)
	{
		if(str[i]=='1' && i==0)
			decimal-=pow(2, n-1-i);
		else if(str[i]=='1')
			decimal+=pow(2, n-1-i);
	}
	return decimal;
}

void make_memory()
{
	// Instruction space
	M[binTodec("000011001000", 12)]="0000000100011111010000000110000111110101";//LOAD M(500)  SUB M(501)
	M[binTodec("000011001001", 12)]="0000110000011111011000100001000111111000";//DIV M(502)  STOR M(504)
	M[binTodec("000011001010", 12)]="0000101000000000000000100001000111110111";//LOAD MQ  STOR M(503)
	M[binTodec("000011001011", 12)]="0000000000000000000000000000000000000000";// HLT

	// Data space
	M[binTodec("000111110100", 12)]="0000000000000000000000000000000000011001";// M(500)=25
	M[binTodec("000111110101", 12)]="0000000000000000000000000000000000000101";// M(501)=5
	M[binTodec("000111110110", 12)]="0000000000000000000000000000000000000100";// M(502)=4
}

/*This function will check if LHS instruction is there or not */
int check_LHS(string MBR)
{
	string temp="00000000000000000000";
	// if there is no lhs instruction all first 20 bits of MBR will be 0
	for(int i=0;i<20;i++)
	{
		if(temp[i]!=MBR[i])
			return 1;
	}
	return 0;
}

/* EXECUTION */

void LOAD(string data)
{
	AC = data;// Transfer data to acc from M(x)
}

void SUB(string data)
{
	int x=binTodec(AC, 40);
	int y=binTodec(data, 40);
	int z=x-y;
	// AC<-AC-M(x)
	AC=decTobin(z, 40);
}

void DIV(string data)
{
	int x=binTodec(AC, 40);
	int y=binTodec(data, 40);
	int a=x/y;
	int b=x%y;
	// AC/M(x)
	// remainder->AC
	// quotient->MQ
	AC=decTobin(b, 40);
	MQ=decTobin(a, 40);
}

void STOR(string address)
{
	// AC->M(x)
	string temp="0000000000000000000000000000000000000000";
	for(int i=0;i<40;i++)
		temp[i]=AC[i];
	M[binTodec(address, 12)]=temp;
}

void LoadMQ()
{
	//MQ->AC
	AC=MQ;
}

void decode(string IR, string address)
{
	// Decoding from opcode which is present in IR
	if(binTodec(IR, 8)==binTodec("00000001", 8))
		LOAD(M[binTodec(address, 12)]);
	else if(binTodec(IR, 8)==binTodec("00000110", 8))
		SUB(M[binTodec(address, 12)]);
	else if(binTodec(IR, 8)==binTodec("00001100", 8))
		DIV(M[binTodec(address, 12)]);
	else if(binTodec(IR, 8)==binTodec("00100001", 8))
		STOR(address);
	else if(binTodec(IR, 8)==binTodec("00001010", 8))
		LoadMQ();

	// stop instruction
	// program will terminate after this instruction
	else if(binTodec(IR, 8)==binTodec("00000000", 8))
	{
		// before terminating the program
		// printing the value of quotient and remainder stored at 503 and 504 repectively in memory
		// for checking result
		string Y=M[binTodec("000111110111", 12)];// Quotient
		for(int i=0;i<40;i++)
			printf("%c",Y[i]);
		printf("\n");
		string X=M[binTodec("000111111000", 12)];// Remainder
		for(int i=0;i<40;i++)
			printf("%c",X[i]);
		printf("\n");
		exit(0);
	}
}

void fetch()
{
	// PC holds the next instruction address
	// size of address field is 12 bits
	string PC="000000000000";

	// MAR holds the address
	// Specifies the address in memory to be read/write
	// size of this is 12 bits
	string MAR="000000000000";

	// MBR is buffer register
	// data or instruction to be read or store in memory pass through memory buffer register
	// size of this is 40 bits
	string MBR="0000000000000000000000000000000000000000";

	// IBR is instruction buffer register
	// It holds the RHS instruction of 40 bit instruction temporarily
	// size of this is 20 bits
	string IBR="00000000000000000000";

	// IR Instruction register
	// It holds the opcode for the instruction to be executed
	// the size of IR is 8 bits
	string IR="00000000";

	// We initialise PC with 200
	PC="000011001000";
	while(1)
	{
		MAR=PC;// Address of PC will go to MAR
		MBR=M[binTodec(MAR, 12)];// Data or instruction will transfer from memory to MBR of given MAR address
		int i=0;
		int flag=check_LHS(MBR);// flag will check if LHS instruction is available or not
		if(flag==1)// LHS is present if flag=1
		{
			// first 8 bits of MBR will transfer into IR
			for(i=0;i<8;i++)
				IR[i]=MBR[i];

			// Next 12 bits transfer into MAR
			for(int j=0;j<12;j++)
			{
				MAR[j]=MBR[i];
				i++;
			}
			
			// RHS instruction will transfer into IBR
			for(int j=0;j<20;j++)
			{
				IBR[j]=MBR[i];
				i++;
			}
			// After transfer of LHS instruction into respective blocks 
			// We will decode the left instruction
			decode(IR, MAR);
			i=0;
			// After decoding LHS instruction we will tranfer RHS instruction from IBR into IR and MAR
			for(int j=0;j<8;j++)
			{
				IR[j]=IBR[i];
				i++;
			}
			for(int j=0;j<12;j++)
			{
				MAR[j]=IBR[i];
				i++;
			}
			// After the respective transfer we will decode the right instruction
			decode(IR, MAR);
		}
		else// flag=0 It means only right instruction is present
		{
			IBR="00000000000000000000";// IBR will be empty in this case
			i=20;
			for(int j=0;j<8;j++)
			{	
				IR[j]=MBR[i];
				i++;
			}
			for(int j=0;j<12;j++)
			{
				MAR[j]=MBR[i];
				i++;
			}
			// decoding right instruction
			decode(IR, MAR);
		}
		// PC=PC+1
		// PC will shift to next address
		int x=binTodec(PC, 12);
		x+=1;
		PC=decTobin(x, 12);
	}
}


int main()
{
	make_memory();
	fetch();
	return 0;
}