#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>

#define NODE_NAME_SIZE 1
#define SLEEP_TIME 5

FILE *fp;

typedef struct tree_node
{
    unsigned int nr_children;
    char name[NODE_NAME_SIZE];
    struct tree_node **children;
} tree_node;

tree_node *read_tree_file()
{
    char a[NODE_NAME_SIZE];
    int num_chld;
    int result = fscanf(fp, "%s %d", a, &num_chld);
    if (result == EOF)
    {
        return NULL;
    }
    else
    {

        tree_node *curr = NULL;
        curr = malloc(sizeof(tree_node));
        strcpy(curr->name, a);
        // printf(a);
        curr->nr_children = num_chld;
        curr->children = malloc(num_chld*sizeof(tree_node *));

        int i;
        for (i = 0; i < num_chld; i++)
        {
            curr->children[i] = read_tree_file();
        }

        return curr;
    }
    // printf("%s %d\n", a, num_chld);
}

void print_tree(tree_node *root, int level)
{

    int i;

    for (i = 0; i < level; i++)
    {
        printf("\t");
    }
    printf("%s\n", root->name);

    for (i = 0; i < root->nr_children; i++)
    {
        print_tree(root->children[i], level + 1);
    }
}

void generate_process_tree(tree_node *root)
{
    int num_forks = root->nr_children;
    pid_t wpid;
    int status = 0;
    int fd[2];
    if(pipe(fd) == -1){
        perror("Pipe error");
    }

    if (num_forks == 0)
    {
        sleep(SLEEP_TIME);

        close(fd[0]);
        // write(fd[1], root->name, NODE_NAME_SIZE);
        printf("%s, \n",root->name);

        exit(0);
    }
    else
    {
        int i;
        for (i = 0; i < num_forks; i++)
        {
            pid_t cpid = fork();
            if (!cpid)
            {
                generate_process_tree(root->children[i]);
                exit(0);
            }
        }

        while ((wpid = wait(&status)) > 0);
        printf("%s, \n",root->name);

    }

}

int main(int argc, char *argv[])
{

    fp = fopen(argv[1], "r");
    tree_node *root = read_tree_file();

    print_tree(root, 0);
    generate_process_tree(root);


    //   fileptr1 = fopen("file1.txt","r");
    //   setvbuf(fileptr1, (char *)NULL, _IONBF, 0);
}