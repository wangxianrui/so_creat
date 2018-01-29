//
// Created by wxrui on 2017/11/15.
//

#ifndef WATERMARK_WATERMARK_H
#define WATERMARK_WATERMARK_H

#include <iostream>
#include <opencv/cv.h>

using namespace std;
using namespace cv;

class Watermark {
public:
    Watermark(string wkfile);

    void imageInsert(string srcfile, string dstfile);

    float detectBlind(string dstfile);

    float getPSNR(string srcfile, string dstfile);

private:
    float *wk;
    float **fingerLib;
    Rect rect;

    bool getWK(string wkfile);

    bool getFingerLib();

    void getBorder(Mat srcImg, int thres);

    float distance(float *ori, float *ext, int len);

    float similar(float *ori, float *ext, int len);

    float norm(float *temp, int len);

    void selectFinger(Mat imgMat, float *rsCode, float *finger);

    void juanji(float *Ypart, float *zsame, float *LocalMean, int width, int height);

    void getNVF(float *Ypart, float *nvf, int width, int height);

    void getTemplate(float *Ypart, float *finger, float *wkTemplate, int width, int height);

    void doInsert(float *Ypart, float *temeplate, int width, int height);

    void getRS(float *extFinger, float *rsCode);

    void zigZag(float *tarMat, float *srcMat, int width, int height);

    void izigZag(float *tarMat, float *srcMat, int width, int height);
};


#endif //WATERMARK_WATERMARK_H
