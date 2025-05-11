#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

// Variável global de leitores ativos
int readcount = 0;
int shared = 0;  // Dado compartilhado entre leitores e escritores

// Definição dos semáforos
sem_t mutex;      // Controle da variável readcount
sem_t writeLock;  // Controle do acesso exclusivo do escritor

// Protótipos das funções
void * leitor(void *);
void * escritor(void *);
int gera_rand(int);

int main(int argc, char ** argv)
{
    pthread_t *tl;
    pthread_t *te;
    long i;

    srand(time(NULL));

    if (argc < 3)
    {
        printf("Usage: %s num_leitores num_escritores\n", argv[0]);
        return 0;
    }

    // Número de leitores e escritores
    int N_LEITORES = atoi(argv[1]);
    int N_ESCRITORES = atoi(argv[2]);

    // Inicialização dos semáforos
    sem_init(&mutex, 0, 1);
    sem_init(&writeLock, 0, 1);

    // Criando lista de threads
    tl = malloc(N_LEITORES * sizeof(pthread_t));
    te = malloc(N_ESCRITORES * sizeof(pthread_t));

    // Criando threads dos leitores
    for (i = 0; i < N_LEITORES; i++)
    {
        pthread_create(&tl[i], NULL, leitor, (void *)i);
    }

    // Criando threads dos escritores
    for (i = 0; i < N_ESCRITORES; i++)
    {
        pthread_create(&te[i], NULL, escritor, (void *)i);
    }

    // Aguardando todas as threads finalizarem
    for (i = 0; i < N_LEITORES; i++)
    {
        pthread_join(tl[i], NULL);
    }

    for (i = 0; i < N_ESCRITORES; i++)
    {
        pthread_join(te[i], NULL);
    }

    // Destruindo semáforos
    sem_destroy(&mutex);
    sem_destroy(&writeLock);

    // Liberando memória alocada
    free(tl);
    free(te);

    return 0;
}

void * leitor(void * id)
{
    usleep(gera_rand(1000000));

    long i = (long)id;
    int shared_in;

    printf("> Leitor %ld tentando acesso\n", i);

    // 🔴 Controle de acesso ao contador de leitores
    sem_wait(&mutex);
    readcount++;
    if (readcount == 1)
    {
        sem_wait(&writeLock); // Bloqueia o acesso de escritores
    }
    sem_post(&mutex);

    // 🔵 Leitor acessando o recurso compartilhado
    printf("> Leitor %ld conseguiu acesso\n", i);
    shared_in = shared;
    usleep(gera_rand(1000000));

    printf("\t> Leitor %ld - tmp: %d - shared: %d - readcount: %d\n",
            i, shared_in, shared, readcount);

    if (shared_in != shared)
    {
        printf("\t==== ALERTA DO LEITOR ====\n");
        printf("\t> Valor interno diferente do compartilhado\n\tshared_in: %d - shared: %d\n",
               shared_in, shared);
        printf("\t==========================\n");
    }

    // 🔴 Controle de saída do leitor
    sem_wait(&mutex);
    readcount--;
    if (readcount == 0)
    {
        sem_post(&writeLock); // Libera o acesso de escritores
    }
    sem_post(&mutex);

    printf("< Leitor %ld liberando acesso\n", i);
}

void * escritor(void * id)
{
    usleep(gera_rand(1000000));

    long i = (long)id;

    printf("+ Escritor %ld tentando acesso\n", i);

    // 🔴 Garante exclusividade ao escritor
    sem_wait(&writeLock);

    printf("\t+ Escritor %ld conseguiu acesso\n", i);

    if (readcount > 0)
    {
        printf("==== ALERTA DO ESCRITOR ====\n");
        printf("Readcount possui valor: %d\n", readcount);
        printf("============================\n");
    }

    int rnd = gera_rand(100);
    printf("\t+ Escritor %ld gravando o valor %d em shared\n", i, rnd);
    usleep(gera_rand(1000000));
    shared = rnd;

    sem_post(&writeLock); // 🔵 Libera acesso após escrita
    printf("+ Escritor %ld saindo\n", i);
}

int gera_rand(int limit)
{
    return rand() % limit;
}
