#ifndef KEYING_H
#define KEYING_H
#include <bits/stdc++.h>
#include "helper.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;


Rect2d get_region(const Mat& img);
param_ycrcb get_params_ycrcb(const Mat& img, Rect2d region);
// param_hls get_params_hls(Mat img, Rect2d region);
Mat brighten(Mat img, double alpha, double beta, double gamma);
Mat segment_ycrcb(const Mat& orig, const param_ycrcb& params, double tola, double tolb);
void mod_mask(Mat& mask_fn, double low, double high);
Mat get_mask(const Mat& img,param_ycrcb params, double tola=16.0, double tolb=50.0, double low_thresh=0.15, double high_thresh=0.25, double sz=2, double space=200,double alpha=1.0,double beta=0.0,double gamma=1.0);
Mat get_bgra(const Mat& img,const Mat& mask);


#endif