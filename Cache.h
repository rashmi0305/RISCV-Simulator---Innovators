
#ifndef Cache_h
#define Cache_h
#include <cmath>
#include <cassert>
#include <iostream>
#include <vector>
#include <string>
#include<random>
#include<algorithm>
#include<limits.h>
class LRUCache {
    struct Node {
        int key=0;
        Node* prev;
        Node* next;
        
        Node(int k) : key(k), prev(nullptr), next(nullptr) {}
    };

    int capacity;//=block size initialise
    std::vector<Node*> sets;//head of each linked list (representing each set).
    std::vector<Node*> tails;//tail of each linked list (representing each set).

public:
    LRUCache(){}
    void set(int _capacity, int _numSets) { capacity=_capacity; sets.resize(_numSets,nullptr); tails.resize(_numSets, nullptr); }

    void print(int setNum)//use this to see if cache is working propeerly,it prints the linked list of that set
    {
        Node * t=sets[setNum];
        while(t!=NULL)
        {
            std::cout<<t->key<<" ";
            t=t->next;
        }
        std::cout<<std::endl;
    }
    int put(int key, int setNum) {//setnum is index
    int a=-1;//since tag is never negative,if updated then miss and block to be replaced is set here.
    if (setNum < 0 || (setNum >= sets.size() && setNum!=0))
        return a; // Invalid set number
    
    Node* node = findNode(sets[setNum], key);
    if (node) {
        if (node == sets[setNum])//it is head
            return -2;
        removeNode(node,setNum);//hit so detach it here
        a=-2;//for hit
    } else {//miss
        node = new Node(key);//create new node for current block to be added
        a=-3;//miss but space in cache
        if (size(sets[setNum]) >= capacity)//to be replaced
            a=removeTail(setNum);//remove lru and return the tag of that block to be replaced.
    }
    addToHead(node, setNum);
    return a;
}


private:
    Node* findNode(Node* head, int key) const {
        Node* curr = head;
        while (curr) {
            if (curr->key == key)
                return curr;
            curr = curr->next;
        }
        return nullptr;
    }


  void removeNode(Node* node, int setNum) {
    if (node->prev)
        node->prev->next = node->next;
    else {
        // If node is the head node
        sets[setNum] = node->next;
        if (node->next)
            node->next->prev = nullptr;
        else
            tails[setNum] = nullptr;
    }
    if (node->next)
        node->next->prev = node->prev;
    else {
        // If node is the tail node
        tails[setNum] = node->prev;
        if (node->prev)
            node->prev->next = nullptr;
        else
            sets[setNum] = nullptr;
    }
}


    void addToHead(Node* node, int setNum) {
        node->next = sets[setNum];
        node->prev = nullptr;
        if (sets[setNum])//when already head initialised
            sets[setNum]->prev = node;
        sets[setNum] = node;
        if (!tails[setNum])
            tails[setNum] = sets[setNum];
    }

    int removeTail(int setNum) {
        if (!tails[setNum])
            return -1;

        Node* tailNode = tails[setNum];
        int key1=tailNode->key;
        if (tailNode->prev)
            tailNode->prev->next = nullptr;
        else
            sets[setNum] = nullptr;

        tails[setNum] = tailNode->prev;
        delete tailNode;
        return key1;
    }

    int size(Node* head) const {
        int count = 0;
        Node* curr = head;
        while (curr) {
            ++count;
            curr = curr->next;
        }
        return count;
    }
};

struct CacheBlock {
    int tag;
    bool valid;
    //int priority;
    int frequency;
};

class CacheSimulator {
private:

    std::vector<std::vector<CacheBlock>> cache; // 2D vector representing sets and blocks
    int cacheSize=0;
    int choice=0;//LRU or LFU or Random
    int blockSize=0;
    int numSets=0;
    int numBlocksPerSet=0;//set associativity
    double cacheHits=0;
    double cacheMisses=0;
    int memacess=1;//time req to acess memory if miss
    LRUCache lru_cache;
std::pair<int,int> splitAddress(int address) {
    int num_bits_offset = static_cast<int>(std::log2(blockSize));
        std::cout<<num_bits_offset<<std::endl;
        address >>= num_bits_offset;
        int num_bits=static_cast<int>(std::log2(numSets));
        int index = address & ((1 << num_bits - 1));
        int num_bits_index =static_cast<int> (std::log2(index));
        int tag = address >>(num_bits);
        std::cout<<"Address:"<<address<<std::endl;//for checking values
        std::cout<<"num bits Offset:"<<num_bits_offset<<std::endl;
        std::cout<<"tag:"<<tag<<std::endl;
        std::cout<<"index"<<index<<std::endl;

    return std::make_pair(tag, index);
}


public:
     CacheSimulator(){}
    void setCache  (int _cacheSize, int _blockSize, int _associativity,int _choice,int memacess)
        {
         cacheSize=_cacheSize; 
         blockSize=_blockSize;
         memacess=memacess;
        assert(cacheSize % blockSize == 0);
        choice=_choice;
        numBlocksPerSet = _associativity;
        numSets = cacheSize / (blockSize * numBlocksPerSet);
        if(choice==1)
            lru_cache.set(numBlocksPerSet,numSets);//initialize only if selected lru..

        cache.resize(numSets, std::vector<CacheBlock>(numBlocksPerSet));
        for (auto& set : cache) {
            for (auto& block : set) {
                block.valid = false;
            }
        }
    }
    double getHitRate(){
        int totalAccesses = cacheHits + cacheMisses;
        if (totalAccesses == 0) {
            return 0.0; // Avoid division by zero
        }
        return static_cast<double>(cacheHits) / totalAccesses;
    }
double getMissRate()  {
    int totalAccesses = cacheHits + cacheMisses;
     if (totalAccesses == 0) {
            return 0.0; // Avoid division by zero
        }
    return static_cast<double>(cacheMisses) / totalAccesses; // Miss rate is 1 - hit rate
    }
double getAcessRate(){
    return 1+(getMissRate()* memacess);//assumed hit time =1 cycle here
}
bool access(int address) {
    // Split address into tag and index
    std::pair<int,int> a = splitAddress(address);
    int tag = a.first;
    int index = a.second;

    // Perform cache access based on chosen replacement policy
    int lru_replaceIndex = -1; // For LRU replacement

    // Check if using LRU policy
    if (choice == 1) {
        lru_replaceIndex = lru_cache.put(tag, index); // Perform LRU cache access
        lru_cache.print(index); // Debugging: Print the cache contents
    }

    // Check if the block is already in the cache
    for (int i = 0; i < numBlocksPerSet; ++i) {
        if (cache[index][i].valid && cache[index][i].tag == tag) { // Cache hit
            if (choice != 1 || (choice == 1 && lru_replaceIndex == -2)) {
                cacheHits++;
            }
            if (choice == 2) {
                cache[index][i].frequency++; // Increment frequency for LFU
            }
            return true; // Cache hit
        }
    }

    // Cache miss
    cacheMisses++;

    // Check which replacement policy is used and update the cache structure accordingly
    if (choice == 1 && lru_replaceIndex != -2) { // LRU
        cache[index][lru_replaceIndex].tag = tag; // Replace block with new tag
        cache[index][lru_replaceIndex].valid = true; // Mark block as valid
    } else if (choice == 2) { // LFU
        int minFrequency = INT_MAX;
        int lfuBlockIndex = -1;
        for (int i = 0; i < numBlocksPerSet; ++i) {
            if (!cache[index][i].valid) { // Found an empty block, use it
                lfuBlockIndex = i;
                break;
            }
            if (cache[index][i].frequency < minFrequency) { // Find block with minimum frequency
                minFrequency = cache[index][i].frequency;
                lfuBlockIndex = i;
            }
        }
        cache[index][lfuBlockIndex].tag = tag; // Replace block with new tag
        cache[index][lfuBlockIndex].valid = true; // Mark block as valid
        cache[index][lfuBlockIndex].frequency = 1; // Reset frequency for the new block
    } else { // Random
        int randomIndex = rand() % numBlocksPerSet; // Select a random block to replace
        cache[index][randomIndex].tag = tag; // Replace block with new tag
        cache[index][randomIndex].valid = true; // Mark block as valid
    }

    return false; // Cache miss
}


};
#endif 
 