#include "onegin.h"

int main(int argc, char *argv[])
{

     //error codes

    const char *filename = "onegin.txt";

    if (argc == 2)
    {
        filename = argv[1];
    }
    if (argc > 2)
    {
        fprintf(stderr, "Required 0 or 1 command line arguments, but get %d\n", argc-1);
    }
    FILE *fp = fopen(filename, "r");
    if (fp == nullptr)
    {
        perror("Unable to open file!");
        return 1;
    }
    Text text = readFile(fp);
    fclose(fp);

    qSort(text.lines, text.length, sizeof(Line), compareStr);
    printFile(&text, "sorted.txt");
    printFile(&text, "All_Data.txt");

    qSort(text.lines, text.length, sizeof(Line), compareStrBack);
    printFile(&text, "sorted_back.txt");
    printFile(&text, "All_Data.txt");

    size_t numParts = 3;
    Poem poem = {generatePoem(&text, numParts),
                 numParts,
                 ShakespeareNumLines};

    printPoem(&poem);

    printFile(&text, "not_sorted.txt", false);
    printFile(&text, "All_Data.txt");

    freeAll(&text, &poem);

    return 0;
}
