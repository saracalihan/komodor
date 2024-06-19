#include<stdio.h>
#include<string.h>

int main(){
    char input[256]={0};
    scanf("%s", input);
    printf("input: %s\n", input);
    memset(input, 0, sizeof(char)*256);
    scanf("%s", input);
    printf("input: %s", input);
    return 0;
}
