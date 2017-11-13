#include <bits/stdc++.h>
#include "keying.h"
#include "projection.h"
#include "helper.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

int main(int argc,char**argv){

    if(argc<5){
        cout<<"Correct Usage"<<endl<<"./virtual_studio <green_screen_video> <background_video> <mask file> <output_path>\n"<<endl;
        exit(-1);
    }
    VideoCapture cap_green(argv[1]);
    VideoCapture cap_vid(argv[2]);
    
    read_mask_corner_points(argv[3]);

    double fps = cap_green.get(CV_CAP_PROP_FPS);
    Mat img,bg,res;
    cap_green.read(img);
    cap_vid.read(bg);

    Rect2d region = get_region(img);
    param_ycrcb key_param = get_params_ycrcb(img,region);
    fill_img_corners(img);
    
    int ct=0;
    int fourcc = VideoWriter::fourcc('X','V','I','D');
    string out_path = argv[4];
    VideoWriter output_vid;
    output_vid.open(out_path,fourcc, fps, Size(bg.size[1],bg.size[0]));
    
    while(true){
        clock_t start = clock();
        if (!(cap_green.read(img) && cap_vid.read(bg)))
            break;
        res = project_img(img,bg,key_param,ct+1);
        // imshow("Video",res);
        // char file_name[100];
        // sprintf(file_name,"../temp/img%04d.png",ct);
        ct++;
        // imwrite(file_name,res);
        output_vid.write(res);
        clock_t end = clock();
        cout<<"CT: "<<ct<<": "<<(end-start)*1.0/CLOCKS_PER_SEC<<endl;
		// if(waitKey(1) == 27){ 
        //     break; 
        // }
    }
    output_vid.release();

    return 0;
    

}