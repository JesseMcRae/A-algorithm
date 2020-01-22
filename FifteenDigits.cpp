#include <stdio.h>
#include <stdlib.h>
const int n = 3;
const int num_open = 10000;//open表最大编号
const int num_close = 10000;//close表最大编号
int si[n][n] = { 1, 2, 3, 4, 5, 6, 7, 8, 0 };//{ 11,9,4,15,1,3,0,12,7,5,8,6,13,2,10,14 };//
int sn[n][n] = { 1, 2, 3, 4, 0, 5, 6, 7, 8 };//{ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0 };//数码的目标状态
int goal[n * n][2];//各个数字应在位置对照表
//--------------------------------------------------
struct ARRAY {
	int s[n][n];//数码二元数组
	int place0[2];//记录0所在的位置
	int d;//深度
	int h;//矩阵中对应数字的距离的个数
	struct ARRAY* prior;//指向扩展之前的数组
}open[num_open], close[num_close];
struct TABLE {//创建open表和close表的结构体
	struct ARRAY* a;
	int k;
};
struct TABLE* popen = (struct TABLE*)malloc(sizeof(struct TABLE));
struct TABLE* pclose = (struct TABLE*)malloc(sizeof(struct TABLE));
//--------------------------------------------------
void getgoal()//计算goal
{
	for (int k = 0; k < n * n; k++)
	{
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				if (k == sn[i][j])
				{
					goal[k][0] = i;
					goal[k][1] = j;
				}
			}
		}
	}
	return;
}
//--------------------------------------------------
int hx(int s[][n])//启发函数(曼哈顿距离)小于等于实际总步数
{
	int cost = 0;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			int w = s[i][j];
			cost += abs(i - goal[w][0]) + abs(j - goal[w][1]);
		}
	}
	return cost;
}
//--------------------------------------------------
void insert_sort()//用插入排序对open表中的数组按照f的大小从小到大排序
{
	for (int i = 0; i < popen->k; i++)
	{
		struct ARRAY temp = popen->a[i];
		int j = i;
		while (j > 0 && popen->a[j - 1].d + popen->a[j - 1].h > temp.d + temp.h)
		{
			popen->a[j] = popen->a[j - 1];
			j--;
		}
		popen->a[j] = temp;
	}
	return;
}
//--------------------------------------------------
void output_array(int s[][n])//输出数码状态
{
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
			printf("%5d", s[i][j]);
		printf("\n");
	}
	return;
}
//--------------------------------------------------
int is_equal(int s1[][n], int s[][n])//判断两个数组是否相等，用于搜索open表和close表
{
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (s1[i][j] != s[i][j])
				return 0;
		}
	}
	return 1;
}
//--------------------------------------------------
void reverint(struct ARRAY* sn)//把目标数组通过先驱节点反序输出
{
	struct ARRAY* cur, * pre, * tem;
	pre = sn;
	cur = pre->prior;
	pre->prior = NULL;
	while (cur->prior != NULL)
	{
		tem = cur->prior;
		cur->prior = pre;
		pre = cur;
		cur = tem;
	}
	cur->prior = pre;
	while (cur != NULL)
	{
		printf("第%d次移动:\n", cur->d);
		output_array(cur->s);
		cur = cur->prior;
	}
	return;
}
//--------------------------------------------------
void delete_table(struct TABLE* t, int d)//从表中删除数组
{
	for (int i = d; i < t->k; i++)
		t->a[i] = t->a[i + 1];
	t->k--;
	return;
}
//--------------------------------------------------
void initial_array()//初始化数组
{
	//printf("输入初始数组\n");
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			popen->a->s[i][j] = si[i][j];
			//scanf_s("%d", &open->a->s[i][j], 1);
			if (si[i][j] == 0)
			{
				popen->a->place0[0] = i;
				popen->a->place0[1] = j;
			}
		}
	}
	popen->a->d = 0;
	popen->a->h = hx(si);
	popen->a->prior = NULL;
	popen->k = 1;
	printf("初始状态：\n");
	output_array(si);
	printf("目标状态：\n");
	output_array(sn);
	return;
}
//--------------------------------------------------
void swap(int* a, int* b)
{
	int tmp;
	tmp = *a;
	*a = *b;
	*b = tmp;
	return;
}
//--------------------------------------------------
int up(struct ARRAY* sn)//0上移
{
	int x = sn->place0[0];
	int y = sn->place0[1];
	if (x == 0)//0在最上面,不能上移
		return 0;
	swap(&sn->s[x][y], &sn->s[x - 1][y]);
	sn->place0[0]--;
	return 1;
}
//-----------------------------------------------
int down(struct ARRAY* sn)//0下移
{
	int x = sn->place0[0];
	int y = sn->place0[1];
	if (x == n - 1)//0在最下面,不能下移
		return 0;
	swap(&sn->s[x][y], &sn->s[x + 1][y]);
	sn->place0[0]++;
	return 1;
}
//-----------------------------------------------
int left(struct ARRAY* sn)//0左移
{
	int x = sn->place0[0];
	int y = sn->place0[1];
	if (y == 0)//0在最左侧,不能左移
		return 0;
	swap(&sn->s[x][y], &sn->s[x][y - 1]);
	sn->place0[1]--;
	return 1;
}
//-----------------------------------------------
int right(struct ARRAY* sn)//0右移
{
	int x = sn->place0[0];
	int y = sn->place0[1];
	if (y == n - 1)//0在最右侧,不能右移
		return 0;
	swap(&sn->s[x][y], &sn->s[x][y + 1]);
	sn->place0[1]++;
	return 1;
}
//--------------------------------------------------
int search_table(struct TABLE* t, struct ARRAY* sn)//在open表中查找移动的数组是否之前出现过
{
	for (int i = 0; i < t->k; i++)
	{
		if (is_equal(t->a[i].s, sn->s))
		{
			if (sn->d < t->a[i].d)
			{
				delete_table(t, i);
				return 2;
			}
			else
				return 0;
		}
	}
	return 1;
}
//--------------------------------------------------
void add_table(struct TABLE* t, struct ARRAY* s)//把数组加入到open表中
{
	t->a[t->k++] = *s;
	return;
}
//--------------------------------------------------
void expand(struct ARRAY* s0)//扩展数组
{
	add_table(pclose, s0);
	delete_table(popen, 0);
	struct ARRAY* pl = (struct ARRAY*)malloc(sizeof(struct ARRAY));//初始化向左移动的数组
	struct ARRAY* pr = (struct ARRAY*)malloc(sizeof(struct ARRAY));//初始化向右移动的数组
	struct ARRAY* pu = (struct ARRAY*)malloc(sizeof(struct ARRAY));//初始化向上移动的数组
	struct ARRAY* pd = (struct ARRAY*)malloc(sizeof(struct ARRAY));//初始化向下移动的数组
	*pu = *s0; *pd = *s0; *pl = *s0; *pr = *s0;
	if (down(pd))//0下移，在open表中和close表中查找是否已经存在
	{
		pd->prior = pclose->a + pclose->k - 1;
		pd->d++;
		pd->h = hx(pd->s);
		if (search_table(popen, pd) + search_table(pclose, pd) > 1)
			add_table(popen, pd);
	}
	if (right(pr))//0右移，在open表中和close表中查找是否已经存在
	{
		pr->prior = pclose->a + pclose->k - 1;
		pr->d++;
		pr->h = hx(pr->s);
		if (search_table(popen, pr) + search_table(pclose, pr) > 1)
			add_table(popen, pr);
	}
	if (left(pl))//0左移，在open表中和close表中查找是否已经存在
	{
		pl->prior = pclose->a + pclose->k - 1;
		pl->d++;
		pl->h = hx(pl->s);
		if (search_table(popen, pl) + search_table(pclose, pl) > 1)
			add_table(popen, pl);
	}
	if (up(pu))//0上移，在open表中和close表中查找是否已经存在
	{
		pu->prior = pclose->a + pclose->k - 1;
		pu->d++;
		pu->h = hx(pu->s);
		if (search_table(popen, pu) + search_table(pclose, pu) > 1)
			add_table(popen, pu);
	}
	free(pu); free(pd); free(pl); free(pr);
	return;
}
//--------------------------------------------------
int main()
{
	getgoal();
	popen->a = open;//创建open表
	pclose->a = close;//创建close表
	//STEP1
	initial_array();//创建初始十五数码
	pclose->k = 0;
	//STEP3
	struct ARRAY* N = (struct ARRAY*)malloc(sizeof(struct ARRAY));
	while (popen->k != 0 && popen->k < num_open && pclose->k < num_close)
	{
		*N = *popen->a;
		//STEP4
		if (is_equal(N->s, sn))
		{
			reverint(N);
			break;
		}
		//STEP6
		expand(N);
		insert_sort();
	}
	free(N);
	//STEP2
	if (popen->k == 0)
		printf("No solution\n");
	else if (popen->k >= num_open)
		printf("Open list is full\n");
	else if (pclose->k >= num_close)
		printf("Close list is full\n");
	else
		printf("Success!\n");
	free(popen); free(pclose);
	system("pause");
	return 0;
}
