## Ravish Trivedi

There are two files in this archive:
1. nb.py : Naive Bayes classification algorithm
2. lr.py : Logistic regression using gradient ascent

The two files are written in Python 3.3.2 and does not include 
any third party libraries. 
----------------------------------------------------------------------------------
To run the nb.py file, use the following command:
>> python nb.py <training set path> <test set path> <use stop words?>

Example:
>>python nb.py train test 1

Give the 'use stop words?' argument as an integer 0 or 1.
It will use stop words to filter text if this argument is 1.

----------------------------------------------------------------------------------
To run the lr.py file, use the following command:
>> python lr.py <training set path> <test set path> <learning rate> <lambda> <tolerance> <max. no. of iterations> <use stop words?>

Example:
>>python lr.py train test 0.01 0.05 0.07 400 1

learning rate: Optimal values are between 0.001 and 0.1
Lambda: Optimal values are between 0.1 and 1
Tolerance: Optimal values are between 0.02 and 0.1 (i.e. 2% and 10%)
Max. no. of iterations: any positive integer value greater than some acceptable minimal (40-1000)
use stop words?: an integer 0 or 1. 1 implies the algorithm will filter out stop words from emails.
