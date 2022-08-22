#include "loopdetector.h"
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include "DBoW2.h"
#include <random>
using namespace std;
using namespace cv;
using namespace DBoW2;


loop_detector::loop_detector()
{
    _pvoc = OrbVocabulary(9,3,TF_IDF,L1_NORM);
}
void loop_detector::GetFeature(Mat image, vector<Mat>&features, vector<KeyPoint> &keypoints)
{
    Mat descriptor;
    _orb_OrbHandle->detectAndCompute(image,noArray(),keypoints,descriptor);
    features.resize(descriptor.rows);
    for(int i=0; i<descriptor.rows; i++)
    {
        features[i] = descriptor.row(i);
    }
}
void loop_detector::AddFeatureToVoc(vector<Mat> &features, vector<KeyPoint> &keypoints)
{
    word_datas.push_back(features);
    keypoints_datas.push_back(keypoints);
    image_size+=1;
}

void loop_detector::CreateVoc()
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(0,word_datas.size()-1);
    vector<vector<Mat>> sample_word;
    for(int i=0; i<200;i++)
    {
        sample_word.push_back(word_datas[dis(gen)]);
    }

    _pvoc.create(sample_word);
}
void loop_detector::CreateDB()
{
    _pdb = OrbDatabase(_pvoc,true,2);
}
void loop_detector::AddImagesToDB()
{
    for(int i=0; i< image_size; i++)
    {
        _pdb.add(word_datas[i]);
    }
}