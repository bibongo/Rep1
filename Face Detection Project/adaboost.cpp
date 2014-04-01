/**
@author Ravish Trivedi
Machine learning curricular project work on Face recognition using haar-like features, perceptron with boosting.
Dates of work: October 15, 2013 to December 12, 2013.
*/

/**
adaboost.cpp code description:

This code implements a learner using perceptron + boosting in a different way then the normal boosting.
During each boosting iteration, it learns 972 different perceptron (one 2-feature perceptron for each facial feature)
and chooses the best one that gives minimum error. This 'best' perceptron is combined with the rest and the final result
is a 972-feature perceptron with only values that are features of all the 'best' perceptrons of boosting iterations.
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
    if (argc!=6){
        cout<<"Argument list: <trainin face files directory> <trainin nonface files directory> <boosting iterations> <perceptron iteration limit> <model file to create>";
        exit(0);
    }

    int BOOSTITER = atoi(argv[3]);
    int IMGATTS;
    IMGATTS= 972;

    int i,j,k,q,ipos,jpos;
    Mat sum=Mat(19,19, CV_32FC1);
    Mat cimg=Mat(19,19, CV_8UC1);
    Mat rimg= Mat(19,19, CV_8UC1);
    Size s;
    int att_count;
    unsigned int rows,cols;
    long int lsum, rsum;

    int iter,limit;
    limit = atoi(argv[4]);
    float margin,o,t,net,eta,epsilon;

    time_t time1,time2;

    DIR *dir;
    struct dirent *ent;

    int train_faces=0;
    int train_nonfaces=0;

    if ((dir = opendir (argv[1])) != NULL){
        ent = readdir (dir);ent = readdir (dir);
        while ((ent = readdir (dir)) != NULL)
            train_faces++;
    }
    closedir (dir);

    if ((dir = opendir (argv[2])) != NULL){
        ent = readdir (dir);ent = readdir (dir);
        while ((ent = readdir (dir)) != NULL)
            train_nonfaces++;
    }
    closedir (dir);

    char **trainff , **trainnf;
    trainff = new char *[train_faces];
    for (i=0;i<train_faces;i++)
        trainff[i]=new char[200];
    trainnf = new char *[train_nonfaces];
    for (i=0;i<train_nonfaces;i++)
        trainnf[i]=new char[200];

    cout<<"\n No. of faces = "<<train_faces;
    cout<<"\n No. of non faces = "<<train_nonfaces;

    float **data, *img, *data_weights, *data_error, *best_data_error, *boost_final_alpha, boost_alpha, boost_epsilon;

    boost_final_alpha = new float[BOOSTITER];

    img = new float[IMGATTS];

    data = new float*[train_faces+train_nonfaces];
    for (i=0;i<train_faces+train_nonfaces;i++)
        data[i]=new float[IMGATTS];


    best_data_error = new float[train_faces+train_nonfaces];
    for(i=0;i<train_faces+train_nonfaces;i++)
        best_data_error[i]=0;
    i=0;
    if ((dir = opendir (argv[1])) != NULL){
        ent = readdir (dir);ent = readdir (dir);
        while ((ent = readdir (dir)) != NULL){
            strcpy(trainff[i],"");
            strcat(trainff[i],argv[1]);
            strcat(trainff[i],SLASH);
            strcat(trainff[i],ent->d_name);
            i++;
        }
    }
    closedir (dir);

    i=0;
    if ((dir = opendir (argv[2])) != NULL){
        ent = readdir (dir);ent = readdir (dir);
        while ((ent = readdir (dir)) != NULL){
            strcpy(trainnf[i],"");
            strcat(trainnf[i],argv[2]);
            strcat(trainnf[i],SLASH);
            strcat(trainnf[i],ent->d_name);
            i++;
        }
    }
    closedir (dir);

    for (i=0;i<train_faces;i++){
        cimg=imread(trainff[i],1);
        ipos=0;
        jpos=0;
        att_count=0;
        while(1){
            lsum = (int)cimg.at<uchar>(ipos,jpos);
            rsum = (int)cimg.at<uchar>(ipos,jpos+1);
            data[i][att_count]=float(lsum-rsum)/256;
            att_count++;
            rsum = (int)cimg.at<uchar>(ipos+1,jpos);
            data[i][att_count]=float(lsum-rsum)/256;
            att_count++;
            lsum = (int)cimg.at<uchar>(ipos,jpos) + (int)cimg.at<uchar>(ipos+1,jpos+1);
            rsum = (int)cimg.at<uchar>(ipos+1,jpos) + (int)cimg.at<uchar>(ipos,jpos+1);
            data[i][att_count]=float(lsum-rsum)/512;
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
    }


    for (i=0;i<train_nonfaces;i++){
        cimg=imread(trainnf[i],1);
        ipos=0;
        jpos=0;
        att_count=0;
        while(1){
            lsum = (int)cimg.at<uchar>(ipos,jpos);
            rsum = (int)cimg.at<uchar>(ipos,jpos+1);
            data[train_faces+i][att_count]=float(lsum-rsum)/256;
            att_count++;
            rsum = (int)cimg.at<uchar>(ipos+1,jpos);
            data[train_faces+i][att_count]=float(lsum-rsum)/256;
            att_count++;
            lsum = (int)cimg.at<uchar>(ipos,jpos) + (int)cimg.at<uchar>(ipos+1,jpos+1);
            rsum = (int)cimg.at<uchar>(ipos+1,jpos) + (int)cimg.at<uchar>(ipos,jpos+1);
            data[train_faces+i][att_count]=float(lsum-rsum)/512;
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
    }

    int total = train_faces + train_nonfaces;
    float **perceptron, dw[2];
    perceptron = new float*[IMGATTS];
    for (i=0;i<IMGATTS;i++)
        perceptron[i] = new float[2];

    data_weights = new float[total];
    for(i=0;i<total;i++)
        data_weights[i]=(float)1/(total);

    data_error = new float[total];
    for(i=0;i<total;i++)
        data_error[i]=0;

    int best_perceptron;
    float best_perceptron_error,error, model[IMGATTS+1];
    for(i=0;i<IMGATTS+1;i++)
        model[i]=0;
    float **boost_perceptrons;
    boost_perceptrons=new float*[BOOSTITER];
    float *best_features;
    best_features=new float[BOOSTITER];
    for (i=0;i<BOOSTITER;i++)
        boost_perceptrons[i]=new float[2];
    /** AdaBoost with Perceptron */
    /** Find best classifier with current weights that minimizes weighted error */
    for(int u=0;u<BOOSTITER;u++)
    {
        cout<<"\n Boosting iteration : "<<u;
        for (i=0;i<IMGATTS;i++)
        {
            perceptron[i][0]=float(rand()%100 + 1)/100;
            perceptron[i][1]=float(rand()%100 + 1)/100;
            margin=100;
            epsilon=0.0;
            eta=0.01;
            iter=0;
            while (iter<limit)
            {
                margin=0;
                dw[0]=0;dw[1]=0;
                for (j=0;j<total;j++)
                {
                    net = perceptron[i][1] + perceptron[i][0]*data[j][i];
                    if (net>0)  o=1;
                    else o=-1;
                    if (j<train_faces)  t=1;
                    else    t=-1;
                    dw[0]=eta*(t-o);
                    dw[1]=dw[0];
                    perceptron[i][0]+=dw[0];
                    perceptron[i][1]+=dw[1];
                }
                iter++;
            }
        }

        best_perceptron=0;
        best_perceptron_error=total;
        for(i=0;i<total;i++)    best_data_error[i]=0;

        for (i=0;i<IMGATTS;i++)
        {
            error=0;
            for (j=0;j<total;j++)
            {
                data_error[j]=0;
                net= perceptron[i][0]*data[j][i] + perceptron[i][1];
                if (net>0)  o=1;
                else    o=-1;
                if (j<train_faces)  t=1;
                else    t=-1;
                if (o!=t)
                {
                    data_error[j]=1;
                    error+=data_weights[j];
                }
            }
            if (error < best_perceptron_error)
            {
                for(j=0;j<total;j++)    best_data_error[j]=data_error[j];
                best_perceptron_error=error;
                best_perceptron=i;
            }
        }

        boost_epsilon=0;
        for(i=0;i<total;i++)
            boost_epsilon+=data_weights[i];
        boost_epsilon=1/boost_epsilon;
        boost_epsilon*=best_perceptron_error;
        boost_alpha=log((1-boost_epsilon)/boost_epsilon);

        cout<<"\n\tAlpha = "<<boost_alpha;
        cout<<"\n\tEpsilon = "<<boost_epsilon;

        model[best_perceptron] += boost_alpha * perceptron[best_perceptron][0];
        model[IMGATTS] += boost_alpha * perceptron[best_perceptron][1];
//        boost_final_alpha[u]=boost_alpha;
//        best_features[u]=best_perceptron;
//        boost_perceptrons[u][0]=perceptron[best_perceptron][0];
//        boost_perceptrons[u][1]=perceptron[best_perceptron][1];

        for(i=0;i<total;i++)
            data_weights[i]=data_weights[i]*exp(boost_alpha *best_data_error[i]);
    }


    fstream f;
    f.open(argv[5],ios::out);
    for (i=0;i<IMGATTS+1;i++)
        f<<model[i]<<" ";
    f.close();


    for (i=0;i<train_faces+train_nonfaces;i++)
        delete(data[i]);
    for(i=0;i<BOOSTITER;i++)
        delete(boost_perceptrons[i]);
    delete(boost_final_alpha);
    delete (boost_perceptrons);
    delete (data_weights);
    delete (data_error);
    delete (best_data_error);
    delete (img);
    delete(data);
    for (i=0;i<train_faces;i++)
        delete(trainff[i]);
    delete(trainff);
    for (i=0;i<train_nonfaces;i++)
        delete(trainnf[i]);
    delete(trainnf);
    return 0;
}
