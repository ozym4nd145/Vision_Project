#include <bits/stdc++.h>
#include "helper.h"
#include "keying.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

Mat INT_1C,FLOAT_1C,INT_3C,FLOAT_3C;
Mat FLOAT_1C_2,FLOAT_1C_3,FLOAT_1C_4;
std::vector<Mat> PLANES(4);
Mat INT_4C;

Rect2d get_region(const Mat& img){
    Rect2d r = selectROI(img);
    destroyAllWindows();
    return r;
}

param_ycrcb get_params_ycrcb(const Mat& img, Rect2d region){
    cvtColor(img, INT_3C,COLOR_BGR2YCrCb);
    Mat green_template = INT_3C(region);
    Scalar mean_key;
    mean_key = mean(green_template);
    param_ycrcb params;
    params.Cr = mean_key[1];
    params.Cb = mean_key[2];
    return params;
}

param_hls get_params_hls(const Mat& img, Rect2d region){
    // Mat hls_img;
    // cvtColor(img, hls_img,COLOR_BGR2HLS);
    cvtColor(img, INT_3C,COLOR_BGR2HLS);
    
    Mat green_template = INT_3C(region);
    Scalar mean_key,std_key;
    meanStdDev(green_template,mean_key,std_key);
    return param_hls(mean_key,std_key);
}

// Mat brighten(const Mat& img, double alpha, double beta, double gamma){
//     Mat cor_img = img.clone();
//     Mat bright_img = alpha * cor_img + beta;
//     Mat gam_cor;
//     pow(gam_cor, gamma,bright_img);
//     return gam_cor;
// }

Mat segment_ycrcb(const Mat& orig, const param_ycrcb& params, double tola, double tolb){
    cvtColor(orig, INT_3C, COLOR_BGR2YCrCb);    
    double Cb_key = params.Cb;
    double Cr_key = params.Cr;
    // Mat planes[3];
    // Mat ycrcb_im;
    INT_3C.convertTo(FLOAT_3C,CV_32FC3);
    split(FLOAT_3C,PLANES);
    // Mat blue = planes[2];
    // Mat red = planes[1];
   
    // Mat diffbsq;
    pow((PLANES[2] - Cb_key),2,FLOAT_1C);

    // Mat diffrsq;
    pow((PLANES[1] - Cr_key),2,FLOAT_1C_2);

    // Mat dist;
    sqrt((FLOAT_1C + FLOAT_1C_2),FLOAT_1C_3);

    // Mat mask;
    FLOAT_1C_4 =  ((FLOAT_1C_3 - tola)*1.0 / (tolb - tola));

    FLOAT_1C_4.setTo(0.0, FLOAT_1C_3 < tola);
    FLOAT_1C_4.setTo(1.0, FLOAT_1C_3 > tolb);
    return FLOAT_1C_4;
}

void mod_mask(Mat& mask, double low, double high){
    // Mat mask = mask_fn.clone();
    mask.setTo(1.0,mask > high);
    mask.setTo(0.0,mask < low);
    return;
}

Mat get_mask(const Mat& img,param_ycrcb params, double tola, double tolb, double low_thresh, double high_thresh, double sz, double space,double alpha,double beta,double gamma){
    // cout<<"Entered get mask"<<endl;
    // Mat brimg;
    // brimg = brighten(img,alpha,beta,gamma);
    // clock_t start = clock(); 
    
    bilateralFilter(img,INT_3C,sz, space, space);
    // cout<<"bilateral filter "<<(clock() - start)*1.0/CLOCKS_PER_SEC<<endl;
    // start = clock(); 
    // Mat mask;
    FLOAT_1C = segment_ycrcb(INT_3C, params, tola, tolb);
    // cout<<"segment "<<(clock() - start)*1.0/CLOCKS_PER_SEC<<endl;
    // start = clock(); 
    
    mod_mask(FLOAT_1C, low_thresh, high_thresh);
    // cout<<"mod mask "<<(clock() - start)*1.0/CLOCKS_PER_SEC<<endl;
    // start = clock(); 
    
    return FLOAT_1C;
}
Mat get_bgra(const Mat& img,const Mat& mask){
    // std::vector<Mat> planes;
    split(img,PLANES);
    // Mat mask_temp,mask_int;
    FLOAT_1C = mask*255;
    FLOAT_1C.convertTo(INT_1C,CV_8UC1);
    PLANES.push_back(INT_1C);
    // PLANES[3] =  INT_1C;
    // Mat res;
    merge(PLANES,INT_4C);
    return INT_4C;
}



// int main (int argc, char **arv)
// {
//     // Read image
//     Mat img = imread("../../keying/image/img01.png",CV_LOAD_IMAGE_COLOR);
//     // Mat temp;
//     // cvtColor(img, temp, COLOR_BGR2YCrCb);
//     // imshow("temp",temp);
//     // Mat img;
//     // img_mat.convertTo(img,CV_32FC3);
//     // cout<<img.at<Vec3f>(10,10)<<endl;
    
//     // Rect2d r = get_region(img);
//     // cout<<r<<endl;
//     Rect2d r(415,20,376,684);
//     // cout<<img.size[0]<<img.size[1]<<endl;
//     Point2i p(10,20);
//     cout<<p.x<<endl;
    
//     param_ycrcb params = get_params_ycrcb(img,r);
//     // cout<<params.Cb<<" "<<params.Cr<<endl;

//     Mat mask = get_mask(img,params); 
//     // Crop image
//     // Mat imCrop = im(r);
     
//     // Display Cropped Image
//     // imshow("Mask", mask);
//     // double mi,ma;
//     // minMaxLoc(mask,&mi,&ma);
//     // cout<<"Min-max"<<mi<<" "<<ma<<endl;
//     // waitKey(0);
//     Mat res = get_bgra(img,mask);
//     imwrite("temp.png",res);
     
//     return 0;
// }
