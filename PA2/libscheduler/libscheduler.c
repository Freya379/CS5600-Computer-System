/*
 * Programming Assignment 2
 * CS 5600, Spring 2024
 * Library for Scheduler Code
 * February 6, 2024
 */

#include "libscheduler.h"

/*
 * You may need to define some global variables or a struct to
 * store your job queue elements. Feel free to do it here at the
 * top of the .c file, or in the .h file.
 */

/* See Canvas function descriptions for details of each function */


// Define the structure of job
typedef struct _job{
    int job_number;
    int arrival_time;
    int running_time; // calculate for remain running time when preemptive comes, if it's non preemptive, treat it the regular one
    int priority;
    int init_running_time; // used for preemptive jobs like PPRI
    int response_time;	   // used for both PRI and PPRI
    int start_time;
    struct _job *next;
} job_t;


// define the structure of queue to store jobs
typedef struct _queue{
    job_t* head;
    job_t* tail;
} queue_t;


// Define global variables
queue_t *queue = NULL;
int total_jobs = 0;
int total_waiting_time = 0;
int total_turnaround_time = 0;
int total_response_time = 0;
int scheme = -1;

// Revised version of the create_queue function with error checking
queue_t *create_queue(){
    queue_t *queue = malloc(sizeof(queue_t));
    if (queue == NULL) {
        perror("Failed to allocate memory for queue");
        exit(EXIT_FAILURE);
    }
    queue->head = NULL;
    queue->tail = NULL;
    return queue;
}


int is_queue_empty(queue_t *queue) {
    return queue->head == NULL && queue->tail == NULL;
}

job_t* dequeue(queue_t *queue) {
    if (is_queue_empty(queue)) {
        return NULL;
    }
    job_t *job = queue->head;
    if (job != NULL) {
        queue->head = job->next;
        if (queue->head == NULL) {
            queue->tail = NULL; // Update tail if queue becomes empty
        }
    }
    return job;
}


// Add a job to the end of the queue
void enqueue(queue_t *queue, job_t *job) {
    job->next = NULL;
    if (queue->tail == NULL) {
        // The queue is empty
        queue->head = job;
        queue->tail = job;
    } else {
        queue->tail->next = job;
        queue->tail = job;
    }
}


// Displays each job's ID of the current queue
void scheduler_show_queue(){
    // create the queue if not exist
    if(queue == NULL){
        queue = create_queue();
    }

    // get the head of the job node and iterate them all
    job_t *current = queue->head;
    while (current != NULL){
        printf("%d ", current->job_number);
        current = current->next;
    }
    printf("\n");
}


int scheduler_new_job(int job_number, int time, int running_time, int priority)
{
    // FCFS situation
    if(scheme == FCFS){
        // create a new node when new job comes
        job_t *new_job = (job_t *)malloc(sizeof(job_t));
        new_job->job_number = job_number;
        new_job->arrival_time = time;
        new_job->running_time = running_time;
        new_job->priority = priority;
        new_job->next = NULL;

        // Append new job to the end of the queue
        if (queue->head == NULL)
        {
            queue->head = new_job;
        }
        else
        {
            job_t *current = queue->head;
            while (current->next != NULL)
            {
                current = current->next;
            }
            current->next = new_job;
        }

        // add the total number of jobs
        total_jobs++;
        return queue->head->job_number;
    }

    // SJF Situation
    else if (scheme == SJF)
    {
        // Create a new job node
        job_t *new_job = (job_t *)malloc(sizeof(job_t));
        new_job->job_number = job_number;
        new_job->arrival_time = time;
        new_job->running_time = running_time;
        new_job->priority = priority; // Assuming priority is kept for consistency with other parts of the code
        new_job->start_time = time;
        new_job->next = NULL;

        // If the queue is empty, add the new job as the head
        if (queue->head == NULL) {
            queue->head = new_job;
        }

            // If the queue is not empty
        else {

            // If the head's arrival time is the same as the new job's arrival time
            if (queue->head->arrival_time == time && running_time < queue->head->running_time)
            {
                // New job has a shorter running time, place it before the current head
                new_job->next = queue->head;
                queue->head = new_job;
            }

            else
            {
                // Head's arrival time is ahead of the new job, implying the head is already running
                // We need to compare the new job with the rest of the jobs after head
                job_t *current = queue->head;
                while (current->next != NULL && current->next->running_time < running_time) {
                    current = current->next;
                }
                // Insert the new job after finding the right spot based on running time
                new_job->next = current->next;
                current->next = new_job;
            }
        }

        total_jobs++;
        return queue->head->job_number;
    }


    // PSJF situation
    if (scheme == PSJF)
    {
        // Create a new job node
        job_t *new_job = (job_t *)malloc(sizeof(job_t));
        new_job->job_number = job_number;
        new_job->arrival_time = time;
        new_job->running_time = running_time; // This is the remaining running time
        new_job->init_running_time = running_time; // This is the whole running time for preemptive adjustments
        new_job->priority = priority; // Assuming priority is kept for consistency with other parts of the code
        new_job->start_time = time;
        new_job->next = NULL;

        // If the queue is empty, add the new job as the head
        if (queue->head == NULL) {
            queue->head = new_job;
        }
        else {
            // Check if the new job should preempt the current head
            int temp_remain_time = queue->head->running_time - (time - queue->head->start_time);
            if (running_time < temp_remain_time) {
                // Preempt the current head
                queue->head->running_time = temp_remain_time;
                job_t *temp = queue->head;
                queue->head = new_job;
                queue->head->next = temp;
            } 
            else {
                job_t *current = queue->head;
                while (current->next != NULL && running_time > current->next->running_time)
                {
                    current = current->next;
                }
                job_t *temp = current->next;
                current->next = new_job;
                new_job->next = temp;
            }
        }

        total_jobs++;
        return queue->head->job_number;
    }


    // PRI situation
    else if (scheme == PRI)
    {
        // create a new job nodes
        job_t *new_job = malloc(sizeof(job_t));
        new_job->job_number = job_number;
        new_job->arrival_time = time;
        new_job->running_time = running_time;
        new_job->priority = priority;
        new_job->next = NULL;

        // set the reponding time to 0 for the REAL head, other jobs' will be replaced later
        new_job->response_time = 0;

        // queue is empty
        if (queue->head == NULL)
        {
            // add head to the queue
            queue->head = new_job;
        }

        // compare new job's priority with all jobs in queue
        else
        {
            // head and new job have the same arrival time
            if (queue->head->arrival_time == time && queue->head->priority > priority)
            {
                job_t *temp = queue->head;
                queue->head = new_job;
                queue->head->next = temp;
            }

            // compare with the rest jobs in queue
            else
            {
                job_t *current = queue->head;
                int insert = 0;
                while (current->next != NULL)
                {
                    job_t *next_job = current->next;

                    // the smaller number, the bigger priority
                    if (priority < next_job->priority)
                    {
                        current->next = new_job;
                        new_job->next = next_job;
                        insert = 1;
                        break;
                    }
                    else
                    {
                        current = next_job;
                    }
                }

                // insert failed, means the new job is low priority one, just add to the last
                if (insert == 0)
                {
                    current->next = new_job;
                }
            }
        }

        // change the total number of jobs
        total_jobs++;
        return queue->head->job_number;
    }


    // PPRI situation
    else if (scheme == PPRI)
    {
        // create a new job nodes
        job_t *new_job = malloc(sizeof(job_t));
        new_job->job_number = job_number;
        new_job->arrival_time = time;
        new_job->running_time = running_time; // can be changed here as the remain running time
        new_job->priority = priority;
        new_job->init_running_time = running_time; // keep the initial running time
        new_job->next = NULL;

        // set the reponding time to 0 for the REAL head, other jobs' will be replaced later
        new_job->response_time = 0;

        // queue is empty
        if (queue->head == NULL)
        {
            // add head to the queue
            queue->head = new_job;
        }

        // when job exists in queue, compare their priority
        else
        {
            // the new job's priority is advanced than head
            if(priority < queue->head->priority){
                job_t *temp = queue->head;

                // change the old head's remaining runing time
                temp->running_time -= time - temp->arrival_time;

                // change the order of jobs in queue
                queue->head = new_job;
                queue->head->next = temp;
            }

            // the job's priority is not advanced than the head
            else{
                job_t *current = queue->head;
                int insert = 0;
                while (current->next != NULL)
                {
                    job_t *next_job = current->next;

                    // the smaller number, the bigger priority
                    if (priority < next_job->priority)
                    {
                        current->next = new_job;
                        new_job->next = next_job;
                        insert = 1;
                        break;
                    }
                    else
                    {
                        current = next_job;
                    }
                }

                // insert failed, means the new job is a low priority one, just add to the last
                if (insert == 0)
                {
                    current->next = new_job;
                }
            }
        }

        // change the total number of jobs
        total_jobs++;
        return queue->head->job_number;
    }

    // RR
    else if (scheme == RR){
        job_t *new_job = (job_t *)malloc(sizeof(job_t));
        new_job->job_number = job_number;
        new_job->arrival_time = time;
        new_job->running_time = running_time;
        new_job->start_time = 0;
        new_job->init_running_time = running_time;
        new_job->priority = priority;
        new_job->next = NULL;
        new_job->response_time = 0;

        // Add the new job to the end of the queue for RR scheduling
        if (queue->head == NULL) {
            queue->head = new_job;
            queue->tail = new_job; // Assuming a tail pointer exists for efficiency
        } else {
            queue->tail->next = new_job;
            queue->tail = new_job;
        }
        scheduler_show_queue();
        total_jobs++;
        return queue->head->job_number;
    }

    return -1;
}

int scheduler_job_finished(int job_number, int time)
{
    // FCFS situation
    if(scheme == FCFS){
        if (queue->head != NULL && queue->head->job_number == job_number)
        {
            job_t *finished_job = queue->head;
            queue->head = queue->head->next;

            // Calculate statistical data for the finished job
            int job_waiting_time = time - finished_job->arrival_time - finished_job->running_time;
            int job_turnaround_time = time - finished_job->arrival_time;
            int job_response_time = job_waiting_time;

            // Update total statistical data
            total_waiting_time += job_waiting_time;
            total_turnaround_time += job_turnaround_time;
            total_response_time += job_response_time;

            free(finished_job);

            return (queue->head != NULL) ? queue->head->job_number : -1;
        }
    }

    // SJF situation
    else if (scheme == SJF)
    {
        if (queue->head != NULL && queue->head->job_number == job_number)
        {
            job_t *finished_job = queue->head;
            queue->head = queue->head->next;

            // Calculate statistical data for the finished job
            int job_turnaround_time = time - finished_job->arrival_time;
            int job_waiting_time = job_turnaround_time - finished_job->running_time;
            int job_response_time = finished_job->start_time - finished_job->arrival_time; 

            // Update total statistical data
            total_turnaround_time += job_turnaround_time;
            total_waiting_time += job_waiting_time;
            total_response_time += job_response_time;

            // set the start time
            if(queue->head != NULL){
                queue->head->start_time = time;
            }

            free(finished_job);

            // Return the job number of the next job to run, or -1 if the queue is empty
            return (queue->head != NULL) ? queue->head->job_number : -1;
        }
    }

    // PSJF situation
    else  if (scheme == PSJF)
    {
        if (queue->head != NULL && queue->head->job_number == job_number)
        {
            job_t *finished_job = queue->head;
            queue->head = queue->head->next;

            int job_turnaround_time = time - finished_job->arrival_time;
            int job_waiting_time = job_turnaround_time - finished_job->init_running_time;
            int job_response_time = finished_job->start_time - finished_job->arrival_time;

            total_turnaround_time += job_turnaround_time;
            total_waiting_time += job_waiting_time;
            total_response_time += job_response_time;

            // set the start time
            if (queue->head != NULL && queue->head->init_running_time == queue->head->running_time)
            {
                queue->head->start_time = time; 
            }

            free(finished_job);
            return (queue->head != NULL) ? queue->head->job_number: -1;
        }
    }


    // PRI situation
    else if (scheme == PRI)
    {
        if (queue->head != NULL && queue->head->job_number == job_number)
        {
            job_t *finished_job = queue->head;
            queue->head = queue->head->next;

            // Calculate statistical data for the finished job
            int job_turnaround_time = time - finished_job->arrival_time;
            int job_waiting_time = job_turnaround_time - finished_job->running_time;
            int job_response_time = finished_job->response_time;

            // Update total statistical data
            total_turnaround_time += job_turnaround_time;
            total_waiting_time += job_waiting_time;
            total_response_time += job_response_time;

            // calculate the responding time of next head
            if(queue->head != NULL){
                queue->head->response_time = time - queue->head->arrival_time;
            }


            free(finished_job);

            return (queue->head != NULL) ? queue->head->job_number : -1;
        }
    }

    // PPRI situation
    else if (scheme == PPRI)
    {
        if (queue->head != NULL && queue->head->job_number == job_number)
        {
            job_t *finished_job = queue->head;
            queue->head = queue->head->next;

            // Calculate statistical data for the finished job
            int job_turnaround_time = time - finished_job->arrival_time;
            int job_waiting_time = job_turnaround_time - finished_job->init_running_time;
            int job_response_time = finished_job->response_time;

            // Update total statistical data
            total_turnaround_time += job_turnaround_time;
            total_waiting_time += job_waiting_time;
            total_response_time += job_response_time;

            // when it's turn to the new job which never execute before, change the responding time
            if(queue->head != NULL && queue->head->init_running_time == queue->head->running_time){
                queue->head->response_time = time - queue->head->arrival_time;
            }

            free(finished_job);

            return (queue->head != NULL) ? queue->head->job_number : -1;
        }
    }

    else if (scheme == RR) {
        if (queue->head != NULL && queue->head->job_number == job_number)
        {

            job_t* finished_job = dequeue(queue);

            // Calculate statistical data for the finished job
            int job_turnaround_time = time - finished_job->arrival_time;
            int job_waiting_time = time - finished_job->arrival_time - finished_job->init_running_time;
            int job_response_time = finished_job->start_time - finished_job->arrival_time;

            // Update total statistical data
            total_turnaround_time += job_turnaround_time;
            total_waiting_time += job_waiting_time;
            total_response_time += job_response_time;



            free(finished_job);
            if (queue->head){
                if (queue->head->start_time == 0 && queue->head->job_number != 0) {
                    queue->head->start_time = time ;
                }

            }
            return (queue->head != NULL) ? queue->head->job_number : -1;
        }
    }


    return -1;
}

int scheduler_quantum_expired(int time) {
    if (scheme != RR || is_queue_empty(queue)) {
        return -1;
    }

    job_t* current_job = queue->head;


    if (current_job->running_time <= 0) {
        job_t* finished_job = dequeue(queue);
        // Calculate statistical data for the finished job
        int job_waiting_time = time - finished_job->arrival_time - finished_job->init_running_time;
        int job_turnaround_time = time - finished_job->arrival_time;
        int job_response_time = finished_job->start_time - finished_job->arrival_time;

        // Update total statistical data
        total_waiting_time += job_waiting_time;
        total_turnaround_time += job_turnaround_time;
        total_response_time += job_response_time;
        free(finished_job);
    } else {
        job_t* rotated_job = dequeue(queue);
        enqueue(queue, rotated_job);
    }

    if (!is_queue_empty(queue)) {
        job_t* next_job = queue->head;
        if (queue->head->start_time == 0 && queue->head->job_number != 0) {
            next_job->start_time = time ;
        }
        return next_job->job_number;
    } else {
        printf("No more jobs to run.\n");
        return -1;
    }
}



double scheduler_average_waiting_time()
{
    if (total_jobs > 0)
    {
        return (double)total_waiting_time / total_jobs;
    }
    return 0.0;

}

double scheduler_average_turnaround_time()
{
    if (total_jobs > 0)
    {
        return (double)total_turnaround_time / total_jobs;
    }
    return 0.0;
}

double scheduler_average_response_time()
{
    if (total_jobs > 0)
    {
        return (double)total_response_time / total_jobs;
    }
    return 0.0;
}

void scheduler_clean_up()
{
    while (queue->head != NULL)
    {
        job_t *temp = queue->head;
        queue->head = queue->head->next;
        free(temp);
    }
    free(queue);
}
