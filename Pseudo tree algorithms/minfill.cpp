/** \file minfill.cpp
    \brief Code to implement min-fill heuristic to generate pseudo tree for AND/OR search.
    @author Ravish Trivedi
    Statistics for Machine Learning- Curricular project work
    Topic: Develop algorithms for generating minimal depth pseudo trees for AND/OR search technique in graphical models
    Input: Bayesian/Markov network in UAI format
    Output: .txt file containing DFS traversed sequence of pseudo tree variables with a '-1' indicating no child at next branch.
*/

#include<iostream>
#include<vector>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fstream>
#include<math.h>
#include<time.h>
#include<dirent.h>
using namespace std;

bool **graph;           ///< Adjacency Matrix for network variables
int nVariables;         ///< Number of variables in the network
bool *graphState;       ///< Boolean array to indicate whether indexed variable is being considered or not
int *pseudoTree;        ///< Array to store generated pseudo tree variables in DFS visited sequence
int pseudoTreeSize;     ///< Size of pseudo tree array 'pseudoTree'
int pseudoTreeDepth;    ///< Depth of pseudo tree generated

/** \struct Tree
    \brief Data structure to represent the pseudo tree to be generated.
*/
struct Tree {
    int var;    ///< Variable associated with the node
    struct Tree *left;  ///< Left child of node
    struct Tree *right; ///< Right child of node
}*tree;

/** \struct Function
    \brief Data structure to represent each function in the network.
*/
struct Function {
    int nVars;  ///< Number of variables in the function
    int *vars;  ///< Array of variable in the function
}*phi;


/** \fn search
    \brief Search for element 'x' in the array 'a' of size 'n' and return whether 'x' exists in 'a' or not.
    \param a [in] The array 'a'
    \param n [in] The size of array 'a'
    \param x [in] The element to be searched in array 'a'
    \return Whether x'x exists in array 'a' or not
*/
bool search(int *a, int n, int x) {
    int i;
    for (i=0;i<n;i++) {
        if (a[i]==x)
            return true;
    }
    return false;
}
/** \fn findNeighbor
    \brief Function to find neighbor of a variable in Min-fill induced graph.
    \param n [in] variable whose neighbor is to be found
    \return Random neighbor of given variable from the graph
*/
int findNeighbor(int n) {
    int i,neighborCount,randomInt,returnVal;
    int *neighbor = new int[nVariables];
    neighborCount = -1;
    for(i=0;i<nVariables;i++) {
        if (i==n)   continue;
        if (graph[n][i] == true and graphState[i]==true)
            neighbor[++neighborCount] = i;
    }
    if (neighborCount==-1) {
        returnVal = -1;
    }
    else {
        srand(time(NULL));
        randomInt = rand()%(neighborCount+1);
        returnVal = neighbor[randomInt];
    }
    delete(neighbor);
    return returnVal;
}

/** \fn generateTree
    \brief Checks for current node's neighbors in min-fill induced graph and assigns them as child of current node in pseudo tree.
    Recursively calls itself for child node's evaluation until all nodes in the tree are evaluated.
    \param t [in] Tree node that is to be evaluated
    \return Returns updated tree node
*/
Tree* generateTree(Tree *t) {
    int n1,n2;
    n1 = findNeighbor(t->var);
    if (n1 != -1) {
        Tree *node = new Tree;
        node->var = n1;
        node->left = NULL;
        node->right = NULL;
        t->left = node;
        graphState[n1] = false;
    }
    n2 = findNeighbor(t->var);
    if (n2!=-1) {
        Tree *node = new Tree;
        node->var = n2;
        node->left = NULL;
        node->right = NULL;
        t->right = node;
        graphState[n2] = false;
    }

    if (t->left!=NULL) {
        t->left = generateTree(t->left);
    }
    if (t->right!=NULL) {
        t->right = generateTree(t->right);
    }
    return t;
};

/** \fn depthFirstSearch
    \brief DFS traversal of tree. Stores DFS visited sequence of variables in 'pseudoTree' array.
    Calculates depth of pseudo tree in 'pseudoTreeDepth' variable
    \param t [in] Tree node to be checked
    \param depth [in] Depth of tree at the given node 't'
*/
void depthFirstSearch(Tree *t, int depth) {
    if (depth>pseudoTreeDepth)
        pseudoTreeDepth = depth;
    pseudoTree[pseudoTreeSize++] = t->var;
    if (t->left != NULL) {
        depthFirstSearch(t->left, depth+1);
    }
    else {
        pseudoTree[pseudoTreeSize++] = -1;
    }
    if (t->right != NULL) {
        depthFirstSearch(t->right, depth+1);
    }
    else {
        pseudoTree[pseudoTreeSize++] = -1;
    }
    return;
}

/** \fn main
    \brief main function
    \param argc [in] Number of incoming arguments
    \param argv [in] array of strings of size argc
    \return process return value
*/
int main(int argc, char**argv) {
    int a,i,j,k,tsize,x;    ///< Temporary variables
    char ch;                ///< Dummy buffer tor read useless file characters
    string str;             ///< Buffer used for reading file
    string modelType;       ///< String denotes network type (Bayesian/Markov)
    int nFunc;              ///< Number of CPTs or functions in the network
    DIR *dir;
    struct dirent *ent;
    fstream file;           ///< File descriptor for reading and writing files

    // Read network and Initiate data structures

    if (argc!=3) {
        cout<<"Invalid number of arguments!\n";
        cout<<"Argument list: <input file path> <output file path>\n";
        exit(0);
    }

    file.open(argv[1], ios::in);
    getline(file, str);
    modelType = str;
    cout<<"Model type = "<<modelType<<endl;
    file>>nVariables;
    cout<<"Number of variables in network = "<<nVariables<<endl;
    file>>ch;
    getline(file,str);
    file>>nFunc;
    //cout<<"Number of functions (CPT) in network = "<<nFunc<<endl;
    phi = new struct Function [nFunc];
    for (i=0;i<nFunc;i++){
        file>>tsize;
        phi[i].nVars = tsize;
        phi[i].vars = new int[nVariables];
        for (j=0;j<tsize;j++){
            file>>x;
            phi[i].vars[j]=x;
        }
    }
    file.close();
    graph = new bool* [nVariables];
    for (i=0;i<nVariables;i++) {
        graph[i] = new bool [nVariables];
        for (j=0;j<nVariables;j++) {
            if (i==j)   graph[i][j] = true;
            else    graph[i][j] = false;
        }
    }

    // Initiate adjacency matrix for network. Technique differs depending on type of network

    if (modelType=="BAYES") {
        for (i=0;i<nFunc;i++) {
            for (j=0; j<phi[i].nVars-1; j++) {
                for (k=0;k<phi[i].nVars-1; k++) {
                    if (!search(phi[phi[i].vars[j]].vars,phi[phi[i].vars[j]].nVars,phi[i].vars[k]) && j!=k) {
                        phi[phi[i].vars[j]].vars[phi[phi[i].vars[j]].nVars] = phi[i].vars[k];
                        phi[phi[i].vars[j]].nVars++;
                    }
                }
            }
        }
        for (i=0;i<nFunc;i++) {
            for (j=0;j<phi[i].nVars;j++) {
                if (phi[i].vars[j] == i)
                    break;
            }
            x = phi[i].vars[j];
            phi[i].vars[j] = phi[i].vars[phi[i].nVars-1];
            phi[i].vars[phi[i].nVars-1] = x;
        }
        for (i=0;i<nFunc;i++) {
            for (j=0;j<phi[i].nVars-1;j++) {
                graph[phi[i].vars[j]][phi[i].vars[phi[i].nVars-1]]=true;
                graph[phi[i].vars[phi[i].nVars-1]][phi[i].vars[j]]=true;
            }
        }
    }
    else {
        for (i=0;i<nFunc;i++) {
            if (phi[i].nVars==1) continue;
            for (j=0;j<phi[i].nVars;j++) {
                for (k=0;k<phi[i].nVars;k++) {
                    graph[phi[i].vars[j]][phi[i].vars[k]] = true;
                    graph[phi[i].vars[k]][phi[i].vars[j]] = true;
                }
            }
        }
    }

    // Min-fill Heuristic

    int *arr;                   ///< Temporary full length array to store neighbors of node in Min-fill graph induction process
    int counter;                ///< Temporary counter to 'arr' that gives number of neighbors of a node in Min-fill graph induction process
    int ***distanceEdge;
    int *distanceEdgeCounter;
    int *eliminationOrder;      ///< Array to store the calculated elimination order of nodes
    int nextElimination;        ///< Temporarily keeps track of node to be eliminated in Min-fill graph induction process
    unsigned int distance;      ///< Gives number of edges a node may add to the graph if eliminated in Min-fill graph induction process
    unsigned int minDistance;   ///< Minimizing 'distance' over all possible nodes to check the best node to eliminate
    bool go;                    ///< serves as loop condition for variable elimination method. Checks whether all variables in the graph are eliminated
    int *bestNodes;             ///< Temporary array to hold all possible nodes for elimination
    int bestNodesCounter;       ///< Counter to 'bestNodes' to hold number of nodes in the array

    arr = new int[nVariables];
    bestNodes = new int[nVariables];
    eliminationOrder = new int[nVariables];
    graphState = new bool[nVariables];
    for(i=0;i<nVariables;i++) {
        graphState[i] = true;
    }
    distanceEdge = new int** [nVariables];
    distanceEdgeCounter = new int [nVariables];
    for(i=0;i<nVariables;i++) {
        distanceEdge[i] = new int* [nVariables*nVariables];
        for (j=0;j<nVariables*nVariables;j++) {
            distanceEdge[i][j] = new int [2];
        }
    }

    go=true;
    x=0;
    while (go){
        minDistance = nVariables*nVariables;
        nextElimination = -1;
        for (i=0;i<nVariables;i++) {
            if (!graphState[i]) continue;
            int *neighbors;
            counter = 0;
            for (j=0;j<nVariables;j++) {
                if(j==i || !graphState[j])    continue;
                if (graph[i][j]) {
                    arr[counter] = j;
                    counter++;
                }
            }
            neighbors = new int [counter];
            for (j=0;j<counter;j++) {
                neighbors[j] = arr[j];
            }

            distance = 0;
            distanceEdgeCounter[i] = 0;
            for (j=0;j<counter;j++) {
                for (k=j+1;k<counter;k++){
                    if (!graph[neighbors[j]][neighbors[k]]) {
                        distanceEdge[i][distanceEdgeCounter[i]][0]=neighbors[j];
                        distanceEdge[i][distanceEdgeCounter[i]][1]=neighbors[k];
                        distanceEdgeCounter[i]++;
                        distance++;
                    }
                }
            }
            if (distance < minDistance) {
                bestNodesCounter = 0;
                minDistance = distance;
                nextElimination = i;
            }
            if (distance == minDistance) {
                bestNodes[bestNodesCounter++] = i;
            }
        }
        if (bestNodesCounter) {
            srand(time(NULL));
            j = rand() % bestNodesCounter;
            nextElimination = bestNodes[j];
        }
        eliminationOrder[x++] = nextElimination;
        for (i=0;i<distanceEdgeCounter[nextElimination];i++) {
            graph[distanceEdge[nextElimination][i][0]][distanceEdge[nextElimination][i][1]] = true;
            graph[distanceEdge[nextElimination][i][1]][distanceEdge[nextElimination][i][0]] = true;
        }
        graphState[nextElimination] = false;
        go = false;
        for (i=0;i<nVariables;i++){
            if (graphState[i])
                go = true;
        }
    }

    /*// Display contents of adjancency matrix
    cout<<"Graph is ..";
    for (i=0;i<nVariables;i++) {
        cout<<"\n"<<i<<" : ";
        for (j=0;j<nVariables;j++) {
            cout<<graph[i][j]<<" ";
        }
    }
    cout<<endl;
    */
    // Display elimination order
    cout<<"Elimination order : ";
    for (i=0;i<nVariables;i++) {
        cout<<eliminationOrder[i]<<" ";
    }
    cout<<endl;

    for (i=0;i<nVariables;i++) {
        graphState[i] = true;
    }

    tree = new Tree;
    tree->var = eliminationOrder[0];
    tree->left = NULL;
    tree->right = NULL;
    graphState[eliminationOrder[0]] = false;
    tree = generateTree(tree);

    pseudoTree = new int [3*nVariables+1];
    pseudoTreeSize = 0;
    pseudoTreeDepth = 0;
    depthFirstSearch(tree,0);
    cout<<"Pseudo tree : ";
    for(i=0;i<pseudoTreeSize;i++) {
        cout<<pseudoTree[i]<<" ";
    }
    cout<<endl;
    cout<<"Pseudo tree depth = "<<pseudoTreeDepth<<endl;

    file.open(argv[2], ios::out);
    for (i=0;i<pseudoTreeSize;i++) {
        file<<pseudoTree[i];
        file<<' ';
    }
    file.close();

    // Clean up memory for termination
    for (j=0;j<nVariables;j++) {
        for (i=0;i<nVariables*nVariables;i++) {
            delete(distanceEdge[j][i]);
        }
        delete (distanceEdge[j]);
    }
    delete (distanceEdgeCounter);
    delete (arr);

    for (i=0;i<nFunc;i++)
        delete (phi[i].vars);
    delete (phi);

    delete (graphState);
    delete (pseudoTree);
    for (i=0;i<nVariables;i++)
        delete (graph[i]);
    delete (graph);

    return 0;
}
