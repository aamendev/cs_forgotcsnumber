#include "core_tests.h"
#include "tree.h"

void int_print(void* data)
{
    int* d = (int*)data;
    printf("%d", *d);
}
void tree_test()
{
    int* vals = malloc(100 * sizeof(int));
    vals[0] = 10;
    vals[1] = 7;
    vals[2] = 13;
    vals[3] = 13;
    vals[4] = 0;
    vals[5] = 20;
    TreeNode node = {.mkey = "lina", .mvalue = (void*)(&vals[0])};
    TreeNode ela = {.mkey = "ela", .mvalue = (void*)(&vals[1])};
    TreeNode ela2 = {.mkey = "ela2", .mvalue = (void*)(&vals[2])};
    TreeNode zela = {.mkey = "zela", .mvalue = (void*)(&vals[3])};
    TreeNode ylina = {.mkey = "ylina", .mvalue = (void*)(&vals[4])};
    TreeNode zlina = {.mkey = "zlina", .mvalue = (void*)(&vals[5])};
    BinaryTree* root = malloc(sizeof(BinaryTree*));
    root->mheight = -1;
    bt_insert(root, &node);
    bt_insert(root, &ela);
    bt_insert(root, &ela2);
    bt_insert(root, &zela);
    bt_insert(root, &ylina);
    bt_insert(root, &zlina);
    printf("%d\n", root->mheight);
    bt_print_tree(root, int_print);
}

void all_tests()
{
    tree_test();
}
