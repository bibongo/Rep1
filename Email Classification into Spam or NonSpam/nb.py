## Ravish Trivedi

from os import listdir
import sys
import math

## getting directory list for training and testing data
## locally store all files name with path required
if(len(sys.argv)!=4):
    print('Invalid number of arguments!')
    exit(1)
else:
    train_dir=sys.argv[1]
    test_dir=sys.argv[2]
    use_sw=int(sys.argv[3])
train_spam_files=[ train_dir+'/spam/'+f for f in listdir(train_dir+'/spam')]
train_ham_files=[train_dir+'/ham/'+f for f in listdir(train_dir+'/ham')]

test_spam_files=[ test_dir+'/spam/'+f for f in listdir(test_dir+'/spam')]
test_ham_files=[ test_dir+'/ham/'+f for f in listdir(test_dir+'/ham')]

## Naive Bayes algorithm training
## List of stop words
stop_words=['a','about','above','after','again','against','all','am','an','and','any','are','aren\'t','as','at','be','because','been','before','being','below','between','both','but','by','can\'t','cannot','could','couldn\'t','did','didn\'t','do','does','doesn\'t','doing','don\'t','down','during','each','few','for','from','further','had','hadn\'t','has','hasn\'t','have','haven\'t','having','he','he\'d','he\'ll','he\'s','her','here','here\'s','hers','herself','him','himself','his','how','how\'s','i','i\'d','i\'ll','i\'m','i\'ve','if','in','into','is','isn\'t','it','it\'s','its','itself','let\'s','me','more','most','mustn\'t','my','myself','no','nor','not','of','off','on','once','only','or','other','ought','our','ours','ourselves','out','over','own','same','shan\'t','she','she\'d','she\'ll','she\'s','should','shouldn\'t','so','some','such','than','that','that\'s','the','their','theirs','them','themselves','then','there','there\'s','these','they','they\'d','they\'ll','they\'re','they\'ve','this','those','through','to','too','under','until','up','very','was','wasn\'t','we','we\'d','we\'ll','we\'re','we\'ve','were','weren\'t','what','what\'s','when','when\'s','where','where\'s','which','while','who','who\'s','whom','why','why\'s','with','won\'t','would','wouldn\'t','you','you\'d','you\'ll','you\'re','you\'ve','your','yours','yourself','yourselves']    
## Calculating P(Xi/Y=spam)
w_spam={}
spam_count=0
for f in train_spam_files:
    r=open(f, encoding='latin1')
    spam_count+=1
    text=r.read().replace('\n', '').rsplit(' ')
    text.remove('Subject:')
    for word in text:
        if use_sw==1:
            if word in stop_words:
                continue
        if word not in w_spam.keys():
            w_spam[word]=0
        w_spam[word]=w_spam[word]+1
    r.close()

w_spam_count=0
for word in w_spam.keys():
    w_spam_count+=w_spam[word]+1

Pws={}
for word in w_spam.keys():
    Pws[word]=((w_spam[word]+1)/w_spam_count)

## Calculating P(Xi/Y=ham)
w_ham={}
ham_count=0
for f in train_ham_files:
    r=open(f, encoding='latin1')
    ham_count+=1
    text=r.read().replace('\n', '').rsplit(' ')
    text.remove('Subject:')                         ## Not a valid feature (word) for classification as it is present in all the emails
    for word in text:
        if use_sw==1:
            if word in stop_words:
                continue
        if word not in w_ham.keys():
            w_ham[word]=0
        w_ham[word]=w_ham[word]+1                   ##Step of Laplace Smoothing n=1
    r.close()

w_ham_count=0
for word in w_ham.keys():
    w_ham_count+=w_ham[word]+1                      ##Step of Laplace Smoothing n=1

Pwh={}
for word in w_ham.keys():
    Pwh[word]=((w_ham[word]+1)/w_ham_count)

## Calculating P(Yi)
Pspam=spam_count/(spam_count+ham_count)
Pham=1-Pspam

##---total joint distribution calculated ---##
## Testing data now
def test_nb(x,i,j):
    global Pws
    global Pwh
    global w_spam_count
    global w_ham_count
    global Pspam
    global Pham
    sc=0
    hc=0
    for f in x:
        lps=0
        lph=0
        r=open(f, encoding='latin1')
        text=r.read().replace('\n', '').rsplit(' ')
        text.remove('Subject:')
        for word in text:
            if j==1:
                if word in stop_words:
                    continue
            if word not in Pws.keys():              ##Step of Laplace Smoothing n=1
                lps+=math.log(1/w_spam_count)
            else:
                lps=lps+math.log(Pws[word])
            if word not in Pwh.keys():              ##Step of Laplace Smoothing n=1
                lph+=math.log(1/w_ham_count)
            else:
                lph=lph+math.log(Pwh[word])
        lps=lps+math.log(Pspam)
        lph=lph+math.log(Pham)
        if(lps<lph):
            hc+=1
        elif(lps>lph):
            sc+=1
        r.close()
    if(i==1):
        return hc
    else:
        return sc

print('Test result:')
error1=test_nb(test_spam_files,1,use_sw)
print('Error on spam set = ', error1,'/',len(test_spam_files))
error2=test_nb(test_ham_files,0,use_sw)
print('Error on ham set = ', error2,'/',len(test_ham_files))
error=error1+error2
total=len(test_ham_files)+len(test_spam_files)
accuracy=((total-error)/total)*100
print('Overall accuracy = ',accuracy)
