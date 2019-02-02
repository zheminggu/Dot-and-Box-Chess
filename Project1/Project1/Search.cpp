#include<stdio.h>
#include<string.h>
#include<time.h>
#include"Moves.h"
#include"Define.h"
#include"Evaluate.h"
#include"UCTSearch.h"

int chainstack[20][3];     //���������Ϣ
time_t start, end;
int GlobalAlpha;

//��ʱmap&state��ʼ��
void init_temmap()
{
	for (int i = 0; i < 13; i++)
	{
		for (int j = 0; j < 13; j++)
		{
			TemMap[i][j] = Map[i][j];
			TemState[i][j] = State[i][j];
		}
	}
}

//����ʣ��Step
int GetStep()
{
	init_temmap();
	int count = 0;
	for (int i = 1; i <= 11; i++)
	{
		for (int j = 1; j <= 11; j++)
		{
			if (TemMap[i][j] == 0)
			{
				if (TemState[i][j] == LINEONE)
				{
					if (TemState[i + 1][j] < 2 && TemState[i - 1][j] < 2)
					{
						count++;
						TemState[i + 1][j]++;
						TemState[i - 1][j]++;
						TemMap[i][j] = LINEONE;
					}
				}
				else
				{
					if (TemState[i][j + 1] < 2 && TemState[i][j - 1] < 2)
					{
						count++;
						TemState[i][j + 1]++;
						TemState[i][j - 1]++;
						TemMap[i][j] = LINETWO;
					}
				}
			}
		}
	}
	return count;
}

//ȷ��������������&��
int dpMakeMove(int a, int b)
{
	int count = 0;
	int flag = 0;    //�����Ը�Ϊ2,˵��Ϊ��,count��100���
	if (TemState[a][b] == LINEONE)
	{
		TemMap[a][b] = LINEONE;
		TemState[a + 1][b]++;
		TemState[a - 1][b]++;
		if (TemState[a + 1][b] == 3)
		{
			for (int k = 0; k < 4; k++)
			{
				if (TemMap[a + 1 + dx[k]][b + dy[k]] == 0)
				{
					count += dpMakeMove(a + 1 + dx[k], b + dy[k]);
				}
			}
		}
		else if (TemState[a + 1][b] == 4)
		{
			count++;
			flag++;
		}
		if (TemState[a - 1][b] == 3)
		{
			for (int k = 0; k < 4; k++)
			{
				if (TemMap[a - 1 + dx[k]][b + dy[k]] == 0)
				{
					count += dpMakeMove(a - 1 + dx[k], b + dy[k]);
				}
			}
		}
		else if (TemState[a - 1][b] == 4)
		{
			count++;
			flag++;
		}
	}
	else
	{
		TemMap[a][b] = LINETWO;
		TemState[a][b + 1]++;
		TemState[a][b - 1]++;
		if (TemState[a][b + 1] == 3)
		{
			for (int k = 0; k < 4; k++)
			{
				if (TemMap[a + dx[k]][b + 1 + dy[k]] == 0)
				{
					count += dpMakeMove(a + dx[k], b + 1 + dy[k]);
				}
			}
		}
		else if (TemState[a][b + 1] == 4)
		{
			count++;
			flag++;
		}
		if (TemState[a][b - 1] == 3)
		{
			for (int k = 0; k < 4; k++)
			{
				if (TemMap[a + dx[k]][b - 1 + dy[k]] == 0)
				{
					count += dpMakeMove(a + dx[k], b - 1 + dy[k]);
				}
			}
		}
		else if (TemState[a][b - 1] == 4)
		{
			count++;
			flag++;
		}
	}
	if (flag == 2)
	{
		count += 100;
	}
	return count;
}

//�ж��͸���Ŀ;1�����,����0;2�����ϲ���,����1
int dpAlphaBeta()
{
	int flag = 0;    //1����2��4�񻷿���,����0;2����������6�񻷼����ϲ���,����1
	init_temmap();
	for (int i = 2; i <= 10; i += 2)
	{
		for (int j = 2; j <= 10; j += 2)
		{
			if (TemState[i][j] == 3)
			{
				for (int k = 0; k < 4; k++)
				{
					if (TemMap[i + dx[k]][j + dy[k]] == 0)
					{
						int tem = dpMakeMove(i + dx[k], j + dy[k]);
						if (tem > 104 || (tem > 1 && tem < 100))
						{
							return 1;
						}
					}
				}
			}
		}
	}
	return 0;
}

int AlphaBeta(int depth, int alpha, int beta, int color, int eat, int last)
{
	if (judge() == 1)
	{
		return Evaluate(color) + eat * 10;
	}
	if (depth == 0)
	{
		return eat * 10;
	}
	int val;
	MoveType nowmove;

	for (int i = 1; i <= 11; i++)
	{
		for (int j = 1; j <= 11; j++)
		{
			if ((State[i][j] == 11 || State[i][j] == 22) && Map[i][j] == 0)
			{
				nowmove.a = i;
				nowmove.b = j;
				nowmove.color = color;
				int temeat;
				temeat = MakeMove(nowmove, last);
				if (temeat > 0)
				{
					val = AlphaBeta(depth, alpha, beta, color, eat + temeat, color);
					UnMakeMove(nowmove);
					if (val >= beta)
					{
						return beta;
					}
					if (val > alpha)
					{
						alpha = val;
					}
					return alpha;
				}
				else
				{
					UnMakeMove(nowmove);
				}
			}
		}
	}

	for (int i = 1; i <= 11; i++)
	{
		for (int j = 1; j <= 11; j++)
		{
			if ((State[i][j] == 11 || State[i][j] == 22) && Map[i][j] == 0)
			{
				nowmove.a = i;
				nowmove.b = j;
				nowmove.color = color;
				MakeMove(nowmove, last);
				if (dpAlphaBeta() == 1)
				{
					UnMakeMove(nowmove);
					continue;
				}
				val = -AlphaBeta(depth - 1, -beta, -alpha, 1 - color, -eat, color);
				UnMakeMove(nowmove);
				if (val >= beta)
				{
					return beta;
				}
				if (val > alpha)
				{
					alpha = val;
				}
			}
		}
	}
	return alpha;
}

void AlphaBetaFirst(int depth, int alpha, int beta, int color)
{
	int val;
	MoveType nowmove;
	for (int i = 1; i <= 11; i++)
	{
		for (int j = 1; j <= 11; j++)
		{
			if ((State[i][j] == 11 || State[i][j] == 22) && Map[i][j] == 0)
			{
				nowmove.a = i;
				nowmove.b = j;
				nowmove.color = color;
				int eat;
				eat = MakeMove(nowmove, 1 - color);
				if (dpAlphaBeta() == 1)      //�͸��֦
				{
					UnMakeMove(nowmove);
					continue;
				}
				if (eat > 0)
				{
					val = AlphaBeta(depth - 1, alpha, beta, color, eat, color);
					UnMakeMove(nowmove);
				}
				else
				{
					val = -AlphaBeta(depth - 1, -beta, -alpha, 1 - color, 0, color);
					UnMakeMove(nowmove);
				}
				if (val > alpha)
				{
					alpha = val;
					GlobalAlpha = val;
					HopeVal = val;
					BestMove = nowmove;
				}
			}
			/*end = time(NULL);
			if (end - start > 4)
			{
			return;
			}*/
		}
	}
	return;
}

//type=1:������,�����ɳ����������з�;type=2:ֻ�����ɳ���;
int dp(int type)
{
	int flag = 0;    //�Ƿ��пɳ���������̳��ȿ�Ϊ1��
	memset(chainstack, 0, sizeof(chainstack));

	init_temmap();
	for (int i = 2; i <= 10; i += 2)
	{
		for (int j = 2; j <= 10; j += 2)
		{
			if (TemState[i][j] == 3)
			{
				for (int k = 0; k < 4; k++)
				{
					if (TemMap[i + dx[k]][j + dy[k]] == 0)
					{
						chainstack[0][0]++;    //����Ŀ
						chainstack[chainstack[0][0]][0] = dpMakeMove(i + dx[k], j + dy[k]);     //����
						if (chainstack[chainstack[0][0]][0] == 2)
						{
							chainstack[0][1]++;      //����Ϊ2��������Ŀ
						}
						chainstack[chainstack[0][0]][1] = i + dx[k];
						chainstack[chainstack[0][0]][2] = j + dy[k];
						flag = 1;
					}
				}
			}
		}
	}

	init_temmap();
	if (type == 1)
	{
		//����Ϊ1ֱ�ӳ�
		for (int i = 1; i <= chainstack[0][0]; i++)
		{
			if (chainstack[i][0] == 1)
			{
				BestMove.a = chainstack[i][1];
				BestMove.b = chainstack[i][2];
				return flag;
			}
		}
		//��������2�Ҳ�Ϊ����1��
		for (int i = 1; i <= chainstack[0][0]; i++)
		{
			if (chainstack[i][0] > 2 && chainstack[i][0] < 100)
			{
				BestMove.a = chainstack[i][1];
				BestMove.b = chainstack[i][2];
				return flag;
			}
		}
		//����Ϊ2��2
		for (int i = 1; i <= chainstack[0][0]; i++)
		{
			if (chainstack[i][0] == 2)
			{
				int deadbox = 0;
				for (int i = 2; i <= 10; i += 2)
				{
					for (int j = 2; j <= 10; j += 2)
					{
						if (Map[i][j] == -77)
						{
							deadbox++;
						}
					}
				}
				//�վֲ���
				if (deadbox + 2 == 25)
				{
					BestMove.a = chainstack[i][1];
					BestMove.b = chainstack[i][2];
					return flag;
				}
				if (chainstack[0][1] > 1)
				{
					BestMove.a = chainstack[i][1];
					BestMove.b = chainstack[i][2];
					return flag;
				}
				int a = chainstack[i][1];
				int b = chainstack[i][2];
				if (TemState[a][b] == LINEONE)
				{
					TemMap[a][b] = LINEONE;
					TemState[a + 1][b]++;
					TemState[a - 1][b]++;
					if (TemState[a + 1][b] == 3)
					{
						for (int k = 0; k < 4; k++)
						{
							if (TemMap[a + 1 + dx[k]][b + dy[k]] == 0)
							{
								BestMove.a = a + 1 + dx[k];
								BestMove.b = b + dy[k];
							}
						}
					}
					if (TemState[a - 1][b] == 3)
					{
						for (int k = 0; k < 4; k++)
						{
							if (TemMap[a - 1 + dx[k]][b + dy[k]] == 0)
							{
								BestMove.a = a - 1 + dx[k];
								BestMove.b = b + dy[k];
							}
						}
					}
				}
				else
				{
					TemMap[a][b] = LINETWO;
					TemState[a][b + 1]++;
					TemState[a][b - 1]++;
					if (TemState[a][b + 1] == 3)
					{
						for (int k = 0; k < 4; k++)
						{
							if (TemMap[a + dx[k]][b + 1 + dy[k]] == 0)
							{
								BestMove.a = a + dx[k];
								BestMove.b = b + 1 + dy[k];
							}
						}
					}
					if (TemState[a][b - 1] == 3)
					{
						for (int k = 0; k < 4; k++)
						{
							if (TemMap[a + dx[k]][b - 1 + dy[k]] == 0)
							{
								BestMove.a = a + dx[k];
								BestMove.b = b - 1 + dy[k];
							}
						}
					}
				}
				return flag;
			}
		}
		//��������5�Ļ���Ϊ3Ϊ2�Ļ���1��
		for (int i = 1; i <= chainstack[0][0]; i++)
		{
			if (chainstack[i][0] >= 105 || chainstack[i][0] == 103 || chainstack[i][0] == 102)
			{
				BestMove.a = chainstack[i][1];
				BestMove.b = chainstack[i][2];
				return flag;
			}
		}
		//����Ϊ4��4
		for (int i = 1; i <= chainstack[0][0]; i++)
		{
			if (chainstack[i][0] == 104)
			{
				int deadbox = 0;
				for (int i = 2; i <= 10; i += 2)
				{
					for (int j = 2; j <= 10; j += 2)
					{
						if (Map[i][j] == -77)
						{
							deadbox++;
						}
					}
				}
				//�վֲ���
				if (deadbox + 4 == 25)
				{
					BestMove.a = chainstack[i][1];
					BestMove.b = chainstack[i][2];
					return flag;
				}
				int a = chainstack[i][1];
				int b = chainstack[i][2];
				if (TemState[a][b] == LINEONE)
				{
					TemMap[a][b] = LINEONE;
					TemState[a + 1][b]++;
					TemState[a - 1][b]++;
					if (TemState[a + 1][b] == 3)
					{
						for (int k = 0; k < 4; k++)
						{
							if (TemMap[a + 1 + dx[k]][b + dy[k]] == 0)
							{
								BestMove.a = a + 1 + dx[k];
								BestMove.b = b + dy[k];
							}
						}
					}
					if (TemState[a - 1][b] == 3)
					{
						for (int k = 0; k < 4; k++)
						{
							if (TemMap[a - 1 + dx[k]][b + dy[k]] == 0)
							{
								BestMove.a = a - 1 + dx[k];
								BestMove.b = b + dy[k];
							}
						}
					}
				}
				else
				{
					TemMap[a][b] = LINETWO;
					TemState[a][b + 1]++;
					TemState[a][b - 1]++;
					if (TemState[a][b + 1] == 3)
					{
						for (int k = 0; k < 4; k++)
						{
							if (TemMap[a + dx[k]][b + 1 + dy[k]] == 0)
							{
								BestMove.a = a + dx[k];
								BestMove.b = b + 1 + dy[k];
							}
						}
					}
					if (TemState[a][b - 1] == 3)
					{
						for (int k = 0; k < 4; k++)
						{
							if (TemMap[a + dx[k]][b - 1 + dy[k]] == 0)
							{
								BestMove.a = a + dx[k];
								BestMove.b = b - 1 + dy[k];
							}
						}
					}
				}
				return flag;
			}
		}
	}
	else if (type == 2)
	{
		return flag;
	}
	return flag;
}

void Search(int color)
{
	GlobalAlpha = -999;
	BestMove.color = color;
	UCTSearch();
	//if (judge() == 0)
	//{
	//	if (dp(2) == 1)
	//	{
	//		//����Ϊ1ֱ�ӳ�
	//		for (int i = 1; i <= chainstack[0][0]; i++)
	//		{
	//			if (chainstack[i][0] == 1)
	//			{
	//				BestMove.a = chainstack[i][1];
	//				BestMove.b = chainstack[i][2];
	//				return;
	//			}
	//		}
	//		//��������2�Ҳ�Ϊ����1��
	//		for (int i = 1; i <= chainstack[0][0]; i++)
	//		{
	//			if (chainstack[i][0] > 2 && chainstack[i][0] < 100)
	//			{
	//				BestMove.a = chainstack[i][1];
	//				BestMove.b = chainstack[i][2];
	//				return;
	//			}
	//		}
	//		//����Ϊ2������������1��һ����
	//		for (int i = 1; i <= chainstack[0][0]; i++)
	//		{
	//			if (chainstack[i][0] == 2 && chainstack[0][1] > 1)
	//			{
	//				BestMove.a = chainstack[i][1];
	//				BestMove.b = chainstack[i][2];
	//				return;
	//			}
	//		}
	//		//��������5�Ļ���Ϊ3Ϊ2�Ļ���1��
	//		for (int i = 1; i <= chainstack[0][0]; i++)
	//		{
	//			if (chainstack[i][0] >= 105 || chainstack[i][0] == 103 || chainstack[i][0] == 102)
	//			{
	//				BestMove.a = chainstack[i][1];
	//				BestMove.b = chainstack[i][2];
	//				return;
	//			}
	//		}

	//	}

	//	int depth = 6;
	//	Step = GetStep();
	//	/*if (Step < 120)
	//	{
	//	start = time(NULL);
	//	end = time(NULL);
	//	while (end - start < 10)
	//	{
	//	AlphaBetaFirst(depth++, GlobalAlpha, 999, color);
	//	end = time(NULL);
	//	}

	//	}*/
	//	if (Step < 10)
	//	{
	//		AlphaBetaFirst(12, GlobalAlpha, 999, color);
	//	}
	//	else
	//	{
	//		AlphaBetaFirst(6, GlobalAlpha, 999, color);
	//	}
	//	/*else
	//	{
	//	UCTSearch();
	//	}*/

	//	return;
	//}
	//else
	//{
	//	init_temmap();

	//	if (dp(1) == 1)
	//	{
	//		return;
	//	}
	//	if (trap2() > 0)
	//	{
	//		for (int i = 1; i <= TrapStack[0][0]; i++)
	//		{
	//			if (TrapStack[i][0] == 1)
	//			{
	//				BestMove.a = TrapStack[i][1];
	//				BestMove.b = TrapStack[i][2];
	//				TrapStack[i][0] = 0;
	//				return;
	//			}
	//		}
	//		for (int i = 1; i <= TrapStack[0][0]; i++)
	//		{
	//			if (TrapStack[i][0] == 2)
	//			{
	//				BestMove.a = TrapStack[i][1];
	//				BestMove.b = TrapStack[i][2];
	//				TrapStack[i][0] = 0;
	//				return;
	//			}
	//		}
	//	}
	//}
}