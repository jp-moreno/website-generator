#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#define BUFFER_SIZE 250

void usage(){
    printf("webgen requires a style sheet, a post header, a post footer, an index header, an index footer, a an input folder, and an output folder\nexample: webgen -s style.css -i in -o out -h header.html -f footer.html -a iheader.html -b ifooter.html\n");
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
    int number;
    FILE *infile;
    FILE *outfile;
    char *outfile_path;
};

void printTag(FILE *f, char *tag, char *content){
	fprintf(f, "<%s>%s</%s>", tag, content, tag);
}

void generateHeader(Post post, FILE *f){
    rewind(f);
    char buff[BUFFER_SIZE];
    while(fgets (buff, BUFFER_SIZE, f)!=NULL){
        fprintf(post.outfile, buff);
    }
    printTag(post.outfile, "h2", post.name);
    printTag(post.outfile, "h3", post.subtitle);
    printTag(post.outfile, "h3", post.date);
    fprintf(post.outfile,"<img class=\"post-img\"src=\"%s\">", post.img);
}

void generateBody(Post post){
    char buff[BUFFER_SIZE];
    while(fgets (buff, BUFFER_SIZE, post.infile)!=NULL){
	printTag(post.outfile, "p", buff);
    }
}

void generateFooter(Post post, FILE *f){
    char buff[BUFFER_SIZE];
    rewind(f);
    while(fgets (buff, BUFFER_SIZE, f)!=NULL){
        fprintf(post.outfile, buff);
    }
}

void generateIndexHeader(FILE *fIndex, FILE *f){
    char buff[BUFFER_SIZE];
    rewind(f);
    while(fgets (buff, BUFFER_SIZE, f)!=NULL){
        fprintf(fIndex, buff);
    }
}

void addToIndex(Post post, FILE *fIndex){
    fprintf(fIndex, "<li><img class=\"thumbnail\" src=\"%s\"><a href=\"%d%s\">%s - %s</a></li>", post.img, post.id, ".html", post.name, post.date);
}

void generateIndexFooter(FILE *fIndex, FILE *f){
    char buff[BUFFER_SIZE];
    rewind(f);
    while(fgets (buff, BUFFER_SIZE, f)!=NULL){
        fprintf(fIndex, buff);
    }
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
    printf("%d\n", argc);
    if(argc!=15){
        usage();
        return 1;
    }

    int opt;

    
    FILE *styleptr, *fheader, *ffooter,*fiheader, *fifooter;
    DIR *inDir;
    DIR *outDir;
    char inName[250], outName[250];

    struct dirent *pDirent;

    while((opt=getopt(argc, argv, ":s:i:o:a:b:f:h:"))!=-1){
        switch(opt){
            case 'a':
                fiheader = fopen(optarg, "r");
                break;
            case 'b':
                fifooter = fopen(optarg, "r");
                break;
            case 'f':
                ffooter = fopen(optarg, "r");
                break;
            case 'h':
                fheader = fopen(optarg, "r");
                break;
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

    if(fifooter==NULL||fiheader==NULL||ffooter==NULL||fheader==NULL){
        printf("null!");
        return 1;
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
        memset(str, '\0', 10);
        fgets(str, 10, f);
        posts[t].number = strtol(str, NULL, 10);
        fgets(posts[t].subtitle, 250, f);
        curr = curr->next;
        t++;
    }

    memset(dest, '\0', sizeof(dest));
    strcpy(dest, outName);
    strcat(dest, "/");
    strcat(dest, "index.html");
    FILE *fIndex = fopen(dest, "w");
    printf("generating\n");

    int curr_number = INT_MAX;
    Post *p;
    p = &posts[0];
    
    generateIndexHeader(fIndex, fiheader);
    for(int i=0; i<n;i++){
        for(int j=0; j<n;j++){
            if(posts[j].number < curr_number && (posts[j].number > p->number||p->number==curr_number)){
                p = &posts[j];
            }
        }
        curr_number = p->number;
        generateHeader(*p, fheader);
        generateBody(*p);
        generateFooter(*p, ffooter);
        addToIndex(*p, fIndex);
    }
    generateIndexFooter(fIndex, fifooter);

    //make index

    // free stuff

    return 0;
}
