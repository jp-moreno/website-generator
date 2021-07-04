#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 250

void usage(){
    printf("webgen requires a style sheet, an input folder, and an output folder\nexample: webgen -s style.css -i in -o out\n");
}

typedef struct Files Files;

struct Files{
    FILE *file;
    struct Files *next;
};

void addFile(Files *files, FILE *f){
    if(files->file==NULL){
        files->file =f;
        return;
    }
    Files *files2 = (Files*) malloc(sizeof(Files));
    files2->file = f;
    Files *t = files;
    while(t->next != NULL)
        t=t->next;
    t->next=files2;
}

int main(int argc, char **argv){


    int opt;

    FILE *styleptr;
    DIR *inDir;
    DIR *outDir;
    char inName[250], outName[250];

    struct dirent *pDirent;

    while((opt=getopt(argc, argv, ":s:i:o:"))!=-1){
        switch(opt){
            case 's':
                styleptr = fopen(optarg, "r");
                break;
            case 'i':
                inDir = opendir(optarg);
                strcpy(inName, optarg);
                break;
            case 'o':
                outDir = opendir(optarg);
                strcpy(outName, optarg);
                break;
            case ':':
            case '?':
                usage();
                return 1;
                break;
        }
    }

    printf("inDir %s, outDir %s\n", inName, outName);

    // Note do error checking

    // custom number of files

    char buff[BUFFER_SIZE];
    char dest[250];

    Files *files = (Files*) malloc(sizeof(Files));
    Files *curr=files;
    FILE *f;
    
    while((pDirent=readdir(inDir))!=NULL){
        if(pDirent->d_type==4)
            continue;
        memset(dest, '\0', sizeof(dest));
        strcpy(dest, inName);
        strcat(dest, "/");
        strcat(dest, pDirent->d_name);
        printf("opening file: %s\n", dest);
        f = fopen(dest, "r");
        addFile(files, f);
    }

    curr=files;
    while(curr!=NULL){
        f = curr->file;
        while(fgets (buff, BUFFER_SIZE, f)!=NULL)
            puts(buff);
        curr = curr->next;
    }

    return 0;
}