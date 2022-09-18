#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <cstdio>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <sys\stat.h>
#include <cstdint>

/**
 * @brief Struct for storing a string and its' length
 */
struct Line
{
    char *str = nullptr;
    size_t length = 0;
    char r = 0;
};

/**
 * @brief Struct for storing array of strings, its' length and pointer
 * to memory block with not sorted text(array of string)
 */
struct Text
{
    Line *lines = nullptr;
    size_t length = 0;
    char *txt = nullptr;
    size_t *lensOfStrings = nullptr;
};

/**
 * @brief struct for storing nonsense poem
 *
 * @param poem array of pointer for poem
 * @param numParts number of parts of poem
 * @param numLines number of lines in block
 */
struct Poem
{
    char **poem = nullptr;
    size_t numParts = 0;
    size_t numLines = 0;
};

const int ShakespeareNumLines = 7;

/**
 * @brief return length of file in bytes
 *
 * @param fp - opened file
 * @return length of file
 */
long getLenOfFile(FILE *fp);

/**
 * reads file to buffer
 * @param fp - opened file
 * @param lenOfFile - variable for storing length of file
 * @return array storing file
 */
char *readFileToBuf(FILE *fp, long *lenOfFile);

/**
 * reads file to struct Text
 * @param fp - opened file
 * @return struct Text with file
 */
Text readFile(FILE *fp);

/**
 * @brief comparator for strings (operator <)
 *
 * @param lhsVoid first string to compare
 * @param rhsVoid second string to compare
 * @return -1 if the first string is smaller,
 * 0 if is equal and
 * 1 if greater than the second string
 */
int compareStr(const void *lhsVoid, const void *rhsVoid);

/**
 * @brief back side comparator for strings (operator <)
 *
 * @param lhsVoid first string to compare
 * @param rhsVoid second string to compare
 * @return -1 if the first string is smaller,
 * 0 if is equal and
 * 1 if greater than the second string
 */
int compareStrBack(const void *lhsVoid, const void *rhsVoid);

/**
 * @brief print array of string to file
 *
 * @param text array of strings and info
 * @param filename name of file to write
 * @param sorted true(default) if sorted print is required or false if not
 */
void printFile(Text *text, const char *filename, bool sorted = true);

/**
 * @brief swaps two blocks
 *
 * @param lhs first block to swap
 * @param rhs second block to swap
 * @param remainSize remaining size to swap
 * @param copiedSize size alreadyCopied
 * @param sizeOfBlock size of block to copy
 */
void swapBlock(void *lhs, void *rhs, size_t *remainSize, size_t *copiedSize, size_t sizeOfBlock);

/**
 * @brief swaps two objects
 *
 * @param lhsVoid first object to swap
 * @param rhsVoid second object to swap
 * @param size size of object
 */
void swap(void *lhsVoid, void *rhsVoid, size_t size);

/**
 * @brief swap two lines
 *
 * @param lhsVoid first string to swap
 * @param rhsVoid second string to swap
 */
void swapLines(Line *lhs, Line *rhs);

/**
 * @brief partition implementation for qsort with rightmost pivot
 *
 * @param lines pointer to the array to sort
 * @param l the left border of the segment
 * @param r the right border of the segment
 * @param comp comparator for strings
 */
size_t partition(Line *lines, size_t l, size_t r,
                 int (*comp)(const void *, const void *));

/**
 * @brief qsort implementation
 *
 * @param lines pointer to the array to sort
 * @param l the left border of the segment
 * @param r the right border of the segment
 * @param comp comparator for strings
 */
void sort(Line *lines, size_t l, size_t r,
          int (*comp)(const void *, const void *));

/**
 * @brief qsort with signature similar to qsort from stdlib.h
 *
 * @param lines pointer to the array to sort
 * @param count number of elements in the array
 * @param size size of each element in the array in bytes
 * @param comp comparator for strings
 */
void qSort(Line *lines, size_t count, size_t size,
           int (*comp)(const void *, const void *));

/**
 * @brief generate poem with numParts blocks
 *
 * @param text back sorted text for generation
 * @param poem array of pointers to string of poem
 * @return void
 */
void generateBlock(Text *text, char **poem);

/**
 * @brief generate poem with numParts blocks
 *
 * @param text back sorted text for generation
 * @param numParts number of blocks to generate
 * @return array of pointers to string of poem
 */
char **generatePoem(Text *text, size_t numParts);

/**
 * @brief selects random line index from text
 *
 * @param text back sorted text for generation
 * @return random line index from text
 */
size_t generateLineId(Text *text);

/**
 * @brief prints nonsense poem
 *
 * @param poem struct Poem for storing nonsense poem
 */
void printPoem(Poem *poem);

/**
 * @brief frees all memory
 *
 * @param text Struct for storing array of strings, its' length and pointer
 * to memory block with not sorted text(array of string)
 * @param poem struct Poem for storing nonsense poem
 */
void freeAll(Text *text, Poem *poem);