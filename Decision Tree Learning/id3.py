## Ravish Trivedi

import math
import sys
from random import randint
class node(object):
    def __init__(self, value = None, left = None, right = None):
        self.value = value
        self.left = left
        self.right = right

def traverse_tree(current):
    global non
    global spaces
    non+=1
    if(current.left==None or current.right==None):
        print(current.value)
    else:
        print()
    if(current.left!=None):
        print ('|'*spaces+current.value,end="")
        spaces+=1
        print(' = 0 :',end="")
        traverse_tree(current.left)
        spaces-=1
    if(current.right!=None):
        print ('|'*spaces+current.value,end="")
        spaces+=1
        print(' = 1 :',end="")
        traverse_tree(current.right)
        spaces-=1

def get_example_count(tdataset):
    n=0
    p=0
    for i in tdataset:
        if(data[i][-1]=='0'):
            n+=1
        else:
            p+=1
    return [n,p]


def calc_ig(tdataset,a):
    c0n=0
    c1n=0
    c0p=0
    c1p=0
    for i in tdataset:
        if(data[i][a]=='0'):
            if(data[i][-1]=='0'):
                c0n+=1
            else:
                c0p+=1
        else:
            if(data[i][-1]=='0'):
                c1n+=1
            else:
                c1p+=1
    if( c0p+c1n == len(tdataset)):
        gain=100
        return gain
    elif( c0n+c1p == len(tdataset)):
        gain=100
        return gain
    nratio=(c0n+c1n)/(c0n+c1n+c0p+c1p)
    pratio=(c0p+c1p)/(c0p+c1p+c0n+c1n)
    if(nratio==0):
        entropy=-(pratio*(math.log(pratio,2)))
    elif(pratio==0):
        entropy=-(nratio*(math.log(nratio,2)))        
    else:
        entropy=-(nratio*(math.log(nratio,2)))-(pratio*(math.log(pratio,2)))
    
    c0ratio=(c0n+c0p)/(c0n+c0p+c1n+c1p)
    c1ratio=(c1n+c1p)/(c0n+c0p+c1n+c1p)
    if(c0ratio==0):
        gain=entropy-(c1ratio*math.log(c1ratio,2))
    elif(c1ratio==0):
        gain=entropy-(c0ratio*math.log(c0ratio,2))
    else:
        gain=entropy-(c0ratio*math.log(c0ratio,2))-(c1ratio*math.log(c1ratio,2))
    return gain
    
def get_best_attribute(tdataset,free_att):
    A=-1
    max_gain=0
    for i in free_att:
        gain=calc_ig(tdataset,i)
        if(gain>max_gain):
            max_gain=gain
            A=i
    return A
    
def construct_tree(tdataset, t_att,free_att):
    root=node('~')
    np=[]
    np=get_example_count(tdataset)
    if(np[0]==len(tdataset)):
        root.value='0'
        return root
    elif(np[1]==len(tdataset)):
        root.value='1'
        return root
    if not free_att:
        if (np[0]>np[1]):
            root.value='0'
            return root
        else:
            root.value='1'
            return root
    A=get_best_attribute(tdataset,free_att)
    root.value=att[A]
    ltds=[]
    rtds=[]
    free_att.remove(A)
    for i in tdataset:
        if(data[i][A]=='0'):
            ltds.append(i)
        else:
            rtds.append(i)
    if not ltds:
        if(np[0]>np[1]):
            root.left=node('0')
        else:
            root.left=node('1')
    else:
        root.left=construct_tree(ltds,A,free_att)
    if not rtds:
        if(np[0]>np[1]):
            root.right=node('0')
        else:
            root.right=node('1')
    else:
        root.right=construct_tree(rtds,A,free_att)
    free_att.append(A)
    return root


def validate_data(x,root,i):
    global error
    global str
    if(root.value=='1' or root.value=='0'):
        if(x[-1]!=root.value):
            error+=1
        return
    if(x[att.index(root.value)]=='0'):
        validate_data(x,root.left,i)
    else:
        validate_data(x,root.right,i)
    return


def copy_tree(root, prime):
    prime=node(root.value)
    if(root.left!=None):
        prime.left=copy_tree(root.left,prime.left)
    if(root.right!=None):
        prime.right=copy_tree(root.right,prime.right)
    return prime

def nonleaf_node_count(prime,nlCount):
    if(prime.left!=None or prime.right!=None):
        nlCount+=1
    if(prime.left!=None):
        nlCount=nonleaf_node_count(prime.left,nlCount)
    if(prime.left!=None):
        nlCount=nonleaf_node_count(prime.right,nlCount)
    return nlCount

def trim_subtree(temp, tdataset):
    global trim_done
    global nodecnt
    if(temp.left==None and temp.right==None):
        return
    nodecnt-=1
    if (nodecnt==0):
        a=[]
        a=get_example_count(tdataset)
        if(a[0]>a[1]):
            temp.value='0'
        else:
            temp.value='1'
        temp.left=None
        temp.right=None
        trim_done=1
    else:
        ltds=[]
        rtds=[]
        catt=att.index(temp.value)
        for i in tdataset:
            if(data[i][catt]=='0'):
                ltds.append(i)
            else:
                rtds.append(i)
        if(trim_done==1):
            return
        elif(temp.left!=None):
            trim_subtree(temp.left, ltds)
        if(trim_done==1):
            return
        elif(temp.right!=None):
            trim_subtree(temp.right, rtds)

att=[]
data=[[]]

if len(sys.argv)!=7:
    print('Insufficient parameters!')
    sys.exit(1)
L=int(sys.argv[1])
K=int(sys.argv[2])
training_set=sys.argv[3]
validation_set=sys.argv[4]
test_set=sys.argv[5]
to_print=sys.argv[6]

spaces=0
non=0

f = open(training_set, encoding='utf-8')
atts=f.readline()
no_of_data=0

for line in f:
    no_of_data+=1
att=atts.split(",")
att[-1]=att[-1].rstrip('\n')
no_of_att=len(att)-1
print ("No of attributes = ",no_of_att)
print ("No of data values = ",no_of_data)
f.seek(0,0)
f.readline()
for i in range(0,no_of_data):
    data.append(f.readline().rstrip('\n').split(','))
f.close()
## data list starts from index 1
## Create working dataset and attribute list for ID3
c_dataset=[]
free_att=[]
for i in range(0,no_of_att):
    free_att.append(i)
for i in range(1,no_of_data+1):
    c_dataset.append(i)

root=construct_tree(c_dataset,-1,free_att)

if(to_print=='yes'):
    print ('The decision tree is -')
    traverse_tree(root)
print('validating...')
v_data=[]
f = open(validation_set, encoding='utf-8')
f.readline()
vds_line_count=0
for line in f:
    vds_line_count+=1
f.seek(0,0)
error=0
f.readline()
for i in range(0,vds_line_count):
    v_data=(f.readline().rstrip('\n').split(','))
    validate_data(v_data,root,i)
f.close()
accuracy=((vds_line_count-error)/vds_line_count)*100
print ("Accuracy = ",accuracy,'%')

print('Testing...')
f = open(test_set, encoding='utf-8')
f.readline()
vds_line_count=0
for line in f:
    vds_line_count+=1
f.seek(0,0)
error=0
f.readline()
for i in range(0,vds_line_count):
    v_data=(f.readline().rstrip('\n').split(','))
    validate_data(v_data,root,i)
f.close()
print ("Accuracy = ",((vds_line_count-error)/vds_line_count)*100,'%')

print('Pruning...')
for i in range(0,L):
    prime=node('~')
    prime=copy_tree(root,prime)
    M=randint(1,K)
    for j in range(0,M):
        N=nonleaf_node_count(prime,1)
        P=randint(1,N)
        trim_done=0
        nodecnt=P
        trim_subtree(prime, c_dataset)
    f = open(validation_set, encoding='utf-8')
    f.readline()
    vds_line_count=0
    for line in f:
        vds_line_count+=1
    f.seek(0,0)
    error=0
    f.readline()
    for i in range(0,vds_line_count):
        v_data=(f.readline().rstrip('\n').split(','))
        validate_data(v_data,prime,i)
    f.close()
    tacc=((vds_line_count-error)/vds_line_count)*100
    if(tacc>accuracy):
        accuracy=tacc
        root=copy_tree(prime,root)
if(to_print=='yes'):
    print ('The pruned tree is -')
    traverse_tree(root)
print ('Pruning Complete with new accuracy = ',accuracy,'%')
