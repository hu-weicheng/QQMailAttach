/****************************************************************
*Project:QQMailAttach
*Author:Raise
*错误代码表：
*0正常 -2无参数 -3文件大于50M，不用合并 -1子文件路径不对或不存在
*****************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <io.h>

#include <vld.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
using namespace std;

#define Byte50M  52428800

#define PRINTREPORT
#define REPORTSPLIT
void filemerge(char* logfile);

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
        printf("log file: %s\n", argv[i]);
        if (file_size2(argv[i]) < Byte50M)
        {
            filemerge(argv[i]);
        }
        else
        {
            printf("文件大于50M，不用合并。");
            exit(-3);
        }
    }
    return 0;
}

void filemerge(char* logfile)
{
    //1.从日志文件里读出文件列表,确定原文件的文件名.
    //a.将末尾的".log"去掉就是原文件名
    //b.读取日志文件里的子文件名
    //2.按顺序将各个文件的内容写到一个文件里
    string ofn(logfile);
    ofn = ofn.substr(0, ofn.length() - 4);
#ifdef PRINTREPORT
    cout << "源文件名:" << ofn << endl;
#endif /*PRINTREPORT*/
    vector<string> sfn;
    string fname;
    ifstream ifs(logfile, ios::binary | ios::in);
    while (getline(ifs, fname))
    {
        //剔除末尾的控制符
        size_t n = fname.find_last_of("\r\n");
        if (n != string::npos)
        {
            fname.erase(n, fname.size() - n);
        }
        //剔除路径获取文件名
        n = fname.find_last_of('\\');
        if (n != string::npos)
        {
            fname = fname.substr(n + 1, fname.size() - n);
            string locpath = logfile;
            n = locpath.find_last_of('\\');
            locpath = locpath.substr(0, n + 1);
            fname = locpath + fname;
        }
        sfn.push_back(fname);
        if (_access(fname.c_str(), 0) != 0)
        {
            cout << "error:文件不存在或路径不对(-1)。" << endl;
            ifs.close();
            exit(-1);
        }
    }
    ifs.close();
    cout << "共总" << sfn.size() << "个子文件。" << endl;
    ofstream outfs(ofn, ios::out | ios::binary);
    for (int i = 0; i < sfn.size(); i++)
    {
#ifdef PRINTREPORT
        cout << i << "正在引入文件:" << sfn[i] << endl;
#endif /*PRINTREPORT*/
        fstream infs(sfn[i], ios::in | ios::binary);
        char* tmp = new char[1];
        while (!infs.eof())
        {
            infs.read(tmp, 1);
            outfs.write(tmp, 1);
        }
        delete tmp;
        infs.close();
    }

    outfs.close();
}