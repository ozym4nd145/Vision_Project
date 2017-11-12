#include <bits/stdc++.h>
#include "helper.h"
#include "keying.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

Rect2d get_region(Mat img){
    Rect2d r = selectROI(img);
    destroyAllWindows();
    return r;
}

param_ycrcb get_params_ycrcb(Mat img, Rect2d region){
    Mat ycrcb_img;
    cvtColor(img, ycrcb_img,COLOR_BGR2YCrCb);
    Mat green_template = ycrcb_img(region);
    Scalar mean_key;
    mean_key = mean(green_template);
    param_ycrcb params;
    params.Cr = mean_key[1];
    params.Cb = mean_key[2];
    return params;
}

param_hls get_params_hls(Mat img, Rect2d region){
    Mat hls_img;
    cvtColor(img, hls_img,COLOR_BGR2HLS);
    Mat green_template = img(region);
    Scalar mean_key,std_key;
    meanStdDev(green_template,mean_key,std_key);
    return param_hls(mean_key,std_key);
}

Mat brighten(Mat img, double alpha, double beta, double gamma){
    Mat cor_img = img.clone();
    Mat bright_img = alpha * cor_img + beta;
    Mat gam_cor;
    pow(gam_cor, gamma,bright_img);
    return gam_cor;
}

Mat segment_ycrcb(const Mat& orig, const param_ycrcb& params, double tola, double tolb){
    Mat ycrcb_im_mat;
    cvtColor(orig, ycrcb_im_mat, COLOR_BGR2YCrCb);
    double Cb_key = params.Cb;
    double Cr_key = params.Cr;
    Mat planes[3];
    Mat ycrcb_im;
    ycrcb_im_mat.convertTo(ycrcb_im,CV_32FC3);
    split(ycrcb_im,planes);
    Mat blue = planes[2];
    Mat red = planes[1];
   
    Mat diffbsq;
    pow((blue - Cb_key),2,diffbsq);

    Mat diffrsq;
    pow((red - Cr_key),2,diffrsq);

    Mat dist;
    sqrt((diffbsq + diffrsq),dist);

    Mat mask;
    mask =  ((dist - tola)*1.0 / (tolb - tola));

    mask.setTo(0.0, dist < tola);
    mask.setTo(1.0, dist > tolb);
    return mask;
}

Mat mod_mask(const Mat& mask_fn, double low, double high){
    Mat mask = mask_fn.clone();
    mask.setTo(1.0,mask > high);
    mask.setTo(0.0,mask < low);
    return mask;
}

Mat get_mask(const Mat& img,param_ycrcb params, double tola, double tolb, double low_thresh, double high_thresh, double sz, double space,double alpha,double beta,double gamma){

    Mat brimg;
    // brimg = brighten(img,alpha,beta,gamma);
    bilateralFilter(img,brimg,sz, space, space);
    Mat mask;
    // imshow("bright img",brimg);
    mask = segment_ycrcb(brimg, params, tola, tolb);
    // imshow("Mask after Segment",mask);
    mask = mod_mask(mask, low_thresh, high_thresh);
    // imshow("mask after Mod",mask);    
    return mask;
}
Mat get_bgra(const Mat& img,const Mat& mask){
    std::vector<Mat> planes;
    split(img,planes);
    Mat mask_temp,mask_int;
    mask_temp = mask*255;
    mask_temp.convertTo(mask_int,CV_8UC1);
    planes.push_back(mask_int);
    Mat res;
    merge(planes,res);
    return res;
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
