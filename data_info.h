#ifndef ___data_info
#define ___data_info

#define ALPHA 0.8
#define TIER 5
#define STACK 6
#define NBLOCK 25
#define NUMBER 701
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

