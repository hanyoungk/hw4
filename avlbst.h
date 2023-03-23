#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

using namespace std;

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void insertFix(AVLNode<Key,Value>* p, AVLNode<Key,Value>* n);
    void removeFix(AVLNode<Key,Value>* n, int diff);
    void rotateLeft(AVLNode<Key,Value>* n);
    void rotateRight(AVLNode<Key,Value>* n);



};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO

    //check if the tree is empty
    if(this->root_ == nullptr) {
        // create a new AVLNode pointer
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
        return;
    }

    //tree is not empty
    //mark key and value that n has
    Key key = new_item.first;
    Value value = new_item.second;


    //check if key is already in the tree
    AVLNode<Key, Value>* temp = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    
    if(temp != nullptr) {
        temp->setValue(value);
        return;
    }

    //key doesn't exist in the tree
    //set curr as the root to walk the tree to a leaf
    AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->root_); //cast root node to AVLNode
    AVLNode<Key, Value>* p = nullptr;


    //walk through until found position to insert
    while(curr != nullptr) { //while curr is going down the tree
        p = curr; //curr was originally NULL

        //go left if n's key is less than root/current
        if(key < curr->getKey()) { 
            curr = curr->getLeft();
        }
        //go right if n's key is greater than root/current
        else if(key > curr->getKey()) { 
            curr = curr->getRight();
        }
        else { 
            curr->setValue(value); //
            return;
            // break;
        }
    }

    //create a new AVL node with p as parent 
    AVLNode<Key, Value>*n = new AVLNode<Key, Value>(key, value, p);

    if(p == nullptr) {
        this->root_ = n;
        return;
    }

    //set temp as curr's parent & set curr as temp's child
    n->setParent(p);
    if(key > p->getKey()) {
        p->setRight(n);
    }
    else {
        p->setLeft(n);
    }
    // set child's balance to 0
    n->setBalance(0);

    //look at parent (p)
    if(p->getBalance() == -1 || p->getBalance() == 1) { //if they had 1 child before inserting
        p->setBalance(0);
    }
    else { //if they didnt' have any children before inserting (unbalanced now)
        if(key < p->getKey()) {
            p->updateBalance(-1); //added to the left
        }
        else {
            p->updateBalance(1); //added to the right
        }
        insertFix(p,n);
    }

}


template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key,Value>* p, AVLNode<Key,Value>* n) 
{
    //base case
    if(p == nullptr || p->getParent() == nullptr) {
        return;
    }

    //let g be p's parent & n's grandparent
    AVLNode<Key,Value>* g = p->getParent();

    //if p is left child of g
    if(p == g->getLeft()) {
        g->updateBalance(-1); //update g's balance to new accurate value for now

        //case 1
        if(g->getBalance() == 0) { //dont need to fix anything
            return;
        }
        //case 2
        else if(g->getBalance() == -1) { //grandparent's parent could be off balance
            insertFix(g, p); //recurse
        }
        //case 3
        else if(g->getBalance() == -2) { //off balance so rotate
            if(p->getLeft() == n) { //zig-zig
                rotateRight(g);
                p->setBalance(0);
                g->setBalance(0);
            }
            else { //zig-zag
                rotateLeft(p);
                rotateRight(g);

                if(n->getBalance() == -1) {
                    p->setBalance(0);
                    g->setBalance(1);
                    n->setBalance(0);
                }
                else if(n->getBalance() == 0) {
                    p->setBalance(0);
                    g->setBalance(0);
                    n->setBalance(0);
                }
                else if(n->getBalance() == 1) {
                    p->setBalance(-1);
                    g->setBalance(0);
                    n->setBalance(0);
                }
            }
        }
    }
    //p is a right child of g
    else {
        g->updateBalance(1); //update g's balance to new accurate value for now

        //case 1
        if(g->getBalance() == 0) { //dont need to fix anything
            return;
        }
        //case 2
        else if(g->getBalance() == 1) { //grandparent's parent could be off balance
            insertFix(g, p); //recurse
        }
        //case 3
        else if(g->getBalance() == 2) { //off balance so rotate
            if(p->getRight() == n) { //zig-zig
                rotateLeft(g);
                p->setBalance(0);
                g->setBalance(0);
            }
            else { //zig-zag
                rotateRight(p);
                rotateLeft(g);

                if(n->getBalance() == 1) {
                    p->setBalance(0);
                    g->setBalance(-1);
                    n->setBalance(0);
                }
                else if(n->getBalance() == 0) {
                    p->setBalance(0);
                    g->setBalance(0);
                    n->setBalance(0);
                }
                else if(n->getBalance() == -1) {
                    p->setBalance(1);
                    g->setBalance(0);
                    n->setBalance(0);
                }
            }
        }
    }

}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
   AVLNode<Key,Value>* n = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));

    if(n == nullptr) { //if key is not in the tree
        return;
    }
    if(this->root_ == nullptr) {
        return;
    }

    //if node has 2 children (balanced trees mostly have this)
    if(n->getLeft() != nullptr && n->getRight() != nullptr) { 

        //swap with predecessor
        AVLNode<Key,Value>* pred = static_cast<AVLNode<Key,Value>*>(this->predecessor(n));
        nodeSwap(n, pred);
    }

    //get n's parent
    AVLNode<Key,Value>* p = n->getParent();
    int diff = 0;

    if(p != nullptr) {
        if(p->getLeft() == n) {
            diff = 1;
        }
        else if(p->getRight() == n) {
            diff = -1;
        }
    }

    //deleting n
    //if node has 1 child
    if(n->getLeft() != nullptr || n->getRight() != nullptr) { 

        // Node<Key,Value>* parent = n->getParent(); //get n's parent 
        Node<Key,Value>* child;

        //get n's child
        if(n->getRight() == nullptr) { //if no right child
            child = n->getLeft(); //get left child
        }
        else {
            child = n->getRight(); //get right child
        }
        
        if(n != this->root_) { //if n is not root
            if(n == p->getLeft()) { //if n is a left child
                p->setLeft(child); //set n's child as n's parent's left 
            }
            else if(n == p->getRight()) { //if n is a right child
                p->setRight(child); //set n's child as n's parent's right
            }
            child->setParent(p); //set child's parent as n's parent
        }
        else { //n is root
            child->setParent(nullptr); 
            this->root_ = child;
        }
    } 
    else { //both children are nullptr (no children)
        if(n == this->root_) { //if n is a root
            this->root_ = nullptr;
        }
        else {
            Node<Key,Value>* p = n->getParent(); //get parent
            
            //check if the n is a right child or left child
            if(p->getLeft() == n) { 
                p->setLeft(NULL);
            }
            else if (p->getRight() == n) {
                p->setRight(NULL);
            }
        }
    }

    delete n;

    removeFix(p, diff);

}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key,Value>* n, int diff)
{
    if(n == nullptr) {
        return;
    }

    AVLNode<Key,Value>* p = n->getParent();


    int ndiff = 0;

    if(p != nullptr) {
        if(p->getLeft() == n) {
            ndiff = 1;
        }
        else {
            ndiff = -1;
        }   
    }

    //diff == -1 case (left heavy)
    if(diff == -1) {
        //case 1
        if(n->getBalance() + diff == -2) {
            AVLNode<Key,Value>* c = n->getLeft(); //let c = n's left child (taller)

            //case 1a
            if(c->getBalance() == -1) { //zigzig case
                rotateRight(n);
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, ndiff); //recurse
            }
            //case 1b
            else if(c->getBalance() == 0) { //zigzig case
                rotateRight(n);
                n->setBalance(-1);
                c->setBalance(1);
            }
            //case 1c
            else if(c->getBalance() == 1) { //zigzag case
                AVLNode<Key, Value>* g = c->getRight();
                rotateLeft(c);
                rotateRight(n);
                if(g->getBalance() == 1) {
                    n->setBalance(0);
                    c->setBalance(-1);
                    g->setBalance(0);
                }
                else if(g->getBalance() == 0) {
                    n->setBalance(0);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                else if(g->getBalance() == -1) {
                    n->setBalance(1);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                removeFix(p, ndiff); //recurse
            }
        }
        //case 2
        else if(n->getBalance() + diff == -1) {
            n->setBalance(-1);
        }
        //case 3
        else if(n->getBalance() + diff == 0) {
            n->setBalance(0);
            removeFix(p, ndiff);
        }
    }
    //diff == +1
    else if(diff == 1) {
        //case 1
        if(n->getBalance() + diff == 2) {
            AVLNode<Key,Value>* c = n->getRight(); //let c = n's left child (taller)
            
            //case 1a
            if(c->getBalance() == 1) { //zigzig case
                rotateLeft(n);
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, ndiff); //recurse
            }
            //case 1b
            else if(c->getBalance() == 0) { //zigzig case
                rotateLeft(n);
                n->setBalance(1);
                c->setBalance(-1);
            }
            //case 1c
            else if(c->getBalance() == -1) { //zigzag case
                AVLNode<Key, Value>* g = c->getLeft();
                rotateRight(c);
                rotateLeft(n);
                if(g->getBalance() == -1) {
                    n->setBalance(0);
                    c->setBalance(1);
                    g->setBalance(0);
                }
                else if(g->getBalance() == 0) {
                    n->setBalance(0);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                else if(g->getBalance() == 1) {
                    n->setBalance(-1);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                removeFix(p, ndiff); //recurse
            }
        }
        //case 2
        else if(n->getBalance() + diff == 1) {
            n->setBalance(1);
        }
        //case 3
        else if(n->getBalance() + diff == 0) {
            n->setBalance(0);
            removeFix(p, ndiff);
        }
    }

}



/*
* pull n down and to the left (left rotation)
*/
template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key,Value>* n)
{
    AVLNode<Key, Value>* p = n->getParent(); //get parent of n
    AVLNode<Key, Value>* right = n->getRight(); //get right child of n

    //setting right as p's child
    if(p != nullptr) { //if n isn't a root

        if(p->getLeft() == n) { //if n is left child
            p->setLeft(right);
        }
        else { //n is right child
            p->setRight(right); 
        }
        right->setParent(p);
    }
    else { //n is a root
        this->root_ = right;
        right->setParent(nullptr);
    }

    //changing pointers
    n->setParent(right); //set n's parent as right  
    n->setRight(right->getLeft()); //set n's right child as right's left child
    if(n->getRight() != nullptr) { //if n's right child is not null
        n->getRight()->setParent(n); //set that child's parent as n
    }
    right->setLeft(n); //set right's left child as n

}



template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key,Value>* n)
{
    AVLNode<Key, Value>* p = n->getParent(); //get parent of n
    AVLNode<Key, Value>* left = n->getLeft(); //get left child of n

    //setting right as p's child
    if(p != nullptr) { //if n isn't a root

        if(p->getLeft() == n) { //if n is left child
            p->setLeft(left);
        }
        else { //n is right child
            p->setRight(left); 
        }
        left->setParent(p);
    }
    else { //n is a root
        this->root_ = left;
        left->setParent(nullptr);
    }

    //changing pointers
    n->setParent(left); //set n's parent as left  
    n->setLeft(left->getRight()); //set n's left child as left's right child
    if(n->getLeft() != nullptr) { //if n's left child is not null
        n->getLeft()->setParent(n); //set that child's parent as n
    }
    left->setRight(n); //set left's right child as n

}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
