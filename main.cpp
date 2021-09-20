#include <stdlib.h>
#include <time.h>
#include <opencv.hpp>
using namespace cv;

#define PLAYER_NUM 4

char* map_path = "data/sg.png";

// 地点
int positions[PLAYER_NUM][2] = {
		{ 150, 200 },
		{ 640, 85 },
		{ 600, 580 },
		{ 110, 670 }
};

// 玩家颜色
Scalar colors[4] = {
	Scalar(50, 195, 210),
	Scalar(50, 105, 190),
	Scalar(180, 140, 40),
	Scalar(40, 165, 85)
};

// 随机生成, （洗牌）
void random_player_ids(char player_ids[PLAYER_NUM])
{
	int rand_idx;
	char tmp;
	for (int i = PLAYER_NUM - 1; i > 0; i--)
	{
		rand_idx = rand() % (i+1);
		tmp = player_ids[i];
		player_ids[i] = player_ids[rand_idx];
		player_ids[rand_idx] = tmp;
	}
}

// 绘制1个位置
void draw_id_on_map(Mat &map, char player_ids[PLAYER_NUM], int pos_idx)
{
	int player_idx = player_ids[pos_idx];
	int cx = positions[pos_idx][0];
	int cy = positions[pos_idx][1];
	// 绘制圆
	int r = (map.cols + map.rows) / 2 / 750.0 * 15 + 0.5;  // 750 分辨率图像圆圈半径 为15
	cv::Point pos(cx, cy);
	cv::circle(map, pos, r, colors[player_idx], -1);

	// 绘制字
	int font_face = cv::FONT_HERSHEY_SIMPLEX;
	double font_scale = 0.8 * r / 15;
	char str_id[5];
	sprintf(str_id, "%d", player_idx + 1);
	cv::Size str_size = cv::getTextSize(str_id, font_face, font_scale, 2, NULL);
	// 字居中
	int x_min = int(cx - str_size.width / 2.0 + 0.5);
	int y_min = int(cy + str_size.height / 2.0 + 0.5);
	cv::Point orig(x_min, y_min);		// 左下角
	cv::putText(map, str_id, orig, font_face, font_scale, Scalar(255, 255, 255), 2);
}

// 绘制所有玩家位置
void draw_ids_on_map(Mat &map, char player_ids[PLAYER_NUM])
{
	for (int i = 0; i < PLAYER_NUM; i++)
	{
		draw_id_on_map(map, player_ids, i);
	}
}

void test_rand(char player_ids[PLAYER_NUM]);

int main()
{
	srand((unsigned)time(NULL));

	char* player_ids = (char *)malloc(PLAYER_NUM);
	Mat map = cv::imread(map_path);

	for (int i = 0; i < PLAYER_NUM; i++){
		player_ids[i] = i;
	}

	//test_rand(player_ids);

	char* win_name = "result";
	while (1)
	{
		random_player_ids(player_ids);
		draw_ids_on_map(map, player_ids);

		cv::imshow(win_name, map);
		cv::waitKey();
		cv::destroyWindow(win_name);
	}

	free(player_ids);
	system("pause");
	return 0;
}

// 测试随机性
void test_rand(char player_ids[PLAYER_NUM])
{
	int i, j;
	int xxx[4][4];
	memset(xxx, 0, 16 * sizeof(int));
	for (i = 0; i < 1000000; i++)
	{
		random_player_ids(player_ids);
		for (j = 0; j < 4; j++)
		{
			xxx[j][player_ids[j]]++;
		}
	}

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			printf("%d,", xxx[i][j]);
		}
		printf("\n");
	}
}