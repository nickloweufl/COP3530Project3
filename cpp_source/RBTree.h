#pragma once
#include <utility>
#include <iostream>
#include <functional>
#include <vector>
#include <memory>

// Ordered maps are often backed by red black trees, so I am implementing this tree map as a generic red black tree that 
// is ordered by a key and stores a separate value. I am just using string and int keys, but if I were to use keys of a
// a nonstandard type I would have had to add a separate compare function to the template.
template<typename _Key, typename _Value> 
struct Node {
    Node* parent;
    Node* left;
    Node* right;
    _Key key;
    _Value value;
    bool color; // Red == true : Black == false

    Node(_Key key, _Value value) : key(key), value(value) {
        left = nullptr;
        right = nullptr;
        parent = nullptr;
        color = true; // Nodes are initialized to red
    };

    ~Node() {
        // recursively deletes all nodes in the tree
        delete left;
        delete right;
    }

    // Helper function for fix
    bool isLeftChild() {
        if (this->parent == nullptr)
            return false;
        return (parent->left == this);
    }
};

template<typename _Key, typename _Value> 
class RBTree {
    private:
        Node<_Key,_Value>* Root;
        int accessCount; // Used for for full statistics
        
        // Makes given node the left child of its right child and updates parents.
        void  rotateLeft(Node<_Key,_Value>* node) {
            Node<_Key,_Value>* new_parent = node->right;
            node->right = new_parent->left;
            if ( new_parent->left != nullptr ){
                new_parent->left->parent = node;
            }
            new_parent->parent = node->parent;
            if (node->parent == nullptr) {
                this->Root = new_parent;
            } else if ( node == node->parent->left) {
                node->parent->left = new_parent;
            } else {
                node->parent->right = new_parent;
            }
            new_parent->left = node;
            node->parent = new_parent;
        }

        // Makes given node the right child of its left child and updates parents.
        void  rotateRight(Node<_Key,_Value>* node) {
            Node<_Key,_Value>* new_parent = node->left;
            node->left = new_parent->right;
            if ( new_parent->right != nullptr ){
                new_parent->right->parent = node;
            }
            new_parent->parent = node->parent;
            if (node->parent == nullptr) {
                this->Root = new_parent;
            } else if ( node == node->parent->right) {
                node->parent->right = new_parent;
            } else {
                node->parent->left = new_parent;
            }
            new_parent->right = node;
            node->parent = new_parent;
        }

        // Fixes the possible imbalances caused by inserting a node
        void fixInsert(Node<_Key,_Value>* x) {
            Node<_Key,_Value>* uncle;
            // while parent of node, x, exists and its red
            while((x->parent != nullptr && x->parent->color == true)) {
                if (x->parent->isLeftChild()) { // if the parent is a left child of grandparent
                    uncle = x->parent->parent->right;
                    if (uncle != nullptr && uncle->color == true) { // if uncle is red
                        uncle->color = false;
                        x->parent->color = false;
                        x->parent->parent->color = true;
                        x = x->parent->parent; 
                    } else {  // or if uncle is black
                        if (!x->isLeftChild()) {  // if node is right child
                            x = x->parent; 
                            rotateLeft(x);
                        }
                        x->parent->parent->color = true;
                        x->parent->color = false;
                        rotateRight(x->parent->parent); // right rotate grandparent
                    }
                } else { // and if the parent is the right child of grandparent
                    uncle = x->parent->parent->left;
                    if (uncle != nullptr && uncle->color == true) { // if uncle is red
                        uncle->color = false;
                        x->parent->color = false;
                        x->parent->parent->color = true;
                        x = x->parent->parent;
                    } else {
                        if (x->isLeftChild()) {
                            x = x->parent;
                            rotateRight(x);
                        }
                        x->parent->parent->color = true; 
                        x->parent->color = false;
                        rotateLeft(x->parent->parent); // left rotate grandparent
                    }
                }
            }
            this->Root->color = false;
        }
        
        // recursively prints every key inorder below (including) node n
        void inorderHelper(Node<_Key,_Value>* n) {
            if (n == nullptr)
                return;
            inorderHelper(n->left);
            std::cout << n->key << " ";
            inorderHelper(n->right);
        }

        // recursively adds every key-value pair in given range to valVector 
        void forEachHelper(Node<_Key,_Value>* n, std::vector<std::pair<_Key,_Value>> &valVector, _Key loBound, _Key upBound) {
            if (n == nullptr)
                return;
            if (n->key > loBound)
                forEachHelper(n->left, valVector, loBound, upBound);
            if (n->key >= loBound && n->key <= upBound)
                valVector.push_back(std::make_pair(n->key,n->value));
            if (n->key < upBound)
                forEachHelper(n->right, valVector, loBound, upBound);
        }

        // Gets largest key in tree with root n
        Node<_Key,_Value>* maximumHelper(Node<_Key,_Value>* n) {
            if(n == nullptr) { return n; }
            while (n->right) { n = n->right; }
            return n;
        }

        // Gets smallest key in tree with root n
        Node<_Key,_Value>* minimumHelper(Node<_Key,_Value>* n) {
            if(n == nullptr) { return n; }
            while (n->left) { n = n->left; }
            return n;
        }

        // Returns pointer to node with given key
        Node<_Key,_Value>* getHelper(_Key key) {
            Node<_Key,_Value>* x = nullptr;
            Node<_Key,_Value>* temp = this->Root;
            while (temp != nullptr) {
                x = temp;
                if (x->key == key) { return x; }
                if (temp->key > key) {
                    temp = temp->left;
                } else {
                    temp = temp->right;
                }
            }
            return x;
        }

    public:
        // Not including a copy constructor because it is not being used in this project
        // I would normally include it.
        RBTree() : Root(nullptr) { this->accessCount = 0; }
        ~RBTree() { delete this->Root; }

        Node<_Key,_Value>* getRoot() { return this->Root; }
        int getAccessCount() { return this->accessCount;}

        // Prints all keys in tree inorder
        void printKeysInorder() {
            inorderHelper(this->Root);
            std::cout << std::endl;
        }

        // Gets largest key in tree
        _Key maximum() {
            if (this->Root) {
                Node<_Key,_Value>* x = maximumHelper(this->Root); 
                return x->key;
            } else {
                std::cout << "Map is empty" << std::endl;
                exit(1);
            }
        }
        
        // Gets smallest key in tree
        _Key minimum() {
            if (this->Root) {
                Node<_Key,_Value>* x = minimumHelper(this->Root); 
                return x->key;
            } else {
                std::cout << "Map is empty" << std::endl;
                exit(1);
            }
        }

        // Gets node with key, key. Returns nullptr if not present.
        Node<_Key,_Value>* get(_Key key) {
            accessCount++; // This is for the tallies
            Node<_Key,_Value>* x = getHelper(key);
            return x;
        }
        
        // Checks if node with key is in map.
        bool isKeyInMap(_Key key) {
            Node<_Key,_Value>* x = getHelper(key);
            return (x != nullptr && x->key == key);
        }
        
        // Inserts node into tree
        Node<_Key,_Value>* insert(_Key key, _Value value) {
            accessCount++;
            Node<_Key,_Value>* found = nullptr;
            Node<_Key,_Value>* temp = this->Root;
            // Finds appropriate parent given key
            while (temp != nullptr) {
                found = temp;
                if (temp->key > key) {
                    temp = temp->left;
                } else if (temp->key < key) {
                    temp = temp->right;
                } else { // or returns key already exists
                    return temp;
                }
            }
            // If key doesn't exist in tree, places it in appropriate position
            Node<_Key,_Value>* newNode = new Node(key, value);
            newNode->parent = found;
            if (found == nullptr) {  // Root node does not have a parent.
                this->Root = newNode;
            } else if (key < found->key) {
                found->left = newNode;
            } else {
                found->right = newNode;
            }
        
            this->fixInsert(newNode);
            return newNode;
        }

        // Returns the key-value pairs of every node in range loBoudn to upBound in tree.
        std::vector<std::pair<_Key,_Value>> getInRange(_Key loBound, _Key upBound) {
            std::vector<std::pair<_Key,_Value>> valVector;
            forEachHelper(this->Root,valVector, loBound, upBound);
            return valVector;
        }

        // Returns every key-value pair in tree.
        std::vector<std::pair<_Key,_Value>> getAll() {
            std::vector<std::pair<_Key,_Value>> valVector;
            forEachHelper(this->Root,valVector, minimum(), maximum());
            return valVector;
        }
};