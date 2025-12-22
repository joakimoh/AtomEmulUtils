#ifndef MEMORY_SEGMENT_TREE
#define MEMORY_SEGMENT_TREE

// 
// Based on the balanced binary tree on https://www.geeksforgeeks.org/dsa/balanced-binary-tree/
//

#include <iostream>
#include "DeviceMemorySegment.h"

using namespace std;

// Node structure definition
class Node {
public:
    DeviceMemorySegment segment;
    Node* left;
    Node* right;
    int height;

    Node(DeviceMemorySegment segment)
        : segment(segment)
        , left(nullptr)
        , right(nullptr)
        , height(1)
    {
    }
};

class MemorySegmentTree {
private:


    Node* root;

    // Function to get the height of the node
    int height(Node* node);

    // Function to get the balance factor of the node
    int balanceFactor(Node* node);

    // Function to update the height of the node
    void updateHeight(Node* node);

    // Right rotation function
    Node* rotateRight(Node* y);

    // Left rotation function
    Node* rotateLeft(Node* x);

    // Function to insert a node
    Node* insert(Node* node, DeviceMemorySegment segment);

    // Function to find the node with the minimum value
    // (used in deletion)
    Node* findMin(Node* node);

    // Function to delete a node
    Node* remove(Node* node, DeviceMemorySegment segment);

    // Function to search for a segment in the tree
    MemoryMappedDevice* search(Node* node, uint16_t adr);

    // Function for inorder traversal of the tree
    void inorderTraversal(Node* node);

public:
    // Constructor
    MemorySegmentTree();

    // Public insert function
    void insert(DeviceMemorySegment segment);

    // Public remove function
    void remove(DeviceMemorySegment segment);

    // Public search function
    MemoryMappedDevice* search(uint16_t adr);

    // Public function to print the inorder traversal
    void printInorder();
};

#endif