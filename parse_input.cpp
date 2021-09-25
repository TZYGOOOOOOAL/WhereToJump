#include <stdio.h>
#include <string.h>
#include "parse_input.h"

#define IN_RANGE(x, min, max) (((x)>=(min)) && ((x) <= (max)))
#define IS_NUMBER(x) IN_RANGE((x-'0'), 0, 9)
#define IS_NOT_NUMBER(x) !(IS_NUMBER(x))

/*
每个地图对应坐标文件
第一行 总坐标数：  total num
以下 # 地标中文名
     x y
*/
int parse_location(char* loc_filepath, cv::Point** p_locations, int* total_num)
{
	FILE *fp;
	char str0[20], str1[100];

	fp = fopen(loc_filepath, "r");
	if (!fp){
		printf("Cannot open loctiaon file: %s\n", loc_filepath);
		return -1;
	}

	// total num
	fscanf(fp, "%s%d", str0, total_num);
	if (strcmp(str0, "total")){
		printf("No \"total\" segement \n");
		return -1;
	}

	// malloc
	*p_locations = (cv::Point*)malloc(*total_num * sizeof(cv::Point));

	for (int i = 0; i < *total_num;)
	{
		fscanf(fp, "%s%s", str0, str1);
		if (!strcmp(str0, "#")){			// 注释 TODO：可print出
			continue;
		}

		// 不是注释，则为真实坐标
		(*p_locations)[i].x = atoi(str0);
		(*p_locations)[i].y = atoi(str1);
		i++;
	}
	fclose(fp);
	return 0;
}

/*
map sg			（地图名首字母缩写）
player_num 4	（玩家数量）
rand_mode 0		（随机方式,0无重复，1有重复）
keep_pos -1		（地点mask，-1为全选）
*/
int parse_conf(char* conf_filepath, Config* p_conf)
{
	FILE *fp = NULL;
	char str0[20], str1[20];
	cv::Point* p_total_locations = NULL;
	int location_total_num;
	char map_filepath[256] = "data/map/";
	char loc_filepath[256] = "data/location/"; 

	fp = fopen(conf_filepath, "r");
	if (!fp){
		printf("Cannot open config file: %s\n", conf_filepath);
		return -1;
	}

	while (fscanf(fp, "%s%s", str0, str1) != EOF)
	{
		/* ****** read map image & conf ******* */
		if (!strcmp(str0, "map"))
		{
			// 地图文件
			strcat(map_filepath, str1);
			strcat(map_filepath, ".png");
			p_conf->map = cv::imread(map_filepath);
			// 地点坐标文件
			strcat(loc_filepath, str1);
			strcat(loc_filepath, ".txt");
			parse_location(loc_filepath, &p_total_locations, &location_total_num);
		}

		/* ****** parse player_num ******* */
		else if (!strcmp(str0, "player_num"))
		{
			p_conf->player_num = atoi(str1);
		}

		/* ****** parse rand_mode ******* */
		else if (!strcmp(str0, "rand_mode"))
		{
			switch (atoi(str1))
			{
			case 0 :
				p_conf->rang_mode = RAND_NOREPEAT; break;
			case 1:
				p_conf->rang_mode = RAND_REPEAT; break;
			default:
				printf("Rand mode not found !!!\n"); return -1;
			}
		}

		/* ****** 保留的地点 ******* */
		else if (!strcmp(str0, "keep_pos"))
		{
			// 首先保证地点坐标已经被加载
			if (p_total_locations == NULL){
				printf("Map locations data should load first !!!\n");
				return -1;
			}

			int loc_real_num = 0;
			unsigned int *keep_mask = (unsigned int *)malloc(location_total_num * sizeof(unsigned int));
			memset(keep_mask, 0, sizeof(unsigned int) * location_total_num);

			// 获得需要的地点位置
			if (!strcmp(str1, "-1")){		// 全选
				memset(keep_mask, -1, sizeof(unsigned int) * location_total_num);
				loc_real_num = location_total_num;
			}
			else{
				int first_idx = atoi(str1);	// 第一个数字存在str1字符串中
				if (IN_RANGE(first_idx, 0, location_total_num-1))
					keep_mask[atoi(str1)] = 1;

				char c;
				int n;			// char转换成的数字（0-9）
				int idx = -1;	// 哪个loc 下标被选中
				do
				{
					c = fgetc(fp);
					if (c == ' ' || c == EOF || c == '\r' || c == '\n')
					{
						if (IN_RANGE(idx, 0, location_total_num-1))
							keep_mask[idx] = 1;
						if (c != ' ')
							break;
						idx = 0;
					}
					else if (IS_NUMBER(c))
					{
						n = c - '0';
						idx = idx * 10 + n;
					}
					else
					{
						printf("Location not Integer !!!\n");
						return -1;
					}
				} while (true);

				// 防止重复坐标导致长度统计错误
				for (int i = 0; i < location_total_num; i++)
				{
					if (keep_mask[i] == 1)
						loc_real_num++;
				}
			}// end parse keep pos

			p_conf->locations_total = p_total_locations;
			p_conf->loc_total_num = location_total_num;
			p_conf->loc_keeped_num = loc_real_num;

			// TODO: 还是不能保存全部
			p_conf->loc_keeped_idxes = (int *)malloc(p_conf->loc_keeped_num * sizeof(int));
			p_conf->loc_keeped_idxes_tmp = (int *)malloc(p_conf->loc_keeped_num * sizeof(int));

			int idx_real = 0;
			for (int idx_total = 0; idx_total < location_total_num; idx_total++){
				if (keep_mask[idx_total] > 0){
					if (!IN_RANGE(idx_real, 0, loc_real_num - 1))
					{
						printf("Parse Error !!!");
						return -1;
					}
					p_conf->loc_keeped_idxes[idx_real] = idx_total;
					idx_real++;
				}
			}

			memcpy(p_conf->loc_keeped_idxes_tmp, p_conf->loc_keeped_idxes, p_conf->loc_keeped_num * sizeof(int));
			free(keep_mask);
		} // end parse keep_pos

		/* ****** 未知字段 ******* */
		else
		{
			printf("Unknown Segment : \"%s\"\n", str0);
		}
	}
	return 0;
}