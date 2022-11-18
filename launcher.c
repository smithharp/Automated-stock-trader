
#include <stdio.h>
#include <sys/types.h>
#include <wait.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#define MARKETRETURN 0.07
#define RISKFREE 0.02


pthread_mutex_t lock;


// Create a linked list to store candidate investments
typedef struct tickerObject {
  char * ticker;
  int score;
  struct tickerObject* next;
} tickerObject_t;

typedef struct tickerObjectList {
  struct tickerObject* head;
} tickerObjectList_t;

// Initialize the list
tickerObjectList_t wrapper = {NULL};

// Function accomplishes the same thing as strtok but accepts double delimiters 
char* strtoke(char *str, const char *delim, char ** start)
{
  //static char *start = NULL; /* stores string str for consecutive calls */
  char *token = NULL; /* found token */
  /* assign new start in case */
  if (str) *start = str;
  /* check whether text to parse left */
  if (!*start) return NULL;
  /* remember current start as found token */
  token = *start;
  /* find next occurrence of delim */
  *start = strpbrk(*start, delim);
  /* replace delim with terminator and move start to follower */
  if (*start) {
      **start = '\0';
      (*start)++;
  }
  /* done */
  return token;
}

// Function converts .csv files to a string
char * csvToString(char * filePath){
    //Initialize our file stream
    FILE* stream = fopen(filePath, "r");
    char * line = malloc(sizeof(char)* 1024);
    size_t lineSize = 1024;
    int counter = 0;

    //Placeholder char for the next character being read
    char c;
    c = fgetc(stream);

    //Iterate to the end of the file
    while (c != EOF){
        line[counter] = c;
        counter++;
        //If we need more allocated size than we have, double it
        if(counter >= lineSize){
            lineSize *= 2;
            if(!(line = realloc(line, sizeof(char) * lineSize))){
                perror("Realloc failed");
                return NULL;
            }
        }
        c = fgetc(stream);
    }


    if(counter >= lineSize){
        lineSize++;
        if(!(line = realloc(line, sizeof(char) * lineSize))){
            perror("Realloc failed");
            return NULL;
        }
    }
    line[counter] = '\0';

    return(line);
}

// Utilized by value analysis thread to select stocks based on fundamentals
void* valueAnalysis(){
    char * csvString;

    // csvToString returns NULL in case of error or empty file
    if(!(csvString = csvToString("./StockInfo2.csv"))){
        perror("csvToString Failed");
        exit(1);
    }


    char* token;
    char* ticker;
    double peRatio;
    double psRatio;
    double pbRatio;
    char * position;
    // Skip past the introductory column names
    token = strtoke(csvString, "\n", &position);
    // Store ticker
    ticker = strtoke(NULL, ",", &position);

    // Iterate through rows until ticker variable is ""
    while((strcmp(ticker, ""))){
        peRatio = strtod(strtoke(NULL, ",", &position), NULL);
        psRatio = strtod(strtoke(NULL, ",", &position), NULL);
        pbRatio = strtod(strtoke(NULL, "\n", &position), NULL);

        // Exclude tickers with missing information
        if(peRatio == 0 || psRatio == 0 || pbRatio == 0) {
            ticker = strtoke(NULL, ",", &position);
            continue;
        }
        
        // Check if criteria is met. If so, increment score of ticker in global list ---------------REMEMBER TO LOCK LINKED LIST----------------------
        if(peRatio <= 20.00 && psRatio <= 2.00 && pbRatio <= 1.00){
            pthread_mutex_lock(&lock);
            printf("Adding %s to the list (value analysis)\n", ticker);
            tickerObject_t * current = wrapper.head;
            int found = 0;
            //Iterate through the global list of tickers
            while(current){
                if(!(strcmp(ticker, current->ticker))){
                    current->score++;
                    found = 1;
                    break;
                } 
                current = current->next;
            }
            
            // If the list is empty or ticker was not found, create a new node
            if(found == 0){
                tickerObject_t * newNode = malloc(sizeof(tickerObject_t));
                newNode->ticker = ticker;
                newNode->score = 1;
                newNode->next = wrapper.head;
                wrapper.head = newNode; 
            }
            pthread_mutex_unlock(&lock);

            
        }

        // Advance strtoke
        ticker = strtoke(NULL, ",", &position);
    }
    return NULL;
}

//function used by one of the threads to conduct analysis on stock tickers
//using 50-day and 200-day moving averaggggge
void* averageAnalysis(){
    char * csvString;

    // csvToString returns NULL in case of error or empty file
    if(!(csvString = csvToString("./Averages.csv"))){
        perror("csvToString Failed");
        exit(1);
    }

    
    char* token;
    char* ticker;
    double fiftyd;
    double twohundredd;
    char * position;
    
    // Skip past the introductory column names
    token = strtoke(csvString, "\n", &position);
    // Store ticker
    ticker = strtoke(NULL, ",", &position);
    // Iterate through rows until ticker variable is ""
    while((strcmp(ticker, ""))){
        fiftyd = strtod(strtoke(NULL, ",", &position), NULL);
        twohundredd = strtod(strtoke(NULL, "\n", &position), NULL);

        // Exclude tickers with missing information
        if(fiftyd == 0 || twohundredd == 0) {
            ticker = strtoke(NULL, ",", &position);
            continue;
        }
        
        // Check if criteria is met. If so, increment score of ticker in global list ---------------REMEMBER TO LOCK LINKED LIST----------------------
        if(fiftyd > twohundredd){
            pthread_mutex_lock(&lock);

            printf("Adding %s to the list (average analysis)\n", ticker);
            tickerObject_t * current = wrapper.head;
            int found = 0;
            while(current){
                //Iterate through the global list of stocks
                if(!(strcmp(ticker, current->ticker))){
                    current->score++;
                    found = 1;
                    break;
                } 
                current = current->next;
            }
            
            // If the list is empty or ticker was not found, create a new node
            if(found == 0){
                tickerObject_t * newNode = malloc(sizeof(tickerObject_t));
                newNode->ticker = ticker;
                newNode->score = 1;
                newNode->next = wrapper.head;
                wrapper.head = newNode; 
            }
            pthread_mutex_unlock(&lock);
            
        }

        // Advance strtoke
        ticker = strtoke(NULL, ",", &position);
    }

    return NULL;
}

//Function used by one of the threads to conduct analysis
//based on the capital asset pricing model
void * capm() {
        char * csvString;

    // csvToString returns NULL in case of error or empty file
    if(!(csvString = csvToString("./capm.csv"))){
        perror("csvToString Failed");
        exit(1);
    }
    
    char* token;
    char* ticker;
    double fiftytwoweek;
    double beta;
    double expectedReturn;
    char * position;

    // Skip past the introductory column names
    token = strtoke(csvString, "\n", &position);
    // Store ticker
    ticker = strtoke(NULL, ",", &position);

    // Iterate through rows until ticker variable is ""
    while((strcmp(ticker, ""))){
        fiftytwoweek = strtod(strtoke(NULL, ",", &position), NULL);
        beta = strtod(strtoke(NULL, "\n", &position), NULL);

        // Exclude tickers with missing information
        if(fiftytwoweek == 0 || beta == 0) {
            ticker = strtoke(NULL, ",", &position);
            continue;
        }
        
        // Check if criteria is met. If so, increment score of ticker in global list ---------------REMEMBER TO LOCK LINKED LIST----------------------
        expectedReturn = RISKFREE + (beta * (MARKETRETURN - RISKFREE));
        printf("ticker: %s, expected return: %lf, actual return: %lf\n", ticker, expectedReturn, fiftytwoweek);
        if(fiftytwoweek > expectedReturn){
            pthread_mutex_lock(&lock);

            printf("Adding %s to the list (capm analysis)\n", ticker);
            tickerObject_t * current = wrapper.head;
            int found = 0;
            //Iterate through the global list of stocks
            while(current){
                if(!(strcmp(ticker, current->ticker))){
                    current->score++;
                    found = 1;
                    break;
                } 
                current = current->next;
            }
            
            // If the list is empty or ticker was not found, create a new node
            if(found == 0){
                tickerObject_t * newNode = malloc(sizeof(tickerObject_t));
                newNode->ticker = ticker;
                newNode->score = 1;
                newNode->next = wrapper.head;
                wrapper.head = newNode; 
            }
            pthread_mutex_unlock(&lock);

        }

        // Advance strtoke
        ticker = strtoke(NULL, ",", &position);
    }
    
    return NULL;
}

int main() {

    //fork our program so we can launch the python code
    int pid = fork();
    if(pid == -1) {
        perror("fork failed");
    }

    //in the child process, use execvp to transfer to python
    if(pid == 0) {
        //The return here keeps the program from actually pulling data
        //We only have this because yahoo finance is slow 
        //So we did most of our testing on pre-existing data sets
        return 0;
        char* command = "python3";
        char* args[] = {"python3", "project.py", NULL};
        if (execvp(command, args) == -1) {
            perror("execvp() failed");
        }
        return 0;
    }
    //in parent process carry on with the analysis
    else {
        if (waitpid(-1, NULL, 0) == -1) {
            perror("Child process failed");
        }
        //Run the three analysis threads
        pthread_t threads[3];
        pthread_create(&threads[0], NULL, valueAnalysis, NULL);
        pthread_create(&threads[1], NULL, averageAnalysis, NULL);
        pthread_create(&threads[2], NULL, capm, NULL);
        pthread_join(threads[0], NULL);
        pthread_join(threads[1], NULL);
        pthread_join(threads[2], NULL);


    }

    //Placeholder tickerOBject_t to iterate through the global list
    tickerObject_t * iterator = wrapper.head;

    while(iterator != NULL){
        if(iterator->score > 2) {
            printf("Buy: %s\n", iterator->ticker);
        }

        iterator = iterator->next;
    }

    char ch;
    printf("Press ENTER key to Confirm...\n");    
    //here also if you press any other key will wait till pressing ENTER
    scanf("%c",&ch);

    iterator = wrapper.head;


    //Iterate through the global list of tickers
    while(iterator != NULL){
        //If the score is 3 or higher, execute a buy order
        if(iterator->score > 2) {
            printf("forking\n");
            int pid = fork();
            if(pid == -1) {
                perror("fork failed");
            }
            //In child process, launch python script for executing the order
            if (pid == 0) {
                char* command = "python3";
                char* args[4] = {"python3", "buy.py", iterator->ticker, NULL};
                if (execvp(command, args) == -1) {
                    perror("execvp() failed");
                }
            }
        }

        iterator = iterator->next;
    }
    return 0;
}