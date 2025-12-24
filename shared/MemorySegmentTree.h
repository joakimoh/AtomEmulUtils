#ifndef MEMORY_SEGMENT_TREE
#define MEMORY_SEGMENT_TREE

// 
// Based on the balanced binary tree on https://www.geeksforgeeks.org/dsa/balanced-binary-tree/
//

#include <iostream>
#include "DeviceMemorySegment.h"
#include <vector>

using namespace std;

// Tree node representing one continuous segment of a device's memory space
class MemorySegmentNode {
public:
    DeviceMemorySegment segment;
    MemorySegmentNode* left;
    MemorySegmentNode* right;
    int height;

    MemorySegmentNode(DeviceMemorySegment segment)
        : segment(segment)
        , left(nullptr)
        , right(nullptr)
        , height(1)
    {
    }
};

class MemorySegmentTree {

private:

    MemorySegmentNode* mRoot;

    // Get the height of the node
    int getHeight(MemorySegmentNode* node);

    // Get the balance of a subtree
    int getBalance(MemorySegmentNode* node);

    // Update the height of the node
    void updateHeight(MemorySegmentNode* node);

    // Rotate a subtree to the right
    MemorySegmentNode* rotateRight(MemorySegmentNode* y);

     // Rotate a subtree to the left
    MemorySegmentNode* rotateLeft(MemorySegmentNode* x);

     // Insert a node
    MemorySegmentNode* insert(MemorySegmentNode* node, DeviceMemorySegment segment);

    // Function to find the node with the minimum value
    // (used in deletion)
    MemorySegmentNode* findMin(MemorySegmentNode* node);

    // Search the subtree for a memory segment containing a specific address
    MemoryMappedDevice* search(MemorySegmentNode* node, uint16_t adr);
    
    // Depth of a subtree
    int getDepth(MemorySegmentNode* node);

    // Create a graph of the tree
    void setPrintMtx(MemorySegmentNode* node, int hz_pos, int vt_pos, int depth, int width, vector<vector<string>>& tree_mtx);

    // Delete a subtree
    void deleteSubtree(MemorySegmentNode* node);

public:

    // Constructor
    MemorySegmentTree();

    // Destructor
   ~MemorySegmentTree();

   // Delete all nodes
   void deleteNodes();

    // Insert amemory segment
    void insert(DeviceMemorySegment segment);

    // Find the memory segment containing a specific address
    MemoryMappedDevice* search(uint16_t adr);

    // Print the tree as a graph
    void print();
};

#endif