#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "buffer_1.h"
#include "buffer_2.h"
#include "buffer_3.h"
#include "buffer_4.h"
#include "buffer_5.h"
#include "buffer_6.h"
#include "buffer_7.h"
#include "cache.h"
#include "dmem.h"
#include "imem.h"
#include "instCU.h"
#include "RegFile.h"
#include "assembly.h"
using namespace std;


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

int branchPredict(int prev, bool taken)
{
	if (prev == 0)
	{
		if (taken)
			return 0;
		else
			return 1;
	}
	else if (prev == 1)
	{
		if (taken)
			return 0;
		else
			return 2;
	}
	else if (prev == 2)
	{
		if (taken)
			return 1;
		else
			return 3;
	}
	else if (prev == 3)
	{
		if (taken)
			return 2;
		else
			return 3;
	}
}

void PC(int pcin, int & pcout, bool en)
{
	if (en)
		pcout = pcin;
}

unsigned int pc(int oldPC, bool rst, unsigned int BO, 
	unsigned int JCT, bool BEN, unsigned int IM1, unsigned int IM2, bool FEN)
{
	int t;
	if (rst)
		return 0;
	else if (FEN)
	{
		if (JCT == 0)
			return (oldPC + 4);
		else if (JCT == 1)
		{
			if (BEN == 0)
				return oldPC + 4;
			else
				return oldPC + BO;
		}
		else if (JCT == 2)
		{
			t = oldPC & 0b11110000000000000000000000000000;
			cout << oldPC << endl;
			cout << t << endl;
			return t | IM1;

		}
		else if (JCT == 3)
			return IM2;
	}
}


void hazardUnit(int rsD, int rtD, int rsE, int rtE,
	int rDE, int rDM, int rDW, int d1E, int d1M, int d1W,
	int ddE, int ddM, int ddW, int branchD, int jumpD,
	int &forwardA, int & forwardB, bool &flush, bool &stall) {

	if (rsE != 0 && rsE == rDM && d1M == 1)
		forwardA = 2;
	else if (rsE != 0 && rsE == rDW && d1W == 1)
		forwardA = 1;
	else
		forwardA = 0;

	if (rtE != 0 && rtE == rDM && d1M == 1)
		forwardB = 2;
	else if (rtE != 0 && rtE == rDW && d1W == 1)
		forwardB = 1;
	else
		forwardB = 0;

	bool lwstall = (rsD == rtE || rtD == rtE) && (d1M == 1);
	bool branchstall = ((branchD == 1) && (d1E == 1) && (rDE == rsD || rDE == rtD)) || ((branchD == 1) && (ddE == 1) && (rDM == rsD || rDM == rtD));

	stall = lwstall || branchstall;
	flush = lwstall || branchstall || (jumpD == 1);
}


void datapath(RegFile &rf, assembly f)
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
	int pcin = -4, pcout = 0;

	//IM
	imem im;
	for (int i = 0; i < f.inst.size(); i++) {
		im.WriteInst(i * 4, f.inst[i]);
	}

unsigned int inst = 0;

	//RegFile
//	RegFile rf;
	int rd1 = 0, rd2 = 0, c1 = 0, c2 = 0, comp = 0, PCSrcD = 0, PCBranchD = 0, PCJump;

	//ALU
	int SrcAE = 0, SrcBE = 0, aluoutE, z, WriteDataE, WriteRegE;

	//Dmem
	dmem dm;
	int ResultW = 0;
	unsigned int ReadDataM2 = 0;

	//hazard unit outputs
	int StallF = 0, StallD = 0, ForwardAD = 0, ForwardBD = 0, FlushE = 0, ForwardAE = 0, ForwardBE = 0, branchstall = 0, lwstall = 0;

	while (clk<8)
	{
		clk++;	
		cout << "cycle: " << clk << endl;

		lwstall = ((cu.RsD == buf3.RtE) || (cu.RtD == buf3.RtE)) && buf3.MemtoRegE;
		branchstall = ((cu.branch) && (buf3.RegWriteE) && ((buf3.RegWriteE == cu.RsD) || (buf3.MemWriteE == cu.RtD))) || ((cu.branch) && (buf4.MemtoRegM) && ((buf4.WriteRegM == cu.RsD) || (buf4.WriteRegM == cu.RtD)));

		//IF
		cout << "--------------------IF STAGE---------------------" << endl;
		pcin = (PCSrcD == 0) ? pcout + 4 : (PCSrcD == 1) ? PCBranchD : PCJump;
		StallF = lwstall || branchstall;
		PC(pcin, pcout, !StallF);
		im.IF1();
		buf1.inputData(pcout);
		cout << "PC OUT= " << buf1.PC_out << endl;
		
//-----------------------------------------------------------------------------------------------------------
		

		//IS
		//im.IF2(buf1.PC_out, inst);
		cout << "--------------------IS STAGE---------------------" << endl;
		if(buf1.PC_out/4<f.inst.size())
		inst = f.inst[buf1.PC_out/4];

		cout << "inst= " << hex <<inst << endl;
		cout << "lwstall: " << lwstall << endl;
		cout << "bstall: " << branchstall << endl;
		StallD = lwstall || branchstall;
		if (!StallD)
			buf2.inputData(inst, pcout + 4);
		if ((PCSrcD >> 1) | ((PCSrcD >> 1) & 1))
			buf2.CLR();
		cout << "Instruction: " << hex << buf2.instD << endl;

		cout << "STALL D: " << StallD << endl;


		//RF
		cout << "--------------------RF STAGE---------------------" << endl;
		cu.setinst(buf2.instD);

		rf.readRegFile(cu.A1, cu.A2, rd1, rd2);
		
		buf3.inputData(cu.RegWrite, cu.MemtoReg, cu.MemWrite, cu.aluctrl, cu.alusrc, cu.RegDst, rd1, rd2, cu.RsD, cu.RtD, cu.RdE, cu.Iimm);
		cout << "Register 1 || Addr= " << buf3.RtE << ",  Data= " << buf3.RD1E << endl;
		cout << "Register 2 || Addr= " <<buf3.RsE << ",  Data= " <<   buf3.RD2E<< endl;
		cout << "Imm= " << buf3.SignImmE << endl;
		cout << "cu.RdE:  " << dec <<cu.RdE << endl;
		cout << "cu.RegWrite:  " << dec<<cu.RegWrite << endl;
		cout << "cu.RegDst:  " << dec << cu.RegDst << endl;
		FlushE = lwstall || branchstall;
		if (FlushE)
			buf3.flushE();
		ForwardAD = ((cu.RsD != 0) && (cu.RsD == buf4.WriteRegM) && buf4.WriteRegM);
		ForwardBD = ((cu.RtD != 0) && (cu.RtD == buf4.WriteRegM) && buf4.WriteRegM);
		c1 = (ForwardAD == 0) ? rd1 : buf4.ALUOutM;
		c2 = (ForwardBD == 0) ? rd2 : buf4.ALUOutM;
		comp = (c1 <= c2) ? 1 : 0;
		//PCSrcD = (cu.jump << 1 | cu.branch);
		PCSrcD = (cu.jump  | cu.branch);
		PCBranchD = cu.Iimm << 2;
		PCJump = ((pcout >> 28) << 28) | (cu.Jimm << 2);
//-----------------------------------------------------------------------------------------------------------	


		//EX
		cout << "--------------------EX STAGE---------------------" << endl;
		if ((buf3.RsE != 0 && buf3.RsE == buf4.WriteRegM) && buf4.RegWriteM)
			ForwardAE = 2;
		else if ((buf3.RsE != 0 && buf3.RsE == buf7.WriteRegW) && buf7.RegWriteW)
			ForwardAE = 1;
		else
			ForwardAE = 0;
		if ((buf3.RtE != 0 && buf3.RtE == buf4.WriteRegM) && buf4.RegWriteM)
			ForwardBE = 2;
		else if ((buf3.RtE != 0 && buf3.RtE == buf7.WriteRegW) && buf7.RegWriteW)
			ForwardBE = 1;
		else
			ForwardBE = 0;
		SrcAE = (ForwardAE == 0) ? buf3.RD1E : (ForwardAE == 1) ? ResultW : buf4.ALUOutM;
		cout << "src A: " << SrcAE << endl;
		WriteDataE = (ForwardBE == 0) ? buf3.RD2E : (ForwardBE == 1) ? ResultW : buf4.ALUOutM;
		SrcBE = (buf3.ALUSrcE == 0) ? WriteDataE : buf3.SignImmE;
		cout << "src B: " << SrcBE << endl;
		cout << "ALU Control:  " << buf3.ALUControlE << endl;
		ALU(SrcAE, SrcBE, buf3.ALUControlE, aluoutE, z);
		cout << "ALUout= " << buf4.ALUOutM << endl;
		WriteRegE = (buf3.RegDstE == 0) ? buf3.RtE : buf3.RdE;

		buf4.inputData(buf3.RegWriteE, buf3.MemtoRegE, buf3.MemWriteE, aluoutE, WriteDataE, WriteRegE);
//-----------------------------------------------------------------------------------------------------------
	
		//DF
		cout << "--------------------DF STAGE---------------------" << endl;
		dm.DF1(buf4.ALUOutM, buf4.MemtoRegM, buf4.WriteDataM, ReadDataM2);
		buf5.inputData(buf4.RegWriteM, buf4.MemtoRegM, buf4.MemWriteM, buf4.ALUOutM, buf4.WriteDataM, buf4.WriteRegM);
		

		//DS
		cout << "--------------------DS STAGE---------------------" << endl;
		dm.DF2(buf5.ALUOutM2, buf5.MemWriteM2, buf5.WriteDataM2, ReadDataM2);
		buf6.inputData(buf5.RegWriteM2, ReadDataM2, buf5.MemtoRegM2, buf5.ALUOutM2, buf5.WriteRegM2);
		



		//TC
		//dm.TC();
		cout << "--------------------TC STAGE---------------------" << endl;
		buf7.inputData(buf6.RegWriteM3, buf6.ReadDataM3, buf6.MemtoRegM3, buf6.ALUOutM3, buf6.WriteRegM3);
		//buf7.updateData();


		//WB
		cout << "--------------------WB STAGE---------------------" << endl;
		ResultW = (buf7.MemtoRegW == 1) ? buf7.ReadDataW : buf7.ALUOutW;
		cout << "Write Data= " << ResultW << endl;
		if (buf7.RegWriteW == 1) {
			cout << "writing in regfile" << endl;
			cout << "Write Register= " << buf7.RegWriteW << endl;
			rf.writeRegFile(buf7.RegWriteW, ResultW);
		}
		cout << "===================================" << endl << endl;
		//Buffers update
		buf1.updateData();
		buf2.updateData(StallD);
		buf3.updateData();
		buf4.updateData();
		buf5.updateData();
		buf6.updateData();
		buf7.updateData();
	
	
	}

}

int main() {


	assembly file;
	RegFile rf;
	if (file.readFile("asm.txt")) {
		cout << "processing... " << endl;
		datapath(rf, file);
	}
	else
		cout << "error file" << endl;
	rf.print();

	system("pause");
	return 0;
}