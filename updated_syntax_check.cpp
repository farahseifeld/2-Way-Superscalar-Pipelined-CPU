#include<iostream>
#include<fstream>
#include<string>
#include<vector>
using namespace std;
bool allspace(string l)
{
	for (int x = 0; x < l.length(); x++)
	{
		if (l[x] != ' ')
			return false;

	}
};

string removespace(string str1)
{
	string x;
	for (int i = 0; i < str1.length(); i++)
	{
		if (str1[i] != ' ')
			x += str1[i];
	}
	return x;
};
bool InVector(vector <string> vec, string str)
{
	for (int j = 0; j < vec.size(); j++)
	{
		if (vec.at(j) == str)
			return true;
	}
	return false;
};
void check(bool&flag)
{
	vector <string> v;
	ifstream myReadFile;
	myReadFile.open("text.txt");
	string inst;
	string l;
	int i;
	flag = true;
	string arr[3];
	bool felse;
	int found;
	string str;
	if (myReadFile.is_open()) {
		while (!myReadFile.eof()) {

			arr[0] = ""; arr[1] = ""; arr[2] = "";
			myReadFile >> inst;
			i = 0;
			if (inst == "add" || inst == "xor" || inst == "slt")
			{
				getline(myReadFile, l);
				for (int x = 0; x < l.length(); x++)
				{
					if (l[x] != ',')
						arr[i] += l[x];
					else
						i++;
				}
				//cout << arr[0] << arr[1] << arr[2] << endl;
				if ((l == ""))
					flag = false;
				else if (allspace(l))
					flag = false;
				else if (!((stoi(arr[0]) <= 15) && (stoi(arr[1]) <= 15) && (stoi(arr[2]) <= 15)))
				{
					flag = false;
					cout << "add" << endl;
				}
				else
					flag = true;
			}
			else if (inst == "addi")
			{
				string x; bool b = true;
				getline(myReadFile, l);
				for (int x = 0; x < l.length(); x++)
				{
					if ((l[x] != ','))
						arr[i] += l[x];
					else
						i++;
				}
				x = arr[2];
				//cout << "x=" <<x;
				//cout << "Xvals" << endl;
				//for (int a = 0; a < x.length(); a++)
				//{
				//	//cout << x[a]<<endl;
				//	if (/*x[a] != ' ' ||*/ !(isdigit(x[a]))) {
				//		b = false;
				//		
				//	} 

				//}
				//cout <<"b="<< b;
				if ((l == ""))
					flag = false;
				else if (allspace(l))
					flag = false;
				else if (!((stoi(arr[0]) <= 15) && (stoi(arr[1]) <= 15) /*&& (b)*/))
				{
					flag = false;
					cout << "addi" << endl;;
				}

			}
			else if ((inst == "lw") || (inst == "sw"))
			{
				getline(myReadFile, l);
				for (int x = 0; x < l.length() - 1; x++)
				{
					if (l[x] != ',' && l[x] != '(')
						arr[i] += l[x];
					else
						i++;
				}
				if ((l == ""))
					flag = false;
				else if (allspace(l))
					flag = false;
				else if (!((stoi(arr[0]) <= 15) && ((stoi(arr[1])) % 4 == 0) && (stoi(arr[2]) <= 15)))
					flag = false;
			}
			else if (inst == "jr")
			{
				//cout << "in";
				getline(myReadFile, l);
				//cout << l;
				if ((l == ""))
					flag = false;
				else if (allspace(l))
					flag = false;
				else if (stoi(l)>15)
					flag = false;
			}
			else if (inst == "j")
			{

				getline(myReadFile, l);
				//cout << l;

				if ((l == ""))
				{
					flag = false;
					cout << "1";
				}
				else if (allspace(l))
				{
					flag = false;
					cout << "2";
				}
				else
				{
					//cout << l;

					if (InVector(v, removespace(l)) == false)
					{
						flag = false;
						cout << "3" << endl;
					}


				}
			}

			else if (inst == "ble")
			{
				getline(myReadFile, l);
				for (int x = 0; x < l.length(); x++)
				{
					if (l[x] != ',' && l[x] != '(')
						arr[i] += l[x];
					else
						i++;
				}
				cout << arr[0] << "?" << arr[1] << "?" << arr[2] << "?" << endl;
				if ((l == ""))
					flag = false;
				else if (allspace(l))
					flag = false;
				else if (arr[2] == "" || allspace(arr[2]) || (InVector(v, removespace(arr[2])) == false))
					flag = false;
				else if (!((stoi(arr[0]) <= 15) && (stoi(arr[0]) <= 15)))
					flag = false;

			}
			else
			{
				getline(myReadFile, l);
				inst += l;
				for (int i = 0; i < inst.length(); i++)
				{
					if (inst[i] == ':');
					felse = true;
				}
				if (felse)
				{
					found = inst.find(':');
					for (int i = 0; i < found; i++)
					{
						str += inst[i];
					}
					v.push_back(str);


				}
				else
					flag = false;
			}

		}
	}
	myReadFile.close();
}
int main()
{
	bool f;
	check(f);
	if (f == true)
		cout << "C" << endl;
	else
		cout << "W" << endl;
	system("pause");
	return 0;
}