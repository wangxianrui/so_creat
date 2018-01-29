//
// Created by wxrui on 2017/11/16.
//
#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "Watermark.h"
#include "ConstW.h"
#include "RS/ecc.h"

using namespace std;
using namespace cv;

float Watermark::detectBlind(string dstfile) {
    Mat dstimg = imread(dstfile, 0);
    if (dstimg.empty())
        cout << "read dstimg error!" << endl;
    dstimg.convertTo(dstimg, CV_32F);
    resize(dstimg, dstimg, Size(standW, standH));
    dct(dstimg, dstimg);
    float *dctYpart = (float *) dstimg.data;
    float *zigYpart = new float[standW * standH];
    zigZag(zigYpart, dctYpart, standW, standH);
    float *extFinger = new float[FINGERLENGTH];
    memcpy(extFinger,zigYpart+INSPOSITION,sizeof(float)*FINGERLENGTH);
    float *rsCode = new float[RSLENGTH];
    getRS(extFinger, rsCode);
    float *extWK = new float[WKLENGTH];
    /****************rsCode --> extwk****************************************/
    unsigned char code[RSLENGTH];
    for (int i = 0; i < RSLENGTH; i++)
        code[i] = rsCode[i] + '0';
    unsigned char outputarr[WKLENGTH];
    initialize_ecc();
    decode_data(code, outputarr);
    for (int i = 0; i < WKLENGTH; i++)
        extWK[i] = outputarr[i] - '0';
    /****************rsCode -->extwk****************************************/
    float sim = distance(wk, extWK, WKLENGTH);
    delete[] zigYpart;
    delete[] extFinger;
    delete[] rsCode;
    delete[] extWK;
    return sim;
}

void Watermark::getRS(float *extFinger, float *rsCode) {
    float *fingerPart = new float[FINGERLIB];
    float *sim = new float[2 * FINGERLIB];
    int maxsim;
    for (int i = 0; i < RSLENGTH / 4; i++) {
        memcpy(fingerPart, extFinger + i * FINGERLIB, sizeof(float) * FINGERLIB);
        for (int j = 0; j < 2 * FINGERLIB; j++) {
            sim[j] = similar(fingerLib[j], fingerPart, FINGERLIB);
            if (j == 0)
                maxsim = 0;
            else if (sim[j] > sim[maxsim])
                maxsim = j;
        }
        maxsim /= 32;
        rsCode[i * 4] = maxsim / 8;
        rsCode[i * 4 + 1] = (maxsim % 8) / 4;
        rsCode[i * 4 + 2] = ((maxsim % 8) % 4) / 2;
        rsCode[i * 4 + 3] = (((maxsim % 8) % 4) % 2);
    }
    delete[] fingerPart;
    delete[] sim;
}

float Watermark::getPSNR(string srcfile, string dstfile) {
    Mat srcimg = imread(srcfile);
    Mat dstimg = imread(dstfile);
    Mat difmat;
    absdiff(srcimg, dstimg, difmat);
    difmat.convertTo(difmat, CV_32F);
    difmat = difmat.mul(difmat);

    Scalar s = sum(difmat);
    float sse = s.val[0] + s.val[1] + s.val[2];

    if (sse <= 1e-10)
        return 0;
    else {
        float mse = sse / (float) (srcimg.channels() * srcimg.total());
        float psnr = 10.0 * log10((255 * 255) / mse);
        return psnr;//返回PSNR
    }
}

