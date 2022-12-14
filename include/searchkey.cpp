
#include "bptree.h"
#include <stdio.h>
#include <iostream>
//#include "node.h"
#include <queue>
#include <vector>
#include <stack>
#include <cmath>
#include "memorypool.h"

//single key
void BPTree::search(int key){
if (!rootNode){
        std::cout<<"The B+ Tree is Empty" << std::endl;
        return;
    }

    std::cout<<std::endl;
    std::cout<<"Searching~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<std::endl;
    std::stack <Node *> stack;
    Node *current = rootNode;
    Node *parent = NULL;
    int index = -1;
    stack.push(current);
    //start while loop
    while(!current->isLeaf){
        index = -1;
        /*check if the current node has only one key
        if key is less than the value at index 0, traverse to left node 
        if key is more than the value at index 0, traverse to right node
        */
        if (current->currentKeySize==1){
            if (key<current->keys[0]){
                parent = current->childrenNodes[0].getAddressNode();
            }else{
                parent = current->childrenNodes[1].getAddressNode();
            }
        }
        else{
            //run for loop until the correct index is found 
            for (int i =0; i<current->currentKeySize;i++){
                if(key>=current->keys[i] && key < current->keys[i+1]){    
                    parent = ((Node **)current->childrenNodes)[i+1];
                    index = i;
                    break; //stop searching
                }
            }
        }
        
        std::cout<<"Found "<<index<< " address" << current<< std::endl;
        if (index == -1 && key>=current->keys[current->currentKeySize-1]){
            parent = current->childrenNodes[current->currentPointerSize-1].getAddressNode();
            
        }else if (key<current->keys[0]){
            parent = current->childrenNodes[0].getAddressNode();
            
        }   
        
        current = parent;
        

        
    }
    //end while loop

    std::cout<<"current " << current << "Index: " << index <<std::endl;

    index = current->binarySearch(key);
    if (index == -1){
        std::cout<<"Key not found"<<std::endl;
        return;
    }else{
        std::cout<<"Key Found in Node with Address: " << current << "at index: " << index <<std::endl;
    }
}

//multiple keys (TODO)
//Return the number of blocks accessed
int * BPTree::searchRange(int lowKey,int highKey,MemoryPool &disk){
    //No tree -> end function
    static int result[2];
    result[0] = 0;//index block counter
    result[1] = 0;//data block counter
    if (!rootNode){
            std::cout<<"The B+ Tree is Empty" << std::endl;
            return result;
    }
    else{
            std::cout<<std::endl;
            std::cout<<"Searching~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<std::endl;
            Node *current = rootNode;
            
            bool end = false;
            unsigned int currentKey = lowKey;
            int indexNodesAcccessed=0;
            //reach a leaf node, start going right until number is no longer in range

            Address start = queryWithNumVotesAsKey(lowKey,indexNodesAcccessed);

            //incrementing block address along the way
            void* startingBlock = start.blockAddress;
            unsigned short int startingOffset = start.offset;
            // Record* record = (Record*) (disk.loadFromDisk(start,sizeof(Record)));
            Record* record = (Record *)((char*) start.blockAddress +start.offset);
            std::cout<<"\n\nAll records found within first 5 data blocks: "<<record<<std::endl;

            int blockCount = 1; 
            int recordCount = 0;
            float totalRating = 0.0;
            //void* adjacentBlock = startingBlock;
            unsigned short int newOffset = start.offset;
            // Keep accessing the key to the right, until its value is larger than the larger key
            while(!end){
                // std::cout<<"Currently looking at key: " << record->numVotes <<std::endl;
                // std::cout<<"Currently looking at tConst: " << record->tconst <<std::endl;
                if(record->numVotes>highKey||blockCount>5){

                    //end function once numVotes is higher than upper bound or blockCount more than 5
                    // std::cout<<"Breaking at key:  "<<record->numVotes<<std::endl;
                    end = true;
                    break;
                }
                else if(record->numVotes<=highKey){ 
                    //else print out the numVotes in record if numVotes of record is in range
                    if(record->numVotes>=lowKey ){
                        // std::cout<<"**Record address in disk: " << record    <<std::endl;
                        std::cout<<"********************Record Content "<< recordCount+1<<" at address: "<< (void*)((char*) start.blockAddress+(start.offset))   <<" ********************"<<std::endl;

                        std::cout<<"Record address in disk: " << (void*)((char*) start.blockAddress+(start.offset))   <<std::endl;

                        std::cout<<"NumVotes for current record: " << record->numVotes  <<std::endl;
                        std::cout<<"Average Rating for current record: " << record->averageRating  <<std::endl;
                        std::cout<<"tconst for current record: " << record->tconst  <<std::endl;
                        recordCount++;
                        totalRating+= record->averageRating;
                    }
                    //go right
                    // std::cout<<"Currently looking at offset: " << newOffset <<std::endl;
                    newOffset+=sizeof(Record);
                    if(newOffset%disk.getBlockSize()==0){
                        blockCount++;
                    }
                    // std::cout<<"Updated offset: " << newOffset <<std::endl;
                    //Address newAddress = {(void *)adjacentBlock,newOffset};
                    //record = (Record*) (disk.loadFromDisk(newAddress,sizeof(Record)));
                    record = (Record *)((char*) start.blockAddress +newOffset);
                }
            }
            

            result[0] = indexNodesAcccessed;
            if(blockCount>5){
                blockCount = 5;
            }
            result[1] = blockCount;
            std::cout<<"\nSummarised Information: " << std::endl;
            std::cout<<"total number of records accessed: " << recordCount  <<std::endl;
            std::cout<<"Average avgRating of the records: " << float(totalRating/(float)recordCount)  <<std::endl;
            return result;
            
    }
    


    
}
 
