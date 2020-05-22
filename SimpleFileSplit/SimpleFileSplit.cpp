/****************************************************************
*Project:QQMailAttach
*Author:Raise
*错误代码表：
*0正常 -2无参数 1文件小于50M，不用分割
*****************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#include <vld.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
using namespace std;

#define Byte50M  52428800

#define PRINTREPORT
#define REPORTSPLIT

#pragma execution_character_set("utf-8")

void filesplit(char* filename);

int file_size(char* filename)
{
    FILE* fp = fopen(filename, "r");
    if (!fp) return -1;
    fseek(fp, 0L, SEEK_END);
    int size = ftell(fp);
    fclose(fp);

    return size;
}

int file_size2(char* filename)
{
    struct stat statbuf;
    stat(filename, &statbuf);
    int size = statbuf.st_size;

    return size;
}



int main(int argc, char** argv)
{
    if (argc < 1)
    {
        printf("您没有输入参数！");
        exit(-2);
    }

    for (int i = 1; i < argc; i++)
    {
        printf("file size:%d %s\n", file_size(argv[i]), argv[i]);
        if (file_size(argv[i]) > Byte50M)
            filesplit(argv[i]);
        else
        {
            printf("文件小于50M,不分割。\n");
            exit(1);
        }

    }
    return 0;
}


void filesplit(char* filename)
{
    int fsize = file_size(filename);
    int num = fsize / Byte50M;//子文件个数
    if (fsize % Byte50M > 0)
        num++;
    int avg = fsize / num;
    //记录每个文件的大小
    vector<int> sfsize;//每个子文件的大小
    for (int i = 0; i < num - 1; i++)
        sfsize.push_back(avg);
    sfsize.push_back(fsize - avg * (num - 1));
    //预算文件名
    vector<string> fname;
    for (int i = 0; i < num; i++)
    {
        string str(filename);
        str += "_";
        stringstream ss;
        string temp;
        ss << i;
        ss >> temp;
        str += temp;
        str += ".div";
        fname.push_back(str);
    }
#ifdef REPORTSPLIT
    //写报告文件,合并文件的时候根据报告文件合并。
    string dlog = filename;
    dlog.append(".log");
    FILE* fp3 = fopen(dlog.c_str(), "w");
    for (int i = 0; i < num; i++)
    {
        int len = fname[i].size();
        char* buf3 = (char*)malloc(len);
        memcpy(buf3, fname[i].c_str(), len);
        fwrite(buf3, sizeof(char), len, fp3);
        char temp = '\n';
        fwrite(&temp, sizeof(char), 1, fp3);
        free(buf3);
    }
    fclose(fp3);
#endif /*REPORTSPLIT*/
    FILE* fp = fopen(filename, "rb");

    for (int i = 0; i < num; i++)
    {
        FILE* fp2 = fopen(fname[i].c_str(), "wb");

        //写文件内容
        char* buf2 = (char*)malloc(sfsize[i]);
        fread(buf2, 1, sfsize[i], fp);
        fwrite(buf2, 1, sfsize[i], fp2);
        free(buf2);

        fclose(fp2);
        //打印文件表
#ifdef PRINTREPORT
        cout << fname[i] << ":" << sfsize[i] << "Bytes" << endl;
#endif /*PRINTREPORT*/
    }
    fclose(fp);
}

