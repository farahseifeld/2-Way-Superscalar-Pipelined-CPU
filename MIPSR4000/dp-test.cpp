#include<iostream>
#include<string>
#include"instCU.h"
#include"buffer_1.h"
#include"buffer_2.h"
#include"buffer_3.h"
#include"buffer_4.h"
#include"buffer_5.h"
#include"buffer_6.h"
#include"buffer_7.h"
#include"dmem.h"
#include"imem.h"
#include"RegFile.h"
#include"assembly.h"
using namespace std;

void datapath(assembly f);
void ALU(int a, int b, int ALU_CT, int &result, int &z);
void PC(int pcin, int &pcout, bool en);
//void buff1(int en,int clear,int instF, int PcPlus4F,int &instD,int & PcPlus4D);
/*void PC(int in, int out, int en);*/

int main()
{
		assembly file;
	if (file.readFile("asm.txt")) {
		cout << "processing... " << endl;
		datapath( file);
	}
	else
		cout << "error file" << endl;
	


	


	system("Pause");
	return 0;
}
void ALU(int a, int b, int ALU_CT, int &result, int &z) {

	switch (ALU_CT) {
	case 0:
		result = a & b;
		break;
	case 1:
		result = a - b;
		break;
	case 2:
		result = a + b;
		break;
	case 4:
		result = a ^ b;
		break;
	case 6:
		result = a - b;
		break;
	default:
		result = result;
	}
	if (result == 0) z = 1;
}
void PC(int pcin, int & pcout, bool en)
{
	if (en)
		pcout = pcin;
}
void datapath(assembly f)
{
	int clk = 0, rst;

	//buffers
	buffer_1 buf1;
	buffer_2 buf2;
	buffer_3 buf3;
	buffer_4 buf4;
	buffer_5 buf5;
	buffer_6 buf6;
	buffer_7 buf7;
	
	//instdec and CU
	instCU cu;
	
	//PC
	int pcin =0,pcout=-4;
	
	//IM
	imem im;
	unsigned int inst=0;
	for (int i = 0; i < f.inst.size(); i++) 
	{
		im.WriteInst(i * 4, f.inst[i]);
	}
	
	//RegFile
	RegFile rf;
	unsigned int comp=0,PCSrcD=0,PCBranchD=0,PCJump;
	int c1 = 0, c2 = 0,rd1 = 0, rd2 = 0;
	//ALU
	int SrcAE = 0, SrcBE = 0, aluoutE,z,WriteDataE,WriteRegE;
	
	//Dmem
	dmem dm;
	int ResultW = 0;
	unsigned int ReadDataM2 = 0;
	
	//hazard unit outputs
	int StallF=0,StallD=0,ForwardAD=0,ForwardBD=0,FlushE=0,ForwardAE=0,ForwardBE=0,branchstall=0,lwstall=0;
	
	while (clk<20)
	{
		clk++;
		cout << "Cycle " << clk<<endl; 
		lwstall = ((cu.RsD == buf3.RtE) || (cu.RtD == buf3.RtE)) && buf3.MemtoRegE;
		branchstall = ((cu.branch) && (buf3.RegWriteE) && ((buf3.RegWriteE == cu.RsD) || (buf3.MemWriteE == cu.RtD))) || ((cu.branch) && (buf4.MemtoRegM) && ((buf4.WriteRegM == cu.RsD) || (buf4.WriteRegM == cu.RtD)));
		
		//IF
		pcin = (PCSrcD == 0) ? pcout + 4 : (PCSrcD == 1) ? PCBranchD : PCJump;
		StallF = lwstall || branchstall;
		PC (pcin,pcout, !StallF);
		im.IF1();
		buf1.inputData(pcout);
		cout << "IF stage: " << endl;
		cout << "PC: " << pcout << endl;
		cout << "StallF: " << StallF << endl;
		cout << "--------------------------------------------------" << endl<<endl;
		//IS
		im.IF2(buf1.PC_out, inst);
		StallD = lwstall || branchstall;
		if(!StallD)
		buf2.inputData(inst, pcout + 4);
		if ((PCSrcD >> 1) | ((PCSrcD >> 1) & 1))
			buf2.clr();
		cout << "IS stage: " << endl;
		cout << "Instruction: " << hex << inst << endl;
		cout << "StallD: " << StallD << endl;
		cout << "PCSrc: " << dec<<PCSrcD<<endl;
		cout << "--------------------------------------------------" << endl<<endl;

		//RF
		cu.setinst(buf2.instD);
		rf.readRegFile(cu.A1, cu.A2, rd1, rd2);
		buf3.inputData(cu.RegWrite, cu.MemtoReg, cu.MemWrite, cu.aluctrl, cu.alusrc, cu.RegDst, rd1, rd2, cu.RsD, cu.RtD, cu.RdE, cu.Iimm);
		FlushE = lwstall || branchstall;
		if (FlushE)
			buf3.flushE();
		ForwardAD = ((cu.RsD != 0) && (cu.RsD == buf4.WriteRegM) && buf4.WriteRegM);
		ForwardBD = ((cu.RtD != 0) && (cu.RtD == buf4.WriteRegM) && buf4.WriteRegM);
		c1 = (ForwardAD == 0) ? rd1 : buf4.ALUOutM;
		c2 = (ForwardBD == 0) ? rd2 : buf4.ALUOutM;
		comp = (c1 <= c2) ? 1 : 0;
		PCSrcD = (cu.jump << 1 | cu.branch);
		PCBranchD = cu.Iimm << 2;
		PCJump = ((pcout >> 28) << 28) | (cu.Jimm << 2);
		
		cout << "RF stage: " << endl;
		cout << "FlushE: " << FlushE << endl;
		cout << "InstructionD: " << hex << buf2.instD << endl;
		cout << "A1: " << cu.A1 << endl;
		cout << "A2: " << cu.A2 << endl;
		cout << "RD1: " << rd1<<endl;
		cout << "RD2: " << rd2<<endl;
		cout << "RegWriteD: " << cu.RegWrite << endl;
		cout << "MemtoRegD: " << cu.MemtoReg << endl;
		cout << "MemWriteD: " << cu.MemWrite << endl;
		cout << "ALUControlD: " << cu.aluctrl << endl;
		cout << "ALUSrc: " << cu.alusrc << endl;
		cout << "RegDstD: " << cu.RegDst << endl;
		cout << "JumpD: " << cu.jump << endl;
		cout << "BranchD: " << cu.branch << endl;
		cout << "RsD: " << cu.RsD<<endl;
		cout << "RtD: " << cu.RtD<<endl;
		cout << "RdE: " << cu.RdE<<endl;
		cout << "I-imm: " << cu.Iimm << endl;
		cout << "--------------------------------------------------" << endl<<endl;

		//EX
		if ((buf3.RsE != 0 && (buf3.RsE == buf4.WriteRegM)) && buf4.RegWriteM)
			ForwardAE = 2;
		else if ((buf3.RsE != 0 && (buf3.RsE == buf7.WriteRegW)) && buf7.RegWriteW)
			ForwardAE = 1;
		else if ((buf3.RsE != 0 && (buf3.RsE == buf5.WriteRegM2)) && buf5.RegWriteM2)
			ForwardAE = 3;
		else if ((buf3.RsE != 0 && (buf3.RsE == buf6.WriteRegM3) && buf6.RegWriteM3))
			ForwardAE = 4;
		else
			ForwardAE = 0;

		if ((buf3.RtE != 0 && buf3.RtE == buf4.WriteRegM) && buf4.RegWriteM)
			ForwardBE = 2;
		else if ((buf3.RtE != 0 && buf3.RtE == buf7.WriteRegW) && buf7.RegWriteW)
			ForwardBE = 1;
		else if ((buf3.RtE != 0 && buf3.RtE == buf5.WriteRegM2) && buf5.RegWriteM2)
			ForwardBE = 3;
		else if ((buf3.RtE != 0 && buf3.RtE == buf6.WriteRegM3) && buf6.RegWriteM3)
			ForwardBE = 4;
		else
			ForwardBE = 0;
		SrcAE = (ForwardAE == 0) ? buf3.RD1E : (ForwardAE == 1) ? buf7.ALUOutW : (ForwardAE == 2) ? buf4.ALUOutM : (ForwardAE == 3) ? buf5.ALUOutM2: buf6.ALUOutM3;
		//WriteDataE = (ForwardBE == 0) ? buf3.RD2E : (ForwardBE == 1) ? ResultW : buf4.ALUOutM;
		
		switch (ForwardBE)
		{
		case 0: WriteDataE = buf3.RD2E;
			break;
		case 1: WriteDataE = ResultW;
			break;
		case 2: WriteDataE = buf4.ALUOutM;
			break;
		case 3: WriteDataE = buf5.ALUOutM2;
			break;
		case 4: WriteDataE = buf6.ALUOutM3;
			break;
		default:
			break;
		}
		SrcBE = (buf3.ALUSrcE == 0) ? WriteDataE : buf3.SignImmE;
		ALU(SrcAE, SrcBE, buf3.ALUControlE, aluoutE, z);
		WriteRegE = (buf3.RegDstE == 0) ? buf3.RtE : buf3.RdE;
		buf4.inputData(buf3.RegWriteE, buf3.MemtoRegE, buf3.MemWriteE, aluoutE, WriteDataE, WriteRegE);
		cout << "EX stage: " << endl;
		cout << "RegDstE: " << buf3.RegDstE << endl;
		cout << "ALUSrcE: " << buf3.ALUSrcE << endl;
		cout << "ALUControlE: " << buf3.ALUControlE << endl;
		cout << "SrcAE: " << SrcAE << endl;
		cout << "SrcBE: " << SrcBE << endl;
		cout << "ALUResult: " << aluoutE << endl;
		cout << "RegWriteE: " << buf3.RegWriteE << endl;
		cout << "MemtoRegE: " << buf3.MemtoRegE << endl;
		cout << "MemWriteE: " << buf3.MemWriteE << endl;
		cout << "WriteDataE: " << WriteDataE << endl;
		cout << "WriteRegE: " << WriteRegE << endl;
		cout << "buf3.RsE: " << buf3.RsE << endl;
		cout << "Buf4.WriteregM: " << buf4.WriteRegM << endl;
		cout << "Buf5.WriteRegM2: " << buf5.WriteRegM2 << endl;
		cout << "Buf6.WriteRegM3: " << buf6.WriteRegM3 << endl;
		cout << "Buf7.WriteRegW: " << buf7.WriteRegW << endl;
		cout << "FA: " << ForwardAE << endl;
		cout << "FB: " << ForwardBE << endl;
		cout << "--------------------------------------------------" << endl << endl;
		//DF
		dm.DF1(buf4.ALUOutM, buf4.MemtoRegM, buf4.WriteDataM, ReadDataM2);
		buf5.inputData(buf4.RegWriteM, buf4.MemtoRegM, buf4.MemWriteM, buf4.ALUOutM, buf4.WriteDataM, buf4.WriteRegM);
		cout << "DF stage: " << endl;
		cout << "RegWriteM: " << buf4.RegWriteM << endl;
		cout << "MemtoRegM: " << buf4.MemtoRegM << endl;
		cout << "MemWriteM: " << buf4.MemWriteM << endl;
		cout << "ALUOutM: " << buf4.ALUOutM << endl;
		cout << "WriteDataM: " << buf4.WriteDataM << endl;
		cout << "WriteRegM: " << buf4.WriteRegM << endl;
		cout << "--------------------------------------------------" << endl << endl;

		//DS
		dm.DF2(buf5.ALUOutM2, buf5.MemWriteM2, buf5.WriteDataM2, ReadDataM2);
		buf6.inputData(buf5.RegWriteM2, ReadDataM2, buf5.MemtoRegM2, buf5.ALUOutM2, buf5.WriteRegM2);
		cout << "DS stage: " << endl;
		cout << "RegWriteM2: " << buf5.RegWriteM2 << endl;
		cout << "MemtoRegM2: " << buf5.MemtoRegM2 << endl;
		cout << "MemWriteM2: " << buf5.MemWriteM2 << endl;
		cout << "ALUOutM2: " << buf5.ALUOutM2 << endl;
		cout << "WriteDataM2: " << buf5.WriteDataM2 << endl;
		cout << "WriteRegM2: " << buf5.WriteRegM2 << endl;
		cout << "ReadDataM2: " << ReadDataM2<<endl;
		cout << "--------------------------------------------------" << endl << endl;
		//TC
		dm.TC();
		buf7.inputData(buf6.RegWriteM3, buf6.ReadDataM3, buf6.MemtoRegM3, buf6.ALUOutM3, buf6.WriteRegM3);
		cout << "TC stage: " << endl;
		cout << "RegWriteM3: " << buf6.RegWriteM3 << endl;
		cout << "MemtoRegM3: " << buf6.MemtoRegM3 << endl;
		cout << "ALUOutM3: " << buf6.ALUOutM2 << endl;
		cout << "WriteRegM3: " << buf6.WriteRegM3 << endl;
		cout << "ReadDataM3: " << buf6.ReadDataM3<<endl;
		cout << "--------------------------------------------------" << endl << endl;
		//WB
		ResultW = (buf7.MemtoRegW == 1) ? buf7.ReadDataW : buf7.ALUOutW;
		if (buf7.RegWriteW == 1)
			rf.writeRegFile(buf7.WriteRegW, ResultW);
		cout << "WB stage: " << endl;
		cout << "MemtoRegW: " << buf7.MemtoRegW << endl;
		cout << "ALUOutW: " << buf7.ALUOutW << endl;
		cout << "ReadDataW: " << buf7.ReadDataW << endl;
		cout << "RegWriteW: " << buf7.RegWriteW << endl;
		cout << "WriteRegW: " << buf7.WriteRegW << endl;
		cout << "ResultW: " << ResultW << endl;
		cout << "====================================================================" << endl << endl;

		//Buffers update
		buf1.updateData();
		buf2.updateData();
		buf3.updateData();
		buf4.updateData();
		buf5.updateData();
		buf6.updateData();
		buf7.updateData();
	}

	rf.print();

}
