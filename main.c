#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define INITIAL_BALANCE 1000
#define NUM_THREADS 10
#define TRANSFER_AMOUNT 50

//Matheus Chediac Rodrigues	10417490
//Leonardo Magalhaes    10417121
//Marcos Arambasic	10443260

// Estrutura para armazenar informações da conta
typedef struct {
    int saldo;
    pthread_mutex_t trava; // Mutex para controle de concorrência
} ContaBancaria;

// Função para depositar na conta
void depositar(ContaBancaria *conta, int quantia) {
    pthread_mutex_lock(&conta->trava); 
    conta->saldo += quantia;
    printf("Depositado: %d, Saldo Atual: %d\n", quantia, conta->saldo);
    pthread_mutex_unlock(&conta->trava); 
}

// Função para sacar da conta
void sacar(ContaBancaria *conta, int quantia) {
    pthread_mutex_lock(&conta->trava); 
    if (conta->saldo >= quantia) {
        conta->saldo -= quantia;
        printf("Sacado: %d, Saldo Atual: %d\n", quantia, conta->saldo);
    } else {
        printf("Saldo Insuficiente para saque: %d\n", quantia);
    }
    pthread_mutex_unlock(&conta->trava);
}

// Função executada pelas threads de depósito
void* transacao_dep(void* arg) {
    ContaBancaria *conta = (ContaBancaria*)arg;

    for (int i = 0; i < 6; i++) {
        depositar(conta, TRANSFER_AMOUNT);
        sleep(1);
    }

    return NULL;
}

// Função executada pelas threads de saque
void* transacao_sac(void* arg) {
    ContaBancaria *conta = (ContaBancaria*)arg;

    for (int i = 0; i < 5; i++) {
        sacar(conta, TRANSFER_AMOUNT);
        sleep(1);
    }

    return NULL;
}

int main() {
    pthread_t threads_dep[NUM_THREADS];
    pthread_t threads_sac[NUM_THREADS];
    ContaBancaria conta;
    conta.saldo = INITIAL_BALANCE;
    pthread_mutex_init(&conta.trava, NULL); 

    // Criação das threads de depósito
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&threads_dep[i], NULL, transacao_dep, &conta) != 0) {
            perror("pthread_create failed");
            exit(EXIT_FAILURE);
        }
    }

    // Criação das threads de saque
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&threads_sac[i], NULL, transacao_sac, &conta) != 0) {
            perror("pthread_create failed");
            exit(EXIT_FAILURE);
        }
    }

    // Espera todas as threads de depósito terminarem
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads_dep[i], NULL) != 0) {
            perror("pthread_join failed");
            exit(EXIT_FAILURE);
        }
    }

    // Espera todas as threads de saque terminarem
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads_sac[i], NULL) != 0) {
            perror("pthread_join failed");
            exit(EXIT_FAILURE);
        }
    }

    // Destrói o mutex
    pthread_mutex_destroy(&conta.trava);

    // Imprime o saldo final
    printf("Saldo Final: %d\n", conta.saldo);

    return 0;
}
