#ifndef _BPTREE_H
#define _BPTREE_H

#include "node.h"
#include "types.h"

class BPTree
{

public:
    Node *rootNode{nullptr};
    int nodeSize;
    BPTree(int nodeSize);

    // inserting a key
    Node **insert(Node *parentNode, int key, Address* incomingRecord);

    // deleting a key
    void remove(int key);

    // displaying the tree
    void display();

    // make a test BPTree
    void makeTestTree();

    Node **linkParentToNewChildrenNode(Node *parentNode, Node *leftChildNode, Node *rightChildNode);

    Node *findParentNode(Node *cursor, Node *child);
    int findMinimumKeyInBPTree(Node* node);


private:
    // helper function for when the node size is exceeded and requires splitting

    Node *linkNonLeafNode(Node *parentNode, Node *leftNode, Node *rightNode);
    Node *linkLeafNode(Node *parentNode, Node *leftNode, Node *rightNode);

    // helper function for when the node size is unbalanced and requires merging
    void merge(Node *currentNode, int deletedKey);

    // function to recursively search the subtree for the node to insert key
    int insertInternal(Node *currentSubTreeRootNode, int key);

    // remove first key-pointer pair from a split right node
    int removeFirstKeyFromNode(Node* node);

    //
    
};

#endif