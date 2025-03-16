/*
        sem_init - Intialize a semaphore

int sem_init(sem_t *sem, int pshared, unsigned int value); 

Arguments: 
- sem (sem_t*): A pointer to the semaphore object to be intialized. THis 
is typically declared as sem_t sem; and is passed by reference. 

- pshared (int): A flag indicating whether the semaphore is shared between 
processes or threads: 
    + 0: The semaphore is shared between threads of the same process 
    + Non-zero: The semaphore is shared between processes

- value (unsigned int):The intial value of the semaphore, which defines 
the number of "permits" available. A value of 0 means the semaphore is 
initially locked, and a value greater than 0 means there are that many 
available permits

Return value: 
- returns 0 on success or -1 on failures.
*/

/*
        sem_wait - Decrement (lock) a semaphore

int sem_wait(sem_t *sem); 

Arguments: 
- sem (sem_t*): A pointer to the semaphore to be decremented (locked). It
represents the semaphore to be used 

Return value: 
- returns 0 on success or -1 on failture

*/

/*
        sem_post - Increment (unlock) a semaphore

int sem_post(sem_t *sem); 

Arguments: 
- sem (sem_t*): A pointer to the semaphore to be incremented (unlocked). 
This represents the semaphore to signal. 

Return value: 
- returns 0 on success or -1 on failure 

*/

/*
        sem_destroy - Destroy a semaphore

int sem_destroy(sem_t *sem) 

Arguments: 
- sem (sem_t*): A pointer to the semaphore to be destroyed. After calling 
the function, the semaphore should not be used again.

Return value: 
- returns 0 on success or -1 on failure

*/