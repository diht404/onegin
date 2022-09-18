#include "onegin.h"

long getLenOfFile(FILE *fp)
{
    struct stat buff = {};
    fstat(fileno(fp), &buff);
    long lenOfFile = buff.st_size;
    return lenOfFile;
}

char *readFileToBuf(FILE *fp, long *lenOfFile)
{
    assert(fp != nullptr);
    assert(lenOfFile != nullptr);

    *lenOfFile = getLenOfFile(fp);

    char *txt = (char *) calloc(*lenOfFile + 1, sizeof(char));
    if (txt == nullptr)
    {
        fprintf(stderr, "Can't allocate memory.");
    }
    fread(txt, sizeof(char), *lenOfFile, fp);
    return txt;
}

size_t countLines(const char *txt, long lenOfFile)
{
    size_t numLines = 1;
    for (long i = 0; i < lenOfFile; i++)
    {
        if (txt[i] == '\n')
            numLines++;
    }
    return numLines;
}

Text readFile(FILE *fp)
{
    assert(fp != nullptr);

    long lenOfFile = 0;
    char *txt = readFileToBuf(fp, &lenOfFile);

    size_t numLines = countLines(txt, lenOfFile);

    Line *lines = (Line *) calloc(numLines + 1, sizeof(lines[0]));
    size_t *lensOfStrings =
        (size_t *) calloc(numLines + 1, sizeof(lensOfStrings[0]));

    if (lines == nullptr)
    {
        fprintf(stderr, "Can't allocate memory.");
    }
    size_t position = 0;
    size_t line_id = 0;

    for (long i = 0; i < lenOfFile; i++)
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
    return {lines, numLines, txt, lensOfStrings};
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
        // как тут удалять
        if (lhs->str[l_pos] < rhs->str[r_pos])
        {
            return -1;
        }

        if (lhs->str[l_pos] > rhs->str[r_pos])
            return 1;

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

    long l_pos = lhs->length - 1;
    long r_pos = rhs->length - 1;

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

        if (lhs->str[l_pos] < rhs->str[r_pos])
        {
            return -1;
        }
        if (lhs->str[l_pos] > rhs->str[r_pos])
        {
            return 1;
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

void swapLines(Line *lhs, Line *rhs)
{
    assert(lhs != nullptr);
    assert(rhs != nullptr);

    swap(lhs, rhs, sizeof(Line));
}

size_t partition(Line *lines, const size_t l, const size_t r,
                 int (*comp)(const void *, const void *))
{
    assert(lines != nullptr);
    assert(comp != nullptr);

    Line *pivot = &lines[r];
    int greaterLine = l - 1;

    for (int j = l; j < r; j++)
    {
        if (comp(&lines[j], pivot) <= 0)
        {
            swapLines(&lines[++greaterLine], &lines[j]);
        }
    }

    swapLines(&lines[++greaterLine], pivot);

    return greaterLine;
}

void sort(Line *lines, const size_t l, const size_t r,
          int (*comp)(const void *, const void *))
{
    assert(lines != nullptr);
    assert(comp != nullptr);

    if (l < r)
    {
        size_t pivotInd = partition(lines, l, r, comp);
        sort(lines, l, pivotInd - 1, comp);
        sort(lines, pivotInd + 1, r, comp);
    }
}

void qSort(Line *lines, const size_t count, size_t size,
           int (*comp)(const void *, const void *))
{
    assert(lines != nullptr);
    assert(comp != nullptr);

    sort(lines, 0, count - 1, comp);
}

void printFile(Text *text, const char *filename, bool sorted)
{
    assert(text != nullptr);
    assert(filename != nullptr);

    FILE *fp = fopen(filename, "a");

    if (sorted)
    {
        // print of sorted array
        for (int i = 0; i < text->length; i++)
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

void generateBlock(Text *text, char **poem)
{
    assert(text != nullptr);

    size_t lineId = 0;
    // Shakespeare wrote 8 lines in block
    // Rhymes:
    // 0 and 2
    // 1 and 3 and 4
    // 5 and 6

    lineId = generateLineId(text);
    poem[0] = (text->lines[lineId].str);
    poem[2] = (text->lines[lineId + 1].str);

    lineId = generateLineId(text);
    poem[1] = (text->lines[lineId].str);
    poem[3] = (text->lines[lineId + 1].str);
    poem[4] = (text->lines[lineId + 2].str);

    lineId = generateLineId(text);
    poem[5] = (text->lines[lineId].str);
    poem[6] = (text->lines[lineId + 1].str);
}

char **generatePoem(Text *text, size_t numParts)
{
    assert(text != nullptr);

    // Shakespeare wrote 7 lines in block
    char **poem = (char **) calloc(ShakespeareNumLines * numParts,
                                   sizeof(char *));

    srand(time(0));
    for (int i = 0; i < numParts; i++)
    {
        generateBlock(text, poem);
        poem += ShakespeareNumLines;
    }
    poem -= ShakespeareNumLines * numParts;
    return poem;
}

void printPoem(Poem *poem)
{
    for (size_t i = 0; i < poem->numParts * poem->numLines; i++)
    {
        printf("%llu: %s\n", i % poem->numLines * 1, poem->poem[i]);
        if (i % poem->numLines == poem->numLines - 1)
            printf("\n");
    }
}

void freeAll(Text *text, Poem *poem)
{
    free(text->txt);
    free(text->lines);
    free(poem->poem);
    free(text->lensOfStrings);
}