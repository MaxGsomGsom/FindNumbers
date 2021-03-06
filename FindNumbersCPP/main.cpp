#include "stdafx.h"
#include <math.h>
#include <stdlib.h>
#include <omp.h> 

enum Op
{
	Sum = 0,
	Sub,
	Mul,
	Div,
	Pow,
	Concat
};

float toSearch = 10958;

int OpsNum = 9;
int OpdNum = 10;

long iter = 0;
Op* answerOps = new Op[OpsNum];
int* answerOrd = new int[OpsNum];
bool stop = false;

const char* op(Op value) {
	switch (value)
	{
	case Sum:
		return "+";
		break;
	case Sub:
		return "-";
		break;
	case Mul:
		return "*";
		break;
	case Div:
		return "/";
		break;
	case Pow:
		return "^";
		break;
	case Concat:
		return "";
		break;
	}
	return "";
}

//=========================

class Num
{
private:
	float val;
	Num* ref;

public:
	void Clear(float value)
	{
		ref = nullptr;
		val = value;
	}

	void SetRef(Num* val) {
		ref = val;
	}

	void SetValue(float value) {
		if (ref != nullptr)
			ref->SetValue(value);
		else
			val = value;
	}

	float GetValue() {
		if (ref != nullptr)
			return ref->GetValue();
		else
			return val;
	}
};

//=========================

void Calculate(Op* operations, int* order, Num* results)
{
	if (stop) return;

	//Fill array with 1,2,3,4,5,6,7,8,9
	int i = 0;
	for (; i < OpdNum; i++)
		results[i].Clear((float)i);

	//For every operation in selected order
	//Calculate it and write result to neighbor cells
	for (i = 0; i < OpsNum; i++)
	{
		switch (operations[order[i]])
		{
		case Sum:
			results[order[i]].SetValue(results[order[i]].GetValue() + results[order[i] + 1].GetValue());
			break;
		case Sub:
			results[order[i]].SetValue(results[order[i]].GetValue() - results[order[i] + 1].GetValue());
			break;
		case Mul:
			results[order[i]].SetValue(results[order[i]].GetValue() * results[order[i] + 1].GetValue());
			break;
		case Div:
			//check for 0 division
			if (results[order[i] + 1].GetValue() == 0)
				return;
			results[order[i]].SetValue(results[order[i]].GetValue() / results[order[i] + 1].GetValue());
			break;
		case Concat:
			results[order[i]].SetValue(results[order[i]].GetValue() * 10 + results[order[i] + 1].GetValue());
			break;
		case Pow:
			results[order[i]].SetValue(powf(results[order[i]].GetValue(), results[order[i] + 1].GetValue()));
			break;
		}

		//if too big
		if (results[order[i]].GetValue() > 1000000 || results[order[i]].GetValue() < -1000000)
			return;

		results[order[i] + 1].SetRef(&(results[order[i]]));
	}

	if (results[order[7]].GetValue() == toSearch)
	{
		memcpy(answerOps, operations, OpsNum * sizeof(Op));
		memcpy(answerOrd, order, OpsNum * sizeof(int));
		stop = true;
	}
};

//=========================

void IterateBraces(Op* operations, int* order, int orderIndex, Num* results)
{
	if (stop) return;
	//Calculate
	if (orderIndex == OpsNum)
	{
		Calculate(operations, order, results);
		return;
	}

	//Go deeper
	int i = 0;
	for (; i < OpsNum; i++)
	{
		//if concatenation found, go deeper, because it's required computation
		if (operations[i] == Concat
			&& order[0] != i && order[1] != i && order[2] != i && order[3] != i && order[4] != i && order[5] != i && order[6] != i && order[7] != i && order[8] != i)
		{
			order[orderIndex] = i;
			IterateBraces(operations, order, orderIndex + 1, results);
			return;
		}
	}
	for (i = 0; i < OpsNum; i++)
	{
		//check all other combinations
		if (order[0] != i && order[1] != i && order[2] != i && order[3] != i && order[4] != i && order[5] != i && order[6] != i && order[7] != i && order[8] != i)
		{
			order[orderIndex] = i;
			IterateBraces(operations, order, orderIndex + 1, results);
			order[orderIndex] = -1;
		}
	}
};

//=========================

void IterateOperations(Op* operations, int opIndex, Num* results, int* order, Op firstOp = Sum, Op secondOp = Sum)
{
	if (stop) return;
	iter++;

	if (iter % 10 < 1)
		printf("%d", iter);

	//Check combination
	for (int i = 0; i < OpsNum; i++)
		order[i] = -1;
	int orderIndex = 0;

	IterateBraces(operations, order, orderIndex, results);

	//Go deeper
	if (opIndex == OpsNum)
		return;

	if (opIndex == 0)
	{
		operations[0] = firstOp;
		operations[1] = secondOp;
		IterateOperations(operations, 2, results, order);
	}
	else
	{
		for (int i = 0; i < 6; i++)
		{
			operations[opIndex] = (Op)i;
			IterateOperations(operations, opIndex + 1, results, order);
		}
	}
};

//=========================

int main()
{
	printf("Number to search:\n");
	scanf_s("%f", &toSearch);

	//Main loop
	#pragma omp parallel for
	for (int i = 0; i < 12; i++)
	{
		Op* operations = new Op[OpsNum]; //between digits
		int* order = new int[OpsNum];
		for (int i = 0; i < OpsNum; i++)
			operations[i] = Sum;
		Num* result = new Num[OpdNum];
		IterateOperations(operations, 0, result, order, (Op)(i % 2), (Op)(i % 6));
	}

	//Print result
	printf("0%s1%s2%s3%s4%s5%s6%s7%s8%s9", op(answerOps[0]), op(answerOps[1]), op(answerOps[2]), op(answerOps[3]), op(answerOps[4]),
		op(answerOps[5]), op(answerOps[6]), op(answerOps[7]), op(answerOps[8]));
	printf(", order: %d-%d-%d-%d-%d-%d-%d-%d-%d\n", answerOrd[0], answerOrd[1], answerOrd[2], answerOrd[3], answerOrd[4], answerOrd[5],
		answerOrd[6], answerOrd[7], answerOrd[8]);

	printf("Completed!\n");

	int temp;
	scanf_s("%d", &temp);

	return 0;
}

