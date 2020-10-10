
/*
 * Write a simple version of the UNIX find program: find all the files in a directory tree whose name matches a string. Your solution should be in the file user/find.c.
 *
 *
Some hints:
-   Look at user/ls.c to see how to read directories.
-   Use recursion to allow find to descend into sub-directories.
-   Don\'t recurse into \".\" and \"..\".
-   Changes to the file system persist across runs of qemu; to get a
    clean file system run [make clean]{.kbd} and then [make qemu]{.kbd}.
-   You\'ll need to use C strings. Have a look at K&R (the C book), for
    example Section 5.5.
Optional: support regular expressions in name matching. grep.c has some primitive support for regular expressions.

 * find in read world:
     -type t
             True if the file is of the specified type.  Possible file types are as
             follows:

             b       block special
             c       character special
             d       directory
             f       regular file
             l       symbolic link
             p       FIFO
             s       socket

       -name pattern

	 True if the last component of the pathname being examined matches
	 pattern.  Special shell pattern matching characters (``['', ``]'',
	 ``*'', and ``?'') may be used as part of pattern.  These characters
	 may be matched explicitly by escaping them with a backslash (``\'').
 *
 * find enhanced:
 *	fd
 * */

#include "kernel/types.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

int matchhere(const char*, const char*);
int matchstar(char, const char*, const char*);
int match(const char*,const char*);

const char* fmtname(const char* path)
{
    static char buf[DIRSIZ+1];
    const char* p;
    for (p=path+strlen(path); p>=path && *p!='/'; p--) {
    }{}
    p++;
    if (strlen(p)>=DIRSIZ) {
        return p;
    }
    memmove(buf, p, strlen(p));
    // reset left
    memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
    return buf;
}

void find(const char* pattern, const char* path)
{
    int fd;
    /* // Directory is a file containing a sequence of dirent structures. */
    /* #define DIRSIZ 14 */
    /* struct dirent { */
    /*   ushort inum; */
    /*   char name[DIRSIZ]; */
    /* }; */
    struct dirent de; // disk
    struct stat st;   // fd info

    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: open %s failed", path);
        exit();
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: fstat %s failed", path);
        exit();
    }

    char buf[512], *p;
    /* struct stat { */
    /*   int dev;     // File system's disk device */
    /*   uint ino;    // Inode number */
    /*   short type;  // Type of file */
    /*   short nlink; // Number of links to file */
    /*   uint64 size; // Size of file in bytes */
    /* }; */
    const char* name=fmtname(path);
    if (match(pattern, name)) {
        fprintf(1,"%s\n", path);
    }
    if (st.type == T_DIR) {
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
            fprintf(2,"find: path too long\n");
            exit();
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            if (de.inum == 0 || de.name[0]=='.') {
                // skip special i number
                // skip . and ..
                continue;
            }
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            find(pattern, buf);
        }
    }
    close(fd); // release fd
}

int matchstar(char c,const char* re,const char* text)
{
    while (*text != '\0' && (*text == c || c == '.')) {
        if (matchhere(re, text)) {
            return 1;
        }
        text++;
    }
    return 0;
}

// match single character
// finite state machine
int matchhere(const char* re,const char* text)
{
    if (re[0] == '\0') {
        return 1;
    }
    if (re[1] == '*') {
        return matchstar(re[0], re + 2, text);
    }
    if (re[0] == '$' && re[1] == '\0') {
        return *text == '\0';
    }
    if (*text != '\0' && (re[0] == '.' || re[0] == *text)) {
        return matchhere(re + 1, text + 1);
    }
    return 0;
}
// test if text match regex
// 1:true 0: false
int match(const char* re,const char* text)
{
    if (re[0] == '^') {
        return match(re + 1, text);
    }
    while (*text != '\0') {
        if (matchhere(re, text)) {
            return 1;
        }
        text++;
    }
    return 0;
}

int main(int argc, char* argv[])
{
    if (argc <= 1) {
        fprintf(2, "usage: find [path] pattern\n");
        exit();
    }
    if (argc <= 2) {
        find(argv[1], ".");
        exit();
    }
    find(argv[2], argv[1]);
    exit();
}
