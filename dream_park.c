#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

sem_t moonAsleep, starAsleep;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
bool isMoonAsleep = false;
bool isStarAsleep = false;
int totalSteps = 0;
int currentStep = 0;

void sleep_for_a_while() {
    sleep(rand() % 3 + 1);
}

void* whisper(void* arg) {
    while (currentStep < totalSteps) {
        sleep(1);
        pthread_mutex_lock(&mutex);

        if (isMoonAsleep && isStarAsleep) {
            printf("Whisper: Both Moon and Star are asleep. They're meeting in Dream Park!\n");
        } else if (isMoonAsleep) {
            printf("Whisper: Moon is asleep. Waiting for Star...\n");
        } else if (isStarAsleep) {
            printf("Whisper: Star is asleep. Waiting for Moon...\n");
        }

        if (rand() % 10 == 0) {
            printf("Whisper: Alert! Potential disruption detected!\n");
        }

        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void* moon_routine(void* arg) {
    while (currentStep < totalSteps) {
        printf("Moon is awake.\n");
        sleep_for_a_while();

        pthread_mutex_lock(&mutex);
        isMoonAsleep = true;
        printf("Moon is going to sleep.\n");
        sem_post(&moonAsleep);
        pthread_mutex_unlock(&mutex);

        sleep_for_a_while();

        pthread_mutex_lock(&mutex);
        isMoonAsleep = false;
        printf("Moon is waking up.\n");
        sem_wait(&moonAsleep);
        currentStep++;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void* star_routine(void* arg) {
    while (currentStep < totalSteps) {
        printf("Star is awake.\n");
        sleep_for_a_while();

        pthread_mutex_lock(&mutex);
        isStarAsleep = true;
        printf("Star is going to sleep.\n");
        sem_post(&starAsleep);
        pthread_mutex_unlock(&mutex);

        sleep_for_a_while();

        pthread_mutex_lock(&mutex);
        isStarAsleep = false;
        printf("Star is waking up.\n");
        sem_wait(&starAsleep);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    pthread_t moon_thread, star_thread, whisper_thread;

    printf("Enter the number of steps for the simulation: ");
    scanf("%d", &totalSteps);

    sem_init(&moonAsleep, 0, 0);
    sem_init(&starAsleep, 0, 0);

    pthread_create(&moon_thread, NULL, moon_routine, NULL);
    pthread_create(&star_thread, NULL, star_routine, NULL);
    pthread_create(&whisper_thread, NULL, whisper, NULL);

    pthread_join(moon_thread, NULL);
    pthread_join(star_thread, NULL);
    pthread_join(whisper_thread, NULL);

    sem_destroy(&moonAsleep);
    sem_destroy(&starAsleep);

    printf("Simulation completed after %d steps.\n", totalSteps);

    return 0;
}
