/** 2902
@author Ravish Trivedi
Machine learning curricular project work on Face recognition using haar-like features, perceptron with boosting.
Dates of work: October 15, 2013 to December 12, 2013.
**/

/**
testing.cpp code description:
This code evaluates the model learned by adaboost.cpp or boosting.cpp.
It uses a test dataset with images of faces and nonfaces in it and gives
stats about true positives/negatives and false positives/negatives.
*/
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include<stdio.h>
#include<string.h>
#include<math.h>
#include<fstream>
#include<iostream>
#include <sys/types.h>
#include<unistd.h>
#include <dirent.h>
#include<time.h>
#define SLASH "/"

using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
    if (argc!=4){
        cout<<"Argument list: <testing face files directory> <testing nonface files directory> <model file>";
        exit(0);
    }

    int IMGATTS;
    IMGATTS= 18*18*3;

    float *img = new float[IMGATTS];

    int i,j,k,q,ipos,jpos;
    Mat sum=Mat(19,19, CV_32FC1);
    Mat cimg=Mat(19,19, CV_8UC1);
    Mat rimg= Mat(19,19, CV_8UC1);
    Size s;
    int att_count;
    unsigned int rows,cols;
    long int lsum, rsum;

    time_t time1,time2;

    DIR *dir;
    struct dirent *ent;

    int test_faces=0;
    int test_nonfaces=0;

    if ((dir = opendir (argv[1])) != NULL){
        ent = readdir (dir);ent = readdir (dir);
        while ((ent = readdir (dir)) != NULL)
            test_faces++;
    }
    closedir (dir);

    if ((dir = opendir (argv[2])) != NULL){
        ent = readdir (dir);ent = readdir (dir);
        while ((ent = readdir (dir)) != NULL)
            test_nonfaces++;
    }
    closedir (dir);

    char **testff , **testnf;
    testff = new char *[test_faces];
    for (i=0;i<test_faces;i++)
        testff[i]=new char[50];
    testnf = new char *[test_nonfaces];
    for (i=0;i<test_nonfaces;i++)
        testnf[i]=new char[50];

    if (test_nonfaces>10000)    test_nonfaces=9999;

    int total = test_faces + test_nonfaces;
    float net;
    int false_positive=0, false_negative=0, true_positive=0, true_negative=0;

    cout<<"\n No. of faces = "<<test_faces;
    cout<<"\n No. of non faces = "<<test_nonfaces;


    i=0;
    if ((dir = opendir (argv[1])) != NULL){
        ent = readdir (dir);ent = readdir (dir);
        while ((ent = readdir (dir)) != NULL){
            strcpy(testff[i],"");
            strcat(testff[i],argv[1]);
            strcat(testff[i],SLASH);
            strcat(testff[i],ent->d_name);
            i++;
        }
    }
    closedir (dir);

    i=0;
    if ((dir = opendir (argv[2])) != NULL){
        ent = readdir (dir);ent = readdir (dir);
        while ((ent = readdir (dir)) != NULL){
            strcpy(testnf[i],"");
            strcat(testnf[i],argv[2]);
            strcat(testnf[i],SLASH);
            strcat(testnf[i],ent->d_name);
            i++;
        }
    }
    closedir (dir);

    float *model = new float[IMGATTS+1];
    fstream f;
    f.open(argv[3], ios::in);
    for (i=0;i<IMGATTS+1;i++)
        f>>model[i];
    f.close();

    for (i=0;i<total;i++){
        if (i < test_faces)
        cimg=imread(testff[i],1);
        else
        cimg=imread(testnf[test_faces+i],1);
        ipos=0;
        jpos=0;
        att_count=0;
        while(1){
            lsum = (int)cimg.at<uchar>(ipos,jpos);
            rsum = (int)cimg.at<uchar>(ipos,jpos+1);
            img[att_count]=float(lsum-rsum)/256;
            att_count++;
            rsum = (int)cimg.at<uchar>(ipos+1,jpos);
            img[att_count]=float(lsum-rsum)/256;
            att_count++;
            lsum = (int)cimg.at<uchar>(ipos,jpos) + (int)cimg.at<uchar>(ipos+1,jpos+1);
            rsum = (int)cimg.at<uchar>(ipos+1,jpos) + (int)cimg.at<uchar>(ipos,jpos+1);
            img[att_count]=float(lsum-rsum)/512;
            att_count++;
            jpos = jpos + 1;
            if(jpos + 1 >= cimg.cols)
            {
                jpos = 0;
                ipos = ipos + 1;
                if(ipos + 1 >= cimg.rows)
                    break;
            }
        }

        net = 0;
        for (j=0;j<IMGATTS;j++)
            net+=img[j]*model[j];
        net+=model[j];
        if(net > 0){
            if (i >= test_faces)
                false_positive++;
            else    true_positive++;
        }
        else {
            if (i >= test_faces)
                true_negative++;
            else    false_negative++;
        }
    }

    cout << "\n\nStats are :\n";
    cout<<"False positives : "<<false_positive<<"\t True Positives : "<<true_positive;
    cout<<"\nFalse negatives : "<<false_negative<<"\t True negatives : "<<true_negative<<"\n";

    delete (img);
    for (i=0;i<test_faces;i++)
        delete(testff[i]);
    delete(testff);
    for (i=0;i<test_nonfaces;i++)
        delete(testnf[i]);
    delete(testnf);
    return 0;
}
