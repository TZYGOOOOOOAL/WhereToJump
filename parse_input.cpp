#include <stdio.h>
#include <string.h>
#include "parse_input.h"

#define IN_RANGE(x, min, max) (((x)>=(min)) && ((x) <= (max)))
#define IS_NUMBER(x) IN_RANGE((x-'0'), 0, 9)
#define IS_NOT_NUMBER(x) !(IS_NUMBER(x))

/*
ÿ����ͼ��Ӧ�����ļ�
��һ�� ����������  total num
���� # �ر�������
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
		if (!strcmp(str0, "#")){			// ע�� TODO����print��
			continue;
		}

		// ����ע�ͣ���Ϊ��ʵ����
		(*p_locations)[i].x = atoi(str0);
		(*p_locations)[i].y = atoi(str1);
		i++;
	}
	fclose(fp);
	return 0;
}

/*
map sg			����ͼ������ĸ��д��
player_num 4	�����������
rand_mode 0		�������ʽ,0���ظ���1���ظ���
keep_pos -1		���ص�mask��-1Ϊȫѡ��
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
			// ��ͼ�ļ�
			strcat(map_filepath, str1);
			strcat(map_filepath, ".png");
			p_conf->map = cv::imread(map_filepath);
			// �ص������ļ�
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

		/* ****** �����ĵص� ******* */
		else if (!strcmp(str0, "keep_pos"))
		{
			// ���ȱ�֤�ص������Ѿ�������
			if (p_total_locations == NULL){
				printf("Map locations data should load first !!!\n");
				return -1;
			}

			int loc_real_num = 0;
			unsigned int *keep_mask = (unsigned int *)malloc(location_total_num * sizeof(unsigned int));
			memset(keep_mask, 0, sizeof(unsigned int) * location_total_num);

			// �����Ҫ�ĵص�λ��
			if (!strcmp(str1, "-1")){		// ȫѡ
				memset(keep_mask, -1, sizeof(unsigned int) * location_total_num);
				loc_real_num = location_total_num;
			}
			else{
				int first_idx = atoi(str1);	// ��һ�����ִ���str1�ַ�����
				if (IN_RANGE(first_idx, 0, location_total_num-1))
					keep_mask[atoi(str1)] = 1;

				char c;
				int n;			// charת���ɵ����֣�0-9��
				int idx = -1;	// �ĸ�loc �±걻ѡ��
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

				// ��ֹ�ظ����굼�³���ͳ�ƴ���
				for (int i = 0; i < location_total_num; i++)
				{
					if (keep_mask[i] == 1)
						loc_real_num++;
				}
			}// end parse keep pos

			p_conf->locations_total = p_total_locations;
			p_conf->loc_total_num = location_total_num;
			p_conf->loc_keeped_num = loc_real_num;

			// TODO: ���ǲ��ܱ���ȫ��
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

		/* ****** δ֪�ֶ� ******* */
		else
		{
			printf("Unknown Segment : \"%s\"\n", str0);
		}
	}
	return 0;
}