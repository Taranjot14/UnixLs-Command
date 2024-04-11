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

void printFileDetails(const char *fileName, const char *filePath, int printInode) {
    struct stat fileStat;
    if (lstat(filePath, &fileStat) == -1) {
        perror("Error");
        return;
    }

    // Print inode number if requested
    if (printInode) {
        printf("%lu ", (unsigned long)fileStat.st_ino);
    }

    // File permissions
    printf((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
    printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
    printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
    printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
    printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
    printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
    printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
    printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
    printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
    printf((fileStat.st_mode & S_IXOTH) ? "x " : "- ");

    // Number of hard links
    printf("%ld ", (long)fileStat.st_nlink);

    // Owner's username and group name
    getAndPrintUserName(fileStat.st_uid);
    getAndPrintGroup(fileStat.st_gid);

    // File size
    printf("%ld ", (long)fileStat.st_size);

    // Last modified time
    struct tm *tm_info = localtime(&fileStat.st_mtime);
    char modTime[20];
    strftime(modTime, sizeof(modTime), "%b %d %Y %H:%M", tm_info);
    printf("%s ", modTime);

    // File path
    printf("%s\n", fileName);
}


int main(int argc, char *argv[]) {
    // Get the home directory path
    const char *homeDir = getenv("HOME");
    if (homeDir == NULL) {
        struct passwd *pw = getpwuid(getuid());
        if (pw != NULL) {
            homeDir = pw->pw_dir;
        } else {
            perror("Error");
            return 1;
        }
    }
    //Need to figure out ??
    // // Replace ~ symbol with home directory path
    // if (argc > 1 && (argv[2][0] == '~')) {
    //     printf("home\n");
    //     argv[2] = (char *)homeDir;
    // }

    int printInode = 0;
    int printDetails = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0) {
            printInode = 1;
        } else if (strcmp(argv[i], "-l") == 0) {
            printDetails = 1;
        } else if (strcmp(argv[i], "-il") == 0 || strcmp(argv[i], "-li") == 0) {
            printInode = 1;
            printDetails = 1;
        } else if (strcmp(argv[i], "-l") == 0 && strcmp(argv[i+1], "-i") == 0) {
            printInode = 1;
            printDetails = 1;
        } else if (strcmp(argv[i], "-i") == 0 && strcmp(argv[i+1], "-l") == 0) {
            printInode = 1;
            printDetails = 1;
        }
    }

    // Set directory name
    char *dirName = ".";
    if (argc > 1 && argv[1][0] != '-') {
        dirName = argv[1];
    }

    // Open directory
    DIR *dir = opendir(dirName);
    if (dir == NULL) {
        perror("Error");
        return 1;
    }

    // Read directory entries
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skip . and ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Skip ".git" directory
        if (strcmp(entry->d_name, ".git") == 0) {
            continue;
        }

        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", dirName, entry->d_name);

        if (printDetails) {
            printFileDetails(entry->d_name, path, printInode);
        } else {
            if (printInode) {
                struct stat fileStat;
                if (stat(path, &fileStat) == -1) {
                    perror("Error");
                    return 1;
                }
                printf("%lu %s\n", (unsigned long)fileStat.st_ino, entry->d_name);
            } else {
                printf("%s\n", entry->d_name);
            }
        }
    }

    // Close directory
    closedir(dir);

    return 0;
}


