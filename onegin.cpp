#include "onegin.h"

int main(int argc, char *argv[])
{
    int error = NO_ERRORS;

    const char *filename = "onegin.txt";

    if (argc == 2)
    {
        filename = argv[1];
    }
    if (argc > 2)
    {
        error = GET_MORE_THAN_1_COMMAND_LINE_ARGUMENT;
        processError(error);
    }

    FILE *fp = nullptr;
    error = openFile(filename, "r", &fp);
    if(!error)
        processError(error);

    Text text = {};
    error = readFile(fp, &text);
    if(!error)
        processError(error);

    fclose(fp);

    qSort(text.lines, text.length, sizeof(Line), compareStr);
    printFile(&text, "sorted.txt");
    printFile(&text, "All_Data.txt");

    qSort(text.lines, text.length, sizeof(Line), compareStrBack);
    printFile(&text, "sorted_back.txt");
    printFile(&text, "All_Data.txt");

    printFile(&text, "not_sorted.txt", false);
    printFile(&text, "All_Data.txt");

    size_t numParts = 3;
    Poem poem = {nullptr,
                 numParts,
                 ShakespeareNumLines};
    error = generatePoem(&text, numParts, &poem.poem);
    if(!error)
        processError(error);

    printPoem(&poem);

    freeAll(&text, &poem);

    return 0;
}
