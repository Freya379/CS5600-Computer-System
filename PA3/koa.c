#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define ROOM_CAPACITY 3
#define GENDER_LIMIT 2
#define DELAY 5 // Time each person spends in the restroom (in seconds)


pthread_mutex_t room_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t men_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t women_cond = PTHREAD_COND_INITIALIZER;

int num_men = 0;
int num_women = 0;
int waiting_men = 0;
int waiting_women = 0;
int departure_index = 0;


void Arrive(int id, int gender) {
    pthread_mutex_lock(&room_mutex);

    if (gender == 0) {
        waiting_men++;
    } else {
        waiting_women++;
    }

    while ((gender == 0 && (num_women > 0 || (num_men >= GENDER_LIMIT && waiting_women > 0))) ||
           (gender == 1 && (num_men > 0 || (num_women >= GENDER_LIMIT && waiting_men > 0))) ||
           (num_men + num_women == ROOM_CAPACITY)) {
        if (gender == 0) {
            printf("Man %d waiting to enter...\n", id);
            pthread_cond_wait(&men_cond, &room_mutex);
        } else {
            printf("Woman %d waiting to enter...\n", id);
            pthread_cond_wait(&women_cond, &room_mutex);
        }
    }

    if (gender == 0) {
        num_men++;
        waiting_men--;
        printf("Man %d entered the restroom.\n", id);
    } else {
        num_women++;
        waiting_women--;
        printf("Woman %d entered the restroom.\n", id);
    }

    pthread_mutex_unlock(&room_mutex);
}


void UseFacilities(int id, int gender, int time) {
    printf("Restroom status: %d men, %d women\nWaiting line: men %d, women %d\n", num_men, num_women, waiting_men, waiting_women);
    sleep(time);
    printf("Person %d (gender %d) has used the facilities.\n", id, gender);
}


void Depart(int id, int gender) {

    pthread_mutex_lock(&room_mutex);
    int idx = 0;

    if (gender == 0) {
        num_men--;
        if(waiting_women > 0){
            if(num_men == 0 && waiting_women == 1){
                pthread_cond_signal(&women_cond); // Signal to a waiting woman
            }else if(num_men == 0 && waiting_women >= 2){
                while(idx < GENDER_LIMIT){
                    pthread_cond_signal(&women_cond); // Signal to a waiting woman
                    idx++;
                }
            }
        }else{
            while(idx + num_men < ROOM_CAPACITY){
                pthread_cond_signal(&men_cond); // Signal to a waiting man
                idx++;
            }
        }
    } else {
        num_women--;
        if(waiting_men > 0){
            if(num_women == 0 && waiting_men == 1){
                pthread_cond_signal(&men_cond); // Signal to a waiting man
            }else if(num_women == 0 && waiting_men >= 2){
                while(idx < GENDER_LIMIT){
                    pthread_cond_signal(&men_cond); // Signal to a waiting man
                    idx++;
                }
            }
        }else{
            while(idx + num_women < ROOM_CAPACITY){
                pthread_cond_signal(&women_cond); // Signal to a waiting woman
                idx++;
            }
        }
    }

    departure_index++;
    printf("Person %d (gender %d) has departed. Departure index: %d\n", id, gender, departure_index);

    pthread_mutex_unlock(&room_mutex);

}



void* OnePerson(void* arg) {
    int id = *(int*)arg;
    free(arg);

    int gender = drand48() < 0.6 ? 1 : 0; // Determine gender randomly
    int time = DELAY; // Assume every person spends DELAY seconds in the restroom

    Arrive(id, gender);
    UseFacilities(id, gender, time);
    Depart(id, gender);

    return NULL;
}

int main() {
    srand48(time(NULL)); // Seed the random number generator

    pthread_t threads[20];
    int ids[20];

    // Schedule (i)
    printf("Schedule (i): 5 : DELAY(10) : 5 : DELAY(10) : 5 : DELAY(10) : 5\n");
    for (int i = 0; i < 5; i++) {
        ids[i] = i + 1;
        int* id_ptr = malloc(sizeof(int));
        *id_ptr = ids[i];
        pthread_create(&threads[i], NULL, OnePerson, id_ptr);
    }
    sleep(10);
    for (int i = 5; i < 10; i++) {
        ids[i] = i + 1;
        int* id_ptr = malloc(sizeof(int));
        *id_ptr = ids[i];
        pthread_create(&threads[i], NULL, OnePerson, id_ptr);
    }
    sleep(10);
    for (int i = 10; i < 15; i++) {
        ids[i] = i + 1;
        int* id_ptr = malloc(sizeof(int));
        *id_ptr = ids[i];
        pthread_create(&threads[i], NULL, OnePerson, id_ptr);
    }
    sleep(10);
    for (int i = 15; i < 20; i++) {
        ids[i] = i + 1;
        int* id_ptr = malloc(sizeof(int));
        *id_ptr = ids[i];
        pthread_create(&threads[i], NULL, OnePerson, id_ptr);
    }

    for (int i = 0; i < 20; i++) {
        pthread_join(threads[i], NULL);
    }

    // Schedule (ii)
    printf("\nSchedule (ii): 10 : DELAY(10) : 10\n");
    for (int i = 0; i < 10; i++) {
        ids[i] = i + 1;
        int* id_ptr = malloc(sizeof(int));
        *id_ptr = ids[i];
        pthread_create(&threads[i], NULL, OnePerson, id_ptr);
    }
    sleep(10);
    for (int i = 10; i < 20; i++) {
        ids[i] = i + 1;
        int* id_ptr = malloc(sizeof(int));
        *id_ptr = ids[i];
        pthread_create(&threads[i], NULL, OnePerson, id_ptr);
    }

    for (int i = 0; i < 20; i++) {
        pthread_join(threads[i], NULL);
    }

    // Schedule (iii)
    printf("\nSchedule (iii): 20\n");
    for (int i = 0; i < 20; i++) {
        ids[i] = i + 1;
        int* id_ptr = malloc(sizeof(int));
        *id_ptr = ids[i];
        pthread_create(&threads[i], NULL, OnePerson, id_ptr);
    }

    for (int i = 0; i < 20; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}