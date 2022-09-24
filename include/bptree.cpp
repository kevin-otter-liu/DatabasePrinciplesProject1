#include "bptree.h"
#include <stdio.h>
#include <iostream>
#include "node.h"
#include <queue>
#include <vector>
#include "types.h"
#include <stack>
#include <cmath>

BPTree::BPTree(int nodeSize)
{
    this->nodeSize = nodeSize;
}

Node **BPTree::insert(Node *parentNode, int key, Address *incomingRecord)
{
    std::cout << "**********for key " << key << " *****************\n";
    // if there is no root, just insert in root
    if (this->rootNode == nullptr)
    {
        this->rootNode = new Node(this->nodeSize, true);
        this->rootNode->keys[0] = key;
        ((Address **)this->rootNode->childrenNodes)[0] = incomingRecord;
        this->rootNode->currentPointerSize++;
        this->rootNode->currentKeySize++;
        return nullptr;
    }
    else
    {
        // search for place to insert
        int insertionIndex = parentNode->binarySearchInsertIndex(key);
        std::cout << "parentNode: " << parentNode << std::endl;
        std::cout << "InsertionIndex: " << insertionIndex << std::endl;

        // todo: duplicate key
        // insert record in the index
        if (insertionIndex == -1)
        {
            std::cout << "Duplicate key: " << key << std::endl;
            throw 1;
        }

        // terminal case for recursion: parentNode is a Leaf
        if (parentNode->isLeaf)
        {
            // if leaf Node is full, split and return left and right leaf node
            // todo: leaf nodes are unlinked currently
            if (parentNode->isFull())
            {
                // left And right nodes are leafs
                Node *leftNode = new Node(this->nodeSize, true);
                Node *rightNode = new Node(this->nodeSize, true);
                std::cout<<"CALCULATION: "<<(this->nodeSize + 1) / 2<<std::endl;
                int minimumKeySizeLeft = ceil((this->nodeSize + 1) / 2);
                int minimumKeySizeRight = this->nodeSize+1 - minimumKeySizeLeft;

                // fill the left and right subtrees

                // filling left subtree
                int nodeToSplitCounter = 0;
                int newNodesCounter = 0;
                int tempInsertionIndex = insertionIndex;
                while (newNodesCounter < minimumKeySizeLeft)
                {
                    if (nodeToSplitCounter == tempInsertionIndex)
                    {
                        leftNode->keys[newNodesCounter] = key;
                        ((Address **)leftNode->childrenNodes)[newNodesCounter] = incomingRecord;

                        tempInsertionIndex = -1;
                        newNodesCounter++;
                    }
                    else
                    {
                        leftNode->keys[newNodesCounter] = parentNode->keys[nodeToSplitCounter];

                        newNodesCounter++;
                        nodeToSplitCounter++;
                    }
                    leftNode->currentKeySize++;
                    leftNode->currentPointerSize++;
                }

                // filling the right subtree
                newNodesCounter = 0;
                std::cout<<"minimumKeySizeRighht: "<<minimumKeySizeRight<<std::endl;
                std::cout<<"minimumKeySizeLeft: "<<minimumKeySizeLeft<<std::endl;

                while (newNodesCounter < minimumKeySizeRight)
                {
                    if (nodeToSplitCounter == tempInsertionIndex)
                    {
                        rightNode->keys[newNodesCounter] = key;
                        ((Address **)rightNode->childrenNodes)[newNodesCounter] = incomingRecord;
                        tempInsertionIndex = -1;
                        newNodesCounter++;
                    }
                    else
                    {
                        rightNode->keys[newNodesCounter] = parentNode->keys[nodeToSplitCounter];

                        newNodesCounter++;

                        nodeToSplitCounter++;
                    }
                    rightNode->currentKeySize++;
                    rightNode->currentPointerSize++;
                }

                // link left node to right node
                leftNode->linkToAnotherLeafNode(rightNode);  

                // terminal case where the parent is full, a leaf and a root node
                // we just create a new parent and set that as the new root Node
                if (parentNode == this->rootNode)
                {
                    std::cout<<"constructing BPTree of 2 levels"<<std::endl;
                    Node *newParentNode = new Node(3, false);
                    newParentNode->insertInitialInNonLeafNode(rightNode->keys[0], leftNode, rightNode);
                         
                    this->rootNode = newParentNode;
                    std::cout << "newRootNode: " << std::endl;
                    newParentNode->printNode();
                    std::cout << "returned Left Node: " << std::endl;
                    leftNode->printNode();
                    std::cout << "returned Right Node: " << std::endl;
                    rightNode->printNode();
                    return nullptr;
                }

                std::cout << "returned Left Node: " << std::endl;
                leftNode->printNode();
                std::cout << "returned Right Node: " << std::endl;
                rightNode->printNode();
                return new Node *[2]
                { leftNode, rightNode };
            }
            // or if leaf node is not full we can just insert a key pointer pair
            else
            {
                std::cout << "inserting subsequent pair into the parentNode\n";
                parentNode->insertSubsequentPair(key, incomingRecord);
                return nullptr;
            }
        }
        // if parentNode is not a leaf
        // this is the recursive call, we recursively find the index to insert on the node according to the key with binary search
        // then we recursively call that child using insert()
        // todo: leaf nodes are currently unlinked
        else
        {
            Node *childNode = ((Node **)parentNode->childrenNodes)[insertionIndex];

            // recursive call on the child
            // if nullptr is returned from insert() it means that the parent need not do any insertions
            // if [ Node* leftSubTree, Node* rightSubTree] is returned, it means that the child node was full
            // and requires insertion on the parent
            Node **returnedChildSubTrees = this->insert(childNode, key, incomingRecord);

            // if nullptr is returned, that means theres nothing to be inserted on the parent
            if (returnedChildSubTrees == nullptr)
            {
                return nullptr;
            }

            // if [leftSub,rightSub] is returned this means the child was full and split
            Node *leftChildSubTree = returnedChildSubTrees[0];
            Node *rightChildSubTree = returnedChildSubTrees[1];
            int keyToInsertIntoParent;

            // if child is a leaf, insert thhe first key into the parent Node
            if (rightChildSubTree->isLeaf)
            {
                keyToInsertIntoParent = rightChildSubTree->keys[0];
                //todo:link returned left child to the nearest right child
                ((Node**)parentNode->childrenNodes)[insertionIndex-1]->linkToAnotherLeafNode(leftChildSubTree);
            }
            // if child is a non-leaf, remove the first key from the right node and insert it into the parent node
            else
            {
                keyToInsertIntoParent = this->removeFirstKeyFromNode(rightChildSubTree);
            }

            if (parentNode->isFull())
            {
                // split parent node in the non root node way into 2 non-leaf nodes
                Node *leftNode;
                Node *rightNode;

                leftNode = new Node(this->nodeSize, false);
                rightNode = new Node(this->nodeSize, false);
                // child is non-leaf

                // minimum and maximum bounds of non-leaf nodes
                int minimumKeySizeRight = this->nodeSize / 2;
                int minimumKeySizeLeft = this->nodeSize - minimumKeySizeRight;
                std::cout<<"non-leaf minimumkeysizeRight: "<<minimumKeySizeRight<<std::endl;
                std::cout<<"non-leaf minimumkeysizelight"<<minimumKeySizeLeft<<std::endl;
                // build a virtual array to help with the splitting
                // todo: probably more memory and time efficient to do this in place but in hurry HAHA
                int virtualKeyArray[this->nodeSize + 1]{0};
                Node *virtualPointerArray[this->nodeSize + 2]{nullptr};
                parentNode->printNode();
                int i = 0;
                for (i; i < this->nodeSize; i++)
                {
                    virtualKeyArray[i] = parentNode->keys[i];
                    virtualPointerArray[i] = ((Node **)parentNode->childrenNodes)[i];
                }
                virtualPointerArray[i] = ((Node **)parentNode->childrenNodes)[i];

                // insert the key to be inserted
                for (int i = this->nodeSize; i > insertionIndex; i--)
                {
                    virtualKeyArray[i] = virtualKeyArray[i-1];
                }

                virtualKeyArray[insertionIndex] = keyToInsertIntoParent;
                virtualPointerArray[insertionIndex] = leftChildSubTree;

                // insert the pointer to be inserted
                for (int i = this->nodeSize+1; i > insertionIndex+1; i--)
                {
                    virtualPointerArray[i] = virtualPointerArray[i-1];
                }
                virtualPointerArray[insertionIndex + 1] = rightChildSubTree;

                //* debug
                // std::cout << "*******virtual array*****" << std::endl;
                // std::cout << "[ ";
                // for (int i = 0; i < this->nodeSize + 1; i++)
                // {
                //     std::cout << virtualKeyArray[i] << ",";
                // }
                // std::cout << "]" << std::endl;
                // std::cout << "[ ";
                // for (int i = 0; i < this->nodeSize + 2; i++)
                // {
                //     std::cout << virtualPointerArray[i] << ",";
                // }
                // std::cout << "]" << std::endl;

                // fill the left and right subtrees
                int virtualKeyCounter = 0;
                int newNodesKeyCounter = 0;

                // fill the left and right subtrees we want to return to the upper level
                while (newNodesKeyCounter < minimumKeySizeLeft)
                {
                    leftNode->keys[newNodesKeyCounter] = virtualKeyArray[virtualKeyCounter];
                    ((Node **)leftNode->childrenNodes)[newNodesKeyCounter] = virtualPointerArray[virtualKeyCounter];

                    newNodesKeyCounter++;
                    virtualKeyCounter++;
                    leftNode->currentKeySize++;
                    leftNode->currentPointerSize++;
                }

                // add the right most pointer of left subtree
                ((Node **)leftNode->childrenNodes)[newNodesKeyCounter] = virtualPointerArray[virtualKeyCounter];

                virtualKeyCounter++;
                leftNode->currentPointerSize++;

                // fill the right subtree
                newNodesKeyCounter = 0;
                while (newNodesKeyCounter < minimumKeySizeRight)
                {
                    rightNode->keys[newNodesKeyCounter] = virtualKeyArray[virtualKeyCounter];
                    ((Node **)rightNode->childrenNodes)[newNodesKeyCounter] = virtualPointerArray[virtualKeyCounter];

                    newNodesKeyCounter++;
                    virtualKeyCounter++;
                    rightNode->currentKeySize++;
                    rightNode->currentPointerSize++;
                }

                // add the right most pointer of right subtree
                ((Node **)rightNode->childrenNodes)[newNodesKeyCounter] = virtualPointerArray[virtualKeyCounter];
                rightNode->currentPointerSize++;

                // if parent node is root node and is full create new root node and link
                if (parentNode == this->rootNode)
                {
                    //  create new parent
                    Node *newRoot = new Node(this->nodeSize, false);
                    newRoot->insertInitialInNonLeafNode(keyToInsertIntoParent, leftNode, rightNode);
                    // dealloc old memory to prevent leaks
                    delete this->rootNode;
                    this->rootNode = newRoot;

                    return nullptr;
                }

                std::cout<<"non-leaf left nodes to return up: "<<std::endl;
                leftNode->printNode();
                std::cout<<"non-leaf right nodes to return up: "<<std::endl;
                rightNode->printNode();
                return new Node *[2]
                { leftNode, rightNode };
            }
            // if parent node is not full, just insert a key-pointer pair in the parent node
            else
            {
                //  deallocate memory for original left subtree
                delete ((Node **)parentNode->childrenNodes)[insertionIndex];
                ((Node **)parentNode->childrenNodes)[insertionIndex] = leftChildSubTree;
                parentNode->insertSubsequentPair(rightChildSubTree->keys[0], rightChildSubTree);
                return nullptr;
            }
        }
    }
}

void BPTree::makeTestTree()
{
    Node *rootNode = new Node(3, false);
    Node *leafNode1 = new Node(3, true);
    Node *leafNode2 = new Node(3, true);
    Node *leafNode3 = new Node(3, true);

    this->rootNode = rootNode;

    // // 3rd subtree
    leafNode3->insertInitialInLeafNode(70, nullptr, nullptr);
    leafNode3->insertSubsequentPair(60, nullptr);
    leafNode3->insertSubsequentPair(50, nullptr);

    leafNode2->insertInitialInLeafNode(40, nullptr, leafNode3);
    leafNode2->insertSubsequentPair(30, nullptr);

    leafNode1->insertInitialInLeafNode(20, nullptr, leafNode2);
    leafNode1->insertSubsequentPair(10, nullptr);

    // root node
    rootNode->insertInitialInNonLeafNode(30, leafNode1, leafNode2);
    rootNode->insertSubsequentPair(50, leafNode3);
    // testing
    rootNode->insertSubsequentPair(60, nullptr);

    // std::cout << "root children 1: " << ((Node **)rootNode->childrenNodes)[0] << std::endl;
    // std::cout << "root children 2: " << ((Node **)rootNode->childrenNodes)[1] << std::endl;
    // std::cout << "root children 3: " << ((Node **)rootNode->childrenNodes)[2] << std::endl;
};


// displaying the tree
// do a BFS of the B+tree
void BPTree::display()
{
    std::priority_queue<std::vector<Node *>> pq;

    std::vector<Node *> rootLevel = std::vector<Node *>{this->rootNode};
    pq.push(rootLevel);
    std::cout << "*********display() debug********" << std::endl;
    int level = 0;
    while (!pq.top().empty())
    {
        // gets current top Node and removes the top Node from the queue
        std::vector<Node *> currentLevel = (pq.top());
        pq.pop();

        // display keys in each node in each level
        std::cout << "level: " << level << std::endl;
        std::cout << "********************" << std::endl;

        // to store nodes in next level
        std::vector<Node *> nextLevel;

        for (int i = 0; i < currentLevel.size(); i++)
        {

            Node *currentNode = currentLevel.at(i);

            std::cout << "Node " << i << "\n";
            std::cout << "Node address: " << currentNode << "\n";
            std::cout << "currentKeySize: " << currentNode->currentKeySize << "\n";
            std::cout << "currentPointerSize: " << currentNode->currentPointerSize << "\n";
            std::string nodeType = currentNode->isLeaf ? "LEAF" : "INTERNAL NODE";
            std::cout << "nodeType: " << nodeType << "\n";

            int j;
            std::cout << "keys: [";

            // displaying each element in the node
            // if its a non leaf node we need to loop through j+1

            for (j = 0; j < currentNode->maxKeySize; j++)
            {

                std::cout << currentNode->keys[j] << ",";

                // add children nodes to nextLevel nodes if it is not nullptr
                if (((Node **)currentNode->childrenNodes)[j] != nullptr && !currentNode->isLeaf)
                {
                    nextLevel.push_back(((Node **)currentNode->childrenNodes)[j]);
                }
            }

            // add children nodes to nextLevel nodes if it is not nullptr

            // add last children Node to the nextLevel list cos there are n+1 children where n= number of keys
            if (((Node **)currentNode->childrenNodes)[j] != nullptr && !currentNode->isLeaf)
            {
                nextLevel.push_back(((Node **)currentNode->childrenNodes)[j]);
            }

            std::cout << "]" << std::endl;

            std::cout << "Pointers: [";

            // displaying each element in the node
            // if its a non leaf node we need to loop through j+1
            for (int i = 0; i < currentNode->maxPointerSize; i++)
            {
                std::cout << ((Node **)currentNode->childrenNodes)[i] << ",";
            }
            // add last children Node to the nextLevel list cos there are n+1 children where n= number of keys
            std::cout << "]" << std::endl;
            std::cout << std::endl;
        }

        pq.push(nextLevel);
        level++;
    }
    std::cout << "**** <Finished traversal> ****" << std::endl;
};

// for finding parent node
Node *BPTree::findParentNode(Node *cursor, Node *child)
{
    Node *parent;

    // when cursor reaches end of tree
    if (cursor->isLeaf)
    {
        return nullptr;
    }

    // cursor is root
    for (int i = 0; i < cursor->currentPointerSize; i++)
    {
        // if cursor's children is child, return cursor as parent
        if (((Node **)cursor->childrenNodes)[i] == child)
        {
            parent = cursor;
            return parent;
        }
        else
        {
            parent = findParentNode(((Node **)cursor->childrenNodes)[i], child);

            if (parent != nullptr)
            {
                return parent;
            }
        }
    }

    return parent;
}

// helper function for when the node size is unbalanced and requires merging
void BPTree::merge(Node *currentNode, int deletedKey){};

// helper function for insert()
int BPTree::removeFirstKeyFromNode(Node *node)
{
    int removedKey = node->keys[0];
    if (node->isLeaf)
    {
        std::cout << "cannot remove first key-pointer pair from a leaf node" << std::endl;
        throw 1;
    }

    for (int i = 0; i < node->currentKeySize; i++)
    {
        node->keys[i] = node->keys[i + 1];
    }

    for (int i = 1; i < node->currentKeySize + 1; i++)
    {
        ((Node **)node->childrenNodes)[i] = ((Node **)node->childrenNodes)[i + 1];
    }
    node->currentKeySize--;
    node->currentPointerSize--;
    return removedKey;
};

// finds the minimum key for the BPTree
int BPTree::findMinimumKeyInBPTree(Node *node)
{
    if (node->isLeaf)
    {
        return node->keys[0];
    }
    return findMinimumKeyInBPTree(((Node **)node->childrenNodes)[0]);
}