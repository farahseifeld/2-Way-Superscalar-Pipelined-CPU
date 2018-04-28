#include<iostream>
#include"imem.h"
void main()
{
	imem memory;
	memory.WriteInst(0, 100);
	memory.WriteInst(4, 200);
	unsigned int inst;
	memory.IF2(4, inst);
	cout << inst << endl;
	system("pause");
}