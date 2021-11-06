#include <stdio.h>
#include <stdlib.h>
#include "IntDequeue.h"
#include "lower_bound.h"
#include "branch_and_bound.h"
#include "data_info.h"

#ifdef EITHER
int branch_and_bound(IntDequeue *q, int UB,int UB_cur, int LB,int priority, direction Dir, LB_idx *BT,int TableSize) {
	static int BlockingFlag = 0;
	static int DstStack = 0;
	static int num_ret;
	static int depth = 0;
	static int SecondPosition = 0;
	static int MinRelocation = 0;
	static int LB_temp = 0;
	static int NumBlocking = 0;
	static int p_before = 1;
	static int temp = 0;
	if (UB == UB_cur) {
		depth = 0;
		return MinRelocation = UB;
	}
	int i = 0;
	int j = 0;
	int k = 0;
	int BlockSpace = 0;
	int Size = 0;
	int PriorityEdge = 0;
	direction dir = Dir;
	direction DirNext = Dir;
	IntDequeue *q_temp = NULL;
	LB_idx *BlockingTable = NULL;
	LB_idx *LB_next = NULL;
	switch (dir)
	{
	case both:
		qsort(q + 1, STACK - 1, sizeof(IntDequeue), (int(*)(const void *, const void *))pricmp);
		break;
	default:
		depth++;

#if TEST==0
		printf("Block relocation(depth=%d)\n", depth);
#endif

		LB--;
		if (dir == lower) {
			PriorityEdge = q[0].que[q[0].front];
			SecondPosition = (q[0].front + 1) % q[0].max;
		}
		else if (dir == upper) {
			PriorityEdge = q[0].que[(q[0].front + q[0].num - 1) % q[0].max];
			SecondPosition = (q[0].front + q[0].num - 2) % q[0].max;
		}
		NumBlocking = nblocking(q, dir);
		if (NumBlocking == 1) {
			DirNext = both;
			if (PriorityEdge == priority + 1 && q[1].que[q[1].min_idx[0]] != priority && q[0].num_min == 1) {
				q_temp = malloc(STACK*(sizeof *q_temp));
				Array_initialize(q_temp);
				Array_copy(q_temp, q);
				Deque(&q_temp[0], &num_ret, dir);

#if TEST==0
				printf("Number Retrieval:%d\n", num_ret);
#endif

				Deque(&q_temp[0], &num_ret, dir);

#if TEST==0
				printf("Priority Retrieval:%d\n", num_ret);
#endif

				insert_media(q_temp, 0);

#if TEST==0
				Array_print(q_temp);
#endif

				p_before = 0;
				priority = q_temp[0].que[q_temp[0].min_idx[0]];
				if (branch_and_bound(q_temp, UB, UB_cur, LB, priority, DirNext, NULL, 0)) {
					Array_terminate(q_temp);
					free(q_temp);
					return MinRelocation;
				}

#if TEST==0
				printf("Lower termination.\n");
				Array_print(q_temp);
#endif

				depth--;
				Array_terminate(q_temp);
				free(q_temp);
				return 0;
			}
		}
		if (BlockingFlag) {
			BlockingTable = malloc(TableSize*(sizeof(*BlockingTable)));
			for (i = 0; i < TableSize; i++) {
				BlockingTable[i] = BT[i];
				if (BlockingTable[i].idx == DstStack) {
					BlockingTable[i].blocking = Enque(&q[DstStack], PriorityEdge, dir);
					if (BlockingTable[i].blocking != -1) Deque(&q[DstStack], &num_ret, dir);
				}
				if (BlockingTable[i].blocking && (LB + depth == UB_cur)) k++;
			}
		}
		else {
			BlockingTable = CreateBlockingTable(q, dir, &TableSize);
			if (LB + depth == UB_cur) {
				for (i = 0; i < TableSize; i++) {
					if (BlockingTable[i].blocking) k++;
				}
			}
		}
		if (k == TableSize) {

#if TEST==0 
			switch (dir) {
			case lower:
				printf("Lower termination!\n");
				break;
			case upper:
				printf("Upper termination!\n");
				break;
			}
#endif

			depth--;
			free(BlockingTable);
			return 0;
		}
		if (DirNext != both) {
			Deque(&q[0], &num_ret, dir);
			BlockingFlag = 1;
			LB_next = CreateBlockingTable(q, dir, &Size);
			NumBlocking = 0;
			if (LB + depth == UB_cur) {
				NumBlocking = 0;
				for (i = 0; i < Size; i++) {
					if (LB_next[i].blocking) NumBlocking++;
					else break;
				}
				if (NumBlocking == Size) {
					NumBlocking = 0;
					for (i = 0; i < TableSize; i++) {
						if (BlockingTable[i].blocking == 0) {
							Enque(&q[BlockingTable[i].idx], PriorityEdge, dir);
							temp = Enque(&q[BlockingTable[i].idx], q[0].que[SecondPosition], dir);
							if (temp == 0) {
								Deque(&q[BlockingTable[i].idx], &num_ret, dir);
								Deque(&q[BlockingTable[i].idx], &num_ret, dir);
								break;
							}
							else if (temp == 1) {
								Deque(&q[BlockingTable[i].idx], &num_ret, dir);
								Deque(&q[BlockingTable[i].idx], &num_ret, dir);
								NumBlocking++;
							}
							else {
								Deque(&q[BlockingTable[i].idx], &num_ret, dir);
								NumBlocking++;
							}
						}
						if ((k + NumBlocking) == TableSize) {

#if TEST==0 
							printf("Lower termination!\n");
#endif

							depth--;
							Enque(&q[0], PriorityEdge, dir);
							free(BlockingTable);
							free(LB_next);
							return 0;
						}
					}
				}
			}
		}
		else BlockingFlag = 0;
		if (DirNext == both) {
			q_temp = malloc(STACK*(sizeof *q_temp));
			Array_initialize(q_temp);
			Array_copy(q_temp, q);
			Deque(&q_temp[0], &num_ret, dir);
		}
		for (i = TableSize - 1; i >= 0; i--) {
			if (BlockingTable[i].blocking != 0)	continue;
			DstStack = BlockingTable[i].idx;
			if (DirNext == both) {
				Enque(&q_temp[BlockingTable[i].idx], PriorityEdge, dir);

#if TEST==0
				Array_print(q_temp);
#endif

				if (branch_and_bound(q_temp, UB, UB_cur, LB, p_before = priority, DirNext, NULL, 0)) {
					free(BlockingTable);
					Array_terminate(q_temp);
					free(q_temp);
					return MinRelocation;
				}
				Array_copy(q_temp, q);

#if TEST==0
				Array_print(q_temp);
#endif

				Deque(&q_temp[0], &num_ret, dir);
			}
			else {
				Enque(&q[BlockingTable[i].idx], PriorityEdge, dir);

#if TEST==0
				Array_print(q);
#endif

				if (branch_and_bound(q, UB, UB_cur, LB, priority, DirNext, LB_next, Size)) {
					free(BlockingTable);
					return MinRelocation;
				}

#if TEST==0
				Array_print(q);
#endif

				Deque(&q[BlockingTable[i].idx], &num_ret, dir);
			}
		}
		LB += 1;

#if TEST==0
		printf("Blocking:\n");
#endif

		if (LB + depth > UB_cur);
		else {
			for (i = TableSize - 1; i >= 0; i--) {
				if (BlockingTable[i].blocking != 1)	continue;
				DstStack = BlockingTable[i].idx;
				if (DirNext == both) {
					Enque(&q_temp[BlockingTable[i].idx], PriorityEdge, dir);

#if TEST==0
					Array_print(q_temp);
#endif

					if (branch_and_bound(q_temp, UB, UB_cur, LB, p_before = priority, DirNext, NULL, 0)) {
						free(BlockingTable);
						Array_terminate(q_temp);
						free(q_temp);
						return MinRelocation;
					}
					Array_copy(q_temp, q);

#if TEST==0
					Array_print(q_temp);
#endif

					Deque(&q_temp[0], &num_ret, dir);
				}
				else {
					Enque(&q[BlockingTable[i].idx], PriorityEdge, dir);

#if TEST==0
					Array_print(q);
#endif

					if (branch_and_bound(q, UB, UB_cur, LB, priority, DirNext, LB_next, Size)) {
						free(BlockingTable);
						return MinRelocation;
					}

#if TEST==0
					Array_print(q);
#endif

					Deque(&q[BlockingTable[i].idx], &num_ret, dir);
				}
			}
		}

#if TEST==0
		printf("Termination.\n");
#endif

		if (DirNext == both) {
			Array_terminate(q_temp);
			free(q_temp);
		}
		else {
			Enque(&q[0], PriorityEdge, dir);
			free(LB_next);
		}
		free(BlockingTable);
		depth--;
		return 0;
	}
		while (q[k].que[q[k].min_idx[0]] == priority && !(IsEmpty(&q[k]))) {
			if (k != 0) {
				Swap_IntDequeue(&q[0], &q[k]);

#if TEST==0
				Array_print(q);
#endif

			}
			while ((q[0].que[q[0].front] == priority) || (q[0].que[(q[0].front + q[0].num - 1) % q[0].max] == priority)) {
				if (q[0].que[q[0].front] == priority) {
					DequeFront(&q[0], &num_ret);

#if TEST==0
					printf("Number Retrieval:%d\n", num_ret);
					Array_print(q);
#endif

				}
				else  if (q[0].que[(q[0].front + q[0].num - 1) % q[0].max] == priority) {
					DequeRear(&q[0], &num_ret);

#if TEST==0
					printf("Number Retrieval:%d\n", num_ret);
					Array_print(q);
#endif

				}
				if (ArrayIsEmpty(q)) {
					depth = 0;
					return MinRelocation = UB_cur;
				}
				if (IsEmpty(&q[0]))	break;
			}
			if (!(insert_sort(q))) {
				if (priority != q[0].que[q[0].min_idx[0]]) {
					k = 0;
					priority = q[0].que[q[0].min_idx[0]];
				}
				else {
					if (depth != 0 && priority == p_before) {
						k = 1;
						while (q[k].que[q[k].min_idx[0]] == priority) {
							k++;
							if (k == STACK) break;
						}
						break;
					}
					else k++;
				}
			}
			else {
				if (priority != q[0].que[q[0].min_idx[0]]) {
					k = 0;
					priority = q[0].que[q[0].min_idx[0]];
				}
				else {
					if (depth != 0 && priority == p_before) {
						k = 1;
						while (q[k].que[q[k].min_idx[0]] == priority) {
							k++;
							if (k == STACK) break;
						}

#if TEST==0
						Array_print(q);
#endif

						break;
					}
					else;
				}
			}
			if (k == STACK) {
				break;
			}
		}
		qsort(q, k, sizeof(IntDequeue), (int(*)(const void *, const void *))BlockingCmp);

#if TEST==0
		Array_print(q);
#endif

		for (i = 0; i < STACK; i++)	BlockSpace += TIER - q[i].num;
		for (i = 0; i< k; i++) {
			BlockingFlag = 0;
			if (i != 0) {
				Swap_IntDequeue(&q[0], &q[i]);

#if TEST==0
				printf("swap(%d,%d)\n", 0, i);
				Array_print(q);
#endif

			}
			dir = q[0].dir;
			if (dir == both) dir = q[0].que[q[0].front] < q[0].que[(q[0].front + q[0].num - 1) % q[0].max] ?
				lower : upper;
			switch (dir) {
			case lower:
				if (nblocking_lower(&q[0]) > BlockSpace - (TIER - q[0].num)) {

#if TEST==0
					printf("No space!\n");
#endif

					Swap_IntDequeue(&q[0], &q[i]);
					break;
				}
				else {
					if (LB + depth == UB_cur - 1) {
						j = UpperBound(q, priority, dir) + depth;
						if (j < UB) UB = j;
					}
					if (branch_and_bound(q, UB, UB_cur, LB, priority, dir, NULL, 0)) {
						return MinRelocation;
					}
					Swap_IntDequeue(&q[0], &q[i]);
				}
				break;
			case upper:
				if (nblocking_upper(&q[0]) > BlockSpace - (TIER - q[0].num)) {

#if TEST==0
					printf("No space!\n");
#endif

					Swap_IntDequeue(&q[0], &q[i]);
					break;
				}
				else {
					if (LB + depth == UB_cur - 1) {
						j = UpperBound(q, priority, dir) + depth;
						if (j < UB) UB = j;
					}
					if (branch_and_bound(q, UB, UB_cur, LB, priority, dir, NULL, 0)) {
						return MinRelocation;
					}
					Swap_IntDequeue(&q[0], &q[i]);
				}
				break;
			}
		}
		for (i= 0; i < k; i++) {
			BlockingFlag = 0;
			if (i != 0) {
				Swap_IntDequeue(&q[0], &q[i]);

#if TEST==0
				printf("swap(%d,%d)\n", 0, i);
				Array_print(q);
#endif

			}
			dir = q[0].dir;
			if (dir == both) dir = q[0].que[q[0].front] < q[0].que[(q[0].front + q[0].num - 1) % q[0].max] ?
				lower : upper;
			switch (dir) {
			case lower:
				if (nblocking_upper(&q[0]) > BlockSpace - (TIER - q[0].num)) {

#if TEST==0
					printf("No space!\n");
#endif

					Swap_IntDequeue(&q[0], &q[i]);
					continue;
				}
				else {
					LB_temp = LB - q[0].LB + min_relocation(&q[0],upper);
					if (LB_temp + depth > UB_cur) {

#if TEST==0
						printf("LB over!\n");
#endif

						Swap_IntDequeue(&q[0], &q[i]);
						continue;
					}
					else {
						if (LB_temp + depth == UB_cur - 1) {
							j = UpperBound(q, priority, upper)+depth;
							if (j < UB) UB = j;
						}
						if (branch_and_bound(q, UB, UB_cur, LB_temp, priority, upper, NULL, 0)) {
							return MinRelocation;
						}
						Swap_IntDequeue(&q[0], &q[i]);
					}
				}
				break;
			case upper:
				if (nblocking_lower(&q[0]) > BlockSpace - (TIER - q[0].num)) {

#if TEST==0
					printf("No space!\n");
#endif

					Swap_IntDequeue(&q[0], &q[i]);
					continue;
				}
				else {
					LB_temp = LB - q[0].LB + min_relocation(&q[0],lower);
					if (LB_temp + depth > UB_cur) {

#if TEST==0
						printf("LB over!\n");
#endif

						Swap_IntDequeue(&q[0], &q[i]);
						continue;
					}
					else {
						if (LB_temp + depth == UB_cur - 1) {
							j = UpperBound(q, priority, lower)+depth;
							if (j < UB) UB = j;
						}
						if (branch_and_bound(q, UB, UB_cur, LB_temp, priority, lower, NULL, 0)) {
							return MinRelocation;
						}
						Swap_IntDequeue(&q[0], &q[i]);
					}
				}
				break;
			}
		}
		if (depth == 0) {
			UB_cur++;

#if TEST==0
			printf("UB_cur++\n");
#endif

			if (branch_and_bound(q, UB, UB_cur, LB, priority, both, NULL, 0)) {
				return MinRelocation;
			}
			return -1;
		}
		else {
			return 0;
		}
	}

#endif
