#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/param.h>
#include <string.h>

char* name_file = "testOnegin.txt";

size_t read_file_to_buffer(char** buffer, int fd);
int make_pointer_array(char* buffer, char*** pointers, size_t size_of_buffer);
void do_one_sort(int fd, char* buffer, size_t size_of_buffer, char** sorted_buffer, char** pointers, int compar(const void* a, const void* b));
size_t file_size(char* name_of_file);
size_t my_strlen(char* str);
int count_nlines(char* buffer, size_t size_of_buffer);
int compar_1_look_note(const void* a, const void* b);
int compar_2_look_note(const void* a, const void* b);
void change_end_for_newline_symbols(char** pointers, char* buffer, size_t size_of_buffer, char** sorted_buffer);


//NOTE - compar_1_look_note compares strings in ascending order
//NOTE - starting from the first letter and ignoring all non-letter characters

//NOTE - compar_2_look_note compares strings in ascending order
//NOTE - starting from the last letter and ignoring all non-letter characters


int main() {
    int fd = open(name_file, O_RDWR);
    char* buffer = 0;
    char** pointers = 0;
    char* sorted_buffer = 0;

    fprintf(stderr, "start\n\n");
    size_t size_of_buffer = read_file_to_buffer(&buffer, fd);
    printf("file already read\n\n");

    fprintf(stderr, "in make_pointer_array\n");
    make_pointer_array(buffer, &pointers, size_of_buffer);
    printf("pointers already done\n\n");

    fprintf(stderr, "FIRST SORT\tFIRST SORT\tFIRST SORT\tFIRST SORT\tFIRST SORT\tFIRST SORT\tFIRST SORT\tFIRST SORT\n");
    do_one_sort(fd, buffer, size_of_buffer, &sorted_buffer, pointers, compar_1_look_note);
    printf("first sort already done\n");

    free(sorted_buffer);

    fprintf(stderr, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    fprintf(stderr, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

    // char* bak = "\nAAAAAA\n\n";
    // write(fd, bak, strlen(bak));
    fprintf(stderr, "SECOND SORT\tSECOND SORT\tSECOND SORT\tSECOND SORT\tSECOND SORT\tSECOND SORT\tSECOND SORT\tSECOND SORT\n");
    do_one_sort(fd, buffer, size_of_buffer, &sorted_buffer, pointers, compar_2_look_note);
    fprintf(stderr, "second sort already done\n");

    free(buffer);
    free(pointers);
    free(sorted_buffer);
}

size_t read_file_to_buffer(char** buffer, int fd) {
    assert(buffer);

    const size_t size_from_stat = file_size((char*)name_file);

    fprintf(stderr, "size of file <%zu>\n", size_from_stat);

    *buffer = (char*)calloc(size_from_stat+2, sizeof(char));//NOTE - 1 байт на первый символ \n, 1 байт на \0

    if (*buffer == NULL) {
        // TODO: use perror
        printf("ERROR/nToo few memory\n");
        return -1;
    }

    (*buffer)[0] = '\n';
    (*buffer)++;

    ssize_t read_res = read(fd, *buffer, size_from_stat);
    fprintf(stderr, "have read <%zu> bytes\n", (size_t)read_res);

    if ((size_t) read_res != size_from_stat) {
        // TODO: use perror
        printf("ERROR\nCan not read file\n");
        return -1;
    }

    (*buffer)--;

    fprintf(stderr, "in read_file_to_buffer buffer has /n \n");
    return strlen(*buffer);
}

int make_pointer_array(char* buffer, char*** pointers, size_t size_of_buffer) {
    assert(buffer);
    assert(pointers);

    fprintf(stderr, "\tcall count_lines\n");
    size_t nlines = (size_t)count_nlines(buffer, size_of_buffer);  //NOTE - count_nlines(buffer) called first
    fprintf(stderr, "\tnumber of lines <%zu>\n", nlines);

    *pointers = (char**) calloc(nlines, sizeof(char*));
    // printf("size pointers 0 <%zu>\n", sizeof(pointers));

    for (size_t i = 0, k = 0; i < nlines; i++) {

        // TODO: strnul strchr(s, '\0')
        while (buffer[k] != '\0')
            k++;
        // printf("i = <%zu>\n", i);
        (*pointers)[i] = &(buffer[++k]);//NOTE \0 is in the end of string
        // printf("\t pointer = %s\n", &(buffer[k]));
    }

    // printf("not error before return\n");
    return 0;
}

void do_one_sort(int fd, char* buffer, size_t size_of_buffer, char** sorted_buffer, char** pointers,  int compar(const void* a, const void* b)) {
    fprintf(stderr, "in function do_one_sort\n");
    fprintf(stderr, "\tcall qsort call comparator\n");
    qsort(pointers, (size_t)count_nlines(buffer, size_of_buffer), sizeof(char *), compar); //NOTE - count_nlines(buffer) called second time
    fprintf(stderr, "\tpointers already sorted\n");

    fprintf(stderr, "\tcall change_end_for_newline_symbols\n");
    change_end_for_newline_symbols(pointers, buffer, size_of_buffer, sorted_buffer);
    fprintf(stderr, "sorted_buffer already done\n\tPiece of buffer\n\t");

    for (int i = 0; i < 50; i++) {
        fprintf(stderr, "|%c|", buffer[i]);
    }
    fprintf(stderr, "\tstrlen of buffer which is used by wrote <%zu>\n", size_of_buffer);
    int wrote = (int) write(fd, *sorted_buffer, size_of_buffer);

    printf("\twrote <%d>\n", wrote);
}

size_t file_size(char* name_of_file) {
    assert(name_of_file);

    struct stat buf;
    int stat_res = stat(name_of_file, &buf);
    size_t size_from_stat = (size_t)buf.st_size;

    assert(stat_res == 0);
    assert(size_from_stat > 0);

    //fprintf(stderr, "size of test = <%ld>\t stat_res = <%d>\n", buf.st_size, stat_res);
    return size_from_stat;
}

size_t my_strlen(char* str) {
    assert(str != NULL);

    size_t size = 0;
    for (int i = 0; str[i] != '\n'; i++) {
        size++;
    }

    return size;
}

int count_nlines(char* buffer, size_t size_of_buffer) {
    assert(buffer);

    int nlines = 0;
    size_t iterations = size_of_buffer;
    fprintf(stderr, "\t\tin count_nlines iterations = <%zu>\n", iterations);
    for (size_t i = 0; i < iterations; i++) {
        if (buffer[i] == '\n' || buffer[i] == '\0') {
            nlines++;
            buffer[i] = '\0';
        }
    }

    fprintf(stderr, "\t\tall bak n changed to bak 0\n");

    return nlines-1;
}

int compar_1_look_note(const void* a, const void* b) {
    assert(a);
    assert(b);

    char* line_a = *((char **) a);
    char* line_b = *((char **) b);

    size_t iterations = MIN(strlen(line_a), strlen(line_b));

    fprintf(stderr, "\t\tin comparator 1\n");
    fprintf(stderr, "\t\tlen a = <%zu> \t len b = <%zu>\n", strlen(line_a), strlen(line_b));

    assert(iterations > 20 && iterations < 40); //REVIEW - delete than

    for (size_t index_a = 0, index_b = 0; index_a < iterations && index_b < iterations; index_a++, index_b++) {
        while (isalpha(line_a[index_a]) == 0) {
            index_a++;
        }
        while (isalpha(line_b[index_b]) == 0) {
            index_b++;
        }
        if (line_a[index_a] < line_b[index_b])
            return -1;
        else if (line_a[index_a] > line_b[index_b])
            return 1;
    }
    return NULL;
}

int compar_2_look_note(const void* a, const void* b) {
    assert(a);
    assert(b);

    char* line_a = *((char **) a);
    char* line_b = *((char **) b);

    size_t iterations = MIN(strlen(line_a), strlen(line_b));

    fprintf(stderr, "\t\tin comparator 1\n");
    fprintf(stderr, "\t\tlen a = <%zu> \t len b = <%zu>\n", strlen(line_a), strlen(line_b));

    assert(iterations > 20 && iterations < 40); //REVIEW - delete than

    for (size_t index_a = iterations, index_b = iterations; index_a > 0 && index_b > 0; index_a--, index_b--) {
        while (isalpha(line_a[index_a]) == 0) {
            index_a--;
        }
        while (isalpha(line_b[index_b]) == 0) {
            index_b--;
        }
        if (line_a[index_a] - line_b[index_b] != 0)
            return line_a[index_a] - line_b[index_b];
    }
    return NULL;
}//REVIEW - if first string is in second string

void change_end_for_newline_symbols(char** pointers, char* buffer, size_t size_of_buffer, char** sorted_buffer) {
    assert(pointers);
    assert(buffer);

    fprintf(stderr, "\t\tin change_end_for_newline_symbols\n");

    int nlines = count_nlines(buffer, size_of_buffer);  //NOTE - count_nlines(buffer) called
    size_t iterations = size_of_buffer;

    *sorted_buffer = (char*)calloc(iterations+2, sizeof(char));

    fprintf(stderr, "\t\titerations <%zu> buf pointer <%p> or <%d>\n", iterations, *sorted_buffer, *sorted_buffer);

    fprintf(stderr, "\t\tbefore changing 0 to n-------------------------------------------------------------------------\n\t\t");
    for (int i1 = 0; i1 < 50; i1++) {
        fprintf(stderr, "|%c|", buffer[i1]);
    }
    fprintf(stderr, "\n\t\t");

    for (size_t i = 0; i < iterations; i++) {
        if (buffer[i] == '\0') {
            buffer[i] = '\n';
        }
    }
    fprintf(stderr, "\t\tafter changing 0 to n-------------------------------------------------------------------------\n\t\t");
    for (int i2 = 0; i2 < 50; i2++) {
        fprintf(stderr, "|%c|", buffer[i2]);
    }
    fprintf(stderr, "\n");


    size_t filled_buf = 0;
    for (int line_pointer = 0; line_pointer < nlines; line_pointer++) {
        memcpy(*sorted_buffer+filled_buf, pointers[line_pointer], my_strlen(pointers[line_pointer]));
        filled_buf = filled_buf + my_strlen(pointers[line_pointer]);
    }
}
