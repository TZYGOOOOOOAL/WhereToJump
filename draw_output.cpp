#include "main.h"
#include "draw_output.h"



// 绘制1个位置单个玩家
void draw_one_id_on_one_place(Mat &map, cv::Point &location, int player_id, Scalar& player_color,
	int circle_r, double font_scale)
{
	// 绘制圆
	cv::circle(map, location, circle_r, player_color, -1);
	
	// 绘制字
	int font_face = cv::FONT_HERSHEY_SIMPLEX;
	char str_id[5];
	sprintf(str_id, "%d", player_id + 1);
	cv::Size str_size = cv::getTextSize(str_id, font_face, font_scale, 2, NULL);
	// 字居中
	int x_min = int(location.x - str_size.width / 2.0 + 0.5);
	int y_min = int(location.y + str_size.height / 2.0 + 0.5);
	cv::Point orig(x_min, y_min);		// 左下角
	cv::putText(map, str_id, orig, font_face, font_scale, Scalar(255, 255, 255), 2);
	
	return;
}


// 绘制1个位置所有玩家
void draw_all_ids_on_one_place(Mat &map, cv::Point &center_loc, 
	vector<int>& player_ids_in_one_place, vector<Scalar>& player_colors)
{
	int player_num = player_ids_in_one_place.size();
	int cx = center_loc.x;
	int cy = center_loc.y;

	// 圆半径
	int r = (map.cols + map.rows) / 2 / 750.0 * 15 + 0.5;  // 750 分辨率图像圆圈半径 为15
	// 字大小
	double font_scale = 0.8 * r / 15;

	// 遍历每个玩家
	int d = 2 * r;
	double total_len = d * player_num;

	for (int i = 0; i < player_num; i++)
	{
		int x = cx + (i * d + r) - total_len/2;
		Point sub_center(x, cy);
		draw_one_id_on_one_place(map, sub_center, player_ids_in_one_place[i],
			player_colors[i], r, font_scale);
	}
	
	return;
}

// 绘制所有玩家位置
void draw_players_on_map(Config* p_conf, cv::Scalar *colors)
{
	vector<int> player_locs(p_conf->player_num);

	// 每个位置玩家
	for (int i = 0; i < p_conf->player_num; i++)
	{
		player_locs[i] = p_conf->loc_idxes[i];
	}

	for (int loc_idx = 0; loc_idx < p_conf->loc_num; loc_idx++)
	{
		vector<int> player_ids_in_one_place;
		vector<Scalar> player_colors;
		for (int player_idx = 0; player_idx < p_conf->player_num; player_idx++)
		{
			if (player_locs[player_idx] == loc_idx)
			{
				player_ids_in_one_place.push_back(player_idx);
				player_colors.push_back(colors[player_idx]);
			}
		}
		
		// 结束一个位置遍历
		if (!player_ids_in_one_place.empty())
			draw_all_ids_on_one_place(p_conf->map, p_conf->locations[loc_idx],
			player_ids_in_one_place, player_colors);

	}
	return;
}