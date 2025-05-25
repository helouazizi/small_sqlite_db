#include <stdio.h>

unsigned long hash(const char *str){
    unsigned long hash = 5381;
    int c;
 
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

int main() {
    const char *text = "hello";
    printf("Hash of '%s' is: %lu\n", text, hash(text));
    return 0;
}
