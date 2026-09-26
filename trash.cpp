FILE * fp = fopen("testOnegin.txt", "r");
    int size = 0;
    int c;
    while ((c = fgetc(fp)) != EOF) {
        size++;
    }
    printf("size for each elem read - <%d>\n", size);

-------------------------------------------------------------------------------------------------
//ANCHOR - from main to read_file_to_buffer
    const size_t size_from_stat = buf_size(name_file);        //printf("size_from_stat of test = <%ld>\n", size_from_stat);

    char* buffer = (char*)malloc(size_from_stat);
    if (buffer == 0)
        printf("Too few memory\n");

    ssize_t read_res = read(fd, buffer, size_from_stat);
    assert(read_res != -1);
//ANCHOR -
-------------------------------------------------------------------------------------------------
int compar_1_look_note(const void* a, const void* b) {
    assert(a);
    assert(b);

    char* line_a = *((char **) a);//MENTOR - why * not **
    char* line_b = *((char **) b);

    size_t iterations = MIN(strlen(line_a), strlen(line_b));
    // fprintf(stderr, "len a = <%zu> \t len b = <%zu>\n", strlen(line_a), strlen(line_b));
    assert(iterations > 20 && iterations < 40); //REVIEW - delete than

    for (size_t index_a = 0, index_b = 0; index_a < iterations && index_b < iterations; index_a++, index_b++) {
        // FIXME: overflow \0
        while (isalpha(line_a[index_a]) == 0) { //MENTOR - NULL or 0
            index_a++;
        }
        while (isalpha(line_b[index_b]) == 0) {
            index_b++;
        }
        if (line_a[index_a] < line_b[index_b])
            return -1;
        else if (line_a[index_a] >= line_b[index_b])
            return 1;
    }
    return NULL;
}

---------------------------------------------------------------------------------------

    const size_t size_from_stat = file_size((char*)name_file); //ANCHOR - delete after debug
    fprintf(stderr, "------------------------------------------------------------\n");
    for (size_t i = 0; i<size_from_stat+2; i++)
        fprintf(stderr, "%c", buffer[i]);
    fprintf(stderr, "------------------------------------------------------------\n");
