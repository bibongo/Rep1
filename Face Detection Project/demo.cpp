/**
@author Ravish Trivedi
Machine learning curricular project work on Face recognition using haar-like features, perceptron with boosting.
Dates of work: October 15, 2013 to December 12, 2013.
**/

/**
demo.cpp code description:
This code receives an image from the user and uses face detection on it using the
learner model supplied as an argument. It draws rectangles on the image where a face
has been detected by the model.
*/

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include<stdio.h>
#include<string.h>
#include<math.h>
#include<stdlib.h>
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
    if(argc!=3)
    {
        cout<<"\n Usage : <image path> <model file>";
        exit(0);
    }
    int i,j;
    int MAXATT = 972;
    String image = argv[1];
    Mat orignal,img,rimg,sum;
    orignal=imread(image,1);
    namedWindow("Image",CV_WINDOW_AUTOSIZE);
    resize(orignal, rimg, Size(), 1, 1, INTER_NEAREST);
    cvtColor( rimg, img, CV_BGR2GRAY );
    Scalar color = CV_RGB(0,0,255);
    float model[MAXATT+1];
    float data[MAXATT];
    fstream f;
    f.open(argv[2],ios::in);
    for(i=0;i<MAXATT+1;i++)
        f>>model[i];
    f.close();
    integral (img, sum, CV_32FC1);
    int ipos,jpos,mf,atc,ir,jr,fc;
    float lsum,rsum,llsum,lrsum,net;
    mf=2;
    fc=0;
    while (19*mf <= sum.rows && 19*mf <=sum.cols)
    {
        //cout<<"\n mf = "<<mf;
        ir=1;jr=1;
        while(1)
        {
            //cout<<"\n\t ir = "<<ir<<" , jr = "<<jr;
            ipos=ir;jpos=jr;atc=0;
            while(1)
            {
                if (mf==1){
                    lsum = (int)img.at<uchar>(ipos,jpos) - (int)img.at<uchar>(ipos,jpos+1);
                    data[atc++]=lsum/256;
                    rsum = (int)img.at<uchar>(ipos,jpos) - (int)img.at<uchar>(ipos+1,jpos);
                    data[atc++]=rsum/256;
                    lsum = (int)img.at<uchar>(ipos,jpos) + (int)img.at<uchar>(ipos+1,jpos+1) - (int)img.at<uchar>(ipos+1,jpos) - (int)img.at<uchar>(ipos,jpos+1);
                    data[atc++]=lsum/512;
                }
                else {
                    //cout<<"\n\t\t ipos = "<<ipos<<" , jpos = "<<jpos;
                    lsum = sum.at<float>(ipos,jpos) + sum.at<float>(ipos+mf-1,jpos+mf-1) - sum.at<float>(ipos,jpos+mf-1) - sum.at<float>(ipos+mf-1,jpos);
                    rsum = sum.at<float>(ipos,jpos+mf) + sum.at<float>(ipos+mf-1,jpos+2*mf-1) - sum.at<float>(ipos,jpos+2*mf-1) - sum.at<float>(ipos+mf-1,jpos+mf);
                    data[atc++]=(lsum-rsum)/(256*mf*mf);
                    llsum = sum.at<float>(ipos+mf,jpos) + sum.at<float>(ipos+2*mf-1,jpos+mf-1) - sum.at<float>(ipos+mf,jpos+mf-1) - sum.at<float>(ipos+2*mf-1,jpos);
                    data[atc++]=(lsum-llsum)/(256*mf*mf);
                    lrsum = sum.at<float>(ipos+mf,jpos+mf) + sum.at<float>(ipos+2*mf-1,jpos+2*mf-1) - sum.at<float>(ipos+2*mf-1,jpos+mf) - sum.at<float>(ipos+mf,jpos+2*mf-1);
                    data[atc++]=((lsum+lrsum)-(rsum+llsum))/(256*mf*mf*mf);
                }
                jpos += mf;
                if(jpos+mf >= jr+19*mf)
                {
                    jpos=jr;
                    ipos+=mf;
                    if(ipos+mf >= ir+19*mf)
                        break;
                }
            }

            net=0;
            for(i=0;i<MAXATT;i++)
                net+=model[i]*data[i];
            net+=model[MAXATT];
            if (net>0)
            {
                //cout<<net<<endl;
                fc++;
                if(fc<25){
                rectangle(orignal, Point(ir,jr), Point(ir+19*mf,jr+19*mf),color,1,8,0);}
            }
            jr += int(19*mf*0.2);
            if (jr + 19*mf >= sum.cols)
            {
                jr=1;
                ir+=int(0.2*19*mf);
                if(ir+19*mf >= sum.rows)
                    break;
            }
        }
        mf++;
    }
    cout<<"No. of faces detected : "<<fc<<endl;
    cout<<"\nPress 'q' on image window to exit\n";
    while(cvWaitKey(1)!='q'){
        imshow("Image",orignal);
    }
    destroyWindow("Image");
}
