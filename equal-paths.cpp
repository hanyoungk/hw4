#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <algorithm>
#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
int height(Node* root)
{
    if(root == nullptr) { //base case
        return 0; //reached end
    }
    if(root->left && root->right) { //parent has both children
        int height1 = height(root->left); //height of left child
        int height2 = height(root->right); //height of right child
        return 1 + max(height1,height2); //return the max
    }
    else if(root->left){
        return 1 + height(root->left);
    }
    else if(root->right) {
        return 1 + height(root->right);
    }

}

bool equalPaths(Node * root)
{
    if(root == nullptr) {
        return true;
    }
    int left = 1 + height(root->left);
    int right = 1 + height(root->right);
    return (left == right);

}

