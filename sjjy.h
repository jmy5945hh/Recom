#ifndef __SJJY_H__
#define __SJJY_H__

#define BOY_TOTAL 542
#define GIRL_TOTAL 512

#define BOY_NUM 40
#define GIRL_NUM 12

typedef struct item_info
{
	int itemID;
	int iRateCount;
	float iAvgRate, iTotalRate, bRate;
} item_info;

typedef struct user_info
{
	int userID;
	int uRateCount;
	float uAvgRate, uTotalRate;
} user_info;

struct preRes
{
	int preIndex;
	float preRate;
};

void initUsers(user_info *user, int count);
void initItems(item_info *item, int count);
void uDistRecom(int index, user_info *user, int userNum, item_info *item, int itemNum);
void slopeOne(int index, user_info *user, int userNum, item_info *item, int itemNum);
bool cmp(preRes x, preRes y);

float bayesRate(int totalCount, float totalAvg, int itemCount, float itemAvg);

#endif
