#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <time.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "main.h"
#include "parse_input.h"
#include "draw_output.h"
#include "utils.h"

using namespace cv;

// Íæ¼ÒÑÕÉ«
Scalar colors[4] = {
	Scalar(50, 195, 210),
	Scalar(50, 105, 190),
	Scalar(180, 140, 40),
	Scalar(40, 165, 85)
};


int main()
{
	srand((unsigned)time(NULL));
	Config conf;
	_init(&conf);

	parse_conf("config.txt", &conf);


	//test_rand(player_ids);

	char* win_name = "result";
	Mat src_map = conf.map.clone();
	while (1)
	{
		src_map.copyTo(conf.map);
		random_arrangement(&conf);
		draw_players_on_map(&conf, colors);

		cv::imshow(win_name, conf.map);
		cv::waitKey();
		cv::destroyWindow(win_name);
	}

	_deinit(&conf);
	system("pause");
	return 0;
}

void _init(Config* p_conf)
{
	p_conf->loc_num = 0;
	p_conf->locations = NULL;
	p_conf->loc_idxes = NULL;
	p_conf->map = Mat();
	p_conf->player_num = 0;
	p_conf->rang_mode = RAND_NOREPEAT;
}

void _deinit(Config* p_conf)
{
	free(p_conf->locations);
	p_conf->locations = NULL;
	free(p_conf->loc_idxes);
	p_conf->loc_idxes = NULL;
}