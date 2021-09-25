#include "utils.h"

// 随机排列地点
void random_arrangement(Config* p_conf)
{
	if (p_conf->rang_mode == RAND_NOREPEAT)
		random_arrangement_norepeat(p_conf);
	else if (p_conf->rang_mode == RAND_REPEAT)
		random_arrangement_repeat(p_conf);
	else
		printf("Unknow rand mode !!!\n");
	return;
}

void random_arrangement_norepeat(Config* p_conf)
{
	int* loc_idexs = p_conf->loc_keeped_idxes_tmp;
	int rand_idx;
	int tmp;
	for (int i = p_conf->loc_keeped_num - 1; i > 0; i--)
	{
		rand_idx = rand_range(0, i);
		tmp = loc_idexs[i];
		loc_idexs[i] = loc_idexs[rand_idx];
		loc_idexs[rand_idx] = tmp;
	}
	return;
}

void random_arrangement_repeat(Config* p_conf)
{
	int real_num = p_conf->loc_keeped_num;
	int* loc_idexs = p_conf->loc_keeped_idxes;
	int* loc_idexs_tmp = p_conf->loc_keeped_idxes_tmp;
	memcpy(loc_idexs_tmp, loc_idexs, real_num * sizeof(int));

	int rand_idx;
	for (int i = 0; i < real_num; i++)
	{
		rand_idx = rand_range(0, real_num - 1);
		loc_idexs_tmp[i] = loc_idexs[rand_idx];
	}
	return;
}

// [l, r)
int rand_range(int l, int r)
{
	int range = r - l + 1;
	int rand_num = rand() % range + l;
	return rand_num;
}