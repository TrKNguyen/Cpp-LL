/*
        shmget - Create a shared memory segment 

int shmget(key_t key, size_t size, int shmflg); 

Arguments: 
- key (key_t): A unique identifier for the shared memory segment. This 
is typically generated using the ftok() function. It ensures that 
processes can refer to the same segment 

- size (size_t): The size(in bytes) of the shared memory segment to be 
created or accessed 

- shmflg (int); Flags that control the creation or access permissions. 
It typically includes 
    + IPC_CREAT: Creates a new shared memory segment if one 
    does not exist 
    + IPC_EXCL: Ensures that the segment is created, and 
    fails if the segment already 
    + Access permission(0666, 0644, etc): These specify the read, write, 
    and executed for the shared memory segment (just like file permissions
    in Unix)

Return Value: 
+) Return the shared memory ID or -1 if there is an error
*/

/*
        shmat - Attach shared memory segment to the process's address space 

void* shmat(int shm_id, const void* shmaddr, int shmflg);

Arguments: 
- shm_id (int): The shared memory ID returned by shmget(). THis identifies
the segment to attach 
- shmaddr (const void*): A pointer to specific address at which the shared
memory should be attached. If NULL, the system will automatically choose 
an appropriate addresss 
- shmflg (int): Flags that control the attachment   
    + 0: Standard attachment
    + SHM_RDONLY: Attach the shared memory in read-only mode.

Return value: 
- Returns a pointer to the attached shared memory segment on success 
- Returns (void*)-1 if there is an error, and errno is set appropriately 
*/

/*
        shmdt - Detach a shared memory segment from the process's address space
    
int shmdt(const void* shmaddrr);

Arguments: 
- shmaddr (const void*): The address of the shared memory segment to 
detach. This is the same address returned by shmat() 

Return value: 

- Returns 0 on success or -1 on failure.
*/

/*
        shmctl - Control the shared memory segment(e.g remove it)

int shmctl(int shm_id, int cmd, struct shmid_ds* buf);

Arguments: 
- shm_id (int): The shared memory ID returned by shmget() 
- cmd (int): The command that determines the action to be performed on 
the shared memory segment. Some common commands are: 
    + IPC_RMID: Removes the shared memory segment
    + IPC_STAT: Retrieves information about the segment (fills the shmid_ds structure)
    + IPC_SET: Changes the segment's permissions and other atributes
- buf (struct shmid_ds*): A pointer to a shmid_ds structure that holds 
information about the shared memory segment. It's used with IPC_STAT and 
IPC_SET commands. For IPC_RMID, this argument should NULL. 

Return Value: 
- Return 0 on success or -1 on failure.

*/