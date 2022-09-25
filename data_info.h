#ifndef ___data_info
#define ___data_info

#define ALPHA 0.2
#define TIER 6
#define STACK 8
#define NBLOCK 44
#define NUMBER 6482
#define TEST 1
#define EITHER
#define DISTINCTIVE
typedef  enum DIRECTION{
	upper,
	lower,
	both
}	direction;

//distictive:一度取り出す方向を決めたら、ターゲットブロックが取り出されるまで方向を固定		either:ブロックの移動先は取り出した方向と同じ
//selective:毎度、取り出す方向を変えられる																								both:ブロックの移動先は両側どちらでもよい


#endif // !1

