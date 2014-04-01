/**
@author Ravish Trivedi
Machine learning curricular project work on Face recognition using haar-like features, perceptron with boosting.
Dates of work: October 15, 2013 to December 12, 2013.
**/

/**
boosting.cpp code description:
This code implements a learner using perceptron + boosting.
During each boosting iteration, an effective perceptron with 972 features is learned
and is combined later with the final perceptron.
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
    if (argc!=7){
        cout<<"Argument list: <trainin face files directory> <trainin nonface files directory> <boosting iterations> <classifiers/iteration> <perceptron iteration limit> <model file to create>";
        exit(0);
    }
    int BOOSTITER = atoi(argv[3]);
    int BCFLIMIT = atoi(argv[4]);
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
    limit = atoi(argv[5]);
    float margin,o,t,net,eta,epsilon;
    float *w , *best_w, *dw, **boost_weight;
    w = new float[IMGATTS+1];
    best_w = new float[IMGATTS+1];
    dw = new float[IMGATTS+1];
    boost_weight = new float *[BOOSTITER];
    for (i=0;i<BOOSTITER;i++)
        boost_weight[i]= new float[IMGATTS+1];

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

    float **data, *img, *data_weights, *data_error, *best_data_error, weighted_error, min_weighted_error;
    float *boost_final_alpha;
    boost_final_alpha = new float[BOOSTITER];
    float boost_epsilon, boost_alpha;
    int bcf_count;
    img = new float[IMGATTS];
    data = new float*[train_faces+train_nonfaces];
    for (i=0;i<train_faces+train_nonfaces;i++)
        data[i]=new float[IMGATTS];
    data_weights = new float[train_faces+train_nonfaces];
    for(i=0;i<train_faces+train_nonfaces;i++)
        data_weights[i]=(float)1/(train_faces+train_nonfaces);
    data_error = new float[train_faces+train_nonfaces];
    for(i=0;i<train_faces+train_nonfaces;i++)
        data_error[i]=0;
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

    /** Boosting with Perceptron */
    /** Find best classifier with current weights that minimizes weighted error */
    for(int u=0;u<BOOSTITER;u++)
    {
        cout<<"\n Boosting iteration no. "<<u+1;
        min_weighted_error = 100000;
        bcf_count=0;
        //cout<<"\n Finding best classifier that minimizes weighted error...";
        while (bcf_count < BCFLIMIT)
        {
            //cout<<"\n\t Attempt no.: "<<bcf_count+1;
            /** Perceptron Algorithm to learn model over data */
            for (i=0;i<=IMGATTS;i++)
                w[i] = float(rand()%100 + 1)/100;

            epsilon = 0.0;
            eta = 0.01;
            margin = 100;
            iter=0;
            while(margin > epsilon && iter < limit){
                margin=0;
                for(i=0;i<IMGATTS;i++)
                    dw[i]=0;
                for(i=0;i<train_faces+train_nonfaces;i++)
                {
                    net=0;
                    for(j=1;j<IMGATTS;j++)
                        net+=w[j]*data[i][j];
                    net+=w[j];
                    if(net==0) o=0.5;
                    else if(net>0) o=1;
                    else      o=-1;
                    if(i<train_faces)       t=1;
                    else                    t=-1;

                    for(j=1;j<=IMGATTS;j++)
                        dw[j] += eta * (t-o);
                }
                for(i=0;i<=IMGATTS;i++)
                {
                    w[i] += dw[i];
                    if(dw[i]<0)
                        dw[i]*=-1;
                    if(dw[i]>margin)
                        margin=dw[i];
                }
                iter++;
            }

            /** Perceptron testing on training data to find weighted error */
            weighted_error=0;

            for (i=0;i<train_faces+train_nonfaces;i++)
            {
                data_error[i]=0;
                if (i<train_faces)
                    cimg=imread(trainff[i],1);
                else
                    cimg=imread(trainnf[i-train_faces],1);
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
                net=0;
                for (j=0;j<IMGATTS;j++)
                    net+=w[j]*img[j];
                net+=w[j];
                if (net>0) o=1;
                else o=-1;
                if (i>=train_faces){
                    if (o==1)  {
                        weighted_error+=data_weights[i];
                        data_error[i]=1;
                    }
                }
                else {
                    if (o==-1) {
                        weighted_error+=data_weights[i];
                        data_error[i]=1;
                    }
                }
            }
            //cout<<"  Error = "<<weighted_error;
            if (weighted_error < min_weighted_error)
            {
                for (q=0;q<=IMGATTS;q++)
                    best_w[q]=w[q];
                for (q=0;q<train_faces+train_nonfaces;q++)
                    best_data_error[q]=data_error[q];
                min_weighted_error = weighted_error;
            }
            bcf_count++;
        }
        boost_epsilon = 0;
        for (i=0;i<train_faces+train_nonfaces;i++)
            boost_epsilon+=data_weights[i];
        boost_epsilon=1/boost_epsilon;
        boost_epsilon*=min_weighted_error;
        boost_alpha = log((1-boost_epsilon)/boost_epsilon);
        boost_final_alpha[u] = boost_alpha;
        for (i=0;i<=IMGATTS;i++)
            boost_weight[u][i]=best_w[i];
        for (i=0;i<train_faces+train_nonfaces;i++)
            data_weights[i]=data_weights[i]*exp(boost_alpha*best_data_error[i]);
        //cout<<"\n Best classifier has Boosting parameters :";
        cout<<"\n\t Epsilon = "<<boost_epsilon;
        cout<<"\n\t Alpha   = "<<boost_alpha<<endl;
    }

    float *model = new float[IMGATTS+1];
    for (i=0;i<BOOSTITER;i++)
    {
        for (j=0;j<=IMGATTS;j++)
            model[j] += boost_weight[i][j]*boost_final_alpha[i];
    }
    fstream f;
    f.open(argv[6],ios::out);
    for(i=0;i<=IMGATTS;i++)
        f<<model[i]<<" ";

    f.close();


    delete(w);
    delete(dw);
    for (i=0;i<train_faces+train_nonfaces;i++)
        delete(data[i]);
    delete (model);
    delete (data_weights);
    delete (data_error);
    delete (best_data_error);
    delete (img);
    delete (best_w);
    delete(data);
    for (i=0;i<train_faces;i++)
        delete(trainff[i]);
    delete(trainff);
    for (i=0;i<train_nonfaces;i++)
        delete(trainnf[i]);
    delete(trainnf);
    return 0;
}
