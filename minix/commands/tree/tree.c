#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

static void printIdent(int depth) 
{
    for (int i = 0; i < depth; i++) 
    {
        printf("  ");
    }
}

static void tree(const char *path, int depth) 
{
    DIR *dir = opendir(path);
    if (dir == NULL) 
    {
        printIdent(depth);
        return;
    }

    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL) 
    {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) 
        {
            continue;
        }

        struct stat st;

        printIdent(depth + 1);

        if (S_ISLNK(st.st_mode)) //is symbolyc link
        {
            char target[1024]; // symbolic-link's target

            ssize_t n = readlink(path, target, sizeof(target) - 1); // read target of link's path

            if (n >= 0)
            {
                target[n] = '\0'; // close the string
                printf("%s -> %s\n", ent->d_name, target);
            }
            else // readlink return error
            {
                printf("%s -> \n", ent->d_name);
            }

            continue;
        }

        char newPath[1024];
        snprintf(newPath, 1024, "%s/%s" , path, ent->d_name); //set next path to check
        lstat(newPath, &st);

        if (S_ISDIR(st.st_mode)) 
        {
            printf("%s/\n", ent->d_name);
            tree(newPath, depth + 1);
        } 
        else 
        {
            printf("%s\n", ent->d_name);
        }
    }

    closedir(dir);
}

int main(int argc, char **argv) 
{
    const char *root = (argc >= 2) ? argv[1] : ".";

    printf("%s/\n", root);

    tree(root, 0);
    return 0;
}