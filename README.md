# PlaceRecognitonDBoW2
place recognition program using DBoW2 library

[DBow2 깃허브링크](https://github.com/dorian3d/DBoW2)
### 구현설명
- PyQT5로 기능사용 및 c++코드에서 DBOW2라이브러리를 사용하여 장소인식 합니다.

### 필요한 라이브러리
- python3.8 : 기본 3도 잘되지만, 저는 3.8버전을 사용해서 이렇게합니다.
- OpenCV3
- DBow2
  
### 설치하기 

```
pip3.8 install pyqt5
mkdir build
```

### 실행하기
```
sh test.sh
```

### 구현 모습

: Database내에서 선택한 query 이미지와 같은 유사한 장소를 찾아내서, 오른쪽의 이미지가 같은 장소로 바뀝니다. 

유사도가 높으면 Database에서 인덱스를 출력하고, 낮으면 "fail"을 출력합니다.

<p align="center"><img src="./images/PR_video.gif"  width="500"/></p>