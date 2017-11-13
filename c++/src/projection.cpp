#include <bits/stdc++.h>
#include "keying.h"
#include "projection.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

std::vector< std::vector<Point2f> > MASK_CORNERS;
std::vector<Point2f> IMAGE_CORNERS;
Mat FLOAT_1C_PROJ,INT_3C_PROJ;
Mat FLOAT_1C_PROJ_2,FLOAT_1C_PROJ_3,FLOAT_1C_PROJ_4;
std::vector<Mat> PLANES_RES,PLANES_BG,PLANES_ANS;
std::vector<Mat> PLANES_RES_FLOAT,PLANES_BG_FLOAT;
std::vector<Mat> INT_1C_PROJ;
Mat INT_4C_PROJ,INT_4C_2_PROJ;

void read_mask_corner_points(string filename){
    ifstream ifile;
    ifile.open(filename,ios::in);
    if(!ifile){
        cout << "Unable to open file "<<filename<<endl;
        exit(1);
    }
    while(!ifile.eof()){
        Point2f topl,topr,botr,botl;
        ifile >> topl.x >> topl.y >> topr.x >> topr.y >> botr.x >> botr.y >> botl.x >> botl.y;
        std::vector<Point2f> v = {topl,topr,botr,botl};
        MASK_CORNERS.push_back(v);
    }
}


void fill_img_corners(const Mat& img){
    IMAGE_CORNERS.push_back(Point2f(0,0));
    IMAGE_CORNERS.push_back(Point2f((img.size[1] - 1), 0));
    IMAGE_CORNERS.push_back(Point2f((img.size[1] - 1), (img.size[0] - 1)));
    IMAGE_CORNERS.push_back(Point2f(0, (img.size[0] - 1)));
}


Mat project_to_mask(const Mat& img,int frame_no,Size bg_size){
    Mat transform = getPerspectiveTransform(IMAGE_CORNERS,MASK_CORNERS[frame_no]);
    warpPerspective(img, INT_4C_2_PROJ,transform, bg_size);
    return INT_4C_2_PROJ;
}

Mat project_img(const Mat& img, const Mat& bg, param_ycrcb key_param, int frame_no){
    FLOAT_1C_PROJ = get_mask(img,key_param);
    INT_4C_PROJ = get_bgra(img,FLOAT_1C_PROJ);
    INT_4C_2_PROJ = project_to_mask(INT_4C_PROJ,frame_no,Size(bg.size[1],bg.size[0]));
    
    split(INT_4C_2_PROJ,PLANES_RES);
    split(bg,PLANES_BG);
    PLANES_RES[3].convertTo(FLOAT_1C_PROJ,CV_32FC1,1.0/255);
    PLANES_RES_FLOAT.resize(PLANES_BG.size());
    PLANES_BG_FLOAT.resize(PLANES_BG.size());
    INT_1C_PROJ.resize(PLANES_BG.size());
    PLANES_ANS.clear();
    
    for(int i=0;i<PLANES_BG.size();i++){
        PLANES_RES[i].convertTo(PLANES_RES_FLOAT[i],CV_32FC1,1.0/255);
        PLANES_BG[i].convertTo(PLANES_BG_FLOAT[i],CV_32FC1,1.0/255);
        multiply(FLOAT_1C_PROJ,PLANES_RES_FLOAT[i],FLOAT_1C_PROJ_2);
        multiply((1-FLOAT_1C_PROJ),PLANES_BG_FLOAT[i],FLOAT_1C_PROJ_3);
        FLOAT_1C_PROJ_4 = (FLOAT_1C_PROJ_2 +  FLOAT_1C_PROJ_3)*255;
        FLOAT_1C_PROJ_4.convertTo(INT_1C_PROJ[i],CV_8UC1);
        PLANES_ANS.push_back(INT_1C_PROJ[i]);
    }
    merge(PLANES_ANS,INT_3C_PROJ);
    return INT_3C_PROJ;
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