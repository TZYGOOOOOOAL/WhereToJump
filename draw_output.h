#ifndef __WHERETOJUMP_DRAW_OUTPUT_H__
#define __WHERETOJUMP_DRAW_OUTPUT_H__

#include "vector"
#include "main.h"

using namespace cv;
using namespace std;

void draw_one_id_on_one_place(Mat &map, cv::Point &location, int player_id, Scalar& player_color, 
	int circle_r, double font_scale);
void draw_all_ids_on_one_place(Mat &map, cv::Point &center_loc, vector<int>& player_ids_in_one_place, vector<Scalar>& player_colors);
void draw_players_on_map(Config* p_conf, cv::Scalar *colors);

#endif