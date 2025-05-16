#include "bar.h"
#include "seven.h"
#include "cherry.h"
#include "orange.h"
#include "cocos.h"
#include "pear.h"
#include "bell.h"
#include "melon.h"


#define SYM_WIDTH 22
#define SYM_HEIGHT 18

const uint16_t *slot_symbols[] = {
	bar,
	seven,
	cherry,
	orange,
	cocos,
	pear,
	bell,
	melon,
};

const uint16_t slot_weights[] = {
	5, //bar,
    2, //seven,
	10, //cherry,
	10, //orange,
	10, //cocos,
	10, //pear,
	7, //bell,
	11, //melon,
};
const int slot_payout_counts[4] = {5, 7, 9, 10};
// Weights RTP 400+% https://docs.google.com/spreadsheets/d/1mRw2tgj_1QKSO62u3RxwVNOK8eTZqZutsa4-Hi4BFa4/edit?gid=0#gid=0
const int slot_payout[][4] = {
    // Symbol Index | 5 times | 7 times | 9 times | 10 times
    /* bar (0)    */ {  3,   15, 	 100,	1000	},
    /* seven (1)  */ {  100, 400, 	 40000,	100000	}, // 'seven' usually has high payouts
    /* cherry (2) */ {  0,   1, 	 2,		2		},
    /* orange (3) */ {  0,   1, 	 2,		2		},
    /* cocos (4)  */ {  0,   1, 	 2,		2		},
    /* pear (5)   */ {  0,   1, 	 2,		2		},
    /* bell (6)   */ {  3,   3, 	 3,		3	 	},
    /* melon (7)  */ {  0,   0, 	 0,		2	 	}
};