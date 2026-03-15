#ifndef MEMORY_SEGMENT_TREE
#define MEMORY_SEGMENT_TREE

// 
// Based on the balanced binary tree on https://www.geeksforgeeks.org/dsa/balanced-binary-tree/
//

#include <iostream>
#include "DeviceMemorySegment.h"
#include <vector>
#include "DeviceTypes.h"

using namespace std;

// Tree node representing one continuous segment of a device's memory space
class MemorySegmentNode {
public:
    DeviceMemorySegment segment;
    MemorySegmentNode* left;
    MemorySegmentNode* right;

    MemorySegmentNode(DeviceMemorySegment segment)
        : segment(segment)
        , left(nullptr)
        , right(nullptr)
    {
    }
};

class MemorySegmentTree {

private:

    MemorySegmentNode* mRoot;

    // Get the height of the node
    int getHeight(MemorySegmentNode* node);

    // Rotate a subtree to the right
    MemorySegmentNode* rotateRight(MemorySegmentNode* y);

     // Rotate a subtree to the left
    MemorySegmentNode* rotateLeft(MemorySegmentNode* x);

     // Insert a node
    MemorySegmentNode* insert(MemorySegmentNode* node, DeviceMemorySegment segment);

    // Search the subtree for a memory segment containing a specific address
    MemoryMappedDevice* find(MemorySegmentNode* node, BusAddress adr);
    
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
    MemoryMappedDevice* find(BusAddress adr);

    // Print the tree as a graph
    void print();
};

#endif