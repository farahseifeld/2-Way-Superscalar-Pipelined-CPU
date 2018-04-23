/*
This buffer is between the 2 Instruction fetch stages IF & ID
*/
#ifndef BUFFER_1_H
#define BUFFER_1_H
class buffer_1
{
public:
	buffer_1();
	~buffer_1();
	unsigned int PC_in,PC_out;
	void inputData(unsigned int);
	void updateData(unsigned int);
private:

};

buffer_1::buffer_1()
{
	PC_in = 0;
	PC_out = 0;
}

buffer_1::~buffer_1()
{
}
void buffer_1::inputData(unsigned int PC)
{
	PC_in = PC;
}
void buffer_1::updateData(unsigned int stallD)
{
	if(stallD==0)
	PC_out = PC_in;
}
#endif
