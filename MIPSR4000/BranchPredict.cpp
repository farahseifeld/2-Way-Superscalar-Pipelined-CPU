int branchPredict(int prev, bool taken)
{
	if (prev == 0)
	{
		if (taken)
			return 0;
		else
			return 1;
	}
	else if (prev == 1)
	{
		if (taken)
			return 0;
		else
			return 2;
	}
	else if (prev == 2)
	{
		if (taken)
			return 1;
		else
			return 3;
	}
	else if (prev == 3)
	{
		if (taken)
			return 2;
		else
			return 3;
	}
}