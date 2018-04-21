#ifndef CACHE_H
#define CACHE_H
class cache
{
public:
	cache(int size);
	~cache();
	void read(int address, unsigned int& tag, unsigned int& data,int& valid);
	void write(int address, int tag, int data);
private:
	struct cacheCell
	{
		int valid = 0;
		unsigned int tag = 0;
		unsigned int data;
	};
	int size;
	unsigned int tagBits, indexBits, offsetBits=5;
	cacheCell* cacheArr;
};
#endif
#include<cmath>
cache::cache(int n)
{
	size = n;
	int blocks = size / 32;
	cacheArr = new  cacheCell[blocks];
	indexBits = log2(blocks);
	tagBits = 32 - indexBits - offsetBits;
}
cache::~cache()
{
	delete[] cacheArr;
}
void cache::read(int address, unsigned int& tag, unsigned int& data, int& valid)
{
	unsigned int temp, offset, index, tagaddress;
	offset = address & 31;
	address = address >> offsetBits;
	temp = pow(2, indexBits) - 1;
	index = address & temp;
	tag = cacheArr[index].tag;
	data = cacheArr[index].data;
	valid = cacheArr[index].valid;
}
void cache::write(int address, int tag, int data)
{
	unsigned int temp, offset, index, tagaddress;
	offset = address & 31;
	address = address >> offsetBits;
	temp = pow(2, indexBits) - 1;
	index = address & temp;
	address = address >> indexBits;
	tagaddress = address;
	cacheArr[index].tag = tagaddress;
	cacheArr[index].data = data;
	cacheArr[index].valid = 1;
}