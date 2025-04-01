/* The second program is the producer and allows us to enter data for consumers.
 It's very similar to shm1.c and looks like this. */

 #include <unistd.h>
 #include <stdlib.h>
 #include <stdio.h>
 #include <string.h>
 
 #include <sys/shm.h>
 
 #include "shm_com.h"
 
 #include <sys/sem.h>
 #include "semun.h"
 
 static int set_semvalue(int sem_id);
 static void del_semvalue(void);

 static int down(int sem_id);
 static int up(int sem_id);
 
 int main()
 {
     
    //int running = 1;
    void *shared_memory = (void *)0;
    struct shared_use_st *shared_stuff;
    //char buffer[BUFSIZ];
    int shmid;
    int db, mutex;
     // Criar ou acessar o semáforo
    //sem_id = semget((key_t)1234, 1, 0666 | IPC_CREAT);
    db = semget((key_t)1234, 1, 0666 | IPC_CREAT);
    mutex = semget((key_t)5678, 1, 0666 | IPC_CREAT);


    if (db == -1 || mutex == -1) {
         perror("Erro ao criar/acessar semáforo");
         exit(EXIT_FAILURE);
    }
 
    // Apenas inicializar se o produtor for o primeiro a criar o semáforo
    int sem_exists_db = semctl(db, 0, GETVAL);
    if (sem_exists_db == -1 || sem_exists_db == 0) { // Inicializa se não existir ou se o valor for 0
        if (!set_semvalue(db)) {
            perror("Falha ao inicializar o semáforo db");
            exit(EXIT_FAILURE);
        }
        printf("Semáforo db inicializado com sucesso.\n");
    } else {
        printf("Semáforo db já existente com valor: %d\n", sem_exists_db);
    }

    int sem_exists_mutex = semctl(mutex, 0, GETVAL);
    if (sem_exists_mutex == -1 || sem_exists_mutex == 0) { // Inicializa se não existir ou se o valor for 0
        if (!set_semvalue(mutex)) {
            perror("Falha ao inicializar o semáforo mutex");
            exit(EXIT_FAILURE);
        }
        printf("Semáforo mutex inicializado com sucesso.\n");
    } else {
        printf("Semáforo mutex já existente com valor: %d\n", sem_exists_mutex);
    }
 
    shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0666 | IPC_CREAT);
    if (shmid == -1) {// Depurando esse porra q n vai
        perror("Erro ao criar/acessar memória compartilhada");
        printf("Verifique se a chave já está em uso ou se os recursos do sistema estão disponíveis.\n");
        printf("Tamanho da estrutura: %lu bytes\n", sizeof(struct shared_use_st));
        exit(EXIT_FAILURE);
    }

    shared_memory = shmat(shmid, (void *)0, 0);
    if (shared_memory == (void *)-1) { // tbm depurando esse desgraça
        perror("Erro ao associar memória compartilhada");
        exit(EXIT_FAILURE);
    }
    
 
    printf("Memory attached at %p\n", shared_memory);
 
    shared_stuff = (struct shared_use_st *)shared_memory;
    shared_stuff->written_by_you = 0; // Inicializa o valor de escrita
    printf("Escritor: Valor inicial de written_by_you = %d\n", shared_stuff->written_by_you);

    while (shared_stuff->written_by_you == 1) {
        printf("Escritor: Aguardando o consumidor... (written_by_you = %d)\n", shared_stuff->written_by_you);
        sleep(1);
    }
    while(1) {
        // Espera até que o leitor consuma os dados
        while (shared_stuff->written_by_you == 1) {
            printf("Aguardando o consumidor...\n");
            sleep(1);
        }
         //while(shared_stuff->written_by_you == 1) {
        printf("Escritor: Tentando bloquear o semáforo (down)...\n"); //depuração
        int sem_val_db = semctl(db, 0, GETVAL); // depuração
        printf("Escritor: Valor atual do semáforo db = %d\n", sem_val_db); // depuração
        down(db); //Vai bloquear e deixar exclusivo para escritores
        printf("Escritor: Entrando na seção crítica\n"); //depuração
             //sleep(1);            
             //printf("waiting for client...\n");
         //}
         printf("Enter some text: ");
         //fgets(buffer, BUFSIZ, stdin);
         fgets(shared_stuff->some_text, TEXT_SZ, stdin);
         shared_stuff->written_by_you = 1; // Indica que o escritor está escrevendo

         printf("Escritor: Liberando o semáforo (up)...\n"); //depuração
         up(db); //Libera o semáforo para leitores
         sleep(1); // Simula o tempo de escrita
         
         //strncpy(shared_stuff->some_text, buffer, TEXT_SZ);
         //shared_stuff->written_by_you = 1;
 
        //  if (strncmp(buffer, "end", 3) == 0) {
        //          running = 0;
        //  }
        //  up(sem_id);
     }
 
     shmdt(shared_memory);
     exit(EXIT_SUCCESS);
}
 
 
 /* The function set_semvalue initializes the semaphore using the SETVAL command in a
  semctl call. We need to do this before we can use the semaphore. */
 
static int set_semvalue(int sem_id)
    {
        union semun sem_union;
  
        sem_union.val = 1;
        if (semctl(sem_id, 0, SETVAL, sem_union) == -1) return(0);
        return(1);
    }
 
//   static int down(int sem_id)
//  {
//      struct sembuf sem_b;
     
//      sem_b.sem_num = 0;
//      sem_b.sem_op = -1; /* P() */
//      sem_b.sem_flg = SEM_UNDO;
//      if (semop(sem_id, &sem_b, 1) == -1) {
//          fprintf(stderr, "down failed\n");
//          return(0);
//      }
//      return(1);
//  }

static int down(int sem_id) {
    struct sembuf sem_b = {0, -1, SEM_UNDO};
    return semop(sem_id, &sem_b, 1);
}
 
 /* up is similar except for setting the sem_op part of the sembuf structure to 1,
  so that the semaphore becomes available. */
 
//  static int up(int sem_id)
//  {
//      struct sembuf sem_b;
     
//      sem_b.sem_num = 0;
//      sem_b.sem_op = 1; /* V() */
//      sem_b.sem_flg = SEM_UNDO;
//      if (semop(sem_id, &sem_b, 1) == -1) {
//          fprintf(stderr, "up failed\n");
//          return(0);
//      }
//      return(1);
//  }
 
static int up(int sem_id) {
    struct sembuf sem_b = {0, 1, SEM_UNDO};
    return semop(sem_id, &sem_b, 1);
}
 