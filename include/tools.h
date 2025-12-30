#ifndef TOOLS_H
#define TOOLS_H

#define ARRAY_L(a) (sizeof(a) / sizeof(a[0]))
#define CAT(a, b) a ## b

int binarySearch(int arr[], int left, int right, int key);
int filesInDir(char *path);
char **fillArr(char *path, int *outCount);

#endif