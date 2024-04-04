#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
// #include "infodemo.c"

void getAndPrintGroup(gid_t grpNum) {
    struct group *grp = getgrgid(grpNum);
    
    if (grp) {
        printf("%s ", grp->gr_name);
    } else {
        printf("%u ", grpNum);
    }
}

void getAndPrintUserName(uid_t uid) {
    struct passwd *pw = getpwuid(uid);

    if (pw) {
        printf("%s ", pw->pw_name);
    } else {
        printf("%u ", uid);
    }
}

void printFileDetails(const char *pathName, int printInode) {
    struct stat pathStats;
    if (lstat(pathName, &pathStats) == -1) {
        perror("Error");
        return;
    }

    // Print inode number if requested
    if (printInode) {
        printf("%lu ", (unsigned long)pathStats.st_ino);
    }

    // File permissions
    printf((S_ISDIR(pathStats.st_mode)) ? "d" : "-");
    printf((pathStats.st_mode & S_IRUSR) ? "r" : "-");
    printf((pathStats.st_mode & S_IWUSR) ? "w" : "-");
    printf((pathStats.st_mode & S_IXUSR) ? "x" : "-");
    printf((pathStats.st_mode & S_IRGRP) ? "r" : "-");
    printf((pathStats.st_mode & S_IWGRP) ? "w" : "-");
    printf((pathStats.st_mode & S_IXGRP) ? "x" : "-");
    printf((pathStats.st_mode & S_IROTH) ? "r" : "-");
    printf((pathStats.st_mode & S_IWOTH) ? "w" : "-");
    printf((pathStats.st_mode & S_IXOTH) ? "x " : "- ");

    // Number of hard links
    printf("%ld ", (long)pathStats.st_nlink);

    // Owner's username and group name
    getAndPrintUserName(pathStats.st_uid);
    getAndPrintGroup(pathStats.st_gid);

    // File size
    printf("%ld ", (long)pathStats.st_size);

    // Last modified time
    struct tm *tm_info = localtime(&pathStats.st_mtime);
    char recentTime[20];
    strftime(recentTime, sizeof(recentTime), "%b %d %Y %H:%M" , tm_info);
    printf("%s ", recentTime);

    // File name
    printf("%s\n", pathName);
}

int main(int argc, char *argv[]) {
    int printInode = 0;
    int printDetails = 0;
    char *dirName = NULL;

    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0) {
            printInode = 1;
        } else if (strcmp(argv[i], "-l") == 0) {
            printDetails = 1;
        } else if (strcmp(argv[i], "-il") == 0 || strcmp(argv[i], "-li") == 0) {
            printInode = 1;
            printDetails = 1;
        } else if (strcmp(argv[i], "-l") == 0 && strcmp(argv[i+2], "-i") == 0) {
            printInode = 1;
            printDetails = 1;
        } else if (strcmp(argv[i], "-i") == 0 && strcmp(argv[i+2], "-l") == 0) {
            printInode = 1;
            printDetails = 1;
        }
    }

    // Set directory name
    if (argc == 1 || (argc == 2 && (printInode || printDetails))) {
        dirName = ".";
    } else {
        dirName = argv[argc - 1];
    }

    // Open the directory
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(dirName)) == NULL) {
        perror("Error");
        return 1;
    }

    // Iterate over directory entries
    while ((entry = readdir(dir)) != NULL) {
        // Skip current directory (.) and parent directory (..)
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", dirName, entry->d_name);

        if (printDetails) {
            printFileDetails(path, printInode);
        } else {
            if (printInode) {
                struct stat pathStats;
                if (stat(path, &pathStats) == -1) {
                    perror("Error");
                    return 1;
                }
                printf("%lu %s\n", (unsigned long)pathStats.st_ino, entry->d_name);
            } else {
                printf("%s\n", entry->d_name);
            }
        }
    }

    closedir(dir);
    return 0;
}
