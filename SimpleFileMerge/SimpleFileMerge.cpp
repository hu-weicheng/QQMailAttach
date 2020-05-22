/****************************************************************
*Project:QQMailAttach
*Author:Raise
*��������
*0���� -2�޲��� -3�ļ�����50M�����úϲ� -1���ļ�·�����Ի򲻴���
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
        printf("��û�����������");
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
            printf("�ļ�����50M�����úϲ���");
            exit(-3);
        }
    }
    return 0;
}

void filemerge(char* logfile)
{
    //1.����־�ļ�������ļ��б�,ȷ��ԭ�ļ����ļ���.
    //a.��ĩβ��".log"ȥ������ԭ�ļ���
    //b.��ȡ��־�ļ�������ļ���
    //2.��˳�򽫸����ļ�������д��һ���ļ���
    string ofn(logfile);
    ofn = ofn.substr(0, ofn.length() - 4);
#ifdef PRINTREPORT
    cout << "Դ�ļ���:" << ofn << endl;
#endif /*PRINTREPORT*/
    vector<string> sfn;
    string fname;
    ifstream ifs(logfile, ios::binary | ios::in);
    while (getline(ifs, fname))
    {
        //�޳�ĩβ�Ŀ��Ʒ�
        size_t n = fname.find_last_of("\r\n");
        if (n != string::npos)
        {
            fname.erase(n, fname.size() - n);
        }
        //�޳�·����ȡ�ļ���
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
            cout << "error:�ļ������ڻ�·������(-1)��" << endl;
            ifs.close();
            exit(-1);
        }
    }
    ifs.close();
    cout << "����" << sfn.size() << "�����ļ���" << endl;
    ofstream outfs(ofn, ios::out | ios::binary);
    for (int i = 0; i < sfn.size(); i++)
    {
#ifdef PRINTREPORT
        cout << i << "���������ļ�:" << sfn[i] << endl;
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