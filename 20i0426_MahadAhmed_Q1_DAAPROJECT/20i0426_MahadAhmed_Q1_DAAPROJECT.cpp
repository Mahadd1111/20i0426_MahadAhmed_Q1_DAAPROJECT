// 20i0426_MahadAhmed_Q1_DAAPROJECT.cpp : This file contains the 'main' function. Program execution begins and ends there.
// DAA PROJECT Q1

#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<vector>
#include<string>
#include<ctime>
#include<fstream>
#include<chrono>
using namespace std;

const int maxVariables = 2000;
const int maxClauses = 5000;
int numOfClauses = 0;
int numOfVariables = 0;

//Variable 1 will be stored at index 0, 2 at index 1 ... and so on
struct optimizedFlipper {
    bool value;
    int occurances_neg;
    int occurances_pos;
    bool failedFlip = 0;
}variables[maxVariables];


string names[12] = {"N1","N2","N3","N4","N5","N6","OR1","OR2","OR3","OR4","OR5","OUTPUT"};
bool setTruthAssignment(int,int,int,ofstream&);

//-------------------------------UTILITY FUNCTIONS----------------------------------------------------------

void breakString(string a,int cn,ofstream& myfile) {
    int length = a.length();
    int count = 0;
    int node = 0;
    bool flag = false;
    string word = "";
    for (int i = 0; a[i] != '\0', count <= length; i++) {
        word = "";
        if (a[i] == '0') {
            break;
        }
        while (a[i] != '\t') {
            word = word + a[i];
            i++;
            count++;
        }
        flag=setTruthAssignment(stoi(word),cn,node,myfile);
        node++;
        if (flag == true) { return; } // if even one variable sets to true in OR clause we no longer need to check the rest
    }
}

void randomizeVariables() {
    srand((unsigned)time(0));
    for (int i = 0; i < numOfVariables; i++) {
        int val = rand() % 2;
        variables[i].occurances_neg = 0;
        variables[i].occurances_pos = 0;
        variables[i].value = val;
    }
}

int countNodes(string a) {
    int length = a.length();
    int count = 0;
    int wordCount = 0;
    int node = 0;
    string word = "";
    for (int i = 0; a[i] != '\0', count <= length; i++) {
        word = "";
        if (a[i] == '0') {
            break;
        }
        while (a[i] != '\t') {
            word = word + a[i];
            i++;
            count++;
        }
        node++;
    }
    //cout<<"Total words are: "<<node<<endl;
    return node;
}

//---------------------------------------LINKED LIST STRUCTURE----------------------------------------------

struct LLNode {
    LLNode* nextNode;
    string nodeName;
    bool data;
    LLNode() {
        nodeName = "";
        data = 0;
        nextNode = NULL;
    }
};

LLNode* makeNode(string a,bool d) {
    LLNode* temp = new LLNode();
    temp->data = d;
    temp->nextNode = NULL;
    temp->nodeName = a;
    return temp;
}

class LinkedList {
public:
    LLNode* head;
    LinkedList() {
        head = NULL;
    }
    void insertIntoLL(string a, bool d) {
        LLNode* temp = new LLNode();
        temp->data = d;
        temp->nextNode = NULL;
        temp->nodeName = a;
        if (head == NULL) {
            head = temp;
        }
        else {
            LLNode* currNode = new LLNode();
            currNode = head;
            while (currNode->nextNode != NULL) {
                currNode = currNode->nextNode;
            }
            currNode->nextNode = temp;
        }
    }
    void insertIntoLL(LLNode* val) {
        if (head == NULL) {
            head = val;
        }
        else {
            LLNode* currNode = new LLNode();
            currNode = head;
            while (currNode->nextNode != NULL) {
                currNode = currNode->nextNode;
            }
            currNode->nextNode = val;
        }
    }
    void displayList() {
        printf("Displaying List\n");
        LLNode* temp = new LLNode();
        temp = head;
        while (temp != NULL) {
            cout << "Node name is: " << temp->nodeName << " and Node value is: " << temp->data << endl;
            temp = temp->nextNode;
        }
    }
};

//----------------------------------------GRAPH STRUCTURE------------------------------------------------------

/* A List (0 to 10) of Linked lists is our graph, each graph represents a clause */
class Graph {
public:
    int maxVertices;// Just N nodes
    int totalVertices; // with OR nodes
    LinkedList* adjList;
    bool result;
    Graph() {
        result = 0;
        //maxVertices = 11;
        //adjList = new LinkedList[maxVertices];
    }
    void setGraph(int v) {
        maxVertices = v;
        totalVertices = maxVertices + 5;
        adjList = new LinkedList[totalVertices];
    }
};

//vector<Graph>clauses;
Graph clauses[3668];

//-----------------------------------------------------------------------------------------------------------------------

void makeClause(int n,int v) {
    clauses[n].setGraph(v+5);
    int i = 0;
    //creating V number of N_ Node heads in the graph
    for (; i < v; i++) {
        clauses[n].adjList[i].insertIntoLL(makeNode(names[i], 0));
    }
    int temp = i;
    int j = 6;
    //creating the OR_ heads in the graph
    for (; i < temp + 5; i++) {
        clauses[n].adjList[i].insertIntoLL(makeNode(names[j], 0));
        j++;
    }
    clauses[n].adjList[0].insertIntoLL(clauses[n].adjList[temp].head); // insert OR1 infront of N1
    int k = 1;
    for (; k < v; k++) { // inserting ORs infront of other N nodes
        clauses[n].adjList[k].insertIntoLL(clauses[n].adjList[k + (v - 1)].head);
    }
    temp = k;
    for (; k < temp + 4; k++) { // inserting infront of OR nodes
        clauses[n].adjList[k].insertIntoLL(clauses[n].adjList[k + 1].head);
    }
    clauses[n].adjList[k+1].insertIntoLL(makeNode(names[11], 0));
    clauses[n].adjList[k].insertIntoLL(clauses[n].adjList[k + 1].head);
}

void resetClauses() {
    for (int i = 0; i < numOfClauses; i++) {
        for (int j = 0; j < clauses[i].totalVertices; j++) {
            LLNode* temp = clauses[i].adjList[j].head;
            while (temp != NULL) {
                temp->data = 0;
                temp = temp->nextNode;
            }
        }
        clauses[i].result = 0;
    }
}

int readingFile(ofstream& myfile,bool flag) {
    ofstream statfile("stats.txt");
    int count = 0;
    ifstream file;
    string line;
    file.open("input.txt");
    if (file.is_open()) {
        while (file.good()) {
            //cout << "\n-------------------------------------CLAUSE NUMBER " << count << " ----------------------------------------" << endl << endl;;
            auto start = chrono::steady_clock::now();
            getline(file, line);
            if (flag == 0) { // initial clause creation
                int numberOfNodes = countNodes(line);
                makeClause(count,numberOfNodes);
            }
            breakString(line,count,myfile);
            count++;
            auto end = chrono::steady_clock::now();
            statfile << "Elapsed time in nanoseconds for clause " << count << " is : " << chrono::duration_cast<chrono::nanoseconds>(end - start).count() << " ns" << endl;
            //cout << "Elapsed time in milliseconds is : " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
        }
    }
    statfile.close();
    file.close();
    return count;
}

bool setTruthAssignment(int index, int cn, int node,ofstream& myfile) {
    bool val;
    // calculating variable value based on index number from dataset
    if (index >= 0) { 
        val = variables[index-1].value; 
        variables[index-1].occurances_pos++;
    } 
    else if (index < 0) {
        index = index * -1;
        val = variables[index-1].value;
        variables[index-1].occurances_neg++;
        val = !val;
    }
    myfile << "--------------------------------------------------------Clause: "<<cn<<"  ------------------------" << endl;
    // setting the clause result through graph traversal
    bool flag = false;
    LLNode* currNode = clauses[cn].adjList[node].head;
    while (currNode) {
        bool prev = currNode->data;
        currNode->data = currNode->data + val;
        if (currNode->data == 1) { flag = true; }
        myfile << "IN NODE " << currNode->nodeName << " Previous Value was: " << prev << " and Now New Value is: " << currNode->data << endl;
        currNode = currNode->nextNode;
    }
    myfile << "--------------------------------------------------------------------------------------------------" << endl;
    if (flag == true) {
        clauses[cn].result = true;
        return true;
    }
    return false;
}

int findMin(int a[], int n) {
    int min = INT_MAX;
    int minIndex = -1;
    for (int x = 0; x < n; x++) {
        if (abs(variables[a[x]].occurances_pos - variables[a[x]].occurances_neg) <= min) {
            min=abs(variables[a[x]].occurances_pos - variables[a[x]].occurances_neg);
            minIndex=x;
        }
    }
    return minIndex; // returns index of a which maps to an index of variables with minimum occurances amongst these n values
}

// this functions finds n indexes to flip based on 2 conditions, 1- the variable is 0 and 2- it has most occurrences in truth statment, this way the most optimal flip is achieved
void performOptimizedFlip(int a[],int n) {
    int filled = 0;
    int aMinIndex = 0;
    int j = 0;
    for (int i = 0; i < numOfVariables; i++) {
        //add first n variables that satisfy condition, and then in else if optimize it so more occurrences has more values
        if (abs(variables[i].occurances_pos - variables[i].occurances_neg) > 0 and filled < n) { // checks if it occurs alot
            if (((variables[i].value == 0 and variables[i].occurances_pos > variables[i].occurances_neg) or (variables[i].value==1 and variables[i].occurances_neg>variables[i].occurances_pos)) and variables[i].failedFlip!=1) { //check if flipping it will be useful
                a[j] = i;
                filled++;
                j++;
            }
        }
        else if (filled==n) {
            aMinIndex = findMin(a, n);
            if (abs(variables[i].occurances_pos - variables[i].occurances_neg) > abs(variables[a[aMinIndex]].occurances_pos - variables[a[aMinIndex]].occurances_neg)) {
                if (((variables[i].value == 0 and variables[i].occurances_pos > variables[i].occurances_neg) or (variables[i].value == 1 and variables[i].occurances_neg > variables[i].occurances_pos))and variables[i].failedFlip != 1) {
                    a[aMinIndex] = i;
                }
            }
        }
    }
    cout << "Filled is: "<<filled << endl;
    //incase number of variables used is less than provided n change random variables to fill the gaps
    if (filled < n) {
        srand((unsigned)time(0));
        while (filled != n) {
            int d = rand() % maxVariables;
            cout << "Generated random variable: " << d << endl;
            a[filled] = d;
            filled++;
        }
    }
}

int checkEmptyFile() {
    ifstream file;
    file.open("input.txt");
    if (file.is_open()) {
        file.seekg(0, ios::end);
        size_t size = file.tellg();
        if (size == 0) {
            return -1;
        }
    }
}

//-------------------------------------------------------PROGRAM SEQUENCE FUNCTIONS------------------------------------------------------------------------------
int performAssignment(ofstream& myfile) {
    cout << "________________________________________________________________INITIAL TRUTH ASSIGNMENT________________________________________________________________________" << endl << endl;
    int count = 0;
    numOfClauses = readingFile(myfile,0);
    for (int i = 0; i < numOfClauses; i++) {
        if (clauses[i].result == 1) { 
            count++; 
        }
    }
    cout << "\n__________________________________________________________________________________________________________________________________________________________________" << endl;
    return count;
}

int performKflip(int k,int t,ofstream& myfile) {
    int maxFlip = 0;
    int maxTruth = t;
    int i = 1;
    int count = 0;
    while (i<=k) {
        auto Istart = chrono::steady_clock::now();
        while (count < maxTruth) {
            cout << "_____________________________________________________________PERFORMING " << i << " FLIP" << "____________________________________________________________________________________" << endl << endl;
            myfile << "_____________________________________________________________PERFORMING " << i << " FLIP" << "____________________________________________________________________________________" << endl << endl;
            int* newvals = new int[i];
            performOptimizedFlip(newvals, i);
            //After finding best variables to change , flip them
            cout << "Changed variables are: " << endl;
            for (int j = 0; j < i; j++) {
                variables[newvals[j]].value = !variables[newvals[j]].value;
                cout << "Variable " << newvals[j] << " , it now holds value: " << variables[newvals[j]].value << endl;
            }
            //perform traversal and solving of clauses again with new values
            resetClauses();
            numOfClauses = readingFile(myfile, 1);
            for (int y = 0; y < numOfClauses; y++) {
                if (clauses[y].result == 1) { count++; }
            }
            myfile << "\n\n\n\n";
            cout << "TRUE clauses: " << count << endl;
            if (count > maxTruth) {
                maxTruth = count;
                maxFlip = i;
                cout << "The first flip to get a better result was : " << i << " - flip, satisfying " << count << " clauses in total, " << count - t << " more than the original statement" << endl;
                return 0; // comment out this statement to run all K flips without breaking
            }
            else {
                // flip back to original value
                for (int j = 0; j < i; j++) {
                    variables[newvals[j]].value = !variables[newvals[j]].value;
                    variables[newvals[j]].failedFlip = 1;
                }
                count = 0;
            }
            auto Iend = chrono::steady_clock::now();
            if ((chrono::duration_cast<chrono::milliseconds>(Iend - Istart).count()>5000)) {
                break; //timeout i-flip after 10s and perform another flip
            }
        }
        for (int y = 0; y < numOfVariables; y++) {
            variables[y].failedFlip = 0;
        }
        i++; // go to next K flip since this one could not be resolved
        count = 0;
        cout << "\n__________________________________________________________________________________________________________________________________________________________________" << endl;    
    }
    if (i == k and count == t) { cout << "Cannot find better clauses" << endl; }
    return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

int main()
{
   ofstream myfile("traversal.txt");
   auto Mstart = chrono::steady_clock::now();
   cout << "Hello World!\n"; 
   cout << "Enter Number of Variables input: "; cin >> numOfVariables; cout << endl;
   int fileCheck = checkEmptyFile();
   if (fileCheck == -1) {
       cout << "File is empty, There are no Clauses" << endl;
       return 0;
   }
   randomizeVariables();
   myfile << "__________________________________________INITIAL ASSIGNEMENT________________________________-----------------------------------" << endl;
   int initialTrueClauses=performAssignment(myfile);
   myfile << "\n\n\n\n";
   cout << "Clauses: " << numOfClauses << endl;
   cout << "TRUE Clauses: " << initialTrueClauses << endl;
   if (initialTrueClauses == numOfClauses) { cout << "Maximum number of clauses satisfied, no flip will satisfy more" << endl; }
   performKflip(20, initialTrueClauses, myfile);
   auto Mend = chrono::steady_clock::now();
   cout << "Elapsed time in milliseconds is : " << chrono::duration_cast<chrono::milliseconds>(Mend - Mstart).count() << " ms"<<endl;
   myfile.close();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

   /*for (int i = 0; i < numOfVariables; i++) {
       cout << "Val of " << i << " is: " << variables[i].value << " , + = " << variables[i].occurances_pos << " , - = " << variables[i].occurances_neg <<" Difference = "<< variables[i].occurances_pos - variables[i].occurances_neg << endl;
   }
   for (int i = 0; i < clauses[0].totalVertices; i++) {
       LLNode* temp = clauses[0].adjList[i].head;
       while (temp) {
           cout << "In Node " << temp->nodeName << endl;
           temp = temp->nextNode;
       }
       cout << endl;
   }*/
