#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "IntDequeue.h"
#include "lower_bound.h"
#include "branch_and_bound.h"
#include "data_info.h"

#define BUFFER 256
/*変数の定義
Stack:スタックの最大個数
Tier:積み上げられるブロックの最大値
Nblocking_lower:priorityの下側に位置するブロック数
Nblocking_upper:priorityの上側に位置するブロック数
stack[i]:i番目のスタック
Nblocking_upper_again:上側から移動させた後もブロッキングブロックとなる個数
Nblocking_lower_again:下側から移動させた後もブロッキングブロックとなる個数

*/

int main(void)
{
	clock_t time = 0;
	IntDequeue *stack = malloc(STACK * (sizeof *stack));
	Array_initialize(stack);
	int nblock = NBLOCK;
	int i, j, x, l;
	int k = 0;
	int invalid = 0;
	int sum = 0;
	int gap = 0;
	int max_gap = 0;
	int missmatch = 0;
	double max_time = 0;
	char filename[BUFFER];
	char str[BUFFER];
	FILE *fp_csv = NULL;
	FILE *fp_write = NULL;
	for (int a = NUMBER; a < NUMBER + 100 * TIER; a++)
	{
		FILE *fp = NULL;
		sprintf(filename, "../alpha=%.1f/%d-%d-%d/%05d.txt", ALPHA, TIER, STACK, nblock, a);
		printf("%s\n", filename);

		clock_t max_s = clock();
		//	読み込みモードでファイルを開く
		fp = fopen(filename, "r");

		fgets(str, BUFFER, fp);
		fgets(str, BUFFER, fp);
		fgets(str, BUFFER, fp);

		// スタック数と高さを読み込む　//
		for (i = 0; i < STACK; i++)
		{
			fscanf(fp, "%d ", &l);
			for (j = 0; j < l; j++)
			{
				fscanf(fp, "%d ", &x);
				EnqueRear(&stack[i], x);
			}
		}

		//*---LB1---*//
		int LB1 = lower_bound1(stack);
		printf("LB1:%d\n", LB1);

		qsort(stack, STACK, sizeof(IntDequeue), (int (*)(const void *, const void *))pricmp);

		printf("sort:\n");
		Array_print(stack);
		int UB_cur = LB1;
		int priority = 1;
		// int UB = UpperBound(stack,priority,both);
		int min_relocation =
			branch_and_bound(stack, 100, UB_cur, LB1, priority, both, 0, 0, clock());
		// int min_relocation = enumerate_relocation(stack, depth, priority, both);
		printf("min_relocation:%d,difference%d\n", min_relocation, min_relocation - LB1);
		if (min_relocation == -1)
		{
			invalid++;
		}
		else
		{
			clock_t max_e = clock();
			time += max_e - max_s;
			if (max_time < (max_e - max_s))
				max_time = max_e - max_s;
			sum += min_relocation;
			gap += min_relocation - LB1;
			if (max_gap < min_relocation - LB1)
				max_gap = min_relocation - LB1;
		}
		if (min_relocation == LB1)
		{
			k++;
		}
		fclose(fp);
		// if (a % 100 == 1)
		// {
		// 	sprintf(filename, "/Users/watanabeshun/Documents/alpha=%.1f/%d-%d-%d.csv", ALPHA, TIER, STACK, nblock);
		// 	fp_csv = fopen(filename, "r");
		// 	sprintf(filename, "/Users/watanabeshun/Documents/Benchmark/%d-%d-%d_unfixed.csv", TIER, STACK, nblock);
		// 	fp_write = fopen(filename, "w");
		// }
		// fscanf(fp_csv, "%d ", &x);
		// if (x != min_relocation)
		// {
		// 	if (x < min_relocation)
		// 		getchar();
		// 	printf("missmatch\n");
		// 	missmatch++;
		// }
		// fprintf(fp_write, "%d\n", min_relocation);
		if (a % 100 == 0)
		{
			nblock++;
			// fclose(fp_csv);
			// fclose(fp_write);
		}
		Array_clear(stack);
	}
	clock_t end = clock();
	Array_terminate(stack);
	printf("optimal value:%f,ave_gap:%f,max_gap:%d,ave_time:%f,max_time=%f\n", (double)sum / (100 * TIER - invalid), (double)gap / (100 * TIER - invalid), max_gap, (double)time / (CLOCKS_PER_SEC * (100 * TIER - invalid)), max_time / CLOCKS_PER_SEC);
	printf("invalid:%d\n", invalid);
	return 0;
}
