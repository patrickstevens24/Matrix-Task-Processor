/*
 *  Matrix Task Processor - tasks module
 *  Based on Operating Systems: Three Easy Pieces by R. Arpaci-Dusseau and A. Arpaci-Dusseau
 * 
 *  Assignment 3 code
 *  Program operates on tasks submitted to the tasks_input directory
 *  Results are created in the tasks_output directory
 *
 *  A bounded buffer is used to store pending tasks
 *  A producer thread reads tasks from the tasks_input directory 
 *  Consumer threads perform tasks in parallel
 *  Program is designed to run as a daemon (i.e. forever) until receiving a request to exit.
 *
 *  This program mimics the client/server processing model without the use of any networking constructs.
 *
 *  Wes J. Lloyd
 *  University of Washington, Tacoma
 *  TCSS 422 - Operating Systems
 *  Spring 2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <sys/time.h>
#include "matrix.h"

// Maximum command filename length
#define MAXFILENAMELEN 256

// Maximum absolute filename length
#define FULLFILENAME 2048

// Buffer size for reading commands from command files
// and also the size of the in_dir and out_dir name length
#define BUFFSIZ 80

// MAX should defined the size of the bounded buffer 
#define MAX 200
#define OUTPUT 1

// Define bounded buffer here - use static size of MAX
char * tasks[MAX];

// Define variables for get/put routines
int task_open = MAX;
int task_count = 0;
pthread_cond_t fill = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// task data structure
// used to capture command information
// c - create matrix (saves output as .mat file)
// a - average matrix (saves output as .avg file)
// s - sum matrix (saves output as .sum file)
// d - display matrix : displays matrix to terminal
// r - remove matrix file on disk : removes .mat file from disk
// x - exit program
//
// standard format of commands:
// cmd name row col ele
// cmd - one letter code indicating command
// name - name of matrix file to be created
// row - number of rows
// col - number of cols
// ele - 1-makes every element one, 2-makes elements equal to the column number, 3 to 100- selects a random value up to 100
typedef struct __task_t {
  char * name;
  char cmd;
  int row;
  int col;
  int ele;
} task_t; 

// TO DO
// Implement sleep in ms 
void sleepms(int milliseconds) {

    int sleep = milliseconds * 1000;
    usleep(sleep);
}

// Implement Bounded Buffer put() here
void put(char *var) {
	tasks[task_count] = var;
	task_open--;
	task_count++;
}
// Implement Bounded Buffer get() here
char *get() {
    char * tar = tasks[0];
    for (unsigned int i = 0; i < task_count; i++)
        tasks[i] = tasks[i + 1];
    tasks[task_count] = NULL;
    task_open++;
    task_count--;
    return tar;
}
// This routine continually reads the contents of the "in_dir" to look for 
// command files to process.  Commands are parsed and should be added to the
// bounded buffer...
void *readtasks(void *arg)
{
    // TO DO
    // The sleep duration in ms should be passed in using pthread_create
    // lecture slides from class provide example code
    //

    // CHECK TO SEE IF NULL
    int sleep_ms = (int) arg;
    char in_dir[BUFFSIZ] = "tasks_input";
    DIR* FD = NULL;
    struct dirent* in_file = NULL;
    FILE *entry_file;
    char buffer[BUFFSIZ];

    char cwd[1024];
    if (!(getcwd(cwd, sizeof(cwd)) != NULL))
      fprintf(stderr, "getcwd error\n");

    printf("Processing tasks in dir='%s'\n",in_dir);

    /* Scanning the in directory */
    if (NULL == (FD = opendir (in_dir))) 
    {
        fprintf(stderr, "Error : Failed to open input directory - %s\n", strerror(errno));
        return 1;
    }

    // continuously process the command files in the "in_dir" directory 
    while (1)
    {
        if (FD != NULL)
          in_file = readdir(FD);

        // Close and reopen when we run out of files...
        // This essentially repeats the processing of the "in_dir" forever in an endless loop...
        if ((in_file == NULL) || (in_file == 0))
        {
           if (FD != NULL)
           {
             closedir(FD);
             //implement sleep command in ms here  
             sleepms(sleep_ms);
             FD = NULL;
           }
           if (NULL == (FD = opendir (in_dir))) 
           {
             fprintf(stderr, "Error : Failed to open input directory - %s\n", strerror(errno));
             return 1;
           }
        }
        else
        {
          /* On linux/Unix we don't want current and parent directories
           * On windows machine too, thanks Greg Hewgill
           */
          if (!strcmp (in_file->d_name, "."))    // ignore the present working dir
              continue;
          if (!strcmp (in_file->d_name, ".."))   // ignore the previous dir 
              continue;

          // build an absolute path to the files in the "in_dir" for processing
          char tmpfilename[FULLFILENAME];
          sprintf(tmpfilename,"%s/%s/%s",cwd,in_dir,in_file->d_name);
          printf("full path=%s\n",tmpfilename);
          printf("Read file OPENING: '%s'\n",in_file->d_name);

          // open one file at a time for processing
          entry_file = fopen(tmpfilename, "rw");
          if (entry_file == NULL)
          {
              printf("Unable to open read file %s\n",in_file->d_name);
              fprintf(stderr, "Error : Failed to open entry file - %s\n", strerror(errno));
              return 1;
          }
          printf("read file %s opened\n",in_file->d_name);

          /* Read command file - add command to bounded buffer */
          while (fgets(buffer, BUFFSIZ, entry_file) != NULL)
          {
              // remove newline from buffer string
              strtok(buffer, "\n");
#if OUTPUT
              printf("read form command file='%s'\n",buffer);
#endif

              //
              // TO DO
              //
              // THE NEW COMMAND WILL BE IN "buffer"
              printf("Read the command='%s'\n",buffer);
              
              // First make a copy of the string in the buffer
              char * copy = calloc(sizeof(char), strlen(buffer) + 1);
              strcpy(copy, buffer);

              // Add this copy to the bounded buffer for processing by consumer threads...
              // Use of locks and condition variables and call to put() routine...
              pthread_mutex_lock(&mutex);
              while (task_open == 0)
                pthread_cond_wait(&fill, &mutex); 
              put(copy);
              pthread_cond_signal(&fill);
              pthread_mutex_unlock(&mutex);
          }

          /* When you finish with the file, close it */
          fclose(entry_file);
        }
    }
    // This function never returns as we continously process the "in_dir"...
    return 0;
}

/*
 *  This is a helper routine which parses an int using strtok.
 *  This helper captures the null and returns as a zero int.
 */
int strtokgetint()
{
  char * tmp;
  tmp = strtok(NULL, " ");
  if (tmp != NULL)
    return atoi(tmp);
  else
    return 0;
}


/*
 * This routine parses the command and places it into a new
 * task_t struct for later processing.
 */
task_t *processTask(char * task)
{
  task_t * t = (task_t *) malloc(sizeof(task_t));
  t->cmd = strtok(task," ")[0];
  t->name = strtok(NULL, " ");
  t->row = strtokgetint();
  t->col = strtokgetint();
  t->ele = strtokgetint();
 
#if OUTPUT 
  printf("cmd=%c row=%d col=%d ele=%d\n",t->cmd,t->row,t->col,t->ele);
#endif
  return t;
}

/*
 *  This routine is run by the consumer threads.
 *  It grabs a task from the bounded buffer of commands, 
 *  determines what the command is, and executes it...
 */
void *dotasks(void * arg)
{

  char out_dir[BUFFSIZ] = "tasks_output"; 
  FILE *matrix_file;
  int ** matrix;

  // Implement the consumer thread code
  // The consumer should run forever - constantly performing tasks from the bounded buffer
  // The consumer should cause the program to exit when the 'x' command is received
  while (1)
  {
    //
    // TO DO
    //
    // Read command to perform from the bounded buffer HERE
      //REMOVE ONCE BOUNDED BUFFER

    pthread_mutex_lock(&mutex);
			while (task_count == 0)
				pthread_cond_wait(&fill, &mutex);
			char * task = get();
		pthread_cond_signal(&fill);
		pthread_mutex_unlock(&mutex);
    // create matrix command example
    //sprintf(task, "c a1 20 20 100");
    // display matrix command example
    //sprintf(task, "d a2 10 10 100");
    // sum matrix command example
    //sprintf(task, "s a3 5 5 1");
    // avg matrix command example
    //sprintf(task, "a a4 5 5 1");
    // remove matrix command example
    //sprintf(task, "r a1 20 20 100");
    // exit command example
    //sprintf(task, "x");

    // TO DO
    // Remove this sleep command - it is here for demonstration purposes only
    // For now this puts a 1 sec interval between repeating the same command over and over again 


    printf("***************DO TASK: '%s'\n",task);

    task_t * newtask = processTask(task);
    switch (newtask->cmd)
    { 
      case 'c':
      {
        char cwd[1024];
        if (!(getcwd(cwd, sizeof(cwd)) != NULL))
          fprintf(stderr, "getcwd error\n");
        matrix = AllocMatrix(newtask->row,newtask->col);
        GenMatrixType(matrix,newtask->row, newtask->col, newtask->ele); 
        char tmpfilename[FULLFILENAME];
        sprintf(tmpfilename,"%s/%s/%s.mat",cwd,out_dir,newtask->name);
        matrix_file = fopen(tmpfilename, "w");
        DisplayMatrix(matrix,newtask->row, newtask->col, matrix_file);
        fclose(matrix_file);
        FreeMatrix(matrix,newtask->row,newtask->col);
        break;
      }
      case 'd':
        matrix = AllocMatrix(newtask->row,newtask->col);
        GenMatrixType(matrix,newtask->row, newtask->col, newtask->ele);
        DisplayMatrix(matrix,newtask->row, newtask->col, stdout);
        FreeMatrix(matrix,newtask->row,newtask->col);
        break;
      case 's':
      {
        char cwd[1024];
        if (!(getcwd(cwd, sizeof(cwd)) != NULL))
          fprintf(stderr, "getcwd error\n");
        matrix = AllocMatrix(newtask->row,newtask->col);
        GenMatrixType(matrix,newtask->row, newtask->col, newtask->ele);
        char tmpfilename[FULLFILENAME];
        sprintf(tmpfilename,"%s/%s/%s.sum",cwd,out_dir,newtask->name);
        matrix_file = fopen(tmpfilename, "w");
        fprintf(matrix_file,"sum=%d\n",SumMatrix(matrix,newtask->row,newtask->col)); 
        fclose(matrix_file);
        FreeMatrix(matrix,newtask->row,newtask->col);
        break;
      }
      case 'a':
      {
         char cwd[1024];
        if (!(getcwd(cwd, sizeof(cwd)) != NULL))
          fprintf(stderr, "getcwd error\n");
        matrix = AllocMatrix(newtask->row,newtask->col);
        GenMatrixType(matrix,newtask->row, newtask->col, newtask->ele);
        char tmpfilename[FULLFILENAME];
        sprintf(tmpfilename,"%s/%s/%s.avg",cwd,out_dir,newtask->name);
        matrix_file = fopen(tmpfilename, "w");
        fprintf(matrix_file,"avg=%d\n",AvgElement(matrix,newtask->row,newtask->col)); 
        fclose(matrix_file);
        FreeMatrix(matrix,newtask->row,newtask->col);
        break;
      } 
      case 'r':
      {
        char cwd[1024];
        if (!(getcwd(cwd, sizeof(cwd)) != NULL))
          fprintf(stderr, "getcwd error\n");
        char tmpfilename[FULLFILENAME];
        sprintf(tmpfilename,"%s/%s/%s.mat",cwd,out_dir,newtask->name);
        remove(tmpfilename);
        matrix = NULL;
        break;
      }
      case 'x':
      {
        printf("Received exit command!\n");
        exit(0);
        break;
      }
    }
  }
}



