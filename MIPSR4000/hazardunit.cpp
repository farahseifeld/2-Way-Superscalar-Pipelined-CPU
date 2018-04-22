#include <iostream>
#include <vector>
#include <bitset>

using namespace std;

void hazardUnit(int rsD, int rtD, int rsE, int rtE,
	int rDE, int rDM, int rDW, int d1E, int d1M, int d1W, 
	int ddE, int ddM, int ddW, int branchD,int jumpD, 
	int &forwardA, int & forwardB, bool &flush, bool &stall) {

	if (rsE != 0 && rsE == rDM && d1M == 1)
		forwardA = 2;
	else if (rsE != 0 && rsE == rDW && d1W == 1)
		forwardA = 1;
	else
		forwardA = 0;

	if (rtE != 0 && rtE == rDM && d1M == 1)
		forwardB = 2;
	else if (rtE != 0 && rtE == rDW && d1W == 1)
		forwardB = 1;
	else
		forwardB = 0;

	bool lwstall = (rsD == rtE || rtD == rtE) && (d1M==1);
	bool branchstall = ((branchD == 1) && (d1E == 1) && (rDE == rsD || rDE == rtD)) || ((branchD == 1) && (ddE == 1) && (rDM==rsD || rDM==rtD));

	stall = lwstall || branchstall;
	flush = lwstall || branchstall || (jumpD == 1);
}

//int main() {
//
//	int rsD = 5;
//	int rtD = 2;
//	int rsE = 5;
//	int rtE = 2;
//	int rDE = 5;
//	int rDM = rDE;
//	int rDW = rDE;
//	int branchD = 0;
//	int jumpD = 1;
//	int ddm=1, ddw=1, dde=1,d1E=1,d1W=1,d1M = 1;
//	int forwardA, forwardB;
//	bool flush  , stall;
//	hazardUnit(rsD, rtD, rsE, rtE,
//		rDE, rDM, rDW, d1E, d1M, d1W,
//		dde, ddm, ddw, branchD, jumpD,
//		forwardA, forwardB, flush, stall);
//		cout << forwardA << " " << forwardB << endl;
//	cout << flush << " " <<stall;
//		system("pause");
//	return 0;
//}