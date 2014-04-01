##	Ravish Trivedi
-------------------------------------------------------
Face recognition using Haar-like features and adaboost
-------------------------------------------------------
All codes are written in C++ language with support of OpenCV 2.4.3 library.
There are 4 C++ code files in this archive.
1. boosting.cpp
2. adaboost.cpp
3. testing.cpp
4. demo.cpp

Please read their description inside them.

You can get the OpenCV library in debian linux from:
>>sudo apt-get install libopencv-dev
---------
Dataset 
---------
The dataset is more than 10 MB in size and so is not included here.
Please download it from the below link:

http://cbcl.mit.edu/projects/cbcl/software-datasets/FaceData1Readme.html

--------
Usage
--------
1. boosting.cpp: <train faces folder path> <train nonfaces folder path> <boosting iterations> <perceptrons to learn/boosting iteration> <perceptron convergence limit> <output model file name>

Example:
>>./boosting train/face train/non-face 100 10 100 boost.model

2. adaboost.cpp: <train faces folder path> <train nonfaces folder path> <boosting iterations> <perceptron convergence limit> <output model file name>

Example:
>>./adaboost train/face train/non-face 1000 100 adaboo.model

3. demo.cpp: <image to run face detection on> <input model file>

Example:
>>./demo friends.jpg adaboo.model

4. testing.cpp: <test faces folder> <test nonfaces folder> <input model file>

Example:
>>./testing test/face test/non-face boost.model
