#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

// Definição dos semáforos globais
sem_t mutex;  // Controle de acesso global para evitar condição de corrida
sem_t chopsticks[5];  // Semáforo individual para cada talher

// Lista de quem está usando cada talher
int *chopstick_use;

// Número de filósofos
int N_FILOS;

// Protótipos das funções
void * filosofo(void *);
void pegar(int, int);
void liberar(int, int);
int gera_rand(int);

int main(int argc, char ** argv)
{
    pthread_t *tids;
    long i;

    srand(time(NULL));

    if (argc < 2)
    {
        printf("Usage: %s num_filosofos\n", argv[0]);
        return 0;
    }

    // Número de filósofos
    N_FILOS = atoi(argv[1]);

    // Inicializando semáforo global
    sem_init(&mutex, 0, 1);

    // Inicializando semáforos para cada talher
    for (i = 0; i < N_FILOS; i++)
    {
        sem_init(&chopsticks[i], 0, 1);
    }

    // Criando lista de threads de filósofos
    tids = malloc(N_FILOS * sizeof(pthread_t));
    chopstick_use = malloc(N_FILOS * sizeof(int));

    // Inicializando os talheres como disponíveis (-1)
    for (i = 0; i < N_FILOS; i++)
    {
        chopstick_use[i] = -1;
    }

    // Criando as threads dos filósofos
    for (i = 0; i < N_FILOS; i++)
    {
        pthread_create(&tids[i], NULL, filosofo, (void *)i);
    }

    // Aguardando que todas as threads finalizem
    for (i = 0; i < N_FILOS; i++)
    {
        pthread_join(tids[i], NULL);
    }

    // Destruindo semáforos
    sem_destroy(&mutex);
    for (i = 0; i < N_FILOS; i++)
    {
        sem_destroy(&chopsticks[i]);
    }

    // Liberando memória alocada
    free(tids);
    free(chopstick_use);

    return 0;
}

void * filosofo(void * id)
{
    long i = (long)id;

    printf("\t> Filosofo %ld pensando\n", i);
    usleep(gera_rand(1000000));

    int c1, c2;
    if (i % 2 == 0)
    {
        c1 = i;
        c2 = (i + 1) % N_FILOS;
    }
    else
    {
