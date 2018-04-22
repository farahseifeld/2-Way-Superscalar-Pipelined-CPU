#include <string>
#include <iostream>
#include <vector>
#include <bitset>
using namespace std;


bool getOpcode(string s, int &opcode, int &funct, char &type) {

	funct = -1;
	if (s == "ADD") {
		opcode = 0;
		funct = 32;
		type = 'R';
	}
	else if (s == "ADDI") {
		opcode = 8;
		type = 'I';
	}
	else if (s == "LW") {
		opcode = 35;
		type = 'I';
	}
	else if (s == "SW") {
		opcode = 43;
		type = 'I';
	}
	else if (s == "JAL") {
		opcode = 3;
		type = 'J';
	}
	else if (s == "J") {
		opcode = 2;
		type = 'J';
	}
	else if (s == "JR") {
		opcode = 0;
		funct = 8;
		type = 'R';
	}
	//pseudoinstruction ask about it
	else if (s == "BLE") {
		opcode = 4;
		type = 'I';
	}
	else if (s == "XOR") {
		opcode = 0;
		funct = 38;
		type = 'R';
	}
	else if (s == "SLT") {
		opcode = 0;
		funct = 42;
		type = 'R';
	}
	else return false;

	return true;
}

bool split(string s, string &name, int &rs, int &rt, int &rd, int &imm) {

	int i = s.find(" ");
	if (i == string::npos)
		return false;

	int j = s.find(",");
	int j1 = s.find(",",j+1);
	 name = s.substr(0, i);

	 if (j != string::npos) {
		 rd = atoi((s.substr(i + 1, j)).c_str());
		 if (rd > 15 || rd<0)
			 return false;
	 }

	 else if (name!="JR") {
		 imm = atoi((s.substr(i + 1)).c_str());
		 return true;
	 }
	 else {
		 rs = atoi((s.substr(i + 1)).c_str());
		 if (rs > 15 || rs<0)
			 return false;
		 rd = 0;
		 rt = 0;
		 return true;
	 }

	 if (j1 != string::npos) {

		 rs = atoi((s.substr(j + 1, j1)).c_str());
		 rt = atoi((s.substr(j1+1)).c_str());

		 if (rs > 15 || rs < 0)
			 return false;
		 if (rt > 15 || rt < 0)
			 return false;
	 }

	 else {
		 int k = s.find("(");
		 int k1 = s.find(")");

		 if (k == string::npos || k1 == string::npos)
			 return false;


		 rt = rd;
		 imm= atoi((s.substr(j + 1,k)).c_str());
		 rs= atoi((s.substr(k+1, k1)).c_str());

		 if (rt > 15 || rt < 0)
			 return false;
		 if (rs > 15 || rs < 0)
			 return false;
	 }
	 return true;
}

bool parseASM(string s, int &inst) {

	int opcode, funct, rs, rt, rd, imm;
	string name;
	char type;


	if (!split(s, name, rs, rt, rd, imm)) {
		cout << "UNKNOWN INSTRUCTION: SYNTAX ERROR" << endl;
		return false;
	}

	if (!getOpcode(name, opcode, funct,type)) {
		cout << "UNKNOWN INSTRUCTION: SYNTAX ERROR" << endl;
		return false;
	}

	opcode = (unsigned)opcode << 26;
	rs = rs << 21;
	rt = rt << 16;
	rd = rd << 11;

	if (type == 'R')
		inst = opcode | rs | rt | rd | funct;
	else if (type == 'I') {
		inst = opcode | rs | rt | imm;
	}
	else if(type=='J') {
		inst = opcode | imm;
	}
	
	return true;

}

void convertASM(vector <string> s, vector <int> & inst) {

	inst.clear();

	for (int i = 0; i < s.size(); i++)
		parseASM(s[i], inst[i]);
}

void ALU(int a, int b, int ALU_CT, int &result, int &z) {

	switch (ALU_CT) {
	case 0: 
		result = a + b;
		break;
	case 1:
		result = a - b;
		break;
	case 2:
		result = a ^ b;
		break;
	default:
		result = result;
	}
	if (result == 0) z = 1;
}

//int main() {
//
//	vector <string> s(5);
//	vector <int> inst(5);
//
//	for(int i=0;i<5;i++)
//	getline(cin, s[i]);
//
//	//if(parseASM(s, inst))
//
//	convertASM(s, inst);
//	for(int i=0;i<5;i++)
//	cout << "inst= " << bitset<32>(inst[i]) << endl;
//
//	system("pause");
//	return 0;
//
//
//}