void datapath()
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
	int pcin = -4,pcout=0;
	
	//IM
	imem im;
	unsigned int inst=0;
	
	//RegFile
	RegFile rf;
	int rd1=0, rd2=0,c1=0,c2=0,comp=0,PCSrcD=0,PCBranchD=0,PCJump;
	
	//ALU
	int SrcAE = 0, SrcBE = 0, aluoutE,z,WriteDataE,WriteRegE;
	
	//Dmem
	dmem dm;
	int ResultW = 0;
	unsigned int ReadDataM2 = 0;
	
	//hazard unit outputs
	int StallF=0,StallD=0,ForwardAD=0,ForwardBD=0,FlushE=0,ForwardAE=0,ForwardBE=0,branchstall=0,lwstall=0;
	
	while (buf2.instD!=-1)
	{
		clk++;
		lwstall = ((cu.RsD == buf3.RtE) || (cu.RtD == buf3.RtE)) && buf3.MemtoRegE;
		branchstall = ((cu.branch) && (buf3.RegWriteE) && ((buf3.RegWriteE == cu.RsD) || (buf3.MemWriteE == cu.RtD))) || ((cu.branch) && (buf4.MemtoRegM) && ((buf4.WriteRegM == cu.RsD) || (buf4.WriteRegM == cu.RtD)));
		
		//IF
		pcin = (PCSrcD == 0) ? pcout + 4 : (PCSrcD == 1) ? PCBranchD : PCJump;
		StallF = lwstall || branchstall;
		PC (pcin,pcout, !StallF);
		im.IF1();
		buf1.inputData(pcout);

		//IS
		im.IF2(buf1.PC_out, inst);
		StallD = lwstall || branchstall;
		if(!StallD)
		buf2.inputData(inst, pcout + 4);
		if ((PCSrcD >> 1) | ((PCSrcD >> 1) & 1))
			buf2.clr();
		
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
		
		//EX
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
		WriteDataE = (ForwardBE == 0) ? buf3.RD2E : (ForwardBE == 1) ? ResultW : buf4.ALUOutM;
		SrcBE = (buf3.ALUSrcE == 0) ? WriteDataE : buf3.SignImmE;
		ALU(SrcAE, SrcBE, buf3.ALUControlE, aluoutE, z);
		WriteRegE = (buf3.RegDstE == 0) ? buf3.RtE : buf3.RdE;
		buf4.inputData(buf3.RegWriteE, buf3.MemtoRegE, buf3.MemWriteE, aluoutE, WriteDataE, WriteRegE);
		
		//DF
		dm.DF1(buf4.ALUOutM, buf4.MemtoRegM, buf4.WriteDataM, ReadDataM2);
		buf5.inputData(buf4.RegWriteM, buf4.MemtoRegM, buf4.MemWriteM, buf4.ALUOutM, buf4.WriteDataM, buf4.WriteRegM);
		
		//DS
		dm.DF2(buf5.ALUOutM2, buf5.MemWriteM2, buf5.WriteDataM2, ReadDataM2);
		buf6.inputData(buf5.RegWriteM2, ReadDataM2, buf5.MemtoRegM2, buf5.ALUOutM2, buf5.WriteRegM2);
		
		//TC
		dm.TC();
		buf7.inputData(buf6.RegWriteM3, buf6.ReadDataM3, buf6.MemtoRegM3, buf6.ALUOutM3, buf6.WriteRegM3);
		
		//WB
		ResultW = (buf7.MemtoRegW == 0) ? buf7.ReadDataW : buf7.ALUOutW;
		if (buf7.RegWriteW == 1)
			rf.writeRegFile(buf7.RegWriteW, ResultW);
		
		//Buffers update
		buf1.updateData();
		buf2.updateData();
		buf3.updateData();
		buf4.updateData();
		buf5.updateData();
		buf6.updateData();
		buf7.updateData();
		
	}

}
