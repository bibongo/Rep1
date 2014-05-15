Ravish Trivedi
--------------------------------------------------------------------------------------------------
Statistics for Machine Learning
Curricular Project work ReadMe file
Topic: Algorithms for generating minimal depth pseudo trees for AND/OR search in graphical models
--------------------------------------------------------------------------------------------------
The project implementation contains two files:
1. minfill.cpp : Implements min-fill heuristic
2. hsd.cpp : Implements hyper-graph separator decomposition heuristic
--------------------------------------------------------------------------------------------------
Both scripts are compiled using standard gcc complier. 
Compile it in standard way:
> gcc -o minfill.o minfill.cpp
> gcc -o hsd.o hsd.cpp
--------------------------------------------------------------------------------------------------
Both scripts take the same command line arguments:
1. input UAI file (.uai)
2. output file to print pseudo tree (.txt)
Example:
>minfill bayesianNetwork.uai myOutputTree.txt
--------------------------------------------------------------------------------------------------
For more description, please read the scripts itself. They are very well documented.