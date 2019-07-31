void lineSplitter (char line[], char word[20][10])
{
    int i, j, count;
    j = 0; count = 0;

    for (i = 0; i <= strlen(line); i++) 
    {
        if (line[i] == ' ' || line[i] == '\0')
        {
            word[count][j] = '\0';
            count = count + 1;
            j = 0;
        }

        else
        {
            word[count][j] = line[i];
            j = j + 1;
        }
    } 
}