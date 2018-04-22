#ifndef DMEM_H
#define DMEM_H
#include "cache.h"
class dmem
{
public:
	dmem();
	~dmem();
	void DF1(unsigned int& address_in, unsigned int& MEM_CT_in, unsigned int& data_wire);
	void DF2(unsigned int& address_in, unsigned int& MEM_CT_in, unsigned int& data_wire);
	void TC(unsigned int& address_in, unsigned int& MEM_CT_in, unsigned int& data_wire);
private:
	cache datacache;
	unsigned int data;
};
dmem::dmem()
{
	data = 0;
}
dmem::~dmem()
{
}
void dmem::DF1(unsigned int& address_in, unsigned int& MEM_CT_in, unsigned int& data_wire)
{
	//NOTHING
}
void dmem::DF2(unsigned int& address_in, unsigned int& MEM_CT_in, unsigned int& data_wire)
{
	if (MEM_CT_in >> 3 == 0)
	{
		data_wire = data;
	}
	else
	{
		if (MEM_CT_in == 0x00000002)//write
		{
			datacache.write(address_in, data_wire);
			data = data_wire;
		}
		else if (MEM_CT_in == 0x00000002)//read
		{
			unsigned int tag;
			int valid;
			datacache.read(address_in, tag, data, valid);
			data_wire = data;
		}
	}
}
void dmem::TC(unsigned int& address_in, unsigned int& MEM_CT_in, unsigned int& data_wire)
{
	//NOTHING
}
#endif
