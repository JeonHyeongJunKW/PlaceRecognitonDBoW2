#include <iostream>
#include "loopdetector.h"
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include "DBoW2.h"
#include <cstdlib>
#include <algorithm>
#include <chrono>
using namespace chrono;

using namespace std;
using namespace cv;
using namespace DBoW2;

loop_detector g_detect;
int timing = 0;
extern "C" {
    void GetTime()
    {
        cout<<timing<<endl;
    }
    void MakeDB(wchar_t* path)
    {
        int len = 0;
        len = wcslen(path) + 1;
        char * c_path = new char[len];
        memset(c_path,0,len);
        wcstombs(c_path, path, len);
        string folder_name(c_path);
        delete c_path;
        vector<String> img_names;
        glob(folder_name,img_names,false);
        cout<<"시퀀스 길이 : "<<img_names.size()<<endl;
        sort(img_names.begin(),img_names.end());
        system_clock::time_point start = system_clock::now();
        
        for(int i=0; i<img_names.size(); i++)
        {
            Mat data_image = imread(img_names[i], cv::IMREAD_ANYCOLOR);
            vector<Mat> descriptor;
            vector<KeyPoint>  keypoint;
            g_detect.GetFeature(data_image,descriptor,keypoint);
            g_detect.AddFeatureToVoc(descriptor,keypoint);
            
        }
        system_clock::time_point end = system_clock::now();  
        microseconds microSec = duration_cast<microseconds>(end - start);  
        cout <<"word 뽑는데 걸린시간"<<microSec.count()/1000000. << " 초\n";
        timing =1;

        start = system_clock::now();
        g_detect.CreateVoc();
        end = system_clock::now();  
        microSec = duration_cast<microseconds>(end - start);  
        cout <<"VOC만드는데 걸린시간"<<microSec.count()/1000000. << " 초\n"; 

        timing =2;

        start = system_clock::now();
        g_detect.CreateDB();
        g_detect.AddImagesToDB();
        end = system_clock::now();  
        microSec = duration_cast<microseconds>(end - start);  
        cout <<"DB만들고 더하는데 걸린시간"<<microSec.count()/1000000. << " 초\n";  
        timing =3;
    }
}