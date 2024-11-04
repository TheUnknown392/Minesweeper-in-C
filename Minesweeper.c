#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <windows.h>
#include "Resources/resources.h"
#else
#inlude <unistd.h>
#endif

// sets row and colunm of matrix
#define R 15
#define C 15
// three levels of dfficulty according to number of bombs
#define EASY 15
#define MEDIUM 30
#define HARD 45
// the extention of file
#define fileExten "MINESWEEPERPLAYER.txt"
#define FolderLocation "./Player Folder/"
// time to wait before exit in miliseconds
#define SleepTime 5000

typedef struct input{// game input
    char f;
    int x,y;
} input;

typedef struct t{ // display in hh:mm:ss form
    int hh;
    int mm;
    int ss;
}t;

typedef struct score{ // to store time in seconds in file
    int eTime;
    int mTime;
    int hTime;
}score;

typedef struct playCount{ // to store how much play count of users
    int totalPlayed,totalwon,lostOrAbandoned;
}playCount;

typedef struct playerInfo{ // to store player details
    char name[50],password[50];
    score pScore;
    playCount pCount;
}playerInfo;

playerInfo active; // store detail of active or logged in player
time_t start,end; // store start and end time
char game[R][C]; // store the detail of game board
char ch[3][3]; // to store temporary data of surrounding relative to some cordinate

//colors
void rst();
void green();
void red();
void white();
void purple();
void blue();
void cyan();

playerInfo login(); // log in
playerInfo mainMenu(); // menue to create, log in your minsweeper account and returns value of active player
int signup(); // sign up
void minesweeper(char gameHardness); // actual process of minsweeper game is in here
char menueOption(); // to choose between menu
void cheakFolder();
void inputFile();// inputs current active player information to file
void rectifyFile();// corrects suprise quits
void seeTime(t *a,int info); // turns seconds into hh:mm:ss
void updateIfHighscore(char gamemode);//updates the player's score if there is high score

int gameover();
int rNum(int a, int b); // remove from number
int charNumber(char c); // changes unflagged hidden numbers into int
void rstMatrix(); // puts 'G' value in mineweeper matrix (game)
void insertBombNumber(input *inputValue, char level); // insert bomb and number in matrix
void surrounding(int r, int c);// returns the value of surrounding boxes to ch[][]
void userInput(input *inputValue); // takes user input
void display(); // displays matrix board
void cheakInput(input *inputValue); // cheaks given input
void dig(int a,int b); // digs
void flag(input *inputValue); // flags
void rFlag(input *inputValue); // removes flags
void quit(input *inputValue); // quit
void spreadOut(int x, int y); // when digged, spreads out until a hidden number is enfountered

void waitExit();


int main(){
#ifdef _WIN32
    HANDLE forEscapeSequence=GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode=0;
    GetConsoleMode(forEscapeSequence, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(forEscapeSequence, dwMode);
#endif

    cheakFolder();
    active = mainMenu();
    rectifyFile();
    char gamemode=menueOption();
    minesweeper(gamemode);
    
    waitExit(0);
    return 0;
}

void inputFile(){
    char filePath[150];  
    strcpy(filePath,FolderLocation);
    strcat(filePath,active.name);
    strcat(filePath,fileExten);
    FILE*fp=fopen(filePath,"w");
    if(fp==NULL){
        printf("\n\nNo file found\n\n");
    }
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
        active.pCount.lostOrAbandoned
    );
    fclose(fp);
}


playerInfo mainMenu(){
    int a=0;
    up:
    do{
        printf("press 1 to log in\npress 2 to sign up\nPress 3 to to exit\n");
        scanf("%d",&a);
    }while(!(a>=1&&a<=3));

    switch (a){
        case 1:
            return login();
            break;
        case 2:
            signup();
            goto up;
            break;
        case 3:
            exit(0);
            break;
        default:
            exit(0);
            break;
    }
    printf("\nlogged in with\nUser:%s\n\n",active.name);
}


int signup(){
    playerInfo newSignin;    
    char filePath[150];  

    printf("Input your name (no numbers): ");
    scanf("%s", newSignin.name);  

    printf("Input your New password (no numbers): ");
    scanf("%s", newSignin.password);

    strcpy(filePath,FolderLocation);//inserts current path ./
    strcat(filePath,newSignin.name);// inserts name
    strcat(filePath, fileExten);// inserts file extention MINESWEEpErPLAYER.txt
    FILE *fp;
    fp = fopen(filePath, "w");
    if(fp == NULL){
        printf("Error opening file for user\n");
        return 0;
    }

    fprintf(
        fp, 
        "%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\n",
        newSignin.name,
        newSignin.password,
        32000, // lowest score. 0 being highest score
        32000,
        32000,
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
    char filePath[150]; 
    strcpy(filePath,FolderLocation);
    strcat(filePath, userName);
    strcat(filePath, fileExten);

    FILE *fp = fopen(filePath, "r");
    if(fp == NULL){
        printf("Error opening file for user\n");
        waitExit(1);
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
        printf("\nIncorrect password or username\n");
        waitExit(1);
    }
}


void seeTime(t *a, int info) {
    a->hh = info / 3600;         
    info = info % 3600;         
    a->mm = info / 60;  
    a->ss = info % 60;          
}




void minesweeper(char gameHardness){
    input inputValue;
    rstMatrix();
    display();
    red();
    printf("\nYou can only dig in first move\n");
    rst();
    do{
        userInput(&inputValue);
    }while(inputValue.f!='D');
    active.pCount.totalPlayed+=1; // Only counts as a play if you have digged
    inputFile();
    srand(time(NULL));
    insertBombNumber(&inputValue,gameHardness);
    spreadOut(inputValue.x,inputValue.y);
    time(&start);
    do{
        display();
        userInput(&inputValue);
        cheakInput(&inputValue);
    }while(gameover());
    display();
    printf("\n\nCongratulations! you have won the game!\n\n\n");
    updateIfHighscore(gameHardness);
}
void rstMatrix(){
    for(int i=0;i<R;i++){
        for(int j=0;j<C;j++){
                game[i][j]='G';           
        }
    }
}

void userInput(input *inputValue){
    int inputAgain=1;
    while(inputAgain){
        do{
            printf("\n[Q 1,1] to quit anywhere in during game\n[D <row>,<colunm> to dig]\n[F <row>,<colunm> to flag]\n[R <row>,<colunm>] to remove from number\n[r <row>,<colunm] to remove flag\ne.g. [D 1,1] to dig the top left thing\n\n");
            scanf(" %c %d,%d",&inputValue->f,&inputValue->x,&inputValue->y);
            inputValue->x-=1; // corrects the user input from display by subtracting 1 to cordinate value being saved
            inputValue->y-=1;
        }while(
            (inputValue->f!='Q')&&
            (inputValue->f!='D')&&
            (inputValue->f!='F')&&
            (inputValue->f!='r')&&
            (inputValue->f!='R')
        );

        inputAgain=0;

        if (!(inputValue->x >= 0 && inputValue->x < R && inputValue->y >= 0 && inputValue->y < C)){
            printf("\nMake sure your co-ordinates are within the matrix\n");
            inputAgain=1;
        }
    }
}

void surrounding(int r, int c){
    int mc=c-1,mr=r-1,pc=c+1,pr=r+1,i,j;
    for(i=0;i<3;i++){
        for(j=0;j<3;j++){
            ch[i][j]='\0';
        }
    }

    //0,0  0,1  0,2     mr,mc   mr,c    mr pc
    //1,0  1,1  1,2     r,mc    r,c     r,pc
    //2,0  2,1  2,2     pr,mc   pr,c    pr,pc


    // cheak top boundary
    if(mr < 0){
        ch[0][0] = 'Z';
        ch[0][1] = 'Z';
        ch[0][2] = 'Z';
    }

    // Check bottom boundary
    if(pr >= R){
        ch[2][0] = 'Z';
        ch[2][1] = 'Z';
        ch[2][2] = 'Z';
    }

    // Check left boundary
    if(mc < 0){
        ch[0][0] = 'Z';
        ch[1][0] = 'Z';
        ch[2][0] = 'Z';
    }

    // Check right boundary
    if(pc >= C){
        ch[0][2] = 'Z';
        ch[1][2] = 'Z';
        ch[2][2] = 'Z';
    }

    if(ch[0][0]!='Z'){
        ch[0][0]=game[mr][mc];
    }
    if(ch[0][1]!='Z'){
        ch[0][1]=game[mr][c];
    }
    if(ch[0][2]!='Z'){
        ch[0][2]=game[mr][pc];
    }
    if(ch[1][0]!='Z'){
        ch[1][0]=game[r][mc];  
    }
    if(ch[1][1]!='Z'){
        ch[1][1]=game[r][c];  
    }
    if(ch[1][2]!='Z'){
        ch[1][2]=game[r][pc];
    }
    if(ch[2][0]!='Z'){
        ch[2][0]=game[pr][mc];
    }
    if(ch[2][1]!='Z'){
        ch[2][1]=game[pr][c];
    }
    if(ch[2][2]!='Z'){
        ch[2][2]=game[pr][pc];
    }

}

void cheakInput(input *inputValue){
        switch(inputValue->f){
            case 'D':
            dig(inputValue->x,inputValue->y);
            break;
            case 'F': 
            flag(inputValue);
            break;
            case 'R':
            rNum(inputValue->x,inputValue->y);
            break;
            case 'r':
            rFlag(inputValue);
            break;
            case 'Q':
            quit(inputValue);
            break;
            default:
            break;
        }
}
void dig(int a, int b){
    if(game[a][b]=='B'){ // digs a bomb
        printf("\n\nGameOver\n\n");
	    active.pCount.lostOrAbandoned+=1;
	    inputFile();
        waitExit(0);
    }
    if(game[a][b]=='F'||(game[a][b]>=('n')&&game[a][b]<=('u'))||game[a][b]=='b'){ // tries to dig a flag
        printf("\n\nFlaggs skipped\n\n");
        return;
    }
    if(game[a][b]>=('N')&&game[a][b]<=('U')){ // tries to dig a number
        game[a][b]=charNumber(game[a][b]);
        return;
    }
    if(game[a][b]=='G'){
        spreadOut(a,b);
        return;
    }
}
void flag(input *inputValue){
    if(game[inputValue->x][inputValue->y]>=('N')&&game[inputValue->x][inputValue->y]<=('U')){ // if hidden numers are flagged, lowercase them
        game[inputValue->x][inputValue->y]=tolower(game[inputValue->x][inputValue->y]);
        return;
    }
    if(game[inputValue->x][inputValue->y]==('B')){ // if hidden bombs are flagged, lowercase bomb
        game[inputValue->x][inputValue->y]=tolower(game[inputValue->x][inputValue->y]);
        return;
    }
    if(game[inputValue->x][inputValue->y]=='G'){
        game[inputValue->x][inputValue->y]=tolower(game[inputValue->x][inputValue->y]);
        return; 
    }
    if((game[inputValue->x][inputValue->y]>=('n')&&game[inputValue->x][inputValue->y]<=('u'))||game[inputValue->x][inputValue->y]=='b'||game[inputValue->x][inputValue->y]=='g'){
        printf("\n\nYou cannot flag, Flagged Items\n\n");
        return;
    }
}
void rFlag(input *inputValue){
    if(game[inputValue->x][inputValue->y]>=('n')&&game[inputValue->x][inputValue->y]<=('u')){ // if hidden numers are unflagged flagged, uppercase them
        game[inputValue->x][inputValue->y]=toupper(game[inputValue->x][inputValue->y]);
        return;
    }
    if(game[inputValue->x][inputValue->y]==('b')){ // if hidden bombs are flagged, uppercase them
        game[inputValue->x][inputValue->y]=toupper(game[inputValue->x][inputValue->y]);
        return;
    }
    if(game[inputValue->x][inputValue->y]=='g'){
        game[inputValue->x][inputValue->y]=toupper(game[inputValue->x][inputValue->y]);
        return;
    }
}
void quit(input *inputValue){
    if(inputValue->x==0&&inputValue->y==0){
        printf("\n\nYou exited the game\n\n");
	    active.pCount.lostOrAbandoned++;
    	inputFile();
    }
}

int rNum(int a, int b){
    if(game[a][b]>='1'&&game[a][b]<='9'){
        int countFlag=0;
        surrounding(a,b);
        
	for(int i=0;i<3;i++){
            for(int j=0;j<3;j++){
                if(ch[i][j]!='Z'){
                    if((ch[i][j]>='n'&&ch[i][j]<='u')||ch[i][j]=='b'||ch[i][j]=='g'){
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
            printf("\nYou can only remove from numbers if you have proper flag\n");
            return 0;
        }
	}else{
    	printf("\nYou can only remove from numbers\n");
	    return 2;
    }
}

int charNumber(char c){
    if(c>='N'&&c<='U'){// changes lower n to ascii numbers
        return c-29; 
    }
    return 0;
 }

void spreadOut(int x, int y) {
    // Base condition: check if out of bounds
    if (x<0||x>=R||y<0||y>=C) {
        return;
    }

    // Check if the tile is already revealed
    if (game[x][y]=='W') {
        return;
    }

    // Check if the tile is number (0-9)
    if (game[x][y]>='0'&&game[x][y]<='9') {
        return;
    }

    // Optionally handle other cases (like 'N'-'U') if needed
    if (game[x][y]>='N'&&game[x][y]<='U') {
        game[x][y]=charNumber(game[x][y]);  // Handle ASCII conversion here
        return;
    }

    // Mark the current tile as revealed (whitespace or revealed grass)
    game[x][y]='W';

    // Recursively spread to adjacent cells (4 directions)
    spreadOut(x-1, y);    // Top
    spreadOut(x, y-1);    // Left
    spreadOut(x, y+1);    // Right
    spreadOut(x+1, y);    // Bottom
}

int gameover(){
    int i,j,numGF=0;
    for(i=0;i<R;i++){
        for(j=0;j<C;j++){
            if(game[i][j]=='G'||game[i][j]=='g'||(game[i][j]>='N'&&game[i][j]<='U')||(game[i][j]>='n'&&game[i][j]<='u')){
                numGF+=1;
            }
        }
    }
    if(numGF==0){ // if everything that was diggable is digged
        time(&end);
        return 0;
    }else{
        return 1;
    }

}


void display(){
    int i,j;
    for(i=-1;i<R;i++){
        white();
        (i==-1)?(printf(" ")):(printf("%d",i+1));
        for(j=0;j<C;j++){
            if(i==-1){
                printf("\t%d",j+1);
                if(j==C-1){
                    printf("\n");
                }
            }
            if(i>=0){
                if(game[i][j]>='N'&&game[i][j]<='U'){ // hidden number
                    green();
                    printf("\tG");
                    continue;
                }
                if(game[i][j]>=('n')&&game[i][j]<=('u')||game[i][j]=='b'||game[i][j]=='g'){ // flagged numbers
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

void insertBombNumber(input *inputValue, char level){
    int nBombs,tempC,tempR, i,j,k,l,count=0;
    //bomb
    if(level=='e'){
        nBombs=EASY;
    }else if(level=='m'){
        nBombs=MEDIUM;
    }else if(level=='h'){
        nBombs=HARD;
    }else{
        printf("Error creating bomb");
        waitExit(0);
    }
    do{
        tempC=rand()%C;
        tempR=rand()%R;
        if((inputValue->x!=tempR&&inputValue->y!=tempC)){ // not bomb in first press
            if(game[tempR][tempC]=='G'){ // if the value is already B there
                game[tempR][tempC]='B';
                --nBombs;
            }
        }
    }while(nBombs>0);

    //number
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

void rectifyFile(){
    if(active.pCount.totalwon+active.pCount.lostOrAbandoned!=active.pCount.totalPlayed){ 
        active.pCount.lostOrAbandoned+=active.pCount.totalPlayed-(active.pCount.totalwon+active.pCount.lostOrAbandoned);
        inputFile();
    }
}

char menueOption(){
    char menueOption='\0';
    do{
        do{
            printf("Difficulty: e=easy m=medium h=hard\nType [<difficulty>] to start a game.\n[d] for account details\n");
            scanf(" %c",&menueOption);
        }while( (menueOption!='e')&&(menueOption!='m')&&(menueOption!='h')&&(menueOption!='d'));
        if(menueOption=='d'){
            t scoreTime;
            printf("\nPlayername: %s\nPlayer Scores: ",active.name);
            seeTime(&scoreTime, active.pScore.eTime);
            printf("e: %d:%d:%d\t",scoreTime.hh,scoreTime.mm,scoreTime.ss);
            seeTime(&scoreTime, active.pScore.mTime);
            printf("m: %d:%d:%d\t",scoreTime.hh,scoreTime.mm,scoreTime.ss);
            seeTime(&scoreTime, active.pScore.hTime);
            printf("h: %d:%d:%d\n",scoreTime.hh,scoreTime.mm,scoreTime.ss);
            printf("Total played: %d\nTotal win: %d\nTotal left of lost: %d\n\n",active.pCount.totalPlayed,active.pCount.totalwon,active.pCount.lostOrAbandoned);
        }
    }while(menueOption=='d');
    return menueOption;
}

void updateIfHighscore(char gamemode){
    int timeScore;
    timeScore=difftime(end,start);
    switch(gamemode){
       	case 'e':
           	if(timeScore<active.pScore.eTime){
               	active.pScore.eTime=timeScore;
           	}
           	break;
        case 'm':
           	if(timeScore<active.pScore.mTime){
               	active.pScore.mTime=timeScore;
           	}
           	break;
        case 'h':
           	if(timeScore<active.pScore.hTime){
               	active.pScore.hTime=timeScore;
           	}
           	break;
	       default:
           		printf("Error in moddifying high score");
	       break;
	}
	active.pCount.totalwon+=1;
   inputFile();
}

void cheakFolder(){
    struct stat Directory; 
    int stats=stat(FolderLocation,&Directory);// returns 0 when sucess otherwise -1

    if(stats==-1){
#ifdef _WIN32
        stats=mkdir(FolderLocation);// returns 0 when sucess otherwise -1
#else
        stats=_mkdir(FolderLocation,S_IRUSR|S_IROTH);
#endif
        if(stats){
            perror("Error");
            waitExit(1);
        }

    }
}

void waitExit(int i){
    printf("\nExiting in ~%d seconds",SleepTime/1000);
#ifdef _WIN32
    Sleep(SleepTime);
#else
    _sleep(SleepTime/1000);
#endif
    exit(1);
}

void rst(){
    printf("\033[0;00m");
}

void green(){
    printf("\033[0;32m");
}

void red(){
    printf("\033[0;31m");
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

void cyan(){
    printf("\033[0;36m");
}
