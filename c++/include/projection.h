#ifndef PROJECTION_H
#define PROJECTION_H

#include <bits/stdc++.h>
#include "helper.h"
#include "keying.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;

vector<Point2i> order_points(vector<Point2i> pts);
vector<Point2i> get_corners_mask(const Mat& mask_fn);
vector<Point2i> get_corners_img(const Mat& img);
Mat get_transform(vector<Point2i> imgcorners,vector<Point2i> maskcorners);
Mat project_to_mask(const Mat& img,const Mat& mask);
Mat project_img(const Mat& img, const Mat& bg, const Mat& mask, param_ycrcb key_param);


#endif