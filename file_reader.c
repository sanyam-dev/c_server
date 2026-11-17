#include <stdio.h>

int main(){
    FILE *fpt; 
    fpt = fopen("./res/index.html", "r");

    if(fpt == NULL){
        printf("Error opening file\n");
        return 1;
    }

    char body[2048];
    while(fgets(body, sizeof(body), fpt)){
        printf("%s", body);
    }

    fclose(fpt);
    return 0;

}