#ifndef SYM_WIDTH

#include "bar.h"
#include "seven.h"
#include "cherry.h"
#include "orange.h"
#include "cocos.h"
#include "pear.h"
#include "bell.h"
#include "melon.h"

#include "dice1.h"
#include "dice2.h"
#include "dice3.h"
#include "dice4.h"
#include "dice5.h"
#include "dice6.h"

#include "roulette.h"


#define ROULETTE_WIDTH 111
#define ROULETTE_HEIGHT 111

#define NUM_ROULETTE_NUMBERS 19

const uint16_t roulette_numbers[] = {
	0,15,4,2,17,6,13,11,8,10,5,16,1,14,9,18,7,12,3
};

const float roulette_degrees[] = {
	0.0f,
	28.41f,
	47.35f,
	66.29f,
	85.23f,
	104.17f,
	123.11f,
	142.05f,
	160.99f,
	179.93f,
	198.87f,
	217.81f,
	236.75f,
	255.69f,
	274.63f,
	293.57f,
	312.51f,
	331.45f,
	350.39f
};

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
    /* cherry (2) */ {  0,   1, 	 2,		3		},
    /* orange (3) */ {  0,   1, 	 2,		3		},
    /* cocos (4)  */ {  0,   1, 	 2,		3		},
    /* pear (5)   */ {  0,   1, 	 2,		3		},
    /* bell (6)   */ {  3,   9, 	 27,	81	 	},
    /* melon (7)  */ {  0,   0, 	 0,		2	 	}
};

#define DICE_WIDTH 35
#define DICE_HEIGHT 35

const uint16_t *dice_sides[] = {
	dice1,
	dice2,
	dice3,
	dice4,
	dice5,
	dice6,
};

#endif // SYM_WIDTH