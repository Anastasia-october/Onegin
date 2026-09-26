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

void do_one_sort(int fd, char* buffer, char* sorted_buffer, char** pointers, int compar(const void* a, const void* b));
size_t file_size(char* name_of_file);
int read_file_to_buffer(char** buffer, int fd);
int count_nlines(char* buffer);
int make_pointer_array(char* buffer, char*** pointers);
int compar_1_look_note(const void* a, const void* b);
int compar_2_look_note(const void* a, const void* b);
char* change_end_for_newline_symbols(char** pointers, char* buffer);


//NOTE - compar_1_look_note compares strings in ascending order
//NOTE - starting from the first letter and ignoring all non-letter characters

//NOTE - compar_2_look_note compares strings in ascending order
//NOTE - starting from the last letter and ignoring all non-letter characters


int main() {
    int fd = open(name_file, O_RDWR);
    char* buffer = 0;
    char** pointers = 0;
    char* sorted_buffer = 0;

    read_file_to_buffer(&buffer, fd);
    printf("file already read\n");

    make_pointer_array(buffer, &pointers);
    printf("pointers already done\n");

    do_one_sort(fd, buffer, sorted_buffer, pointers, compar_1_look_note);
    printf("first sort already done\n");

    // char* bak = "\nAAAAAA\n\n";
    // write(fd, bak, strlen(bak));

    // do_one_sort(fd, buffer, sorted_buffer, pointers, compar_2_look_note);

    free(buffer);
    free(pointers);
    free(sorted_buffer);
}

void do_one_sort(int fd, char* buffer, char* sorted_buffer, char** pointers,  int compar(const void* a, const void* b)) {
    printf("in function do_one_sort\n\n");
    qsort(pointers, (size_t)count_nlines(buffer), sizeof(char *), compar);
    printf("pointers already sorted\n");

    sorted_buffer = change_end_for_newline_symbols(pointers, buffer);
    printf("sorted_buffer already done\n");

    int wrote = (int) write(fd, sorted_buffer, file_size((char*)name_file));

    printf("wrote <%d>\n", wrote);
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

int read_file_to_buffer(char** buffer, int fd) {
    assert(buffer);

    const size_t size_from_stat = file_size((char*)name_file);
    printf("size of file <%zu>\n", size_from_stat);

    *buffer = (char*)calloc(size_from_stat+2, sizeof(char));
    if (*buffer == NULL) {
        // TODO: use perror
        printf("ERROR/nToo few memory\n");
        return -1;
    }

    (*buffer)[0] = '\n';
    (*buffer)++;

    ssize_t read_res = read(fd, *buffer, size_from_stat);
    if ((size_t) read_res != size_from_stat) {
        // TODO: use perror
        printf("ERROR\nCan not read file\n");
        return -1;
    }

    (*buffer)--;
    return 0;
}

int count_nlines(char* buffer) {
    assert(buffer);

    int nlines = 0;
    size_t iterations = file_size((char*)name_file);
    for (size_t i = 0; i < iterations; i++) {
        if (buffer[i] == '\n' || buffer[i] == '\0') {
            nlines++;
            buffer[i] = '\0';
        }
    }

    return nlines;
}

int make_pointer_array(char* buffer, char*** pointers) {
    assert(buffer);
    assert(pointers);

    size_t nlines = (size_t)count_nlines(buffer);
    // printf("number of lines <%zu>\n", nlines);

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

int compar_2_look_note(const void* a, const void* b) {
    assert(a);
    assert(b);

    char* line_a = *((char **) a);//MENTOR - why * not **
    char* line_b = *((char **) b);

    size_t iterations = MIN(strlen(line_a), strlen(line_b));
    assert(iterations > 20 && iterations < 40); //REVIEW - delete than
    // FIXME: difference
    for (size_t index_a = iterations, index_b = iterations; index_a > 0 && index_b > 0; index_a--, index_b--) {
        while (isalpha(line_a[index_a]) == 0) { //MENTOR - NULL or 0
            index_a--;
        }
        while (isalpha(line_b[index_b]) == 0) {
            index_b--;
        }
        if (line_a[index_a] < line_b[index_b])
            return -1;
        else if (line_a[index_a] >= line_b[index_b])
            return 1;
    }
    return NULL;
}

char* change_end_for_newline_symbols(char** pointers, char* buffer) {
    assert(pointers);
    assert(buffer);

    size_t iterations = file_size((char*)name_file);
    char* buf = (char*)calloc(iterations+2, sizeof(char));
    printf("iterations <%d> buf pointer <%p> or <%d>\n");

    for (size_t i = 0; i < iterations; i++) {
        if (buffer[i] == '\0') {
            buffer[i] = '\n';
            printf("%c", buffer[i]);
        }
    }
    printf("\n");
// +- 1
    int nlines = count_nlines(buffer);
    size_t filled_buf = 0;
    for (int line_pointer = 0; line_pointer < nlines; line_pointer++) {
        memcpy(buf+filled_buf, pointers[line_pointer], strlen(pointers[line_pointer]));
        filled_buf = filled_buf + strlen(pointers[line_pointer]);
    }

    for (int i = 0; i < 40; i++) {
        fprintf(stderr, "%c", buf[i]);
    }
    printf("\n");

    return buf;
}
