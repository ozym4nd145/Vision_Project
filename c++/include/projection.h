#ifndef PROJECTION_H
#define PROJECTION_H

#include <bits/stdc++.h>
#include "helper.h"
#include "keying.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;

void read_mask_corner_points(string filename);
void fill_img_corners(const Mat& img);
Mat project_to_mask(const Mat& img,int frame_no,Size bg_size);
Mat project_img(const Mat& img, const Mat& bg, param_ycrcb key_param, int frame_no);


#endif