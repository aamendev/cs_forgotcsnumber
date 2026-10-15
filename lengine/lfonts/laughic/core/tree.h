#ifndef TREE_H
#define TREE_H
#include <stdio.h>
#include <string.h>
#include "../types.h"
typedef struct TreeNode
{
    char* mkey;
    void* mvalue;
} TreeNode;
typedef struct BinaryTree
{
    int mheight;
    TreeNode* mroot;
    struct BinaryTree* mleft;
    struct BinaryTree* mright;
    struct BinaryTree* mparent;
} BinaryTree;

BinaryTree* bt_grandparent(BinaryTree* root);

void bt_print_node(TreeNode* node, void (*value_print)(void*));


void bt_left_rotate(struct BinaryTree* root);
void bt_right_rotate(struct BinaryTree* root);
void bt_insert(struct BinaryTree* root, TreeNode* newnode);
struct TreeNode* bt_get(BinaryTree* root,char* key);
void bt_print_tree(BinaryTree* root, void (*value_print)(void*));
#endif
