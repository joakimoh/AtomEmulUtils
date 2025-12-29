// 
// Based on the balanced binary tree on https://www.geeksforgeeks.org/dsa/balanced-binary-tree/
// 
// Normal c++ set cannot be used as the find method needs to be implemented differently
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

// Get node height
int MemorySegmentTree::getHeight(MemorySegmentNode* node)
{
    if (node == nullptr)
        return 0;
    else
        return 1 + max(getHeight(node->left), getHeight(node->right));

}

//  Rotate a subtree to the right
MemorySegmentNode* MemorySegmentTree::rotateRight(MemorySegmentNode* originalRoot)
{
    MemorySegmentNode* new_root = originalRoot->left;
    MemorySegmentNode* value_between_roots = new_root->right;

    new_root->right = originalRoot;
    originalRoot->left = value_between_roots;

    return new_root;
}

//  Rotate a subtree to the left
MemorySegmentNode* MemorySegmentTree::rotateLeft(MemorySegmentNode* originalRoot)
{
    MemorySegmentNode* new_root = originalRoot->right;
    MemorySegmentNode* value_between_roots = new_root->left;

    new_root->left = originalRoot;
    originalRoot->right = value_between_roots;

    return new_root;
}

// Insert a node
MemorySegmentNode* MemorySegmentTree::insert(MemorySegmentNode* root, DeviceMemorySegment segment)
{
    // Empty subtree - create a new subtree consisting only of the segment
    if (root == nullptr)
        return new MemorySegmentNode(segment);

    // Non-empty subtree - insert segment into it
    if (segment < root->segment)
        root->left = insert(root->left, segment);
    else if (segment > root->segment)
        root->right = insert(root->right, segment);
    else {
        // Error - duplicate key
        throw runtime_error("duplicate key");
    }


    // Get the tree's balance
    int balance = getHeight(root->left) - getHeight(root->right);

    // Balance the tree if required

    // Unbalanced to the left - segment is part of the left-left subtree (S)
    // Rotate right once to balance the tree
    //            ___
    //           /   \
    //          |  R  |
    //           \___/
    //       ___/    \
    //      /   \
    //     |     |
    //      \___/ 
    //   ___/   \
    //  /   \
    // |  S  |
    //  \___/
    //
    if (balance > 1 && segment < root->left->segment)
        return rotateRight(root);

    // Unbalanced to the right - segment is part of the right-right subtree (S)
    // Rotate left once to balance the tree
    //            ___
    //           /   \
    //          |  R  |
    //           \___/
    //           /   \___
    //               /   \
    //              |     |
    //               \___/ 
    //               /   \___
    //                   /   \
    //                  |  S  |
    //                   \___/
    //
    if (balance < -1 && segment > root->right->segment)
        return rotateLeft(root);

    // Unbalanced to the left - segment is part of the left-right subtree (S)
    // First rotate the left subtree to the left and then the complete tree to the right to balance the tree
    //            ___
    //           /   \
    //          |  R  |
    //           \___/
    //       ___/    \
    //      /   \
    //     |     |
    //      \___/ 
    //      /   \___
    //          /   \
    //         |  S  |
    //          \___/
    //

    if (balance > 1 && segment > root->left->segment) {
        root->left = rotateLeft(root->left);
        return rotateRight(root);
    }

    // Unbalanced to the right - segment is part of the right-left subtree (S)
    // First rotate the right subtree to the right and then the complete tree to the left to balance the tree
    //            ___
    //           /   \
    //          |  R  |
    //           \___/
    //           /   \___
    //               /   \
    //              |     |
    //               \___/ 
    //            ___/   \
    //           /   \
    //          |  S  |
    //           \___/
    //
    if (balance < -1 && segment < root->right->segment) {
        root->right = rotateRight(root->right);
        return rotateLeft(root);
    }

    // Already sufficiently balanced (balance is either -1, 0 or 1) => just return the root
    return root;
}

// Search the subtree for a memory segment containing a specific address
MemoryMappedDevice* MemorySegmentTree::find(MemorySegmentNode* node, uint16_t adr)
{
    if (node == nullptr)
        return nullptr;

    if (adr >= node->segment.lowerAdr && adr <= node->segment.upperAdr)
        return node->segment.dev;

    if (adr < node->segment.lowerAdr)
        return find(node->left, adr);

    return find(node->right, adr);
}

// Constructor
MemorySegmentTree::MemorySegmentTree()
    : mRoot(nullptr)
{
}

// Insert a memory segment
void MemorySegmentTree::insert(DeviceMemorySegment segment)
{
    mRoot = insert(mRoot, segment);
}

// Find the memory segment containing a specific address
MemoryMappedDevice* MemorySegmentTree::find(uint16_t adr)
{
    return find(mRoot, adr);
}

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
