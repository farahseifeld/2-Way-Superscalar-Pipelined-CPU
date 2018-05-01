#include<iostream>
#include<fstream>
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
#include "code.h"
#include "HazardUnit.h"
//#include "gui.h"
using namespace std;
ofstream target;
void datapath(assembly f);
void ALU(int a, int b, int ALU_CT, int &result, int &z);
void PC(unsigned int pcin, unsigned int &pcout, bool en);
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


void dp(int &clk, unsigned int &pcin,
	unsigned int &PCSrcD, unsigned int &pcout,
	int &PCBranchD, int &PCJump,
	unsigned int &inst,
	imem &im, dmem &dm, instCU &cu, RegFile &rf,
	buffer_1 &buf1, buffer_2 &buf2, buffer_3 &buf3, buffer_4 &buf4, buffer_5 &buf5,
	buffer_6 &buf6, buffer_7 &buf7, int &rd1, int &rd2, int &ResultW, int&WriteDataE, int &ReadDataM2, int &SrcAE,
	int &SrcBE, int &aluoutE, int &z, int & c1, int &c2, unsigned int &comp, int &WriteRegE, vector <Code> &code, assembly a, unsigned int &bpc) {
	target << "Cycle " << clk << endl;
	HazardUnit h;
	h.inputData(cu.RsD, buf3.RsE, cu.RtD, buf3.RtE, buf3.MemtoRegE, cu.branch, buf3.RegWriteE, buf3.MemtoRegE, buf4.MemtoRegM, buf4.WriteRegM, PCSrcD, buf5.WriteRegM2, buf6.WriteRegM3, buf4.RegWriteM, buf5.RegWriteM2, buf6.RegWriteM3, buf3.RegDstE, buf3.RdE, buf7.WriteRegW, buf7.RegWriteW);
	h.updateData();
	bool flag = false;
	//IF
	pcin = (PCSrcD == 0) ? pcout + 4 : (PCSrcD == 1) ? PCBranchD + bpc : PCJump;
	//WHYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY

	PC(pcin, pcout, !h.StallF);
	im.IF1();
	buf1.inputData(pcout);
	target << "IF stage: " << endl;
	target << "PCSrcD: " << PCSrcD << endl;
	target << "PC: " << pcout << endl;
	target << "PCBRANCHD !!!!!!!!  " << dec << PCBranchD << endl;
	target << "StallF: " << h.StallF << endl;
	buf1.setInstNum(clk, pcout);
	target << "--------------------------------------------------" << endl << endl;
	Code c;
	if (clk > 0) {
		if (inst != 0xffffffff && pcout / 4 < a.inst.size() - 1 && a.inst[pcout / 4] != 0xcccccccc) {
			c.setDetails(a.instructions[pcout / 4], a.inst[pcout / 4], pcout);

		}
		else
			c.setDetails("End", 0, pcout);
		code.push_back(c);
	}

	//IS
	im.IF2(buf1.PC_out, inst);
	if (inst >> 26 == 0x4)
		bpc = buf1.PC_out;
	//Code c;

	//WB
	//buf7.updateData();
	ResultW = (buf7.MemtoRegW == 1) ? buf7.ReadDataW : buf7.ALUOutW;
	if (buf7.RegWriteW == 1)
		rf.writeRegFile(buf7.WriteRegW, ResultW);
	target << "WB stage: " << endl;
	target << "MemtoRegW: " << buf7.MemtoRegW << endl;
	target << "ALUOutW: " << buf7.ALUOutW << endl;
	target << "ReadDataW: " << buf7.ReadDataW << endl;
	target << "RegWriteW: " << buf7.RegWriteW << endl;
	target << "WriteRegW: " << buf7.WriteRegW << endl;
	target << "ResultW: " << ResultW << endl;
	target << "====================================================================" << endl << endl;

	if (!h.StallD)
		buf2.inputData(inst, pcout + 4);
	if ((PCSrcD >> 1) | ((PCSrcD >> 1) & 1))
		buf2.clr();
	target << "IS stage: " << endl;
	target << "Instruction: " << hex << inst << endl;
	target << "StallD: " << h.StallD << endl;
	target << "PCSrc: " << dec << PCSrcD << endl;
	target << "--------------------------------------------------" << endl << endl;
	if (clk > 1)
		buf2.setInstNum(buf1.inst_num - 1, buf1.pc);



	//RF
	cu.setinst(buf2.instD);
	if (cu.branch) {
		flag = true;

		//EX
	EX:
		switch (h.ForwardBE)
		{
		case 0: WriteDataE = buf3.RD2E;
			break;
		case 1: WriteDataE = ResultW;
			break;
		case 2: WriteDataE = buf4.ALUOutM;
			break;
		case 3: WriteDataE = buf5.ALUOutM2;
			break;
		case 4: WriteDataE = (buf6.MemtoRegM3) ? buf6.ReadDataM3 : buf6.ALUOutM3;
			break;
		default:
			break;
		}
		SrcBE = (buf3.ALUSrcE == 0) ? WriteDataE : buf3.SignImmE;
		ALU(SrcAE, SrcBE, buf3.ALUControlE, aluoutE, z);
		WriteRegE = (buf3.RegDstE == 0) ? buf3.RtE : buf3.RdE;
		buf4.inputData(buf3.RegWriteE, buf3.MemtoRegE, buf3.MemWriteE, aluoutE, WriteDataE, WriteRegE);
		target << "EX stage: " << endl;
		target << "RegDstE: " << buf3.RegDstE << endl;
		target << "ALUSrcE: " << buf3.ALUSrcE << endl;
		target << "ALUControlE: " << buf3.ALUControlE << endl;
		target << "SrcAE: " << SrcAE << endl;
		target << "SrcBE: " << SrcBE << endl;
		target << "ALUResult: " << aluoutE << endl;
		target << "RegWriteE: " << buf3.RegWriteE << endl;
		target << "MemtoRegE: " << buf3.MemtoRegE << endl;
		target << "MemWriteE: " << buf3.MemWriteE << endl;
		target << "WriteDataE: " << WriteDataE << endl;
		target << "WriteRegE: " << WriteRegE << endl;
		target << "buf3.RsE: " << buf3.RsE << endl;
		target << "Buf4.WriteregM: " << buf4.WriteRegM << endl;
		target << "Buf5.WriteRegM2: " << buf5.WriteRegM2 << endl;
		target << "Buf6.WriteRegM3: " << buf6.WriteRegM3 << endl;
		target << "Buf7.WriteRegW: " << buf7.WriteRegW << endl;
		target << "FA: " << h.ForwardAE << endl;
		target << "FB: " << h.ForwardBE << endl;
		if (clk > 3)
			buf4.setInstNum(buf3.inst_num, buf3.pc);

		buf5.inputData(buf4.RegWriteM, buf4.MemtoRegM, buf4.MemWriteM, buf4.ALUOutM, buf4.WriteDataM, buf4.WriteRegM);

		target << "--------------------------------------------------" << endl << endl;
	}

	rf.readRegFile(cu.A1, cu.A2, rd1, rd2);
	buf3.inputData(cu.RegWrite, cu.MemtoReg, cu.MemWrite, cu.aluctrl, cu.alusrc, cu.RegDst, rd1, rd2, cu.RsD, cu.RtD, cu.RdE, cu.Iimm);
	//FlushE = lwstall || branchstall || cu.jump;


	switch (h.ForwardBD)
	{
	case 0: c2 = buf3.RD2E;
		break;
	case 2: c2 = buf4.ALUOutM;
		break;
	case 3: c2 = buf5.ALUOutM2;
		break;
	case 4: c2 = (buf6.MemtoRegM3) ? buf6.ReadDataM3 : buf6.ALUOutM3;
		break;
	default:
		break;
	}



	//ForwardAD = ((cu.RsD != 0) && (cu.RsD == buf4.WriteRegM) && buf4.WriteRegM);
	//ForwardBD = ((cu.RtD != 0) && (cu.RtD == buf4.WriteRegM) && buf4.WriteRegM);
	//c1 = (ForwardAD == 0) ? rd1 : buf4.ALUOutM;
	//c2 = (ForwardBD == 0) ? rd2 : buf4.ALUOutM;
	comp = (c1 <= c2) ? 1 : 0;
	PCSrcD = (cu.jump << 1 | cu.branch);
	target << "PCBRANCH BEFORE " << dec << cu.Iimm << endl;
	PCBranchD = cu.Iimm * 4;
	PCJump = ((pcout >> 28) << 28) | (cu.Jimm << 2);
	//we need to check for data dependency here
	target << "RF stage: " << endl;
	target << "ForwardAD " << h.ForwardAD << endl;
	target << "ForwardBD " << h.ForwardBD << endl;

	target << "FlushE: " << h.FlushE << endl;
	target << "InstructionD: " << hex << buf2.instD << endl;
	target << "c1: " << c1 << endl;
	target << "c2: " << c2 << endl;
	target << "A1: " << cu.A1 << endl;
	target << "A2: " << cu.A2 << endl;
	target << "RD1: " << rd1 << endl;
	target << "RD2: " << rd2 << endl;
	target << "RegWriteD: " << cu.RegWrite << endl;
	target << "MemtoRegD: " << cu.MemtoReg << endl;
	target << "MemWriteD: " << cu.MemWrite << endl;
	target << "ALUControlD: " << cu.aluctrl << endl;
	target << "ALUSrc: " << cu.alusrc << endl;
	target << "RegDstD: " << cu.RegDst << endl;
	target << "JumpD: " << cu.jump << endl;
	target << "BranchD: " << cu.branch << endl;
	target << "RsD: " << cu.RsD << endl;
	target << "RtD: " << cu.RtD << endl;
	target << "RdE: " << cu.RdE << endl;
	target << "I-imm: " << cu.Iimm << endl;
	target << "J-imm: " << cu.Jimm << endl;

	if (clk > 2)
		buf3.setInstNum(buf2.inst_num - 1, buf2.pc);
	target << "--------------------------------------------------" << endl << endl;

	//EX
	if (!flag)

	{
		if (h.FlushE)
			buf3.flushE();
		SrcAE = (h.ForwardAE == 0) ? buf3.RD1E : (h.ForwardAE == 1) ? buf7.ALUOutW : (h.ForwardAE == 2) ? buf4.ALUOutM : (h.ForwardAE == 3) ? buf5.ALUOutM2 : (buf6.MemtoRegM3) ? buf6.ReadDataM3 : buf6.ALUOutM3;
		//WriteDataE = (ForwardBE == 0) ? buf3.RD2E : (ForwardBE == 1) ? ResultW : buf4.ALUOutM;

		switch (h.ForwardBE)
		{
		case 0: WriteDataE = buf3.RD2E;
			break;
		case 1: WriteDataE = ResultW;
			break;
		case 2: WriteDataE = buf4.ALUOutM;
			break;
		case 3: WriteDataE = buf5.ALUOutM2;
			break;
		case 4: WriteDataE = (buf6.MemtoRegM3) ? buf6.ReadDataM3 : buf6.ALUOutM3;
			break;
		default:
			break;
		}
		SrcBE = (buf3.ALUSrcE == 0) ? WriteDataE : buf3.SignImmE;
		ALU(SrcAE, SrcBE, buf3.ALUControlE, aluoutE, z);
		WriteRegE = (buf3.RegDstE == 0) ? buf3.RtE : buf3.RdE;
		buf4.inputData(buf3.RegWriteE, buf3.MemtoRegE, buf3.MemWriteE, aluoutE, WriteDataE, WriteRegE);
		target << "EX stage: " << endl;
		target << "RegDstE: " << buf3.RegDstE << endl;
		target << "ALUSrcE: " << buf3.ALUSrcE << endl;
		target << "ALUControlE: " << buf3.ALUControlE << endl;
		target << "SrcAE: " << SrcAE << endl;
		target << "SrcBE: " << SrcBE << endl;
		target << "ALUResult: " << aluoutE << endl;
		target << "RegWriteE: " << buf3.RegWriteE << endl;
		target << "MemtoRegE: " << buf3.MemtoRegE << endl;
		target << "MemWriteE: " << buf3.MemWriteE << endl;
		target << "WriteDataE: " << WriteDataE << endl;
		target << "WriteRegE: " << WriteRegE << endl;
		target << "buf3.RsE: " << buf3.RsE << endl;
		target << "Buf4.WriteregM: " << buf4.WriteRegM << endl;
		target << "Buf5.WriteRegM2: " << buf5.WriteRegM2 << endl;
		target << "Buf6.WriteRegM3: " << buf6.WriteRegM3 << endl;
		target << "Buf7.WriteRegW: " << buf7.WriteRegW << endl;
		target << "FA: " << h.ForwardAE << endl;
		target << "FB: " << h.ForwardBE << endl;
		if (clk > 3)
			buf4.setInstNum(buf3.inst_num, buf3.pc);
		buf5.inputData(buf4.RegWriteM, buf4.MemtoRegM, buf4.MemWriteM, buf4.ALUOutM, buf4.WriteDataM, buf4.WriteRegM);

		target << "--------------------------------------------------" << endl << endl;
	}



	//DF
	dm.DF1();


	target << "DF stage: " << endl;
	target << "RegWriteM: " << buf4.RegWriteM << endl;
	target << "MemtoRegM: " << buf4.MemtoRegM << endl;
	target << "MemWriteM: " << buf4.MemWriteM << endl;
	target << "ALUOutM: " << buf4.ALUOutM << endl;
	target << "WriteDataM: " << buf4.WriteDataM << endl;
	target << "WriteRegM: " << buf4.WriteRegM << endl;
	if (clk > 4)
		buf5.setInstNum(buf4.inst_num - 1, buf4.pc);
	target << "--------------------------------------------------" << endl << endl;

	//DS
	dm.DF2(buf5.ALUOutM2, buf5.MemWriteM2, buf5.WriteDataM2, ReadDataM2);
	buf6.inputData(buf5.RegWriteM2, ReadDataM2, buf5.MemtoRegM2, buf5.ALUOutM2, buf5.WriteRegM2);
	target << "DS stage: " << endl;
	target << "RegWriteM2: " << buf5.RegWriteM2 << endl;
	target << "MemtoRegM2: " << buf5.MemtoRegM2 << endl;
	target << "MemWriteM2: " << buf5.MemWriteM2 << endl;
	target << "ALUOutM2: " << buf5.ALUOutM2 << endl;
	target << "WriteDataM2: " << buf5.WriteDataM2 << endl;
	target << "WriteRegM2: " << buf5.WriteRegM2 << endl;
	target << "ReadDataM2: " << ReadDataM2 << endl;
	if (clk > 5)
		buf6.setInstNum(buf5.inst_num - 1, buf5.pc);
	target << "--------------------------------------------------" << endl << endl;
	//TC
	dm.TC();
	buf7.inputData(buf6.RegWriteM3, buf6.ReadDataM3, buf6.MemtoRegM3, buf6.ALUOutM3, buf6.WriteRegM3);
	target << "TC stage: " << endl;
	target << "RegWriteM3: " << buf6.RegWriteM3 << endl;
	target << "MemtoRegM3: " << buf6.MemtoRegM3 << endl;
	target << "ALUOutM3: " << buf6.ALUOutM2 << endl;
	target << "WriteRegM3: " << buf6.WriteRegM3 << endl;
	target << "ReadDataM3: " << buf6.ReadDataM3 << endl;
	if (clk > 6)
		buf7.setInstNum(buf6.inst_num - 1, buf6.pc);
	target << "--------------------------------------------------" << endl << endl;



	//Buffers update
	buf1.updateData();
	buf2.updateData();
	buf3.updateData();
	if (h.FlushE)
		buf3.flushE();
	//buf4.updateData();
	buf4.updateData();
	buf5.updateData();
	buf6.updateData();
	buf7.updateData();
}

int main() {
	assembly file;
	target.open("log.txt");
	if (file.readFile("asm.txt")) {
		target << "processing... " << endl;
		datapath(file);
	}
	else
		target << "error file" << endl;

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
	case 3:
		result = (a<b)?1:0;
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
void PC(unsigned int pcin, unsigned int & pcout, bool en)
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
	unsigned int pcin = 0, pcout = -4;

	//IM
	imem im;
	unsigned int inst = 0;
	for (int i = 0; i < f.inst.size(); i++)
	{
		im.WriteInst(i * 4, f.inst[i]);
		target << f.inst[i] << endl;
	}

	//RegFile
	RegFile rf;
	unsigned int comp = 0, PCSrcD = 0;
	int PCBranchD = 0, PCJump;
	int c1 = 0, c2 = 0, rd1 = 0, rd2 = 0;
	//ALU
	int SrcAE = 0, SrcBE = 0, aluoutE, z, WriteDataE, WriteRegE;

	//Dmem
	dmem dm;
	int ResultW = 0;
	int ReadDataM2 = 0;

	//hazard unit outputs
	//initialize all SFML stuff here
	//--

	unsigned int bpc;



	//GUI G;
	vector <Code> code;
	//keep loop going until limit for next is reached and window is still open
	while (clk<25)
	{

		//G.handleEvents();
		//if next is pressed, do this
		if (true) {

			clk++;
			dp(clk, pcin, PCSrcD, pcout,
				PCBranchD, PCJump, inst,
				im, dm, cu, rf,
				buf1, buf2, buf3, buf4, buf5,
				buf6, buf7, rd1, rd2, ResultW, WriteDataE, ReadDataM2, SrcAE,
				SrcBE, aluoutE, z, c1, c2, comp, WriteRegE, code, f, bpc);

			//G.updateInstructions(code);
			//update stages in sfml
			//G.updateStages(clk, code, buf1, buf2, buf3, buf4, buf5, buf6, buf7);

			//update regFile in sfml
			//G.updateRegFile(rf);

			//update memory in sfml
			//G.updateDmem(dm);
			//add instruction to sfml (add to vector of insts)
			//addInst(string inst, int pcinst);
		}
		for (int i = 0; i < 16; i++)
		{
			target << i << "\t" << rf.file[i] << endl;
		}


		//drawing
		//copy paste from GUI
		//G.display(code);


	}

	//rf.print();

}