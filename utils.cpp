#include "onegin.h"

void processError(int error)
{
    switch (error)
    {
        case NO_ERRORS:
            break;
        case GET_MORE_THAN_1_COMMAND_LINE_ARGUMENT:
            fprintf(stderr,
                    "Required 0 or 1 command line arguments, but get more than 1.\n");
            break;
        case CANT_OPEN_FILE:
            fprintf(stderr,
                    "Can't open the file.\n");
            break;
        case CANT_GET_FILE_INFO:
            fprintf(stderr,
                    "Can't get info about file.\n");
            break;
        case CANT_ALLOCATE_MEMORY_FOR_FILE:
            fprintf(stderr,
                    "Not enough free space to allocate memory for storing the file.\n");
            break;
        case CANT_ALLOCATE_MEMORY_FOR_STRINGS:
            fprintf(stderr,
                    "Not enough free space to allocate memory for storing info about lines of file.\n");
            break;
        case CANT_ALLOCATE_MEMORY_FOR_STRINGS_LENGTH:
            fprintf(stderr,
                    "Not enough free space to allocate memory for storing info about length of lines of file.\n");
            break;
        case CANT_ALLOCATE_MEMORY_FOR_POEM:
            fprintf(stderr,
                    "Not enough free space to allocate memory for storing the poem.\n");
            break;
        default:
            fprintf(stderr, "Unknown error code: %d.\n",
                    error);
            break;
    }
}

int openFile(const char *filename, const char *mode, FILE **fp)
{
    assert(filename != nullptr);
    assert(mode != nullptr);
    assert(fp != nullptr);

    *fp = fopen(filename, "r");
    if (*fp == nullptr)
    {
        return CANT_OPEN_FILE;
    }
    return NO_ERRORS;
}

int getLenOfFile(FILE *fp, size_t *lenOfFile)
{
    assert(fp != nullptr);
    assert(lenOfFile != nullptr);

    struct stat buff = {};
    if (fstat(fileno(fp), &buff) != 0)
        return CANT_GET_FILE_INFO;
    *lenOfFile = buff.st_size;
    return NO_ERRORS;
}

int readFileToBuf(FILE *fp, size_t *lenOfFile, char **txt)
{
    assert(fp != nullptr);
    assert(lenOfFile != nullptr);
    assert(txt != nullptr);

    int error = getLenOfFile(fp, lenOfFile);
    if (error)
        return error;

    *txt = (char *) calloc(*lenOfFile + 1, sizeof(char));
    if (*txt == nullptr)
    {
        return CANT_ALLOCATE_MEMORY_FOR_FILE;
    }
    fread(*txt, sizeof(char), *lenOfFile, fp);
    return error;
}

size_t countLines(const char *txt, size_t lenOfFile)
{
    assert(txt != nullptr);

    size_t numLines = 1;
    for (size_t i = 0; i < lenOfFile; i++)
    {
        if (txt[i] == '\n')
            numLines++;
    }
    return numLines;
}

int readFile(FILE *fp, Text *text)
{
    assert(fp != nullptr);
    assert(text != nullptr);

    size_t lenOfFile = 0;
    char *txt = nullptr;
    int error = readFileToBuf(fp, &lenOfFile, &txt);

    if (error)
        return error;

    size_t numLines = countLines(txt, lenOfFile);

    Line *lines = (Line *) calloc(numLines + 1, sizeof(lines[0]));
    if (lines == nullptr)
    {
        return CANT_ALLOCATE_MEMORY_FOR_STRINGS;
    }
    size_t *lensOfStrings =
        (size_t *) calloc(numLines + 1, sizeof(lensOfStrings[0]));

    if (lensOfStrings == nullptr)
    {
        return CANT_ALLOCATE_MEMORY_FOR_STRINGS_LENGTH;
    }

    size_t position = 0;
    size_t line_id = 0;

    for (size_t i = 0; i < lenOfFile; i++)
    {
        if (position == 0)
        {
            lines[line_id] = {&txt[i], 0};
        }

        lines[line_id].length++;
        position++;

        if (txt[i] == '\n')
        {
            lines[line_id].length = position;
            lensOfStrings[line_id] = position;
            position = 0;
            txt[i] = '\0';
            line_id++;
        }
    }

    *text = {lines, numLines, txt, lensOfStrings};
    return error;
}

int compareStr(const void *lhsVoid, const void *rhsVoid)
{
    assert(lhsVoid != nullptr);
    assert(rhsVoid != nullptr);

    const Line *lhs = (const Line *) lhsVoid;
    const Line *rhs = (const Line *) rhsVoid;

    long l_pos = 0;
    long r_pos = 0;

    while (lhs->str[l_pos] != '\0' and rhs->str[r_pos] != '\0')
    {
        if (!isalnum(lhs->str[l_pos]))
        {
            l_pos++;
            continue;
        }
        if (!isalnum(rhs->str[r_pos]))
        {
            r_pos++;
            continue;
        }

        int comparation = lhs->str[l_pos] - rhs->str[r_pos];
        if (comparation != 0)
        {
            return comparation;
        }

        l_pos++;
        r_pos++;
    }
    while (!isalnum(lhs->str[l_pos]))
    {
        l_pos++;
    }
    while (!isalnum(rhs->str[r_pos]))
    {
        r_pos++;
    }
    if (lhs->str[l_pos] == '\0' and rhs->str[r_pos] == '\0')
        return 0;

    if (lhs->str[l_pos] == '\0' and rhs->str[r_pos] != '\0')
    {
        return -1;
    }
    return 1;
}

int compareStrBack(const void *lhsVoid, const void *rhsVoid)
{
    assert(lhsVoid != nullptr);
    assert(rhsVoid != nullptr);

    const Line *lhs = (const Line *) lhsVoid;
    const Line *rhs = (const Line *) rhsVoid;

    long long l_pos = lhs->length - 1;
    long long r_pos = rhs->length - 1;

    while (l_pos >= 0 and r_pos >= 0)
    {
        if (!isalnum(lhs->str[l_pos]) or lhs->str[l_pos] == '\0')
        {
            l_pos--;
            continue;
        }
        if (!isalnum(rhs->str[r_pos]) or rhs->str[r_pos] == '\0')
        {
            r_pos--;
            continue;
        }
        int comparation = lhs->str[l_pos] - rhs->str[r_pos];
        if (comparation != 0)
        {
            return comparation;
        }

        l_pos--;
        r_pos--;
    }

    while (!isalnum(lhs->str[l_pos]))
    {
        l_pos--;
    }
    while (!isalnum(rhs->str[r_pos]))
    {
        r_pos--;
    }
    if (l_pos < 0 and r_pos < 0)
    {
        return 0;
    }

    if (l_pos < 0)
    {
        return -1;
    }
    return 1;
}

void swapBlock(void *lhsVoid,
               void *rhsVoid,
               size_t *remainSize,
               size_t *copiedSize,
               size_t sizeOfBlock)
{
    assert(lhsVoid != nullptr);
    assert(rhsVoid != nullptr);
    assert(remainSize != nullptr);
    assert(copiedSize != nullptr);

    char *lhs = (char *) lhsVoid;
    char *rhs = (char *) rhsVoid;

    uint64_t block = 0;
    size_t numBlocks = *remainSize / sizeOfBlock;

    for (size_t i = 0; i < numBlocks; i++)
    {
        memcpy(&block,
               lhs + *copiedSize + i * sizeOfBlock,
               sizeOfBlock);
        memcpy(lhs + *copiedSize + i * sizeOfBlock,
               rhs + *copiedSize + i * sizeOfBlock,
               sizeOfBlock);
        memcpy(rhs + *copiedSize + i * sizeOfBlock,
               &block,
               sizeOfBlock);
    }
    *copiedSize += numBlocks * sizeOfBlock;
    *remainSize %= sizeOfBlock;
}

void swap(void *lhs, void *rhs, size_t size)
{
    assert(lhs != nullptr);
    assert(rhs != nullptr);

    size_t copiedSize = 0;

    size_t remainSize = size;
    swapBlock(lhs, rhs, &remainSize, &copiedSize, sizeof(uint64_t));
    if (!remainSize)
        return;

    swapBlock(lhs, rhs, &remainSize, &copiedSize, sizeof(uint32_t));
    if (!remainSize)
        return;

    swapBlock(lhs, rhs, &remainSize, &copiedSize, sizeof(uint16_t));
    if (!remainSize)
        return;

    swapBlock(lhs, rhs, &remainSize, &copiedSize, sizeof(uint8_t));
    if (!remainSize)
        return;
}

size_t partition(void *array,
                 const size_t l,
                 const size_t r,
                 size_t size,
                 int (*comp)(const void *, const void *))
{
    assert(array != nullptr);
    assert(comp != nullptr);

    void *pivot = (char *) array + r * size;
    size_t greaterLine = l - 1;

    for (size_t j = l; j < r; j++)
    {
        if (comp((char *) array + j * size, pivot) <= 0)
        {
            ++greaterLine;
            swap((char *) array + greaterLine * size,
                 (char *) array + j * size,
                 size);
        }
    }
    ++greaterLine;
    swap((char *) +array + greaterLine * size, pivot, size);

    return greaterLine;
}

void sort(void *array, const size_t l, const size_t r, size_t size,
          int (*comp)(const void *, const void *))
{
    assert(array != nullptr);
    assert(comp != nullptr);

    if (l < r)
    {
        size_t pivotInd = partition(array, l, r, size, comp);
        sort(array, l, pivotInd - 1, size, comp);
        sort(array, pivotInd + 1, r, size, comp);
    }
}

void qSort(void *array, const size_t count, size_t size,
           int (*comp)(const void *, const void *))
{
    assert(array != nullptr);
    assert(comp != nullptr);

    sort(array, 0, count - 1, size, comp);
}

void printFile(Text *text, const char *filename, bool sorted)
{
    assert(text != nullptr);
    assert(filename != nullptr);

    FILE *fp = fopen(filename, "a");

    if (sorted)
    {
        // print sorted array
        for (size_t i = 0; i < text->length; i++)
        {
            fprintf(fp, "%s\n", text->lines[i].str);
        }
    }
    else
    {
        // print original text
        size_t startOfString = 0;
        size_t numOfLines = 0;
        while (numOfLines < text->length)
        {
            fprintf(fp, "%s\n", text->txt + startOfString);
            startOfString += text->lensOfStrings[numOfLines];
            numOfLines++;
        }
    }
    fclose(fp);
}

size_t generateLineId(Text *text)
{
    assert(text != nullptr);
    // long text required!
    assert(text->length > 10);

    size_t lineId = 0;
    size_t attempts = 0;
    do
    {
        lineId = rand() % (text->length - 2);
        attempts++;
    }
    while (strlen(&text->lines[lineId].str[0]) < 10 or attempts < 100);
    // searches strings more than 10 characters,
    // if can't do it 100 times - return anything

    return lineId;
}

void generateBlock(Text *text, char ***poem)
{
    assert(text != nullptr);
    assert(poem != nullptr);

    size_t lineId = 0;
    // Shakespeare wrote 7 lines in block
    // Rhymes:
    // 0 and 2
    // 1 and 3 and 4
    // 5 and 6

    lineId = generateLineId(text);
    (*poem)[0] = (text->lines[lineId].str);
    (*poem)[2] = (text->lines[lineId + 1].str);

    lineId = generateLineId(text);
    (*poem)[1] = (text->lines[lineId].str);
    (*poem)[3] = (text->lines[lineId + 1].str);
    (*poem)[4] = (text->lines[lineId + 2].str);

    lineId = generateLineId(text);
    (*poem)[5] = (text->lines[lineId].str);
    (*poem)[6] = (text->lines[lineId + 1].str);
}

int generatePoem(Text *text, size_t numParts, char ***poem)
{
    assert(text != nullptr);
    assert(poem != nullptr);

    int error = NO_ERRORS;
    // Shakespeare wrote 7 lines in block
    *poem = (char **) calloc(ShakespeareNumLines * numParts + 1,
                             sizeof(char *));
    if (*poem == nullptr)
        return CANT_ALLOCATE_MEMORY_FOR_POEM;
    srand((unsigned int) time(nullptr));
    for (size_t i = 0; i < numParts; i++)
    {
        generateBlock(text, poem);
        *poem += ShakespeareNumLines;
    }
    *poem -= ShakespeareNumLines * numParts;
    return error;
}

void printPoem(Poem *poem)
{
    assert(poem != nullptr);

    for (size_t i = 0; i < poem->numParts * poem->numLines; i++)
    {
        printf("%llu: %s\n", i % poem->numLines * 1, poem->poem[i]);
        if (i % poem->numLines == poem->numLines - 1)
            printf("\n");
    }
}

void freeAll(Text *text, Poem *poem)
{
    assert(text != nullptr);
    assert(poem != nullptr);

    free(text->txt);
    free(text->lines);
    free(poem->poem);
    free(text->lensOfStrings);
}