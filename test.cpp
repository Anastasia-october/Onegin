#include <stdlib.h>
#include <stdio.h>

int main() {
    int a = 8;
    int* pa = &a;
    char* b = "Hello";
    char** pb = &b;
    printf("size a <%zu> size b <%zu>  za <%d> zb <%p>", sizeof(pa), sizeof(pb), pa, pb);
    print("1) <%zu>\n", sizeof(char));

}
