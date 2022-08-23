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
void loop_detector::SaveSeqNames(vector<String> argNames)
{
    SeqNames = argNames;
}
int loop_detector::FindQuIdx(String QuName)
{
    auto it = find(SeqNames.begin(),SeqNames.end(),QuName);
    if(it ==SeqNames.end())
    {
        return -1;
    }
    else
    {
        return (int)(it-SeqNames.begin());
    }
}
void loop_detector::SaveData(String DbName)
{
    FileStorage store(DbName+String("/Data.xml"),FileStorage::WRITE);
    for(int i=0; i<image_size;i++)
    {
        write(store,"keypoints_datas_"+to_string(i),keypoints_datas[i]);
        write(store,"word_datas_"+to_string(i),word_datas[i]);
    }
    write(store,"SeqNames",SeqNames);
    write(store,"image_size",image_size);
    store.release();
    string db_name = string(DbName.c_str())+string("/_pdb.yml.gz");
    string voc_name = string(DbName.c_str())+string("/_pvoc.yml.gz");
    _pdb.save(db_name);
    _pvoc.save(voc_name);
}
void loop_detector::LoadData(String DbName)
{
    word_datas.clear();
    keypoints_datas.clear();
    
    FileStorage store(DbName+String("/Data.xml"),FileStorage::READ);
    FileNode imsizeNode = store["image_size"];
    read(imsizeNode,image_size,0);
    FileNode seqNode = store["SeqNames"];
    read(seqNode,SeqNames);
    for(int i=0; i<image_size;i++)
    {
        vector<KeyPoint> tempkp;
        vector<Mat> tempWord;
        FileNode kpNode = store["keypoints_datas_"+to_string(i)];
        FileNode wNode = store["word_datas_"+to_string(i)];
        read(kpNode, tempkp);
        read(wNode,tempWord);

        keypoints_datas.push_back(tempkp);
        word_datas.push_back(tempWord);
    }

    store.release();
    cout<<DbName+String("/Data.xml")<<endl;
    string db_name = string(DbName.c_str())+string("/_pdb.yml.gz");
    string voc_name = string(DbName.c_str())+string("/_pvoc.yml.gz");
    _pvoc = OrbVocabulary(voc_name);
    _pdb = OrbDatabase(db_name);
}
int loop_detector::loopDetect(int idx)
{
    return idx;
}
String loop_detector::getNameFromIdx(int idx)
{
    return SeqNames[idx];
}