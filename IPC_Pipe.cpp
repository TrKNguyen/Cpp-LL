#include <bits/stdc++.h> 
#include <unistd.h>
using namespace std; 

#define endl "\n" 

void correctImplementation() {
    int fd[2]; 
    pipe(fd); 
    // fd[0] : read 
    // fd[1] : write 
    if (fork() == 0) {
        // child process for read
        close(fd[1]); 
        char buffer[10000]; 
        read(fd[0], buffer, sizeof(buffer));
        close(fd[0]);
        cout << "Child received: " << buffer << endl;
        cout << "Child's fd" << " " << fd[0] << " "  << fd[1] << endl; 
    } else {
        close(fd[0]); 
        
        char message[10000]; 
        message[0] = 'H'; 
        message[1] = 'e'; 
        message[2] = 'l'; 
        message[3] = 'l'; 
        message[4] = 'o';
        // for (int i = 5; i < 10000; i++) message[i] = 'o';
        
        write(fd[1], message, sizeof(message)); 
        close(fd[1]);
        cout << "Parent send:  " << message << endl;
        cout << "Parent's fd" << " " << fd[0] << " " << fd[1] << endl;
    }
}

void incorrectImplementation() {
    int fd[2];
    pipe(fd);
    if(fork() == 0)
    {
        close(fd[0]);
        char *message = "Hello parent!";
        // strlen(str) returns the length of str excluding
        // the end ‘\0’.
        write(fd[1], message, strlen(message)+1);
        close(fd[1]);
    }
    else
    {
        // close(fd[1]); => add this one to make this code correct!
        // reason: 
        // let analysis this code, so normally in IPC Pipe have 2 end: 
        // fd[0]: Read-end of the pipe 
        // fd[1]: Write-end of the pipe
        // Pipe behavior: read(fd[0], buffer, 1024) return 0 when reach EOF ('\0')
        // However, the parent still has fd[1] open 
        // Important things to know: As long as at least 1 process has fd[1] open, the pipe is not considered empty 
        // => read(fd[0]) keep waiting for the next character
        // Cause 0 character in the pipe but the process "hope" that there's still incoming character from open-write process(process parent))
        // => waiting forever at read(fd[0])!
        // got wrong !!!
        char buffer[1024];
        int len;
        do
        {
            // read returns 0 on end-of-file
            len = read(fd[0], buffer, 1024);
            cout << "Message from child: " <<  buffer << endl;
        } while(len>0);
        close(fd[0]);
    }

}

int main() {
    int check = 1; 
    if (check == 0) correctImplementation(); 
    else incorrectImplementation(); 

}