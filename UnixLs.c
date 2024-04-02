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

void printFileDetails(const char *fileName, int printInode) {
    struct stat fileStat;
    if (lstat(fileName, &fileStat) == -1) {
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

    // File name
    printf("%s\n", fileName);
}

int main(int argc, char *argv[]) {
    int printInode = 0;
    int printDetails = 0;
    char *dirName = NULL;

    for (int i = 0; argv[0][i]; i++) {
        if (argv[0][i] == 'i') {
            printInode = 1;
        } else if (argv[0][i] == 'l') {
            printDetails = 1;
        }
    }

    if (argc == 1) {
        dirName = ".";
    } else {
        dirName = argv[1];
    }

    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(dirName)) == NULL) {
        perror("Error");
        return 1;
    }

    while ((entry = readdir(dir)) != NULL) {
        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", dirName, entry->d_name);

        if (printDetails) {
            printFileDetails(path, printInode);
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

    closedir(dir);
    return 0;
}
