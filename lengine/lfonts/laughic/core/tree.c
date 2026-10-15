#include "tree.h"
#include <stdlib.h>

BinaryTree* bt_grandparent(BinaryTree* root) {
    if (root == NULL || root->mparent == NULL) return NULL;
    return root->mparent->mparent;
}

void bt_print_node(TreeNode* node, void (*value_print)(void*)) {
        printf("key: %s, value: ", node->mkey);
        value_print(node->mvalue);
}
void bt_left_rotate(struct BinaryTree* root)
{
    BinaryTree* temp;
    TreeNode* tempnode;
    temp = root->mleft;
    tempnode = root->mroot;

    root->mleft = root->mright;
    root->mroot = root->mright->mroot;

    root->mright = root->mleft->mright;
    root->mleft->mright = root->mleft->mleft;
    root->mleft->mleft = temp;
    root->mleft->mroot = tempnode;
}
void bt_right_rotate(struct BinaryTree* root)
{
    BinaryTree* temp;
    TreeNode* tempnode;
    temp = root->mright;
    tempnode = root->mroot;

    root->mright = root->mleft;
    root->mroot = root->mleft->mroot;

    root->mleft = root->mright->mleft;
    root->mright->mleft = root->mright->mright;
    root->mright->mright = temp;
    root->mright->mroot = tempnode;
}
void bt_insert(struct BinaryTree* root, TreeNode* newnode)
{
    BinaryTree* neww = malloc(sizeof(BinaryTree));
    neww->mroot = newnode;
    neww->mleft = NULL;
    neww->mright = NULL;
    neww->mheight = 0;
    neww->mparent = NULL;
    if (root->mheight == -1)
    {
        *root = *neww;
        return;
    }

    BinaryTree* curr = root;
    BinaryTree* parent = NULL;
    int comp = 0;
    while (curr != NULL)
    {
        comp = strcmp(newnode->mkey, root->mroot->mkey);
        parent = curr;
        if(comp <= 0)
        {
            curr = curr->mleft;
        }
        else 
        {
            curr = curr->mright;
        }
    }
    neww->mparent = parent;
    if (parent != NULL)
    {
    if (comp <=0)
    {
        parent->mleft = neww;
    }
    else 
    {
        parent->mright = neww; 
    }
    }
    int h = 1;
    while (parent != NULL)
    {
        parent->mheight = h++;
        parent = parent->mparent;
    }
    BinaryTree* grand_parent = bt_grandparent(neww);
    if (grand_parent != NULL)
    {
        int l_height = 0, r_height = 0;
        if (grand_parent->mleft != NULL)
        {
            l_height = grand_parent->mleft->mheight;
        }
        if (grand_parent->mright != NULL)
        {
            r_height = grand_parent->mright->mheight;
        }
        int diff = l_height - r_height;
        if (diff >= 1)
        {
            bt_right_rotate(grand_parent);
        }
        else if (diff <= - 1)
        {
            bt_left_rotate(grand_parent);
        }
    }
}
TreeNode* bt_get(BinaryTree* root, char* key)
{
    BinaryTree* curr = root;
    int comp;
    while (curr != NULL)
    {
        comp = strcmp(key, root->mroot->mkey);
        if (comp == 0) return curr->mroot;
        else if (comp < 0) curr = curr->mleft;
        else curr = curr->mright;
    }

    return NULL;
}


void bt_print_tree(BinaryTree* root, void(*value_print)(void*))
{
    if (root == NULL) return;
    int capacity = 1 << (root->mheight);
    BinaryTree** node_queue = malloc(capacity * sizeof(BinaryTree*));
    BinaryTree* curr;
    int current_front = 0;
    int current_end = -1;
    int size = 0;
    node_queue[(++current_end) % capacity] = root;
    size++;
    int count = -1;
    int current_pow = 0;
    BinaryTree* gp;
    while (size != 0)
    {
        count++;
        size--;
        curr = node_queue[(current_front++) % capacity];
        for (int i = 0; i < curr->mheight; i++)
        {
            printf("\t");
        }
        bt_print_node(curr->mroot, value_print);

        if ((gp = bt_grandparent(curr))!= NULL)
        {
            for (int i = 0; i < gp->mheight; i++)
            {
                printf("\t");
            }
        }
        if (count % (2 << current_pow) == 0)
        {
            printf("\n");
            current_pow <<= 1;
        }
        if (curr->mleft != NULL)
        {
            node_queue[(++current_end) % capacity] = curr->mleft;
            size++;
        }
        if (curr->mright != NULL)
        {
            node_queue[(++current_end) % capacity] = curr->mright;
            size++;
        }
    }
}
