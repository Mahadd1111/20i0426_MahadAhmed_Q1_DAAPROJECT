// 20i0426_MahadAhmed_Q1_DAAPROJECT.cpp : This file contains the 'main' function. Program execution begins and ends there.
// DAA PROJECT Q1

#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<vector>
#include<string>
#include<ctime>
#include<fstream>
using namespace std;

struct optimizedFlipper {
    bool value;
    int occurances;
}variables[1040];
int numOfClauses = 0;
string names[12] = {"N1","N2","N3","N4","N5","N6","OR1","OR2","OR3","OR4","OR5","OUTPUT"};
bool setOriginalTruthAssignment(int,int,int);

//-------------------------------UTILITY FUNCTIONS----------------------------------------------------------

void breakString(string a,int cn) {
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
        flag=setOriginalTruthAssignment(stoi(word),cn,node);
        node++;
        if (flag == true) { return; } // if even one variable sets to true in OR clause we no longer need to check the rest
    }
}

void randomizeVariables() {
    srand((unsigned)time(0));
    for (int i = 0; i < 1040; i++) {
        int val = rand() % 2;
        variables[i].occurances = 0;
        variables[i].value = 0;
    }
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
    int maxVertices;
    LinkedList* adjList;
    bool result;
    Graph() {
        result = 0;
        maxVertices = 11;
        adjList = new LinkedList[maxVertices];
    }
};

Graph clauses[3];

//-----------------------------------------------------------------------------------------------------------------------

void makeClause(int n) {
    for (int i = 0; i < 11; i++) {
        clauses[n].adjList[i].insertIntoLL(makeNode(names[i], 0));
    }
    clauses[n].adjList[0].insertIntoLL(clauses[n].adjList[6].head);
    for (int i = 1; i < 6; i++) {
        clauses[n].adjList[i].insertIntoLL(clauses[n].adjList[i + 5].head);
    }
    for (int i = 6; i < 10; i++) {
        clauses[n].adjList[i].insertIntoLL(clauses[n].adjList[i + 1].head);
    }
    clauses[n].adjList[10].insertIntoLL(makeNode(names[11], 0));
}

int readingFile() {
    int count = 0;
    ifstream file;
    string line;
    file.open("input.txt");
    if (file.is_open()) {
        while (file.good()) {
            cout << "\n-------------------------------------CLAUSE NUMBER " << count << " ----------------------------------------" << endl << endl;;
            getline(file, line);
            makeClause(count);
            breakString(line,count);
            count++;
        }
    }
    file.close();
    return count;
}

bool setOriginalTruthAssignment(int index, int cn, int node) {
    bool val;
    // calculating variable value based on index number from dataset
    if (index >= 0) { 
        val = variables[index].value; 
        variables[index].occurances++;
    } 
    else if (index < 0) {
        index = index * -1;
        val = variables[index].value;
        variables[index].occurances++;
        val = !val;
    }
    // setting the clause result through graph traversal
    bool flag = false;
    LLNode* currNode = clauses[cn].adjList[node].head;
    while (currNode) {
        bool prev = currNode->data;
        currNode->data = currNode->data + val;
        if (currNode->data == 1) { flag = true; }
        cout << "IN NODE " << currNode->nodeName << " Previous Value was: " << prev << " and Now New Value is: " << currNode->data << endl;
        currNode = currNode->nextNode;
    }
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
        if (variables[a[x]].occurances <= min) {
            min=variables[a[x]].occurances;
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
    for (int i = 0; i < 1040; i++) {
        if (variables[i].value == 0 and variables[i].occurances > 0 and filled < n) {
            a[j] = i;
            filled++;
            j++;
        }
        else if (variables[i].value == 0 and filled==n) {
            aMinIndex = findMin(a, n);
            if (variables[i].occurances > variables[a[aMinIndex]].occurances) {
                a[aMinIndex] = i;
            }
        }
    }
}

//-------------------------------------------------------PROGRAM SEQUENCE FUNCTIONS------------------------------------------------------------------------------
int performInitialAssignment() {
    cout << "________________________________________________________________INITIAL TRUTH ASSIGNMENT________________________________________________________________________" << endl << endl;
    int count = 0;
    numOfClauses = readingFile();
    for (int i = 0; i < 3; i++) {
        if (clauses[i].result == 1) { count++; }
    }
    cout << "\n__________________________________________________________________________________________________________________________________________________________________" << endl;
    return count;
}

int performKflip(int k) {
    int maxFlip = -1;
    int maxTruth = 0;
    for (int i = 1; i <= k; i++) {
        cout << "_____________________________________________________________PERFORMING " << i << " FLIP" << "____________________________________________________________________" << endl << endl;
        int* newvals = new int[i];
        performOptimizedFlip(newvals,i);
        for (int j = 0; j < i; j++) {
            variables[newvals[j]].value = !variables[newvals[j]].value;
        }
        cout << "\n__________________________________________________________________________________________________________________________________________________________________" << endl;
    }
    return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

int main()
{
   cout << "Hello World!\n"; 
   randomizeVariables();
   int initialTrueClauses=performInitialAssignment();
   cout << "Clauses: " << numOfClauses << endl;
   cout << "TRUE Clauses: " << initialTrueClauses << endl;
   performKflip(2);
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
