## Ravish Trivedi

from os import listdir
import sys
import math
from random import randint
import time

## getting directory list for training and testing data
## locally get and store all files required
## Arguments in sequence: <train data path> <test data path> <learning rate> <weight penalty constant>
## <tolerance> <max. no. of iterations allowed> 
##if(len(sys.argv)!=8):
##    print('Invalid number of arguments!')
##    exit(1)
##else:
##    train_dir=sys.argv[1]
##    test_dir=sys.argv[2]
##    learning_rate=float(sys.argv[3])
##    lr_lambda=float(sys.argv[4])
##    epsilon=float(sys.argv[5])
##    iteration_limit=int(sys.argv[6])
##    use_sw=int(sys.argv[7])

train_dir='train'
test_dir='test'
learning_rate=0.01
lr_lambda=0.02
epsilon=0.01
iteration_limit=1000
use_sw=1


## List of stop words
stop_words=['a','about','above','after','again','against','all','am','an','and','any','are','aren\'t','as','at','be','because','been','before','being','below','between','both','but','by','can\'t','cannot','could','couldn\'t','did','didn\'t','do','does','doesn\'t','doing','don\'t','down','during','each','few','for','from','further','had','hadn\'t','has','hasn\'t','have','haven\'t','having','he','he\'d','he\'ll','he\'s','her','here','here\'s','hers','herself','him','himself','his','how','how\'s','i','i\'d','i\'ll','i\'m','i\'ve','if','in','into','is','isn\'t','it','it\'s','its','itself','let\'s','me','more','most','mustn\'t','my','myself','no','nor','not','of','off','on','once','only','or','other','ought','our','ours','ourselves','out','over','own','same','shan\'t','she','she\'d','she\'ll','she\'s','should','shouldn\'t','so','some','such','than','that','that\'s','the','their','theirs','them','themselves','then','there','there\'s','these','they','they\'d','they\'ll','they\'re','they\'ve','this','those','through','to','too','under','until','up','very','was','wasn\'t','we','we\'d','we\'ll','we\'re','we\'ve','were','weren\'t','what','what\'s','when','when\'s','where','where\'s','which','while','who','who\'s','whom','why','why\'s','with','won\'t','would','wouldn\'t','you','you\'d','you\'ll','you\'re','you\'ve','your','yours','yourself','yourselves']

## cd : iteration limit count down in convergence loop
cd=iteration_limit

## max_change : max. weight change of W[i] in a single iteration.
##              Used for epsilon checking
max_change=0

## x : dictionary of all words in training dataset.
##     Corresponding to a word is word index i.e. attribute number
x={}

## dsum : temporary variable used to compute sum of all
##        word frequencies in a given example
dsum=0

## ic: dictionary x{} generator. Final value gives total
##     number of distinct words in training set
ic=1

##dc=1

## spam_count : Total number of spam training emails
spam_count=0

## ham_count : Total number of ham training emails
ham_count=0

## Lists containing all training file names
## (complete paths) that are spam and ham respectively
train_spam_files=[ train_dir+'/spam/'+f for f in listdir(train_dir+'/spam')]
train_ham_files=[train_dir+'/ham/'+f for f in listdir(train_dir+'/ham')]

## Lists containing all testing file names
## that are spam and ham respectively
test_spam_files=[ test_dir+'/spam/'+f for f in listdir(test_dir+'/spam')]
test_ham_files=[ test_dir+'/ham/'+f for f in listdir(test_dir+'/ham')]

## m : total number of training files (spams + hams)
m=len(train_spam_files)+len(train_ham_files)

## data: List of dictionary for creating sparse 2D matrix like structure
data=[{} for row in range(0,m)]
t1sec=time.clock()

## Read all training files and initialize x and data
for f in train_spam_files:
    r=open(f, encoding='latin1')
    spam_count+=1
    text=r.read().replace('\n', '').rsplit(' ')
    text.remove('Subject:')
    for word in text:
        if use_sw==1:
            if word in stop_words:
                continue
        if word not in x.keys():
            x[word]=ic
            data[spam_count-1][ic]=1
            ic+=1
        else:
            if word not in data[spam_count-1].keys():
                data[spam_count-1][x[word]]=1
            else:
                data[spam_count-1][x[word]]+=1
    r.close()

for f in train_ham_files:
    r=open(f, encoding='latin1')
    ham_count+=1
    data[spam_count+ham_count-1][0]=1
    text=r.read().replace('\n', '').rsplit(' ')
    text.remove('Subject:')                         
    for word in text:
        if use_sw==1:
            if word in stop_words:
                continue
        if word not in x.keys():
            x[word]=ic
            data[spam_count+ham_count-1][ic]=1
            ic+=1
        else:
            if word not in data[spam_count+ham_count-1].keys():
                data[spam_count+ham_count-1][x[word]]=1
            else:
                data[spam_count+ham_count-1][x[word]]+=1
    r.close()

n=len(x.keys())
t2sec=time.clock()
print('Time to read all files:','%.2f'%(t2sec-t1sec),'s')
t1sec=time.clock()
print('Gradient ascent initiated! Max. time = ',2*iteration_limit,'s')

## Preprocessing (normalizing data and randomly initializing weights)
data_dsum=[0 for row in range(0,m)]
for i in range(0,m):
    dsum=0
    for k in data[i].keys():
        dsum+=data[i][k]
    for k in data[i].keys():
        data[i][k]/=dsum
    data_dsum[i]=dsum
    
pr=[0 for row in range (m)]
w=[0 for row in range (n+1)]
for i in range(0,len(w)):
    w[i]=randint(1,1000)/1000
dw=[0 for row in range (n+1)]

## GRADIENT ASCENT
while(cd):
    for i in range(0,m):
        sig_sum=0
        for k in data[i].keys():
            sig_sum+=data[i][k]*w[k]
        sig_sum+=w[0]
        pr[i]=(math.pow(math.e, sig_sum)/(1+math.pow(math.e, sig_sum)))
    for i in range(0,n+1):
        dw[i]=0
    for i in range(1,n+1):
        for j in range(0,m):
            if i in data[j].keys():
                if m>spam_count-1:
                    dw[i]=dw[i]+data[j][i]*(0-pr[j])
                else:
                    dw[i]=dw[i]+data[j][i]*(1-pr[j])
    for j in range(0,m):
        if j>spam_count-1:
            dw[0]=dw[0]+(1/data_dsum[j])*(0-pr[j])
        else:
            dw[0]=dw[0]+(1/data_dsum[j])*(1-pr[j])

    max_change=0
    for i in range(0,n+1):
        temp=w[i]
        w[i]=w[i]+learning_rate*(dw[i]-lr_lambda*w[i])
        temp=w[i]-temp
        if temp<0:
            temp*=-1
        if temp>max_change:
            max_change=temp
    if max_change<epsilon:
        print ('Converged at iteration no. ',iteration_limit-cd)
        break
    cd-=1
    if(cd==0):
        print ('Iteration hard limit reached!')
t2sec=time.clock()
print('Time taken to converge : ','%.2f'%(t2sec-t1sec),'s')
t1sec=time.clock()

## Testing 
def test(fl):
    hc=0
    sc=0
    for f in fl:
        r=open(f, encoding='latin1')
        text=r.read().replace('\n', '').rsplit(' ')
        text.remove('Subject:')
        tdata={}
        for word in text:
            if use_sw==1:
                if word in stop_words:
                    continue
            if word not in x.keys():
                continue
            else:
                if word not in tdata.keys():
                    tdata[word]=1
                else:
                    tdata[word]+=1
        dsum=0
        for k in tdata.keys():
            dsum+=tdata[k]
        for k in tdata.keys():
            tdata[k]/=dsum
        sig_sum=0
        for k in tdata.keys():
            sig_sum+=w[x[k]]*tdata[k]
        sig_sum+=w[0]
        e=1/(math.pow(math.e,sig_sum)+1)
        if e>0.5:
            hc+=1
        else:
            sc+=1
        r.close()
    return [sc,hc]

t1sec=time.clock()
res1=test(test_ham_files)
res2=test(test_spam_files)

print ('Accuracy = ','%.3f'%((res1[1]+res2[0])*100/(res1[0]+res1[1]+res2[0]+res2[1])),'%')
t2sec=time.clock()
print('Time to test : ','%.2f'%(t2sec-t1sec),'s')
