#include "loopdetector.h"
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include "DBoW2.h"
#include <algorithm>
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
    cout<<db_name<<endl;
    cout<<voc_name<<endl;
    _pvoc.load(voc_name);
    _pdb.load(db_name);
    
    cout<<_pdb.size()<<endl;
}

int loop_detector::loopDetect(int idx)
{
    QueryResults ret;
    QueryDB(idx,ret);
    double denom;
    GetNormal_denom(idx,denom);
    vector<Result> Results;
    SortResult(ret,Results);
    vector<int> candidate_groups;
    int last_cand_idx;
    GroupMatch(Results,idx,denom,candidate_groups,last_cand_idx);
    int match_idx =GometryCheck(idx,last_cand_idx);
    
    return match_idx;
}

String loop_detector::getNameFromIdx(int idx)
{
    return SeqNames[idx];
}

void loop_detector::QueryDB(int idx, QueryResults &ret)
{
    _pdb.query(word_datas[idx],ret,image_size,image_size);
    // for(int i=0; i<10;i++)
    // {
    //     cout<<"idx : "<<ret[i].Id<<", Score"<<ret[i].Score<<endl;
    // }
}

void loop_detector::GetNormal_denom(int idx, double &denom)
{
    int neighbor_bound = 3;
    int neighbor_start =0;
    int neighbor_end =0;
    if(idx <neighbor_bound)
    {
        neighbor_start =idx+1;
        neighbor_end =idx+neighbor_bound;
    }
    else
    {
        neighbor_start =idx-neighbor_bound;
        neighbor_end =idx-1;
    }

    BowVector v;
    _pvoc.transform(word_datas[idx],v);

    double min_score_prev = 0.005;
    double min_score =100;
    for(int i=neighbor_start; i<=neighbor_end; i++)
    {
        BowVector v_neighbor;
        _pvoc.transform(word_datas[i],v_neighbor);
        double score = this->_pvoc.score(v,v_neighbor);
        if(score<min_score_prev)
        {
            continue;
        }
        if(score<min_score)
        {
            min_score= score;
        }
    }
    if(min_score==100)
    {
        cout<<"주변과 너무 다릅니다."<<endl;
    }
    else
    {
        cout<<"최솟값 : "<<min_score<<endl;
    }
    denom = min_score;
}
void loop_detector::SortResult(QueryResults &ret,vector<Result> &sorted_ret)
{
    sorted_ret = vector<Result>(ret.size());

    for(int i=0; i<ret.size();i++)
    {
        sorted_ret[(int)(ret[i].Id)] = ret[i];
    }
    // cout<<sorted_ret.size()<<endl;
}
void loop_detector::GroupMatch(vector<Result> &sorted_ret,int idx,double denom, vector<int> &groups, int &best_idx)
{
    int group_size = 8;
    int query_place_bound = 20;
    double max_score =0;
    vector<int> max_group;
    int groups_count =0;
    int max_idx =-1;
    
    for(int i=0; i<sorted_ret.size(); i++)
    {
        if(i<group_size-1)
        {
            continue;//group이 생성이 안되는 곳이면 버립니다.
        }
        if(abs(idx-i)<query_place_bound)
        {//너무 가까운 장소는 배제합니다. 
            continue;
        }
        bool bad_score= false;
        double sum_score = 0;
        vector<int> cand_group;
        int temp_max_score =0;
        int temp_max_idx =-1;
        for(int place_idx=group_size; place_idx>=0; place_idx--)
        {
            double norm_score = sorted_ret[i-place_idx].Score/denom;
            cand_group.push_back(i-place_idx);
            if(temp_max_score<norm_score)
            {
                temp_max_idx = i-place_idx;
                temp_max_score = norm_score;
            }
            if(norm_score <0.3)
            {
                bad_score =true;
                break;
            }
            sum_score +=norm_score;
        }
        if(bad_score)
        {
            continue;
        }
        groups_count++;
        if(sum_score>max_score)
        {
            max_score = sum_score;
            max_group = cand_group;
            max_idx = temp_max_idx;
        }
    }
    groups =max_group;
    best_idx = max_idx;
}
int loop_detector::GometryCheck(int idx, int last_cand_idx)
{

    return last_cand_idx;
}