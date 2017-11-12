#include <bits/stdc++.h>
#include "keying.h"
#include "projection.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

vector<Point2i> order_points(vector<Point2i> pts){
    vector<Point2i> ans;
    assert(pts.size()==4);
    // cout<<"Orig Points"<<pts<<endl;
    
    int topl=0;
    int topr=0;
    int botl=0;
    int botr=0;
    int sum[4];
    int diff[4];
    for(int i=0;i<pts.size();i++)
    {
        sum[i] = pts[i].x + pts[i].y;
        diff[i] = pts[i].x - pts[i].y;
    }
    
    for(int i=1;i<pts.size();i++)
    {
        if(sum[i] < sum[topl])
            topl = i;
        if(sum[i] > sum[botr])
            botr = i;
        if(diff[i] < diff[topr])
            topr = i;
        if(diff[i] > diff[botl])
            botl = i;
    }
    ans.push_back(pts[topl]);
    ans.push_back(pts[topr]);
    ans.push_back(pts[botr]);
    ans.push_back(pts[botl]);
    return ans;
}



vector<Point2i> get_corners_mask(const Mat& mask_fn){
    Mat mask;
    if (mask_fn.channels() > 2)
        cvtColor(mask_fn, mask,COLOR_BGR2GRAY);
    else
        mask =  mask_fn.clone();
    vector<Point2i> corners;
    goodFeaturesToTrack(mask, corners, 4, 0.01, 10);
    return order_points(corners);
}

vector<Point2i> get_corners_img(const Mat& img){
    vector<Point2i> ans;
    ans.push_back(Point2i(0,0));
    ans.push_back(Point2i((img.size[1] - 1), 0));
    ans.push_back(Point2i((img.size[1] - 1), (img.size[0] - 1)));
    ans.push_back(Point2i(0, (img.size[0] - 1)));
    return ans;
}

Mat get_transform(vector<Point2i> imgcorners,vector<Point2i> maskcorners){
    vector<Point2f> img_corner,mask_corner;
    for(int i=0;i<imgcorners.size();i++){
        img_corner.push_back(Point2f(imgcorners[i].x,imgcorners[i].y));
        mask_corner.push_back(Point2f(maskcorners[i].x,maskcorners[i].y));
    }
    return getPerspectiveTransform(img_corner, mask_corner);
}

Mat project_to_mask(const Mat& img,const Mat& mask){
    Mat new_img;
    vector<Point2i> mask_corner = get_corners_mask(mask);
    vector<Point2i> img_corner = get_corners_img(img);
    // cout<<"Mask Corner"<<mask_corner<<endl;
    // cout<<"Img_Corner"<<img_corner<<endl;
    Mat transform = get_transform(img_corner,mask_corner);
    // cout<<"got mat\n"<<transform<<endl;
    warpPerspective(img, new_img,transform, Size(mask.size[1], mask.size[0]));
    // imshow("new_img",new_img);
    // waitKey(0);
    return new_img;
}

Mat project_img(const Mat& img, const Mat& bg, const Mat& mask_fn, param_ycrcb key_param ){
    Mat ans;

    Mat key_mask = get_mask(img,key_param);
    Mat bgra = get_bgra(img,key_mask);
    Mat res = project_to_mask(bgra,mask_fn);
    vector<Mat> planes_res;
    vector<Mat> planes_bg;
    split(res,planes_res);
    split(bg,planes_bg);
    vector<Mat> planes_res_float(planes_bg.size());
    vector<Mat> planes_bg_float(planes_bg.size());
    Mat mask;
    planes_res[3].convertTo(mask,CV_32FC1,1.0/255);
    vector<Mat> planes_ans;
    for(int i=0;i<planes_bg.size();i++){
        planes_res[i].convertTo(planes_res_float[i],CV_32FC1,1.0/255);
        planes_bg[i].convertTo(planes_bg_float[i],CV_32FC1,1.0/255);
        Mat temp1,temp2;
        multiply(mask,planes_res_float[i],temp1);
        multiply((1-mask),planes_bg_float[i],temp2);
        Mat temp3,temp4;
        temp3 = (temp1 +  temp2)*255;
        temp3.convertTo(temp4,CV_8UC1);
        planes_ans.push_back(temp4);
    }
    merge(planes_ans,ans);
    return ans;
}

// int main(){
//     Mat img = imread("../../image/twoperson.png",CV_LOAD_IMAGE_COLOR);
//     Mat bg = imread("../../image/bg.jpg",CV_LOAD_IMAGE_COLOR);
//     Mat mask = imread("../../image/mask.jpg");
//     Rect2d r = get_region(img);
//     param_ycrcb key_param = get_params_ycrcb(img,r);
//     Mat res = project_img(img,bg,mask,key_param);
//     return 0;

// }