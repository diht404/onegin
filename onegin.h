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

enum ErrorCodes
{
    NO_ERRORS = 0,
    GET_MORE_THAN_1_COMMAND_LINE_ARGUMENT = 1,
    CANT_OPEN_FILE = 2,
    CANT_GET_FILE_INFO = 3,
    CANT_ALLOCATE_MEMORY_FOR_FILE = 4,
    CANT_ALLOCATE_MEMORY_FOR_STRINGS = 5,
    CANT_ALLOCATE_MEMORY_FOR_STRINGS_LENGTH = 6,
    CANT_ALLOCATE_MEMORY_FOR_POEM = 7,
};

void processError(int error);

/**
 * @brief opens file
 *
 * @param filename name of the file to open
 * @param mode mode to open the file
 * @param fp file
 * @return error code
 */
int openFile(const char *filename, const char *mode, FILE **fp);

/**
 * @brief return length of file in bytes
 *
 * @param fp - opened file
 * @param lenOfFile - length of file
 * @return error code
 */
int getLenOfFile(FILE *fp, size_t *lenOfFile);

/**
 * reads file to buffer
 * @param fp - opened file
 * @param lenOfFile - variable for storing length of file
 * @param txt - array storing file
 * @return error code
 */
int readFileToBuf(FILE *fp, size_t *lenOfFile, char **txt);

/**
 * @brief counts lines in buffer
 * @param txt - buffer with text
 * @param lenOfFile - len of buffer
 * @return number of lines in buffer
 */
size_t countLines(const char *txt, size_t lenOfFile);

/**
 * reads file to struct Text
 * @param fp - opened file
 * @param text - struct Text with file
 * @return error code
 */
int readFile(FILE *fp, Text *text);

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
void swapBlock(void *lhs,
               void *rhs,
               size_t *remainSize,
               size_t *copiedSize,
               size_t sizeOfBlock);

/**
 * @brief swaps two objects
 *
 * @param lhsVoid first object to swap
 * @param rhsVoid second object to swap
 * @param size size of object
 */
void swap(void *lhsVoid, void *rhsVoid, size_t size);

/**
 * @brief partition implementation for qsort with rightmost pivot
 *
 * @param array pointer to the array to sort
 * @param left the left border of the segment
 * @param right the right border of the segment
 * @param comp comparator for strings
 */
size_t partition(void *array, size_t left, size_t right, size_t size,
                 int (*comp)(const void *, const void *));

/**
 * @brief qsort implementation
 *
 * @param array pointer to the array to sort
 * @param left the left border of the segment
 * @param right the right border of the segment
 * @param comp comparator for strings
 */
void sort(void *array, size_t left, size_t right, size_t size,
          int (*comp)(const void *, const void *));

/**
 * @brief qsort with signature similar to qsort from stdlib.h
 *
 * @param array pointer to the array to sort
 * @param count number of elements in the array
 * @param size size of each element in the array in bytes
 * @param comp comparator for strings
 */
void qSort(void *array, size_t count, size_t size,
           int (*comp)(const void *, const void *));

/**
 * @brief generate poem with numParts blocks
 *
 * @param text back sorted text for generation
 * @param poem array of pointers to string of poem
 * @return void
 */
void generateBlock(Text *text, char ***poem);

/**
 * @brief generate poem with numParts blocks
 *
 * @param text back sorted text for generation
 * @param numParts number of blocks to generate
 * @param poem array of pointers to string of poem
 * @return error code
 */
int generatePoem(Text *text, size_t numParts, char ***poem);

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
 * @brief frees Text memory
 *
 * @param text Struct for storing array of strings, its' length and pointer
 * to memory block with not sorted text(array of string)
 */
void freeText(Text *text);

/**
 * @brief frees Poem memory
 *
 * @param poem struct Poem for storing nonsense poem
 */
void freePoem(Poem *poem);