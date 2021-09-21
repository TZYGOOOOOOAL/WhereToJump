#include <stdio.h>
#include <string.h>
#include "parse_input.h"

#define MAX_LOCATION_NUM 100

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
	FILE *fp=NULL;
	char str0[20], str1[MAX_LOCATION_NUM * 4];
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
		// read map image & conf
		if (!strcmp(str0, "map")){
			// 地图文件
			strcat(map_filepath, str1);
			strcat(map_filepath, ".png");
			p_conf->map = cv::imread(map_filepath);
			// 地点坐标文件
			strcat(loc_filepath, str1);
			strcat(loc_filepath, ".txt");
			parse_location(loc_filepath, &p_total_locations, &location_total_num);
		}

		// player number
		else if (!strcmp(str0, "player_num")){
			p_conf->player_num = atoi(str1);
		}

		// rand mode
		else if (!strcmp(str0, "rand_mode")){
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

		// 保留的地点
		else if (!strcmp(str0, "keep_pos")){

			// 获得需要的地点位置
			if (!strcmp(str1, "-1")){		// 全选
				p_conf->locations = p_total_locations;
				p_conf->loc_num = location_total_num;
			}
			else{
				int keep_mask[MAX_LOCATION_NUM];
				memset(keep_mask, 0, sizeof(int)*MAX_LOCATION_NUM);
				keep_mask[atoi(str1)] = 1;
				char c;
				int n;			// char转换成的数字（0-9）
				int idx = -1;	// 哪个loc 下标被选中
				do
				{
					c = fgetc(fp);
					if (c == ' ' || c == EOF || c == '\r' || c == '\n')
					{
						if (idx >= 0 && idx < MAX_LOCATION_NUM)
							keep_mask[idx] = 1;
						if (c != ' ')
							break;
						idx = 0;
					}
					else
					{
						n = c - '0';
						idx = idx * 10 + n;
					}					
				} while (true);

				// 防止重复坐标导致长度统计错误
				int loc_num = 0;
				for (int i = 0; i < location_total_num; i++)
				{
					if (keep_mask[i] == 1)
						loc_num++;
				}

				// 选中loction
				p_conf->loc_num = loc_num;
				p_conf->locations = (cv::Point*)malloc(loc_num*sizeof(cv::Point));
				int loc_idx = 0;
				for (int i = 0; i < location_total_num; i++)
				{
					if (keep_mask[i] == 1)
					{
						p_conf->locations[loc_idx].x = p_total_locations[i].x;
						p_conf->locations[loc_idx].y = p_total_locations[i].y;
						loc_idx++;
					}
				}
				free(p_total_locations);
			}// end parse keep pos

			p_conf->loc_idxes = (int *)malloc(p_conf->loc_num * sizeof(int));
			for (int i = 0; i < p_conf->loc_num; i++){
				p_conf->loc_idxes[i] = i;
			}
		}
	}
	return 0;
}