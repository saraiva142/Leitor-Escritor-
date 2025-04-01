/* Our first program is a consumer. After the headers the shared memory segment
 (the size of our shared memory structure) is created with a call to shmget,
 with the IPC_CREAT bit specified. */

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
    int mutex, db;
    int running = 1;
    void *shared_memory = (void *)0;
    struct shared_use_st *shared_stuff;
    int shmid;

     // Criar ou acessar o semáforo
    //  db = semget((key_t)1234, 1, 0666 | IPC_CREAT);
    //  mutex = semget((key_t)5678, 1, 0666 | IPC_CREAT);
     db = semget((key_t)1234, 1, 0666 );
     mutex = semget((key_t)5678, 1, 0666 );

     if (mutex == -1 || db == -1) {
         perror("Erro ao criar/acessar semáforo(s)");
         exit(EXIT_FAILURE);
        }

    // if (!set_semvalue(db)) {
    //     perror("Falha ao inicializar o semáforo db");
    //     exit(EXIT_FAILURE);
    // }
        
    //     // Inicializar o semáforo mutex
    //     if (!set_semvalue(mutex)) {
    //     perror("Falha ao inicializar o semáforo mutex");
    //     exit(EXIT_FAILURE);
    // }
 
     srand((unsigned int)getpid());    

     // memoria comartilhada <- acesso
 
     //shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0666 | IPC_CREAT);
     shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0666);
     if (shmid == -1) {
        perror("Erro ao acessar memória compartilhada");
        exit(EXIT_FAILURE);
    }
 /* We now make the shared memory accessible to the program. */
 
     shared_memory = shmat(shmid, (void *)0, 0);
 
     printf("Memory attached at %X\n", (int)shared_memory);
 
 /* The next portion of the program assigns the shared_memory segment to shared_stuff,
  which then prints out any text in written_by_you. The loop continues until end is found
  in written_by_you. The call to sleep forces the consumer to sit in its critical section,
  which makes the producer wait. */
 
     shared_stuff = (struct shared_use_st *)shared_memory;
     //shared_stuff->written_by_you = 0;
     while(1) {
            //if (shared_stuff->written_by_you) { // Verdadeiro quando alguém escreveu
            printf("Leitor: Tentando bloquear o semáforo mutex (down)...\n");//depuração 
            int sem_val_mutex = semctl(mutex, 0, GETVAL); // depuração
            printf("Leitor: Valor atual do semáforo mutex = %d\n", sem_val_mutex); // depuração
            down(mutex);
             printf("Leitor: Entrando na seção crítica (nl = %d)\n", shared_stuff->nl);//Depuração
             shared_stuff->nl++;
             if (shared_stuff->nl == 1) {
                 down(db); // O primeiro leitor nesse caso vai bloquear os escritores
             }
             printf("Leitor: Liberando o semáforo mutex (up)...\n");
             up(mutex);
             //printf("You wrote: %s", shared_stuff->some_text); 
             //sleep( rand() % 4 ); /* make the other process wait for us ! */
             //Leitura do compartilhado
             printf("Leitor: Valor inicial de written_by_you = %d\n", shared_stuff->written_by_you); //depuração 

             if (shared_stuff->written_by_you == 1) {
                printf("Leitor: Consumindo dados...\n");
                printf("You wrote: %s", shared_stuff->some_text);
                shared_stuff->written_by_you = 0; // Vai passar a vez para o escritor
                printf("Leitor: Atualizou written_by_you para 0.\n"); // depuraçãoooo

            }
             //sleep( 1 ); /* make the other process wait for us ! */
             down(mutex);
             //shared_stuff->written_by_you = 0; //Vai passar a vez para o escritor
             shared_stuff->nl--;
            //  if (strncmp(shared_stuff->some_text, "end", 3) == 0) {
            //      running = 0;
            //  }
            if (shared_stuff->nl == 0) {
                printf("Leitor: Liberando o semáforo db (up)...\n");//depuração
                up(db); // O último leitor nesse caso vai liberar os escritores
            }
             up(mutex);

             sleep(1); /* make the other process wait for us ! */
         //}
     }
 
 /* Lastly, the shared memory is detached and then deleted. */
 
     shmdt(shared_memory); 
 
     shmctl(shmid, IPC_RMID, 0); 
 
     exit(EXIT_SUCCESS);
 }
 
 
 
 /* The function set_semvalue initializes the semaphore using the SETVAL command in a
  semctl call. We need to do this before we can use the semaphore. */
 
static int set_semvalue(int sem_id)
{
    union semun sem_union;
  
    sem_union.val = 0;
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
 
 