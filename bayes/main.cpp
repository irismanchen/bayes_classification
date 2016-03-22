//
//  main.cpp
//  bayes
//
//  Created by 王曼晨 on 15/11/6.
//  Copyright © 2015年 王曼晨. All rights reserved.
//

#include<iostream>
#include<map>
#include<set>
#include<cmath>
#include<vector>
#include<algorithm>
#include<numeric>
#include<cstring>
#include<stdio.h>
#include<cstdlib>
#include<fstream>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<math.h>
#include<iomanip>
using namespace std;

class Bayes
{
private:
    vector< vector<string> > list_of_docs;  //词条向量
    vector<int> list_classes;               //训练数据的分类
    map<string,int>  my_vocab_list;         //单词列表
    int *return_vec;                        //记录每个新闻中各个单词出现的次数
    vector<vector<int>> train_mat;          //训练矩阵：总新闻个数＊总单词个数
    vector<vector<double>> vect;            //记录每个分类中每个单词的概率
    ifstream fin;
    ofstream fout;
    int num_words;
    double classpossible[5]={0.2,0.2,0.2,0.2,0.2};            //记录每个分类的概率
    int wordsinclass[5]={0};                //记录每个分类中单词的个数
    double testpossible[5];             //记录每个测试文件的属于每个分类的概率
    string classification[5]={"Crime","Education","Science","Sports","Weather"};
    string filename = "/Users/Iris/Desktop/bayes/data/";
    ifstream fin1;
    vector<string> stopwords;
    vector<string> ::iterator sw;
    char str[5]={',','.','!','?','"'};
    
    
public:
    bool isstopword(string w)
    {
        sw=find(stopwords.begin(),stopwords.end(),w);
        if(sw!=stopwords.end())
           return true;
        else
            return false;
    }
    bool ispunc(char w)
    {
        for(int i=0;i<5;i++)
        {
            if(w==str[i])
                return true;
        }
        return false;
    }
    Bayes()             //需要修改 针对输入文件格式 每个分类中新闻的数量记录在classpossible中
    {
        vector<string> vec;
        char buf[3];
        string buf_str;
        string word;
        fout.open("/Users/Iris/Desktop/bayes/result.txt");
        fin1.open("/Users/Iris/Desktop/bayes/stopword.txt");
        if(!fin1)
            cout<<"load stopwords error!"<<endl;
        while(fin1>>word)
        {
            stopwords.push_back(word);
        }
        for(int i=0;i<5;i++)
        {
            for(int j=0;j<264;j++)
            {
                sprintf(buf,"%d",j);
                if(j<=9)
                {
                    buf[2]=buf[0];
                    buf[0]='0';
                    buf[1]='0';
                }
                else if(j<=99)
                {
                    buf[2]=buf[1];
                    buf[1]=buf[0];
                    buf[0]='0';
                }
                vec.clear();
                buf_str = filename+classification[i]+"/"+classification[i]+buf+".txt";
                fin.open(buf_str.c_str());
                if(!fin)
                {
                    cout<<"file "<<buf_str<<" error!"<<endl;
                }
                while(fin>>word)
                {
                    if(!isstopword(word))
                    {
                        while(ispunc(word[0]))
                            word = word.substr(1,word.length()-1);
                        while(ispunc(word[word.length()-1]))
                            word = word.substr(0,word.length()-1);
                        vec.push_back(word);
                    }
                }
                list_of_docs.push_back(vec);
                list_classes.push_back(i);
                buf_str.clear();
                fin.close();
            }
        }
    }
    
    ~Bayes()
    {
        fin.close();
        fout.close();
        fin1.close();
        list_of_docs.clear();
        list_classes.clear();
        my_vocab_list.clear();
        train_mat.clear();
        vect.clear();
    }
    
    void create_vocab_list()                          //得到单词与数字的对应关系在my_vocab_list中
    {
        vector< vector<string> > :: iterator it = list_of_docs.begin();
        int index = 1;
        while( it!=list_of_docs.end() )
        {
            vector<string> vec = *it;
            vector<string> :: iterator tmp_it = vec.begin();
            map<string,int>::iterator iter;
            while( tmp_it!=vec.end() )
            {
                iter = my_vocab_list.find(*tmp_it);
                if( iter==my_vocab_list.end() )
                {
                    my_vocab_list.insert(pair<string,int>(*tmp_it,index++));
                }
                tmp_it++;
            }
            it++;
        }
        
    }
    
    void words_to_vec(int idx)                 //每个单词在新闻idx中出现的次数
    {
        int len = (int)my_vocab_list.size()+1;
        return_vec = new int[ len ];
        fill(return_vec,return_vec+len,0);
        vector< vector<string> >:: iterator it = list_of_docs.begin() + idx - 1  ;
        vector<string> vec  = *it;
        vector<string> :: iterator itt = vec.begin();
        int pos = 0 ;
        while( itt!=vec.end() )
        {
            pos = my_vocab_list[ *itt ];
            if(pos!=0)
            {
                return_vec[pos] += 1;
            }
            itt++;
        }
    }
    
    void get_train_matrix()                        //得到训练矩阵
    {
        train_mat.clear();
        for(int i=1;i<=list_of_docs.size();i++)
        {
            words_to_vec(i);
            vector<int> vec( return_vec , return_vec + my_vocab_list.size()+1 );
            train_mat.push_back(vec);
            delete []return_vec;
        }
    }
    
    void train()                                           //得到每个单词在每个分类中的概率，取完对数放在vect中
    {
        num_words = (int)train_mat[0].size() - 1 ;           //总单词数,单词是从1开始编号的
        //vect.resize(5);
        for(int i=0;i<5;i++)
            vect.push_back(vector<double>(num_words+1,1));
        for(int i=0;i<list_classes.size();i++)                  //遍历每个新闻
        {
            int k = list_classes[i];                            //k是该新闻的分类
            for(int j=1;j<=num_words;j++)
            {
                vect[k][j] += train_mat[i][j];
            }
        }
        for(int i=0;i<5;i++)
        {
            for(int j=1;j<=num_words;j++)
            {
                wordsinclass[i]+= vect[i][j];
            }
        }
        for(int i=0;i<5;i++){
            for(int j=1;j<=num_words;j++)
            {
                vect[i][j]=log(vect[i][j]/wordsinclass[i]);
            }
        }
    }
    
    int classify(string filename )
    {
        for(int i=0;i<5;i++)
            testpossible[i]=0;
        return_vec = new int[ my_vocab_list.size()+1 ];            //记录测试文件中每个单词出现的次数
        fin.open(filename);
        if(!fin)
        {
            cout<<"fail to open the file "<<filename<<endl;
        }
        string word;
        while(fin>>word)
        {
            if(isstopword(word))
                continue;
            else
            {
                while(ispunc(word[0]))
                    word = word.substr(1,word.length()-1);
                while(ispunc(word[word.length()-1]))
                    word = word.substr(0,word.length()-1);
            }
            int pos = my_vocab_list[word];
            if( pos!=0 )
            {
                return_vec[pos] += 1;
            }
        }
        fin.close();
        for(int i=0;i<5;i++){
            for(int j=1;j<=num_words;j++){
                testpossible[i]+=vect[i][j]*return_vec[j];
            }
        }
        int max = 0;
        int i;
        for(i=0;i<5;i++){
            if(testpossible[i]>testpossible[max])
                max = i;
        }
        return max;
    }
    void test()
    {
        char buf[3];
        string buf_str;
        int result;
        int errornum=0;
        for(int i=0;i<5;i++)
        {
            for(int j=264;j<330;j++)
            {
                sprintf(buf,"%d",j);
                buf_str = filename+classification[i]+"/"+classification[i]+buf+".txt";
                result = classify(buf_str);
                fout<<result<<endl;
                if(result!=i)
                    errornum++;
            }
        }
        double p = (1 - errornum/330.0)*100;
        cout<<"precision "<<p<<"%"<<endl;
    }
};

int main()
{
    Bayes nb;
    nb.create_vocab_list();
    nb.get_train_matrix();
    nb.train();
    nb.test();
    return 0;
}