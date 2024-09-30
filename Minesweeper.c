//\nType [s <difficulty>] to start a game. [s] for highest score in each game\n[q q] to quit anywhere in during game\n[d <row>,<colunm> to dig]\n[f <row>,<colunm> to flag]\n[r <row>,<colunm> to remove from number]
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define C 16
#define R 16
#define EASY 20
#define MEDIUM 30
#define HARD 40

typedef struct input{
    char f;
    int x,y;
} input;

typedef struct t{
    int hh;
    int mm;
    int ss;
}t;

typedef struct score{
    int eTime;
    int mTime;
    int hTime;
}score;

typedef struct playerInfo{
    char name[50],password[50];
    int id;
    score pScore;
}playerInfo;

time_t start,end;
char game[C][R];
char ch[3][3];

void rst();
void green();
void black();
void red();
void yellow();
void white();
void purple();
void blue();
void redBold();
void cyan();

void seeTime(t *a,int info);
int minesweeper(char c);
playerInfo mainMenu();
int newID();
int signin();
int newUser(playerInfo *data);
playerInfo searchID(int id , char password[]);
playerInfo login();
int innit();

void rstMatrix();
void userInput(input *in);
void display();
void bomb(input *in, char level);
void cheakInput(input *in);
void insertNumber();
int lcharAscii(char c);
void dig(input *in);
void flag(input *in);
void rFlag(input *in);
void quit(input *in);
void spreadOut(int x, int y);
int gameover(char c,int count);
int countBomb();

int main(){
    playerInfo active = mainMenu();
    printf("\nlogged in with\nUser:%s\n\n",active.name);
    char g='a';

    do{
        do{
            printf("Difficulty: e=easy m=medium h=hard\nType [<difficulty>] to start a game.\n[s] for highest score in each game\n");
            scanf(" %c",&g);
        }while( (g!='e')&&(g!='m')&&(g!='h')&&(g!='s'));

        if(g=='s'){
            t scoreTime;
            seeTime(&scoreTime, active.pScore.eTime);
            printf("e: %d:%d:%d\t",scoreTime.hh,scoreTime.mm,scoreTime.ss);
            seeTime(&scoreTime, active.pScore.mTime);
            printf("m: %d:%d:%d\t",scoreTime.hh,scoreTime.mm,scoreTime.ss);
            seeTime(&scoreTime, active.pScore.hTime);
            printf("h: %d:%d:%d\n",scoreTime.hh,scoreTime.mm,scoreTime.ss);
        }
    }while(g=='s');

    minesweeper(g);

    return 0;
}



playerInfo mainMenu(){
    int a=0;
    up:
    do{
        printf("press 1 to log in\npress 2 to sign up\nPress 3 to Innitialize\npress 4 to exit\n");
        scanf("%d",&a);
    }while(a<=1&&a>=4);

    switch (a){
        case 1:
            return login();
            break;
        case 2:
            signin();
            goto up;
            break;
        case 3:
            if(innit()){
                printf("\nInnitialization sucessfull\n");
            }else{
                printf("\nInnitialization unsucessfull\n");
            }
            goto up;
            break;
        case 4:
            exit(0);
            break;
        default:
            exit(0);
            break;
    }
}

int newID(){
    int i,temp=0;
    playerInfo search;
    FILE *fp;
    fp = fopen("userdata.dat","r");
    if (fp == NULL){
        printf("error in opening userdata.dat file -> signin()\nor userdata.dat file missing.");
        exit(1);
    }

    while(fscanf(fp,"%d\t%s\t%s\t%d\t%d\t%d\n",&search.id,&search.name,&search.password,&search.pScore.eTime,&search.pScore.mTime,&search.pScore.hTime)!=EOF){ // inputs formatted value to struct
        if(search.id>temp){
            temp=search.id; // cheaks for the highest ID
        }
    }

    fclose(fp);
    return temp+1; // returns new id

}

int signin(){
    playerInfo newSignin;    

    printf("Input your name(no numbers): ");
    scanf("%s",&newSignin.name);
    printf("Input your New password(no numbers): ");
    scanf("%s",&newSignin.password);
    newSignin.id=newID(); //gets unique id


    newUser(&newSignin); //inputs newUser into file

    printf("\nYou have made your acc.\nPlease remember your ID: %d\n", newSignin.id);
    return 1;
}

int newUser(playerInfo *data){
    FILE *fp;
    fp = fopen("userdata.dat","a+");
    if(fp==NULL){
        printf("Error opening userdata.dat file -> newUser()\nor userdata.dat file missing.\n");
        return 0;
    }
    fprintf(fp,"%d\t%s\t%s\t%d\t%d\t%d\n",data->id,data->name,data->password,0,0,0);
    fclose(fp);
    return 1;
}


playerInfo searchID(int id , char password[]){
    playerInfo search;
    FILE * fp;
    fp=fopen("userdata.dat","r");
    if(fp==NULL){
        printf("Error opening userdata.dat file -> searchID()\nor userdata.dat file missing.\n");
    }
    while(fscanf(fp,"%d\t%s\t%s\t%d\t%d\t%d\n",&search.id,&search.name,&search.password,&search.pScore.eTime,&search.pScore.mTime,&search.pScore.hTime)!=EOF){
        if((id==search.id)&&(strcmp(password,search.password)==0)){ // if given id and password are same returns the unique data of file 
            return search;
        }
    }

    printf("\nError in searchID()\nno similar combination of user log in and password");
    exit(1);
}

playerInfo login(){
    int id;
    char password[50];
    printf("Input your ID: ");
    scanf("%d",&id);
    printf("Input your password: ");
    scanf("%s",&password);
    return searchID(id,password);
}

int innit(){
    FILE *fp;
    fp=fopen("userdata.dat","w");
    if(fp==NULL){
        printf("Some error innitializing -> innit()");
        return 0;
    }
    fprintf(fp,"%d\t%s\t%s\t%d\t%d\t%d\n",100,"TheUnknown","TheUnknown",0,0,0);
    fclose(fp);
    return 1;
}

void seeTime(t *a,int info){
    a->ss=info;
    while(a->hh>=24||a->mm>=60||a->ss>=60){
        if(a->ss>=60){
            a->ss=(a->ss-60);
            a->mm=(a->mm+1);
        }else if(a->mm>=60){
            a->mm=(a->mm-60);
            a->hh=(a->hh+1);
        }
        else if(a->hh>=24){
            a->hh=00;
        }
        else{
            printf("Error in seeTime()");
        }
    }
}



int minesweeper(char c){
    int bombAmt;
    input in;
    rstMatrix();
    display();
    red();
    printf("\nYou can only dig in first move\n");
    rst();
    do{
        userInput(&in);
    }while(in.f!='D');
    srand(time(NULL));
    bomb(&in,c);
    bombAmt=countBomb();
    insertNumber();
    spreadOut(in.x,in.y);
    do{
        display();
        userInput(&in);
        cheakInput(&in);
    }while(gameover(c,bombAmt));

    printf("\n\nCongratulations! you have won the game!\n\n\n");
    
}
int countBomb(){
    int count=0;
    for(int i=0;i<R;i++){
        for(int j=0;j<C;j++){
            if(game[i][j]=='B'){
                count+=1;
            }
        }
    }
    return count;
}
void rstMatrix(){
    for(int i=0;i<R;i++){
        for(int j=0;j<C;j++){
            game[i][j]='G';
        }
    }
}

void userInput(input *in){
    do{
        printf("\n [Q 0,0] to quit anywhere in during game\n[D <row>,<colunm> to dig]\n[F <row>,<colunm> to flag]\n[R <row>,<colunm> to remove from number]\n[r <row>,<colunm] to remove flag\ne.g. D 1,0 to dig the top left thing\n\n");
        scanf(" %c %d,%d",&in->f,&in->x,&in->y);
    }while(in->f!='Q'&&in->f!='D'&&in->f!='F'&&in->f!='R'&&in->f!='Q'&&in->f!='r'&&in->x>=0&&in->x<=10&&in->y>=0&&in->y<=9);
    if(in->f=='Q'&&in->x==0&&in->y==0){
        printf("You have exited the game");
        exit(0);
    }
}

void surrounding(int c, int r){
    int mc=c-1,mr=r-1,pc=c+1,pr=r+1,i,j;
    for(i=0;i<3;i++){
        for(j=0;j<3;j++){
            ch[i][j]='\0';
        }
    }
    // cheak top boundary
    if(mc < 0){
        ch[0][0] = 'Z';
        ch[0][1] = 'Z';
        ch[0][2] = 'Z';
    }

    // Check bottom boundary
    if(pc >= C){
        ch[2][0] = 'Z';
        ch[2][1] = 'Z';
        ch[2][2] = 'Z';
    }

    // Check left boundary
    if(mr < 0){
        ch[0][0] = 'Z';
        ch[1][0] = 'Z';
        ch[2][0] = 'Z';
    }

    // Check right boundary
    if(pr >= R-1){
        ch[0][2] = 'Z';
        ch[1][2] = 'Z';
        ch[2][2] = 'Z';
    }

    if(ch[0][0]!='Z'){
        ch[0][0]=game[mc][mr];
    }
    if(ch[0][1]!='Z'){
        ch[0][1]=game[mc][r];
    }
    if(ch[0][2]!='Z'){
        ch[0][2]=game[mc][pr];
    }
    if(ch[1][0]!='Z'){
        ch[1][0]=game[c][mr];  
    }
    if(ch[1][1]!='Z'){
        ch[1][1]=game[c][r];  
    }
    if(ch[1][2]!='Z'){
        ch[1][2]=game[c][pr];
    }
    if(ch[2][0]!='Z'){
        ch[2][0]=game[pc][mr];
    }
    if(ch[2][1]!='Z'){
        ch[2][1]=game[pc][r];
    }
    if(ch[2][2]!='Z'){
        ch[2][2]=game[pc][pr];
    }

}
void insertNumber(){
    int i,j,k,l,count=0;
    for(i=0;i<C;i++){ // cycles through all matrix except there is a bomb
        for(j=0;j<R;j++){
            if(game[i][j]!='B'){
                surrounding(i,j); // takes surrounding info
                for(k=0;k<3;k++){
                    for(l=0;l<3;l++){
                        if(ch[k][l]=='B'){ // if B found then count increases
                            count+=1;
                        }
                    }
                }
                if(count!=0){
                    game[i][j]='M'+count;
                    count=0;
                }
            }
        }
    }
    
}
void cheakInput(input *in){
    switch(in->f){
        case 'D':
        dig(in);
        break;
        case 'F':
        flag(in);
        break;
        case 'R':
        break;
        case 'r':
        rFlag(in);
        break;
        case 'Q':
        quit(in);
        break;
        default:
        break;
    }
}
void dig(input *in){
    if(game[in->x][in->y]=='B'||game[in->x][in->y]=='b'){ // digs a bomb
        printf("\n\nGameOver\n\n");
        exit(0);
    }
    if(game[in->x][in->y]=='F'||(game[in->x][in->y]>=('n')&&game[in->x][in->y]<=('v'))||game[in->x][in->y]=='b'){ // tries to dig a flag
        printf("\n\nYou can't dig flags\n\n");
    }
    if(game[in->x][in->y]>=('N')&&game[in->x][in->y]<=('V')){ // tries to dig a number
        game[in->x][in->y]=lcharAscii(game[in->x][in->y]);
    }
    if(game[in->x][in->y]=='G'){
        spreadOut(in->x,in->y);
    }
}
void flag(input *in){
    if(game[in->x][in->y]>=('N')&&game[in->x][in->y]<=('V')){ // if hidden numers are flagged, lowercase them
        game[in->x][in->y]=tolower(game[in->x][in->y]);
    }
    if(game[in->x][in->y]==('B')){ // if hidden bombs are flagged, lowercase bomb
        game[in->x][in->y]='b';
    }
    if(game[in->x][in->y]>=('n')&&game[in->x][in->y]<=('v')||game[in->x][in->y]=='b'){
        printf("\n\nYou cannot flag, Flagged Items\n\n");
    }
}
void rFlag(input *in){
    if(game[in->x][in->y]>=('n')&&game[in->x][in->y]<=('v')){ // if hidden numers are unflagged flagged, uppercase them
        game[in->x][in->y]=toupper(game[in->x][in->y]);
    }
    if(game[in->x][in->y]==('b')){ // if hidden bombs are flagged, uppercase them
        game[in->x][in->y]='B';
    }
}void quit(input *in){
    if(in->x==0&&in->y==0){
        printf("\n\nYou exited the game\n\n");
        exit(0);
    }
}
int lcharAscii(char c){
    if(c>='N'&&c<='V'){// changes lower n to ascii numbers
        return c-29; 
    }
    return 0;
 }

void spreadOut(int x, int y) {
    // Base condition: check if out of bounds
    if (x < 1 || x >= C || y < 0 || y >= R-1) {
        return;
    }

    // Check if the tile is already revealed
    if (game[x][y]=='W') {
        return;
    }

    // Check if the tile is not grass ('G') or a number (0-9)
    if (game[x][y] != 'G' && (game[x][y] >= '0' && game[x][y] <= '9')) {
        return;
    }

    // Optionally handle other cases (like 'N'-'V') if needed
    if (game[x][y] >= 'N' && game[x][y] <= 'V') {
        game[x][y] = lcharAscii(game[x][y]);  // Handle ASCII conversion here
        return;
    }

    if(game[x][y] >= '0' && game[x][y] <= '9'){
        return;
    }
    // Mark the current tile as revealed (whitespace or revealed grass)
    game[x][y] = 'W';

    // Recursively spread to adjacent cells (4 directions)
    // spreadOut(x-1, y-1);  // Top-left
    spreadOut(x-1, y);    // Top
    // spreadOut(x-1, y+1);  // Top-right
    spreadOut(x, y-1);    // Left
    spreadOut(x, y+1);    // Right
    // spreadOut(x+1, y-1);  // Bottom-left
    spreadOut(x+1, y);    // Bottom
    // spreadOut(x+1, y+1);  // Bottom-right
}

int gameover(char c,int count){
    int i,j,bombNum=0;
    for(i=0;i<C;i++){
        for(j=0;j<R;j++){
            if(game[i][j]=='B'||game[i][j]=='b'||game[i][j]=='G'){
                bombNum+=1;
            }
        }
    }
    if(count==bombNum){
        return 0;
    }else{
        return 1;
    }
}


void display(){//displayes the current matrix
    int i,j;
    for(i=0;i<C;i++){
        white();
        printf("%d",i);
        for(j=0;j<R-1;j++){
            if(i==0){
                printf("\t%d",j);
                if(j==R-2){
                    printf("\n");
                }
            }
            if(i!=0){
                if(game[i][j]>='N'&&game[i][j]<='V'){ // hidden number
                    green();
                    printf("\tG");
                    continue;
                }
                if(game[i][j]>=('n')&&game[i][j]<=('v')||game[i][j]=='b'){ // flagged numbers
                    purple();
                    printf("\tF");
                    continue;
                }
                if(game[i][j]>='1'&&game[i][j]<='9'){ // shown number
                    blue();
                    printf("\t%c",game[i][j]);
                    continue;
                }
                switch(game[i][j]){
                    case 'G': // grass
                        green();
                        printf("\tG");
                        break;
                    case 'W':
                        white();
                        printf("\tW");
                        break;
                    case 'f': //bomb 
                        cyan();
                        printf("\tF");
                        break;
                    case 'F': // flag
                        cyan();
                        printf("\tF");
                        break;
                    case 'B': // they don't need to know that there is a bomb there
                        green();
                        printf("\tG");
                        break;
                    default:
                    break;
                }
            }
        }
        printf("\n");
    }
    rst();
}

void bomb(input *in, char level){
    int nBombs,tempC,tempR;
    if(level=='e'){
        nBombs=EASY;
    }else if(level=='m'){
        nBombs=MEDIUM;
    }else if(level=='h'){
        nBombs=HARD;
    }else{
        printf("Error in bomb()");
        exit(0);
    }
    do{
        tempR=rand()%R;
        tempC=rand()%C + 1;
        if((in->x!=tempC&&in->y!=tempR)){ // not bomb in first press
            if(game[tempC][tempR]!='B'){ // if the value is already B there
                game[tempC][tempR]='B';
                nBombs-=1;
            }
        }
    }while(nBombs>=0);
}

void rst(){
    printf("\033[0;00m");
}

void green(){
    printf("\033[0;32m");
}

void black(){
    printf("\033[0;30m");
}

void red(){
    printf("\033[0;31m");
}

void yellow(){
    printf("\033[0;33m");
}

void white(){
    printf("\033[0;37m");
}

void purple(){
    printf("\033[0;35m");
}
void blue(){
    printf("\033[0;34m");
}
void redBold(){
    printf("\033[1;31m");
}
void cyan(){
    printf("\033[0;36m");
}