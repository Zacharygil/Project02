/** @file libscheduler.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libscheduler.h"
#include "../libpriqueue/libpriqueue.h"


/**
  Stores information making up a job to be scheduled including any statistics.
  You may need to define some global variables or a struct to store your job queue elements.
*/


typedef struct _job_t
{
  int pid;
  int priority;
  int arrivalTime;
  int startTime;
  int timeRemaining;
  int running_time;
} job_t;

job_t** current_core;


/**
Global Variables being declared: inlcude
  The Queue
  The comparer
  */
priqueue_t queue;
int initial;
int total_core_num;
int(*comparer)(const void *, const void *);

/**
  First Come First Serve
This scheduler works based on the order in which requests are sent
*/

int fcfs(const void *a, const void *b)
{
  job_t* first = (job_t*)a;
  job_t* second = (job_t*)b;
  if(first->pid == second->pid)
    return 0;
  return first->arrivalTime - second->arrivalTime;
}

/**
  Shortest Job First(sjf)
Shortest job first works based on the burst lengths of the job
Sjf is optimal
*/

int sjf(const void *a, const void *b)
{
  job_t* first = (job_t*)a;
  job_t* second = (job_t*)b;
  if(first->pid == second->pid)
  {
return 0;
  }
  int difference = first->timeRemaining - second->timeRemaining;
  if(difference == 0)
  {
    return first->arrivalTime - second->arrivalTime;
  }
  else
  {
    return difference;
   }
}

int pri(const void *a, const void *b)
{
  job_t* first = (job_t*)a;
  job_t* second = (job_t*)b;
  if(first->pid == second->pid)
  {
    return 0;
  }
  int difference = first->priority - second->priority;
  if(difference == 0)
  {
    return first->arrivalTime - second->arrivalTime;
  }
  else
  {
    return difference;
  }
}

/** 
  Round Robin(rr)
This is first in first out but with preemption, so this means that the quantum will be implemented
*/

int rr(const void *a, const void *b)
{
  job_t* first = (job_t*)a;
  job_t* second = (job_t*)b;
  if(first->pid == second->pid)
  { 
 return 0;
  }
  return -1;
}

/**
Global Variables being declared: inlcude
  The wait time, total time, and response time for the Jobs
  The number of jobs in the queue
  And the current time
  */

float wait_Time;
float turnaround_time;
float reponse_time;
int jobCount;
int currentTime;


/**
  Initalizes the scheduler.
  Assumptions:
    - You may assume this will be the first scheduler function called.
    - You may assume this function will be called once once.
    - You may assume that cores is a positive, non-zero number.
    - You may assume that scheme is a valid scheduling scheme.
  @param cores the number of cores that is available by the scheduler. These cores will be known as core(id=0), core(id=1), ..., core(id=cores-1).
  @param scheme  the scheduling scheme that should be used. This value will be one of the six enum values of scheme_t
*/
void scheduler_start_up(int cores, scheme_t scheme)
{
  //Initialize variables to 0
  wait_Time = 0.0;
  turnaround_time = 0.0;
  reponse_time = 0.0;
  jobCount = 0;
  currentTime = 0;

  total_core_num = cores;

  current_core = malloc(sizeof(job_t) * cores);
  
  int i = 0;
 
  while(i < cores)
  {
    current_core[i] = 0;
    i++;
  }
  //determine which camparer will be used

  switch(scheme)
  {
    case FCFS:  comparer = fcfs; initial = 0; 
break;
    case SJF:   comparer = sjf;  initial = 0; 
break;
    case PSJF: comparer = sjf;  initial = 1; 
break;
    case PRI:   comparer = pri;  initial = 0; 
break;
    case PPRI:  comparer = pri;  initial = 1; 
break;
    case RR:    comparer = rr;   initial = 0; 
break;
  }
  //call queue initialize
  priqueue_init(&queue,comparer);
}


/**
  Called when a new job arrives.
  If multiple cores are idle, the job should be assigned to the core with the
  lowest id.
  If the job arriving should be scheduled to run during the next
  time cycle, return the zero-based index of the core the job should be
  scheduled on. If another job is already running on the core specified,
  this will preempt the currently running job.
  Assumptions:
    - You may assume that every job wil have a unique arrival time.
  @param job_number a globally unique identification number of the job arriving.
  @param time the current time of the simulator.
  @param running_time the total number of time units this job will run before it will be finished.
  @param priority the priority of the job. (The lower the value, the higher the priority.)
  @return index of core job should be scheduled on
  @return -1 if no scheduling changes should be made.
 */
int scheduler_new_job(int job_number, int time, int running_time, int priority)
{
  updateTime(time);

  job_t* job_init = malloc(sizeof(job_t));
  job_init->pid = job_number;
  job_init->arrivalTime = time;
  job_init->startTime = -1;
  job_init->running_time = running_time;
  job_init->timeRemaining = running_time;
  job_init->priority = priority;

  //determine if any cores are idle
  int core = checkIdle();

  if(core != -1)
  {
    job_init->startTime = time;
    current_core[core] = job_init;
    return core;
  }

  if(initial)
  {
    core = findWorstJob(job_init);
    if(core > -1)
    {
      job_t* hold = current_core[core];
      if(time == hold->startTime)
      {
        hold->startTime = -1;
      }
      job_init->startTime = time;
      current_core[core] = job_init;
      priqueue_offer(&queue,hold);
      return core;
    }
  }

  priqueue_offer(&queue,(void*)job_init);

  return -1;
}


/**
  Determine the number of cores in idle
*/
int checkIdle()
{
  int i = 0;
  while(i < total_core_num)
  {
    if(current_core[i] == 0)
      return i;
    i++;
  }
  return -1;
}


/**
  Called when a job has completed execution.
  The core_id, job_number and time parameters are provided for convenience. You may be able to calculate the values with your own data structure.
  If any job should be scheduled to run on the core free'd up by the
  finished job, return the job_number of the job that should be scheduled to
  run on core core_id.
  @param core_id the zero-based index of the core where the job was located.
  @param job_number a globally unique identification number of the job.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled to run on core core_id
  @return -1 if core should remain idle.
 */
int scheduler_job_finished(int core_id, int job_number, int time)
{
  updateTime(time);

  job_t* c_job = current_core[core_id];
  jobCount++;
/**
Update the wait time, turnaround time, and response time
*/
  reponse_time= reponse_time + (c_job->startTime - c_job->arrivalTime);
  wait_Time= wait_Time + (time - c_job->arrivalTime - c_job->running_time);
  turnaround_time= turnaround_time + (time - c_job->arrivalTime);
  

  //remove core for memory leaks
  free(current_core[core_id]);

  current_core[core_id] = 0;
  //check queue size and pop a job to make current job
  if(priqueue_size(&queue) > 0)
  {
    job_t* job = (job_t*)priqueue_poll(&queue);
    if(job->startTime == -1)
    {
      job->startTime = time;
      }
    current_core[core_id] = job;
    //return job number
    return job->pid;
  }

  return -1;
}


/**
  When the scheme is set to RR, called when the quantum timer has expired
  on a core.
  If any job should be scheduled to run on the core free'd up by
  the quantum expiration, return the job_number of the job that should be
  scheduled to run on core core_id.
  @param core_id the zero-based index of the core where the quantum has expired.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled on core cord_id
  @return -1 if core should remain idle
 */
int scheduler_quantum_expired(int core_id, int time)
{
  //reset the remaining time
  updateTime(time);

  job_t *job = current_core[core_id];

  if(priqueue_size(&queue) > 0)
  {
    priqueue_offer(&queue,job);
    job = priqueue_poll(&queue);
    //check to see if it is a new job
    if(job->startTime == -1)
      job->startTime = time;
    current_core[core_id] = job;
  }
  return job->pid;
}


/**
  Returns the average waiting time of all jobs scheduled by your scheduler.
  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average waiting time of all jobs scheduled.
 */
float scheduler_average_waiting_time()
{
  if(jobCount > 0)
  {
    return wait_Time/jobCount;
  }
  return 0.0;
}


/**
  Returns the average turnaround time of all jobs scheduled by your scheduler.
  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time()
{
  if(jobCount > 0)
  {
    return turnaround_time/jobCount;
  }
  return 0.0;
}


/**
  Returns the average response time of all jobs scheduled by your scheduler.
  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average response time of all jobs scheduled.
 */
float scheduler_average_response_time()
{
  if(!initial && comparer != rr)
  {
    return wait_Time/jobCount;
  }
  else
    return reponse_time/jobCount;
}


/**
  Free any memory associated with your scheduler.
  Assumptions:
    - This function will be the last function called in your library.
*/
void scheduler_clean_up()
{
  priqueue_destroy(&queue);
}


/**
  This function may print out any debugging information you choose. This
  function will be called by the simulator after every call the simulator
  makes to your scheduler.
  In our provided output, we have implemented this function to list the jobs in the order they are to be scheduled.
  Furthermore, we have also listed the current state of the job (either running on a given core or idle).
  For example, if we have a non-preemptive algorithm and job(id=4) has began running, job(id=2) arrives with a higher priority,
  and job(id=1) arrives with a lower priority, the output in our sample output will be:
    2(-1) 4(0) 1(-1)
  This function is not required and will not be graded. You may leave it
  blank if you do not find it useful.
 */
void scheduler_show_queue()
{
 
}

void updateTime(int time)
{
  int Difference = (time - currentTime);

  int index = 0;
  while(index < total_core_num)
  {
    if(current_core[index] != 0)
    {
current_core[index]->timeRemaining =  current_core[index]->timeRemaining -Difference;
    }
    index++;
  }
  currentTime = time;
}

int findWorstJob(void* job)
{
  job_t* job_in_use = (job_t*)job;
  int core = -1;

  int index = 0;
  while(index < total_core_num)
  {
    if(comparer(job_in_use,current_core[index]) < 0)
    {
      core = index;
      job_in_use = current_core[index];
    }
    index++;
  }
  return core;
}


