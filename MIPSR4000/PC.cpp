#include<iostream>
#include<string>
using namespace std;
unsigned int pc(int oldPC, bool rst , unsigned int BO, unsigned int JCT, bool BEN, unsigned int IM1, unsigned int IM2, bool FEN)
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
			cout << t<<endl;
			return t | IM1;

		}
		else if (JCT == 3)
			return IM2;
	}
}
int main()
{
	cout << pc(34, 0, 6, 1, 1, 888888, 789 ,1);
	system("pause");
	return 0;
}