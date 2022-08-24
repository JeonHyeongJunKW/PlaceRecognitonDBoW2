#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include "DBoW2.h"

using namespace std;
using namespace cv;
using namespace DBoW2;

const static auto& _orb_OrbHandle = ORB::create(2000,1.2,8,31,0,2);

class loop_detector
{
    public:
    OrbVocabulary _pvoc;//불러온 Vocabulary를 저장할 변수
    OrbDatabase _pdb;//불러온 데이터베이스를 저장할 변수 
    vector<vector<Mat>> word_datas;
    vector<vector<KeyPoint>> keypoints_datas;
    int image_size = 0;
    vector<String> SeqNames;
    public:
    
    loop_detector();
    void GetFeature(Mat image, vector<Mat>&features, vector<KeyPoint> &keypoints);//이미지에서 vector형태로 feature를 얻습니다. 
    void AddFeatureToVoc(vector<Mat> &features, vector<KeyPoint> &keypoints);//feature를 vocabulary에 추가합니다.
    void CreateVoc();//추가된 image feature들로 vocabulary를 만듭니다.
    void CreateDB();//추가된 image feature들로 vocabulary를 만듭니다.
    void AddImagesToDB();//DB에 현재이미지의 Feature를 추가합니다. 이때, 상대적인 키포인트 위치들도 저장합니다. 
    void SaveSeqNames(vector<String> argNames);
    int FindQuIdx(String QuName);
    void SaveData(String DbName);
    void LoadData(String DbName);
    int loopDetect(int idx);
    void QueryDB(int idx, QueryResults &ret);
    void GetNormal_denom(int idx,double &denom);
    String getNameFromIdx(int idx);
    void SortResult(QueryResults &ret,vector<Result> &sorted_ret);
    void GroupMatch(vector<Result> &sorted_ret,int idx,double denom, vector<int> &groups, int &best_idx);
    int GometryCheck(int idx, int last_cand_idx);
};