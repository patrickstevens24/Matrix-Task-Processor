/*
 *  Matrix Task Processor
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
#include <pthread.h>
#include <time.h>
#include "matrix.h"
#include "tasks.h"
#include "pcmatrix.h"

int main (int argc, char * argv[])
{
  // Uncomment to see example operation of the readtasks() routine
  int sleep = 500;
  if(argc == 2) {
      sleep = atoi(argv[1]);
  } 
  // Uncomment to see example operation of the dotasks() routine

  pthread_t p;
  pthread_t c, c1;
  // To do
  // Use pthreads
  // Create one pthread for readtasks()
  pthread_create(&p, NULL, readtasks, sleep);
  // Create one or more pthreads for dotasks()
  pthread_create(&c, NULL, dotasks, NULL);
  pthread_create(&c1, NULL, dotasks, NULL);
  pthread_join(c, NULL);
  pthread_join(c1, NULL);
  pthread_join(p, NULL);
  return 0;
}
