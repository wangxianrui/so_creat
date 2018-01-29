#include <iostream>
#include <fstream>
#include <vector>
#include <dirent.h>
#include <cstring>
#include <algorithm>
#include <iomanip>
#include <regex>
#include "Watermark.h"

using namespace std;

void getFiles(string cate_dir, vector<string> &files, string substring = "") {
    DIR *dir;
    struct dirent *ptr;

    if ((dir = opendir(cate_dir.c_str())) == NULL) {
        perror("Open dir error...");
        exit(1);
    }
    while ((ptr = readdir(dir)) != NULL) {
        if (ptr->d_type == 8 && (bool) strstr(ptr->d_name, substring.c_str()))    ///file
            files.push_back(ptr->d_name);
    }
    closedir(dir);
    sort(files.begin(), files.end());
}

int main() {

    /***************test***************/
///*
    clock_t start, end;
    cout << setw(15) << "inserttime" << setw(15) << "sim" << setw(15) << "detecttime" << setw(15) << "psnr"
         << endl;
    //加载水印
    Watermark wm("wk.txt");
    //水印嵌入
    start = clock();
    wm.imageInsert("test.jpg", "testwk.jpg");
    end = clock();
    cout << setw(15) << double(end - start) / CLOCKS_PER_SEC;
    //盲检测水印相似度
    start = clock();
    cout << setw(15) << wm.detectBlind("testwk.jpg");
    end = clock();
    cout << setw(15) << double(end - start) / CLOCKS_PER_SEC;
    //PSNR
    cout << setw(15) << wm.getPSNR("test.jpg", "testwk.jpg") << endl;
//*/
    /***************水印嵌入与检测***************/
/*
    vector<string> files;
    getFiles("../oriImg/", files);
    string srcfile, dstfile;
    ofstream output("resout");
    output << setw(15) << "sim" << setw(15) << "psnr" << endl;
    Watermark wm("wk.txt");
    for (string name : files) {
        srcfile = "../oriImg/" + name;
        dstfile = "../dstImg/" + name;
        cout << srcfile << endl;
        wm.imageInsert(srcfile, dstfile);
        output << setw(15) << wm.detectBlind(dstfile);
        output << setw(15) << wm.getPSNR(srcfile, dstfile) << endl;
    }
    output.close();
*/
    /***************攻击检测***************/
/*
    vector<string> files;
    getFiles("../attackImg/", files);
    ofstream output("attackout");
    string filename;
    Watermark wm("wk.txt");
    for (int i = 0; i < 7; i++)
        output << setw(15) << files[i].substr(7);
    for (int i = 0; i < files.size(); i++) {
        if (i % 7 == 0)
            output << endl;
        filename = "../attackImg/" + files[i];
        cout << filename << endl;
        output << setw(15) << wm.detectBlind(filename);
    }
    output.close();
*/
    /***************无水印检测***************/
/*
    vector<string> files;
    getFiles("../oriImg/", files);
    ofstream output("noneout");
    string filename;
    Watermark wm("wk.txt");
    for (int i = 0; i < files.size(); i++) {
        filename = "../oriImg/" + files[i];
        cout << filename << endl;
        output << setw(15) << wm.detectBlind(filename) << endl;
    }
    output.close();
*/

    cout << "sdfsdafdfasdf";

    return 0;
}
