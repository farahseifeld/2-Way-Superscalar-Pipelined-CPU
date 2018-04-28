#ifndef BUFFER_2_H
#define BUFFER_2_H
class buffer_2
{
public:
	buffer_2();
	~buffer_2();
	void inputData(unsigned int,unsigned int);
	void updateData(unsigned int stallD);
	void clr();
	unsigned int inst, instD,PCPlus4,PCPlus4D;
private:

};

buffer_2::buffer_2()
{
	instD = 0;
	PCPlus4D = 0;
}

buffer_2::~buffer_2()
{
}
void buffer_2::inputData(unsigned int instruction, unsigned int PC)
{
	inst = instruction;
	PCPlus4 = PC;
}
void buffer_2::updateData(unsigned int stallD)
{
	if (stallD == 0)
	{
		instD = inst;
		PCPlus4D = PCPlus4;
	}
}
void buffer_2::clr()
{
	instD = 0;
	PCPlus4D = 0;
}
#endif
