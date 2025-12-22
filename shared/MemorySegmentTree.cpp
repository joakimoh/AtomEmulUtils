// 
// Based on the balanced binary tree on https://www.geeksforgeeks.org/dsa/balanced-binary-tree/
//
#include "MemorySegmentTree.h"

using namespace std;

// Function to get the height of the node
int MemorySegmentTree::height(Node* node)
{
    return node ? node->height : 0;
}

// Function to get the balance factor of the node
int MemorySegmentTree::balanceFactor(Node* node)
{
    return node ? height(node->left)
        - height(node->right)
        : 0;
}

// Function to update the height of the node
void MemorySegmentTree::updateHeight(Node* node)
{
    node->height = 1
        + max(height(node->left),
            height(node->right));
}

// Right rotation function
Node* MemorySegmentTree::rotateRight(Node* y)
{
    Node* x = y->left;
    Node* T2 = x->right;

    x->right = y;
    y->left = T2;

    updateHeight(y);
    updateHeight(x);

    return x;
}

// Left rotation function
Node* MemorySegmentTree::rotateLeft(Node* x)
{
    Node* y = x->right;
    Node* T2 = y->left;

    y->left = x;
    x->right = T2;

    updateHeight(x);
    updateHeight(y);

    return y;
}

// Function to insert a node
Node* MemorySegmentTree::insert(Node* node, DeviceMemorySegment segment)
{
    // Perform the normal BST insertion
    if (!node)
        return new Node(segment);

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
    int balance = balanceFactor(node);

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
Node* MemorySegmentTree::findMin(Node* node)
{
    while (node->left)
        node = node->left;
    return node;
}

// Function to delete a node
Node* MemorySegmentTree::remove(Node* node, DeviceMemorySegment segment)
{
    // Perform standard BST delete
    if (!node)
        return nullptr;

    if (segment < node->segment)
        node->left = remove(node->left, segment);
    else if (segment > node->segment)
        node->right = remove(node->right, segment);
    else {
        if (!node->left || !node->right) {
            Node* temp
                = node->left ? node->left : node->right;
            if (!temp) {
                temp = node;
                node = nullptr;
            }
            else
                *node = *temp;
            delete temp;
        }
        else {
            Node* temp = findMin(node->right);
            node->segment = temp->segment;
            node->right
                = remove(node->right, temp->segment);
        }
    }

    if (!node)
        return nullptr;

    // Update height of the current node
    updateHeight(node);

    // Get the balance factor
    int balance = balanceFactor(node);

    // Balance the node if it has become unbalanced

    // Left Left Case
    if (balance > 1 && balanceFactor(node->left) >= 0)
        return rotateRight(node);

    // Left Right Case
    if (balance > 1 && balanceFactor(node->left) < 0) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }

    // Right Right Case
    if (balance < -1 && balanceFactor(node->right) <= 0)
        return rotateLeft(node);

    // Right Left Case
    if (balance < -1
        && balanceFactor(node->right) > 0) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

// Function to search for a segment in the tree
MemoryMappedDevice* MemorySegmentTree::search(Node* node, uint16_t adr)
{
    if (!node)
        return nullptr;
    if (adr >= node->segment.lowerAdr && adr <= node->segment.upperAdr)
        return node->segment.dev;
    if (adr < node->segment.lowerAdr)
        return search(node->left, adr);
    return search(node->right, adr);
}

// Function for inorder traversal of the tree
void MemorySegmentTree::inorderTraversal(Node* node)
{
    if (node) {
        inorderTraversal(node->left);
        cout << node->segment;
        inorderTraversal(node->right);
        cout << " ";
    }
}


// Constructor
MemorySegmentTree::MemorySegmentTree()
    : root(nullptr)
{
}

// Public insert function
void MemorySegmentTree::insert(DeviceMemorySegment segment) { root = insert(root, segment); }

// Public remove function
void MemorySegmentTree::remove(DeviceMemorySegment segment) { root = remove(root, segment); }

// Public search function
MemoryMappedDevice* MemorySegmentTree::search(uint16_t adr) { return search(root, adr); }

// Public function to print the inorder traversal
void MemorySegmentTree::printInorder()
{
    inorderTraversal(root);
    cout << endl;
}
