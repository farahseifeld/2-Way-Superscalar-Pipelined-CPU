#pragma once
#ifndef CACHE_H
#define CACHE_H
#include <iostream>
using namespace std;
class cache
{
public:
	cache();
	~cache();
	void read(unsigned int address, unsigned int& tag, unsigned int& data, int& valid);
	void write(unsigned int address, unsigned int data);
private:
	struct cacheCell
	{
		int valid = 0;
		unsigned int tag = 0;
		unsigned int data;
	};
	int size = 1000;
	unsigned int tagBits, indexBits, offsetBits = 5;
	cacheCell cacheArr[1000];
};
#endif
#include<cmath>
cache::cache()
{
	int blocks = size / 32;
	//cacheArr = new  cacheCell[blocks];
	indexBits = log2(blocks);
	tagBits = 32 - indexBits - offsetBits;
}
cache::~cache()
{
	//delete[] cacheArr;
}
void cache::read(unsigned int address, unsigned int& tag, unsigned int& data, int& valid)
{
	unsigned int temp, offset, index, tagaddress;
	index = address / 4;
	//cout << "index = " << index << endl;
	//cout << "address:" << address << endl;
	//tag = cacheArr[index].tag;
	data = cacheArr[index].data;
	valid = cacheArr[index].valid;
}
void cache::write(unsigned int address, unsigned int data)
{
	unsigned int temp, offset, index, tagaddress;
	index = address / 4;
	//cacheArr[index].tag = tagaddress;
	cacheArr[index].data = data;
	cacheArr[index].valid = 1;
}