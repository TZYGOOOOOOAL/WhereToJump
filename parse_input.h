#ifndef __WHERETOJUMP_PARSE_INPUT_H__
#define __WHERETOJUMP_PARSE_INPUT_H__

#include "main.h"

int parse_location(char* loc_filepath, cv::Point* locations, int* total_num);
int parse_conf(char* conf_filepath, Config* p_conf);

#endif
