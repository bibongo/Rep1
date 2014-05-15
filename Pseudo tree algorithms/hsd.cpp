/** \file hsd.cpp
    \brief Code to implement hyper-graph separator decomposition heuristic to generate pseudo tree for AND/OR search.
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

/** \fn findSeparator
    \brief Performs the hyper-graph separator decomposition
    Finds the best separator in the given array and then
    divides the array into two 'sub arrays' denoting the two
    sub-graphs and recursively following the same procedure for them.
    \param g [in] input graph array of nodes
    \param n [in] number of nodes in array 'g'
    \return tree node to the sub tree
*/
Tree* findSeparator(int *g, int n) {
    int i,j,k,x,y, maxNeighbors, maxNeighborVar, separator;
    if (n==1) {
        Tree *t = new Tree;
        t->var = g[0];
        t->left = NULL;
        t->right = NULL;
        return t;
    }
    int *hleft,*hright,hln,hrn;
    int *bestHLeft, *bestHRight, bhln,bhrn;
    int *neighbor;
    bool g2g;
    maxNeighbors = 0;
    neighbor = new int[n];
    hleft = new int [n-1];
    hright = new int [n-1];
    bestHLeft = new int [n-1];
    bestHRight = new int [n-1];
    for (i=0;i<n;i++) {
        x = 0;
        for (j=0;j<n;j++){
            if (i==j)   continue;
            if (graph[g[i]][g[j]])    x++;
        }
        if (x>maxNeighbors) {
            maxNeighbors = x;
            maxNeighborVar = g[i];
        }
    }
    if (maxNeighbors == 0) {
        bhln = 0;
        bhrn = 0;
        separator = -1;
        return NULL;
    }
    if (maxNeighbors == 1) {
        bhln = n-1;
        for (i=0;i<bhln;i++) {
            if (g[i]==maxNeighborVar)   continue;
            bestHLeft[i] = g[i];
        }
        bhrn = 0;
        separator = maxNeighborVar;
        Tree *t = new Tree;
        t->var = maxNeighborVar;
        t->left = NULL;
        t->right = NULL;
        return t;
    }
    else {
        separator = -1;
        bhln = 0;
        bhrn = n;
        for (i=0;i<n;i++){
            x=0;
            for (j=0;j<n;j++) {
                if (g[i]==g[j])   continue;
                if (graph[g[i]][g[j]])  x++;
            }
            if (x<2)    {continue;}
            maxNeighbors = 0;
            for (j=0;j<n;j++) {
                if (g[i]==g[j])   continue;
                if (graph[g[i]][g[j]])
                    neighbor[maxNeighbors++] = g[j];
            }
            hln = -1;
            hrn = -1;
            for (j=0;j<maxNeighbors;j++) {
                for (k=0;k<maxNeighbors;k++) {
                    if (neighbor[j]==neighbor[k])   continue;
                    if (!graph[neighbor[j]][neighbor[k]]) {
                        if (hln != -1) {
                            srand(time(NULL));
                            x = rand() % (j+1);
                            if(x==j) {
                                hln = j;
                                hrn = k;
                            }
                        }
                        else {
                            hln = j;
                            hrn = k;
                        }
                    }
                }
            }
            if (hln == -1) {
                srand(time(NULL));
                hln = rand() % maxNeighbors;
                srand(time(NULL));
                hrn = rand() % maxNeighbors;
                while (hln==hrn) {
                    srand(time(NULL));
                    hrn = rand() % maxNeighbors;
                }
            }
            hleft[0] = neighbor[hln];
            hright[0] = neighbor[hrn];
            hln=1;
            hrn=1;
            for (j=0;j<n;j++) {
                if (g[j]==g[i]) continue;
                g2g = true;
                if (g[j]==hleft[0])     g2g=false;
                if (g[j]==hright[0])    g2g = false;
                if (!g2g)   continue;
                x=0;
                y=0;
                for (k=0;k<hln;k++) {
                    if (graph[hleft[k]][g[j]])    x++;
                }
                for (k=0;k<hrn;k++) {
                    if (graph[hright[k]][g[j]])   y++;
                }
                if (x>y) {
                    hleft[hln++] = g[j];
                }
                else if(x==y) {
                    if (hln>hrn)    hright[hrn++] = g[j];
                    else            hleft[hln++] = g[j];
                }
                else {
                    hright[hrn++] = g[j];
                }
            }
            if (abs(hln-hrn) < abs(bhln-bhrn)) {
                separator = g[i];
                bhln = hln;
                bhrn = hrn;
                for (j=0;j<hln;j++) bestHLeft[j]=hleft[j];
                for (j=0;j<hrn;j++) bestHRight[j]=hright[j];
            }
        }
        Tree *t;
        t = new Tree;
        t->var = separator;
        t->left = findSeparator(bestHLeft,bhln);
        t->right = findSeparator(bestHRight,bhrn);
        return t;
    }
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

    // Hyper-graph separator decomposition heuristic

    pseudoTree = new int[2*nVariables+1];
    int *hypergraph, *hleft, *hright, hln, hrn, separator;
    hypergraph = new int [nVariables];
    hleft = new int [nVariables];
    hright = new int [nVariables];
    for (i=0;i<nVariables;i++)
        hypergraph[i] = i;
    Tree *tree;
    tree = findSeparator(hypergraph, nVariables);
    pseudoTreeSize = 0;
    pseudoTreeDepth = 0;
    depthFirstSearch(tree, 0);
    for (i=0;i<pseudoTreeSize;i++) {
        cout<<" "<<pseudoTree[i];
    }
    cout<<"\nPseudo tree depth = "<<pseudoTreeDepth;

    /*
    x=1;
    separator = newgraph[0];
    i=0;
    hln = 0;
    while(1) {
        if (newgraph[x] == -1)    break;
        hleft[hln++] = newgraph[x++];
    }
    i=0;
    hrn = 0;
    x++;
    while(1) {
        if (newgraph[x] == -1)    break;
        hright[hrn++] = newgraph[x++];
    }
    cout<<"HLEFT = ";
    for (i=0;i<hln;i++) cout<<hleft[i]<<" ";
    cout<<endl;
    cout<<"HRIGHT = ";
    for (i=0;i<hrn;i++) cout<<hright[i]<<" ";
    cout<<endl;
    */

    file.open(argv[2], ios::out);
    for (i=0;i<pseudoTreeSize;i++) {
        file<<pseudoTree[i];
        file<<' ';
    }
    file.close();

    // Clean up memory for termination

    for (i=0;i<nFunc;i++)
        delete (phi[i].vars);
    delete (phi);

    delete (graphState);
    delete (pseudoTree);
    for (i=0;i<nVariables;i++)
        delete (graph[i]);
    delete (graph);
    delete(hypergraph);
    delete(hleft);
    delete(hright);

    return 0;
}
