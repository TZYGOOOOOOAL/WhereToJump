#ifndef __WHERETOJUMP_MAIN_H__
#define __WHERETOJUMP_MAIN_H__

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

typedef enum
{
	RAND_NOREPEAT,
	RAND_REPEAT
}RandMode;

typedef struct
{
	cv::Mat map;
	RandMode rang_mode;
	int loc_num;
	cv::Point* locations;
	int player_num;
	int *loc_idxes;
}Config;
void _init(Config* p_conf);
void _deinit(Config* p_conf);

#endif