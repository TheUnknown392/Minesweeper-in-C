#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// sets row and colunm of matrix
#define R 16
#define C 16
// three levels of dfficulty
#define EASY 15
#define MEDIUM 22
#define HARD 30
// the extention of file
#define fileExten "MINESWEEPERPLAYER.txt"

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

typedef struct playCount{
    int totalPlayed,totalwon,lostOrAbandoned;
}playCount;

typedef struct playerInfo{
    char name[50],password[50];
    score pScore;
    playCount pCount;
}playerInfo;

time_t start,end;
char game[R][C];
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
int minesweeper(char c,playerInfo *active);
playerInfo mainMenu();
int signup();
int newUser(playerInfo *data);
playerInfo login();
int innit();

void rstMatrix();
void userInput(input *in);
void display();
void bomb(input *in, char level);
void cheakInput(input *in);
void insertNumber();
int lcharAscii(char c);
void dig(int a,int b);
void flag(input *in);
void rFlag(input *in);
void quit(input *in);
int rNum(int a, int b);
void spreadOut(int x, int y);
int gameover(playerInfo *active);

int main(){
    playerInfo active = mainMenu();
    printf("\nlogged in with\nUser:%s\n\n",active.name);
    char g='a';
    int Time;
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
    minesweeper(g,&active);
    Time=difftime(end,start);
    switch(g){
        case 'e':
            if(Time<active.pScore.eTime){
                active.pScore.eTime=Time;
            }
            break;
        case 'm':
            if(Time<active.pScore.mTime){
                active.pScore.mTime=Time;
            }
            break;
        case 'h':
            if(Time<active.pScore.hTime){
                active.pScore.hTime=Time;
            }
            break;
        default:
            printf("Error in moddifying high score");
        break;
    }

    char temp[100];
    strcpy(temp,active.name);
    strcat(temp,fileExten);
    FILE*fp=fopen(temp,"w");
    fprintf(
        fp, 
        "%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\n",
        active.name,
        active.password,
        active.pScore.eTime,
        active.pScore.mTime,
        active.pScore.hTime,
        active.pCount.totalPlayed,
        active.pCount.totalwon,
        active.pCount.totalPlayed-active.pCount.totalwon
    );
    fclose(fp);
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
            signup();
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


int signup(){
    playerInfo newSignin;    

    printf("Input your name (no numbers): ");
    scanf("%s", newSignin.name);  

    printf("Input your New password (no numbers): ");
    scanf("%s", newSignin.password);  

    char temp[100];  
    strcpy(temp, newSignin.name);
    strcat(temp, fileExten);

    FILE *fp;
    fp = fopen(temp, "w");
    if(fp == NULL){
        printf("Error opening file for user -> signup()\n");
        return 0;
    }

    fprintf(
        fp, 
        "%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\n",
        newSignin.name,
        newSignin.password,
        0,
        0,
        0,
        0,
        0,
        0
    );

    fclose(fp);

    printf("\nYou have created your account.\nPlease remember your Username.\n");
    return 1;
}



playerInfo login(){
    char password[50],userName[50];
    printf("Input your Username: ");
    scanf("%s",&userName);
    printf("Input your password: ");
    scanf("%s",&password);
        playerInfo search;
    char temp[100];
    strcpy(temp, userName);
    strcat(temp, fileExten);

    FILE *fp = fopen(temp, "r");
    if(fp == NULL){
        printf("Error opening file for user -> searchUser()\n");
        exit(0);
    }

    fscanf(
        fp, 
        "%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\n",
        search.name,
        search.password,
        &search.pScore.eTime,
        &search.pScore.mTime,
        &search.pScore.hTime,
        &search.pCount.totalPlayed,
        &search.pCount.totalwon,
        &search.pCount.lostOrAbandoned
    );
    fclose(fp);

    if(strcmp(password, search.password) == 0){
        return search;
    } else {
        printf("\nIncorrect password or username -> searchUser()\n");
        exit(0);
    }
}

int innit(){
    FILE *fp;
    char temp[50];
    strcpy(temp,"TheUnknown");
    strcat(temp,fileExten);
    fp=fopen(temp,"w");
    if(fp==NULL){
        return 0;
    }
    fprintf(fp,"%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\n","TheUnknown","TheUnknown",0,0,0,0,0,0);
    fclose(fp);
    return 1;
}

void seeTime(t *a, int info) {
    a->hh = info / 3600;         
    info = info % 3600;         
    a->mm = info / 60;  
    a->ss = info % 60;          
}




int minesweeper(char c,playerInfo *active){
    int bombAmt;
    input in;
    int gameStatus;
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
    insertNumber();
    spreadOut(in.x,in.y);
    active->pCount.totalPlayed+=1;
    time(&start);
    do{
        display();
        userInput(&in);
        cheakInput(&in);
    }while(gameover(active));
    display();
    printf("\n\nCongratulations! you have won the game!\n\n\n");
}
void rstMatrix(){

    for(int i=0;i<C;i++){
        for(int j=0;j<R;j++){
            game[i][j]='W';
        }
    }

    for(int i=1;i<C;i++){
        for(int j=0;j<=R-1;j++){
                game[i][j]='G';           
        }
    }
}

void userInput(input *in){
        do{
            printf("\n [Q 0,0] to quit anywhere in during game\n[D <row>,<colunm> to dig]\n[F <row>,<colunm> to flag]\n[C <row>,<colunm>] to remove from number\n[r <row>,<colunm] to remove flag\ne.g. D 1,0 to dig the top left thing\n\n");
            scanf(" %c %d,%d",&in->f,&in->x,&in->y);
        }while(in->f!='Q'&&in->f!='D'&&in->f!='F'&&in->f!='C'&&in->f!='Q'&&in->f!='r'&&in->x>=0&&in->x<=10&&in->y>=0&&in->y<=9);
        
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
    if(pc >= R){
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
    if(pr >= C-1){
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
    for(i=0;i<R;i++){ // cycles through all matrix except there is a bomb
        for(j=0;j<C;j++){
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
            dig(in->x,in->y);
            break;
            case 'F':
            flag(in);
            break;
            case 'C':
            rNum(in->x,in->y);
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
void dig(int a, int b){
    if(game[a][b]=='B'){ // digs a bomb
        printf("\n\nGameOver\n\n");
        exit(0);
    }
    if(game[a][b]=='F'||(game[a][b]>=('n')&&game[a][b]<=('v'))||game[a][b]=='b'){ // tries to dig a flag
        printf("\n\nFlaggs skipped\n\n");
        return;
    }
    if(game[a][b]>=('N')&&game[a][b]<=('V')){ // tries to dig a number
        game[a][b]=lcharAscii(game[a][b]);
        return;
    }
    if(game[a][b]=='G'){
        spreadOut(a,b);
        return;
    }
}
void flag(input *in){
    if(game[in->x][in->y]>=('N')&&game[in->x][in->y]<=('V')){ // if hidden numers are flagged, lowercase them
        game[in->x][in->y]=tolower(game[in->x][in->y]);
        return;
    }
    if(game[in->x][in->y]==('B')){ // if hidden bombs are flagged, lowercase bomb
        game[in->x][in->y]=tolower(game[in->x][in->y]);
        return;
    }
    if(game[in->x][in->y]=='G'){
        game[in->x][in->y]=tolower(game[in->x][in->y]);
        return;
    }
    if((game[in->x][in->y]>=('n')&&game[in->x][in->y]<=('v'))||game[in->x][in->y]=='b'||game[in->x][in->y]=='g'){
        printf("\n\nYou cannot flag, Flagged Items\n\n");
        return;
    }
}
void rFlag(input *in){
    if(game[in->x][in->y]>=('n')&&game[in->x][in->y]<=('v')){ // if hidden numers are unflagged flagged, uppercase them
        game[in->x][in->y]=toupper(game[in->x][in->y]);
        return;
    }
    if(game[in->x][in->y]==('b')){ // if hidden bombs are flagged, uppercase them
        game[in->x][in->y]=toupper(game[in->x][in->y]);
        return;
    }
    if(game[in->x][in->y]=='g'){
        game[in->x][in->y]=toupper(game[in->x][in->y]);
        return;
    }
}void quit(input *in){
    if(in->x==0&&in->y==0){
        printf("\n\nYou exited the game\n\n");
        exit(0);
    }
}

int rNum(int a, int b){
    if(game[a][b]>='1'&&game[a][b]<='9'){
        int countFlag=0;
        surrounding(a,b);
        for(int i=0;i<3;i++){
            for(int j=0;j<3;j++){
                if(ch[i][j]!='Z'){
                    if((ch[i][j]>='n'&&ch[i][j]<='v')||ch[i][j]=='b'||ch[i][j]=='g'){
                        countFlag+=1;// counts flags in surrounding.
                    }
                }
            }
        }

        if(countFlag>=/*atoi(&game[a][b])*/game[a][b]-'0'){ // turns character number into an integer
            dig(a-1, b-1);  // Top-left
            dig(a-1, b);    // Top
            dig(a-1, b+1);  // Top-right
            dig(a, b-1);    // Left
            dig(a, b+1);    // Right
            dig(a+1, b-1);  // Bottom-left
            dig(a+1, b);    // Bottom
            dig(a+1, b+1);  // Bottom-right
	        return 1;
        }else{
            printf("\nYou can only remove from numbers if you have proper flag\nor error in ->rNum(countFlag: %d, game[a][b] num=%d)\n",countFlag,game[a][b]-'0');
            return 0;
        }
	}else{
    	printf("\nYou can only remove from numbers\nor error in ->rNum()\n");
	    return 2;
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
    if (x < 1 || x >= R || y < 0 || y >= C-1) {
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

int gameover(playerInfo *active){
    int i,j,numGF=0;
    for(i=1;i<R;i++){
        for(j=0;j<=C-2;j++){
            if(game[i][j]=='G'||game[i][j]=='g'||(game[i][j]>='N'&&game[i][j]<='V')){
                numGF+=1;
            }
        }
    }
    if(numGF==0){// if everything that was diggable is digged
        active->pCount.totalwon+=1;
        time(&end);
        return 0;
    }else{
        return 1;
    }

}


void display(){//displayes the current matrix
    int i,j;
    for(i=0;i<R;i++){
        white();
        printf("%d",i);
        for(j=0;j<C-1;j++){
            if(i==0){
                printf("\t%d",j);
                if(j==C-2){
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
                    case 'b': //bomb 
                        cyan();
                        printf("\tF");
                        break;
                    case 'w':
                        cyan();
                        printf("\tF");
                        break;
                    case 'F': // flag
                        cyan();
                        printf("\tF");
                        break;
                    case 'B': // they don't need to know that there is a bomb there
                        red();
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
        tempC=rand()%(C-1);
        tempR=rand()%(R-1) + 1;
        if((in->x!=tempR&&in->y!=tempC)){ // not bomb in first press
            if(game[tempR][tempC]!='B'&&game[tempR][tempC]!='W'){ // if the value is already B there
                game[tempR][tempC]='B';
                --nBombs;
            }
        }
    }while(nBombs>0);
}

// void inputFile(playerInfo active,int winLoss){
//     char temp[100];
//     strcpy(temp,active.name);
//     strcat(temp,fileExten);
//     FILE*fp=fopen(temp,"w");
//     active.pCount.lostOrAbandoned+=winLoss;
//     fprintf(
//         fp, 
//         "%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\n",
//         active.name,
//         active.password,
//         active.pScore.eTime,
//         active.pScore.mTime,
//         active.pScore.hTime,
//         active.pCount.totalPlayed,
//         active.pCount.totalwon,
//         active.pCount.lostOrAbandoned
//     );
//     fclose(fp);
// }

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
