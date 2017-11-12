#ifndef HELPER_H
#define HELPER_H

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <bits/stdc++.h>
using namespace std;

struct param_ycrcb{
    double Cr;
    double Cb;
    param_ycrcb(double r,double b){
        Cr =r;
        Cb = b;
    }
    param_ycrcb(){
        Cr=0.0;
        Cb=0.0;
    }
};

struct param_hls{
    double h_mean,l_mean,s_mean;
    double h_std,l_std,s_std;
    param_hls(double h_m, double l_m,double s_m, double h_s,double l_s,double s_s){
        h_mean=h_m;
        l_mean=l_m;
        s_mean=s_m;
        h_std=h_s;
        l_std=l_s;
        s_std=s_s;
    }
    param_hls(cv::Scalar mean, cv::Scalar std){
        h_mean=mean[0];
        l_mean=mean[1];
        s_mean=mean[2];
        h_std=std[0];
        l_std=std[1];
        s_std=std[2];
    }
};

string type2str(int type);

#endif