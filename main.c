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

typedef struct Post Post;
struct Post{
    int id;
    char name[250];
    char subtitle[250];
    char img[250];
    char date[250];
    FILE *infile;
    FILE *outfile;
    char *outfile_path;
};

void generateHeader(Post post){
    fprintf(post.outfile, "<h2>");
    fprintf(post.outfile, post.name);
    fprintf(post.outfile, "</h2>");
    fprintf(post.outfile, "<h3>");
    fprintf(post.outfile, post.subtitle);
    fprintf(post.outfile, "</h3>");
    fprintf(post.outfile, "<h3>");
    fprintf(post.outfile, post.date);
    fprintf(post.outfile, "</h3>");
}

void generateBody(Post post){
    char buff[BUFFER_SIZE];
    while(fgets (buff, BUFFER_SIZE, post.infile)!=NULL){
        fprintf(post.outfile, "<p>");
        fprintf(post.outfile, buff);
        fprintf(post.outfile, "</p>");
    }
}

void generateFooter(Post post){
    fprintf(post.outfile, "<p><a href=index.html"">Go back</p>");
}

void addToIndex(Post post, FILE *fIndex){
    fprintf(fIndex, "<li><a href=\"%d%s\">%s - %s</a></li>", post.id, ".html", post.name, post.date);
}

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

int post_id;

int getPostId(){
    post_id++;
    return post_id-1;
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
    int n = 0;
    
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
        n++;
    }

    //make blog posts

    Post *posts = (Post*) malloc(sizeof(Post)*n);
    int t=0;
    curr=files;
    char str[10];
    FILE *f2;
    while(curr!=NULL){
        f = curr->file;
        posts[t].id=getPostId();
        memset(dest, '\0', sizeof(dest));
        strcpy(dest, outName);
        strcat(dest, "/");
        sprintf(str, "%d", posts[t].id);
        strcat(dest, str);
        strcat(dest, ".html");
        posts[t].outfile_path = malloc((strlen(dest)+1)*sizeof(char));
        strcpy(posts[t].outfile_path, dest);
        f2 = fopen(posts[t].outfile_path, "w");
        posts[t].outfile = f2;
        posts[t].infile = f;

        fgets(posts[t].name, 250, f);
        fgets(posts[t].date, 250, f);
        fgets(posts[t].img, 250, f);
        fgets(posts[t].subtitle, 250, f);
        //while(fgets (buff, BUFFER_SIZE, f)!=NULL)
        //    puts(buff);
        curr = curr->next;
        t++;
    }

    memset(dest, '\0', sizeof(dest));
    strcpy(dest, outName);
    strcat(dest, "/");
    strcat(dest, "index.html");
    FILE *fIndex = fopen(dest, "w");

    for(int i=0; i<n;i++){
        generateHeader(posts[i]);
        generateBody(posts[i]);
        generateFooter(posts[i]);
        addToIndex(posts[i], fIndex);
    }

    //make index

    // free stuff

    return 0;
}