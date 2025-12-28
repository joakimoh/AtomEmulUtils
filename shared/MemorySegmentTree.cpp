// 
// Based on the balanced binary tree on https://www.geeksforgeeks.org/dsa/balanced-binary-tree/
//
#include "MemorySegmentTree.h"
#include "MemoryMappedDevice.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "Utility.h"

using namespace std;

MemorySegmentTree::~MemorySegmentTree()
{
    deleteNodes();
}

void MemorySegmentTree::deleteNodes()
{
    deleteSubtree(mRoot);
}

void MemorySegmentTree::deleteSubtree(MemorySegmentNode* node)
{
    if (node != nullptr) {
        deleteSubtree(node->left);
        deleteSubtree(node->right);
        delete node;
    }
}

// Get the height of the node
int MemorySegmentTree::getHeight(MemorySegmentNode* node)
{
    if (node == nullptr)
        return 0;
    else
        return node->height;

}

// Get the balance factor of a subtree
int MemorySegmentTree::getBalance(MemorySegmentNode* node)
{
    if (node == nullptr)
        return 0;
    else
        return getHeight(node->left) - getHeight(node->right);
}

// Update the height of the node
void MemorySegmentTree::updateHeight(MemorySegmentNode* node)
{
    node->height = 1 + max(getHeight(node->left), getHeight(node->right));
}

//  Rotate a subtree to the right
MemorySegmentNode* MemorySegmentTree::rotateRight(MemorySegmentNode* y)
{
    MemorySegmentNode* x = y->left;
    MemorySegmentNode* T2 = x->right;

    x->right = y;
    y->left = T2;

    updateHeight(y);
    updateHeight(x);

    return x;
}

//  Rotate a subtree to the left
MemorySegmentNode* MemorySegmentTree::rotateLeft(MemorySegmentNode* x)
{
    MemorySegmentNode* y = x->right;
    MemorySegmentNode* T2 = y->left;

    y->left = x;
    x->right = T2;

    updateHeight(x);
    updateHeight(y);

    return y;
}

// Insert a node
MemorySegmentNode* MemorySegmentTree::insert(MemorySegmentNode* node, DeviceMemorySegment segment)
{
    // Perform the normal BST insertion
    if (!node)
        return new MemorySegmentNode(segment);

    if (segment < node->segment)
        node->left = insert(node->left, segment);
    else if (segment > node->segment)
        node->right = insert(node->right, segment);
    else
        return node; // Duplicate keys are not allowed

    // Update height of this ancestor node
    updateHeight(node);

    // Get the balance factor to check whether this node
    // became unbalanced
    int balance = getBalance(node);

    // If the node becomes unbalanced, there are 4 cases

    // Left Left Case
    if (balance > 1 && segment < node->left->segment)
        return rotateRight(node);

    // Right Right Case
    if (balance < -1 && segment > node->right->segment)
        return rotateLeft(node);

    // Left Right Case
    if (balance > 1 && segment > node->left->segment) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }

    // Right Left Case
    if (balance < -1 && segment < node->right->segment) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

// Function to find the node with the minimum value
// (used in deletion)
MemorySegmentNode* MemorySegmentTree::findMin(MemorySegmentNode* node)
{
    while (node->left)
        node = node->left;
    return node;
}

// Search the subtree for a memory segment containing a specific address
MemoryMappedDevice* MemorySegmentTree::search(MemorySegmentNode* node, uint16_t adr)
{
    if (!node)
        return nullptr;
    if (adr >= node->segment.lowerAdr && adr <= node->segment.upperAdr)
        return node->segment.dev;
    if (adr < node->segment.lowerAdr)
        return search(node->left, adr);
    return search(node->right, adr);
}

// Constructor
MemorySegmentTree::MemorySegmentTree()
    : mRoot(nullptr)
{
}

// Insert a memory segment
void MemorySegmentTree::insert(DeviceMemorySegment segment) { mRoot = insert(mRoot, segment); }

// Find the memory segment containing a specific address
MemoryMappedDevice* MemorySegmentTree::search(uint16_t adr) { return search(mRoot, adr); }

// Depth of a subtree
int MemorySegmentTree::getDepth(MemorySegmentNode * node)
{
    if (node == nullptr)
        return 0;
    int left_depth = node->left != nullptr ? 1 + getDepth(node->left) : 1;
    int right_depth = node->right != nullptr ? 1 + getDepth(node->right) : 1;
    return max(left_depth, right_depth);
}

// Print the tree as a graph
void MemorySegmentTree::print()
{
    int depth = getDepth(mRoot);
    int width = 2 * (int) pow(2, depth - 1);
    vector<vector<string>> tree_mtx(2*depth, vector<string>(width, ""));
    //cout << "depth: " << depth << ", rows: " << tree_mtx.size() << ", cols: " << tree_mtx[0].size() << "\n";
    setPrintMtx(mRoot, 0, 0, depth, width, tree_mtx);
    for (int row = 0; row < 2*depth; row++) {
        for (int col = 0; col < width; col++) {
            if (tree_mtx[row][col] == "")
                cout << setw(11) << setfill(' ') << " ";
            else
                cout << setw(10) << setfill('`') << tree_mtx[row][col] << " ";
        }
        cout << "\n";
    }
}

// Create a graph of the tree
void MemorySegmentTree::setPrintMtx(MemorySegmentNode* node, int hz_pos, int vt_pos, int depth, int width, vector<vector<string>>& tree_mtx)
{
    int hz_cell_span = 2 * (int)pow(2, depth - vt_pos - 1);
    int n_hz_cells = width / hz_cell_span;
    int hz_cell_pos = hz_pos * hz_cell_span + hz_cell_span / 2;
    DeviceMemorySegment &segment = node->segment;
    tree_mtx[2*vt_pos][hz_cell_pos] = "" + segment.dev->name;
    tree_mtx[2*vt_pos+1][hz_cell_pos] = "" + Utility::int2HexStr(segment.lowerAdr, 4) + ":" + Utility::int2HexStr(segment.upperAdr, 4);

    int offset = hz_cell_span / 4;
    if (node->left != nullptr) {
        for (int c = hz_cell_pos - offset; c < hz_cell_pos; c++)
            tree_mtx[2 * vt_pos + 1][c] = "``````````";
        tree_mtx[2 * vt_pos + 1][hz_cell_pos - offset] = "         /";
        setPrintMtx(node->left, hz_pos * 2, vt_pos + 1, depth, width, tree_mtx);
    }
    if (node->right != nullptr) {
        for (int c = hz_cell_pos + 1; c < hz_cell_pos + offset + 1; c++)
            tree_mtx[2 * vt_pos + 1][c] = "``````````";
        tree_mtx[2 * vt_pos + 1][hz_cell_pos + offset] = "\\         ";
        setPrintMtx(node->right, hz_pos * 2 + 1, vt_pos + 1, depth, width, tree_mtx);
    }
}
