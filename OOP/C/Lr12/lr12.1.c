#include <stdio.h>
#include <string.h>

int main() {
    char food[] = "Yummy";
    char *ptr;
    ptr = food + strlen(food);
    while (--ptr >= food) {
        puts(ptr);
    }
    return 0;
}
