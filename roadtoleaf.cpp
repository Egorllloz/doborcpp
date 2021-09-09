#include <bits/stdc++.h>
using namespace std;
 
struct Node
{
    int data;
    Node *left, *right;
};
 
struct Node* getNode(int data)
{
    struct Node *newNode = new Node;
    newNode->data = data;
    newNode->left = newNode->right = NULL;
    return newNode;
}
 
bool hasPath(Node *root, vector<int>& arr, int x)
{
    if (!root)
        return false;
     
    arr.push_back(root->data);   
     
    if (root->data == x)   
        return true;
     
    if (hasPath(root->left, arr, x) ||
        hasPath(root->right, arr, x))
        return true;
     
    arr.pop_back();
    return false;           
}
 

void printPath(Node *root, int x)
{
    vector<int> arr;
     
   
    if (hasPath(root, arr, x))
    {
        for (int i=0; i<arr.size()-1; i++)   
            cout << arr[i] << "->";
        cout << arr[arr.size() - 1];   
    }
     
    else
        cout << "No Path";
}
 
int main()
{
    struct Node *root = getNode(1);
    root->left = getNode(2);
    root->right = getNode(3);
    root->left->left = getNode(4);
    root->left->right = getNode(5);
    root->right->left = getNode(6);
    root->right->right = getNode(7);
         
    int x = 5;
    printPath(root, x);
    return 0;
}