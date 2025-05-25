#include <stdio.h>
#include <stdlib.h>

typedef struct Node
{
    int data;
    struct Node *left;
    struct Node *right;
} Node;

void inorder(Node *root)
{
    if (root != NULL)
    {
        inorder(root->left);
        printf("%d ", root->data);
        inorder(root->right);
    }
}

Node *create_node(int data)
{
    Node *node = malloc(sizeof(Node));
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    return node;
}

Node *insert(Node *root, int data)
{
    if (root == NULL)
    {
        return create_node(data);
    }
    if (data < root->data)
        root->left = insert(root->left, data);
    else if (data > root->data)
        root->right = insert(root->right, data);
    return root;
}

void free_tree(Node *root)
{
    if (root == NULL)
        return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}
Node *search(Node *root, int data)
{
    if (data < root->data)
    {
        if (data == root->data || root->left == NULL)
        {
            return root;
        }
        search(root->left, data);
    }
    if (data > root->data)
    {
        if (data == root->data || root->right == NULL)
        {
            return root;
        }
        search(root->right, data);
    }
    return root;
}

Node *findMin(Node *node)
{
    while (node && node->left != NULL)
        node = node->left;
    return node;
}
Node *findMax(Node *node)
{
    while (node && node->right != NULL)
        node = node->right;
    return node;
}

Node* delete(Node *root, int data)
{
    if (root == NULL)
        return NULL;
    if (data < root->data)
    {
        root->left = delete(root->left, data);
    }
    else if (data > root->data)
    {
        root->right = delete(root->right, data);
    }
    else
    {
        // found a node data == root.dta
        if (root->left == NULL && root->right == NULL)
        {
            free(root);
            return NULL;
        }
        else if (root->left == NULL)
        {

            Node *tem = root->right;
            free(root);
            return tem;
        }
        else if (root->right == NULL)
        {
            Node *tem = root->left;
            free(root);
            return tem;
        }else
        {
            /* two tcheldern */
            Node* successor = findMin(root->right);
            root->data = successor->data;
            root->right = delete(root->right, successor->data);
        }
        
        
    }
    return root;
}

int main()
{
    Node *root = NULL;
    root = insert(root, 10);
    insert(root, 5);
    insert(root, 15);
    insert(root, 3);
    insert(root, 7);
    insert(root, 12);
    insert(root, 18);

    printf("Inorder traversal: ");
    inorder(root); // should print sorted order
    printf("\n");

    Node *serched = search(root, 58);
    if (serched->data == 58)
    {
        printf("%s\n", "found");
    }
    else
    {
        printf("%s\n", "not found");
    }

    Node *min = findMin(root->left);

    printf(" the min value is %d\n", min->data);
    Node *max = findMax(root->right);

    printf(" the max value is %d\n", max->data);

    Node *node = delete(root,7);
    printf("deleted %d\n", node->data);


    printf("Inorder traversal after deletation: ");
    inorder(root); // should print sorted order
    printf("\n");

    free_tree(root);
    return 0;
}
