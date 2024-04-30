/*
 * HomeWork 4: Traversing inodes
 * file name: contents.c
 * Date: March 25 2024
 * YU MA (Freya)
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <limits.h> // for PATH_MAX

// Function to print detailed information about a directory entry
void printEntry(char *fullPath, struct dirent *entry, struct stat fileStat, struct passwd *pwd, struct group *grp) {
    char relPath[PATH_MAX];
    snprintf(relPath, PATH_MAX, "%s/%s", fullPath, entry->d_name); // Constructing the relative path
    printf("  Relative Entry Name : %s%s\n", entry->d_name, (entry->d_type == DT_DIR) ? " (directory)" : "");
    printf("  Inode Number : %llu\n", (unsigned long long)fileStat.st_ino);
    printf("  Owner : %s%s\n", pwd->pw_name, (pwd->pw_uid == getuid()) ? " (self)" : "");
    printf("  Group : %s%s\n", grp->gr_name, (grp->gr_gid == getgid()) ? " (self)" : "");
    printf("  File Size : %lld bytes\n", (long long)fileStat.st_size);
    printf("  Date of Last Modification : %s", ctime(&fileStat.st_mtime));
    printf("  Access Rights : %#o\n", fileStat.st_mode & 0777); // Prints permissions


    printf("  Access Rights Readable for me: ");
    // Owner
    printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
    printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
    printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
    // Group
    printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
    printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
    printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
    // Others
    printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
    printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
    printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");

    printf("\n");

    // Traversing inodes using link()
    char linkPath[PATH_MAX];
    snprintf(linkPath, PATH_MAX, "/tmp/link_%llu", (unsigned long long)fileStat.st_ino);
    if (link(relPath, linkPath) == 0) {
        printf("  Created link: %s -> %s\n", linkPath, relPath);
        unlink(linkPath); // Remove the temporary link
    } else {
        perror("  Link creation failed");
    }

    printf("\n");
}

int main(int argc, char *argv[]) {
    char cwd[PATH_MAX];
    if (argc < 2) {
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            argv[1] = cwd; // Use the current working directory if no argument is given
            argc++;
        } else {
            perror("getcwd() error");
            return 1;
        }
    }

    for (int i = 1; i < argc; i++) {
        char *path = argv[i];
        DIR *dir = opendir(path);
        if (dir == NULL) {
            printf("%s : Cannot access directory\n\n", path);
            continue; // Proceed to the next argument
        }

        struct dirent **entries;
        int n = scandir(path, &entries, NULL, alphasort); // Sorting entries alphabetically
        if (n < 0) {
            printf("%s : Cannot read directory\n\n", path);
            closedir(dir);
            continue;
        }

        // Print the full path name of the directory
        printf("%s\n", path);
        for (int j = 0; j < n; j++) {
            if (entries[j]->d_name[0] == '.') continue; // Skip entries that start with a dot
            char filePath[PATH_MAX];
            snprintf(filePath, PATH_MAX, "%s/%s", path, entries[j]->d_name);
            struct stat fileStat;
            if (lstat(filePath, &fileStat) < 0) continue; // Skip if cannot get stats
            struct passwd *pwd = getpwuid(fileStat.st_uid);
            struct group *grp = getgrgid(fileStat.st_gid);
            printEntry(path, entries[j], fileStat, pwd, grp); // Print detailed info
            free(entries[j]);
        }
        free(entries);
        closedir(dir);
    }
    return 0;
}
