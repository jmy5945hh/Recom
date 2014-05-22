/************************************************************
Copyright (C), 2013, UESTC-SCIE
FileName: sjjy.cpp
Author: 景梦园 201321010802
Version : 0.1
Last Update: 2013-11-20
Description: 世纪佳缘女友测试推荐系统——基于UESTC研究生打分数据
***********************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <vector>

using namespace std;

#include "sjjy.h"

int itemNum = GIRL_TOTAL;
int userNum = BOY_NUM;

int rateMatrix[GIRL_TOTAL][BOY_NUM]; 	//rate rateMatrix

int main ()
{
	int totalCount = 0;
	float totalAvg = 0, totalRate = 0;

	//Input files
	ifstream ratefin ("sjjy_b2g.data");

	item_info item[itemNum];
	user_info user[userNum];
	initUsers(user, userNum);
	initItems(item, itemNum);

	string strJump;
	ratefin >> strJump;

	//Read data
	for ( int i = 0; i < userNum; i++ )
	{
		ratefin >> user[i].userID;
	}

	for ( int i = 0; i < itemNum; i++ )
	{
		ratefin >> item[i].itemID;
		for ( int j = 0; j < userNum; j++ )
		{
			ratefin >> rateMatrix[i][j];
			if ( rateMatrix[i][j] != 0)
			{
				item[i].iTotalRate += rateMatrix[i][j];
				user[j].uTotalRate += rateMatrix[i][j];
				item[i].iRateCount++;
				user[j].uRateCount++;
				totalCount++;
				totalRate += rateMatrix[i][j];
			}
			user[j].uAvgRate = user[j].uTotalRate / user[j].uRateCount;
		}
		item[i].iAvgRate = item[i].iTotalRate / item[i].iRateCount;
	}

	totalAvg = totalRate / totalCount;
/*
	for ( int i = 0; i < itemNum; i++ )
	{
		item[i].bRate = bayesRate(totalCount, totalAvg, item[i].iRateCount, item[i].iAvgRate);
		cout <<  item[i].itemID << "::" <<item[i].bRate << endl;
	}
*/
	int testID = 0;
	bool flag = false;
	while (testID != -1)
	{
		cout << "Input test ID:";
		cin >> testID;
		for ( int index = 0; index < userNum; index++ )
		{
			if ( testID == user[index].userID )
			{
				uDistRecom(index, user, userNum ,item, itemNum);
				flag = true;
				break;
			}
		}

		if ( !flag )
		{
			cout << "WRONG ID!PLEASE INPUT AGAIN!" << endl;
		}
	}

	return 0;
}

/*--------------------------------------
 * function : initiate user info
 * parameter : user info cluster
 *--------------------------------------*/
void initUsers(user_info *user, int count)
{
	for ( int i = 0; i < count; i++ )
	{
		user[i].uRateCount = 0;
		user[i].uTotalRate = 0;
		user[i].uAvgRate = 0;
	}
}

/*--------------------------------------
 * function : initiate item info
 * parameter : item info cluster
 *--------------------------------------*/
void initItems(item_info *item, int count)
{
	for ( int i = 0; i < count; i++ )
	{
		item[i].iRateCount = 0;
		item[i].iTotalRate = 0;
		item[i].iAvgRate = 0;
		item[i].bRate = 0;
	}
}

/*--------------------------------------
 * function : use bayes theory to adjust score
 * parameter : user number, average score, item number, item average score
 * return : bayes score after adjust
 *--------------------------------------*/
float bayesRate(int totalCount, float totalAvg, int itemCount, float itemAvg)
{
	float numer = (totalCount*totalAvg)+(itemCount*itemAvg);
	float denom = totalCount+itemCount;

	return numer / denom;
}

float preByTotalAvgRate(float totalAvg)
{
	return totalAvg;
}

float preByUserAvgRate(user_info user)
{
	return user.uAvgRate;
}

float preByItemAvgRate(item_info item)
{
	return item.iAvgRate;
}

/*--------------------------------------
 * function : compare two values of a struct
 * parameter : datastruct x and datastruct y
 *--------------------------------------*/
bool cmp(preRes x, preRes y)
{  
    return x.preRate > y.preRate;
}

/*--------------------------------------
 * function : calculate cosin distant between users
 * parameter : user to be calculated, all users' infomation, all items' information
 *--------------------------------------*/
void uDistRecom(int index, user_info *user, int userNum, item_info *item, int itemNum)
{

	float inProduct = 0, sumA = 0, sumB = 0;
	float modA = 0, modB = 0, dist[userNum], sumRate[itemNum];
	float sumDist = 0;

	int iuTotal[itemNum], count[itemNum];
	memset(iuTotal, 0, sizeof(iuTotal));
	memset(count, 0, sizeof(count));
	memset(sumRate, 0, sizeof(sumRate));

	vector<preRes> cosRes;

	for ( int i = 0; i < userNum; i++ )
	{
		sumA = 0;
		sumB = 0;
		inProduct = 0;

		for ( int j = 0; j < itemNum; j++ )
		{
			if ( rateMatrix[j][index]!=0 && rateMatrix[j][i]!=0 )
			{
				inProduct += ((rateMatrix[j][index]-user[index].uAvgRate)*(rateMatrix[j][i]-user[i].uAvgRate));
				sumA += pow(rateMatrix[j][index]-user[index].uAvgRate, 2);
				sumB += pow(rateMatrix[j][i]-user[i].uAvgRate, 2);
			}
		}
		
		modA = sqrt(sumA);
		modB = sqrt(sumB);
		if ( modA!=0 && modB!=0 )
		{
			dist[i] = inProduct / modA / modB;
		}
		
		if ( i!=index && dist[i]!=0 )
		{
			sumDist += dist[i];

			for ( int j = 0; j < itemNum; j++ )
			{
				sumRate[j] += (dist[i]*rateMatrix[j][i]);
			}
		}
	}

	for ( int j = 0; j < itemNum; j++ )
	{
		preRes data;	
		data.preRate = sumRate[j]/sumDist;
		data.preIndex = j;
		cosRes.push_back(data);	
	}

	vector<preRes>::iterator begin, end;
	end = cosRes.end();
	begin = cosRes.begin();
	sort(begin, end, cmp);

	cout << "With COS_SIM Algorithm, We Recommond you:" << endl;
	for ( int i = 0; i < 5; begin++)
	{
		if( rateMatrix[(*begin).preIndex][index]==0 )
		{
			cout << "ID:" << item[(*begin).preIndex].itemID << " " << (*begin).preRate << endl;
			i++;
		}
	}
	cout << endl;
}


/*--------------------------------------
 * function : compare two values of a struct
 * parameter : datastruct x and datastruct y
 *--------------------------------------*/
void slopeOne(int index, user_info *user, int userNum, item_info *item, int itemNum)
{

	float diffMatrix[itemNum][itemNum];
	int countMatrix[itemNum][itemNum];

	memset(diffMatrix, 0, sizeof(diffMatrix));
	memset(countMatrix, 0, sizeof(countMatrix));

	vector<preRes> slopeRes;

	for ( int i = 0; i < itemNum; i++ )
	{
		for ( int j = 0; j < itemNum; j++ )
		{
			for ( int k = 0; k < userNum; k++ )
			{
				//
			}
			diffMatrix[j][i] = -diffMatrix[i][j];
		}
	}

	for ( int i = 0; i < itemNum; i++ )
	{
		for ( int j = 0; j < itemNum; j++ )
		{
			if ( countMatrix[i][j] != 0)
			{
				diffMatrix[i][j] /= countMatrix[i][j];
			}
		}
	}

	for ( int i = 0; i < itemNum; i++ )
	{
		preRes data;	
		int countSum = 0;
		data.preIndex = i;
		data.preRate = 0;

		if ( rateMatrix[i][index] == 0 )
		{
			for ( int j = 0; j < itemNum; j++ )
			{
				if ( rateMatrix[j][index]!=0 )
				{
					//
				}
			}
		}

		if ( countSum != 0 )
		{
			data.preRate /= countSum;
		}
		slopeRes.push_back(data);	
	}

	vector<preRes>::iterator begin, end;
	end = slopeRes.end();
	begin = slopeRes.begin();
	sort(begin, end, cmp);

	cout << "With SLOPE_ONE Algorithm, We Recommond you:" << endl;
	for ( int i = 0; i < 5; begin++)
	{
		if( rateMatrix[(*begin).preIndex][index]==0 )
		{
			cout << "ID:" << item[(*begin).preIndex].itemID << " " << (*begin).preRate << endl;
			i++;
		}
	}
	cout << endl;
}
