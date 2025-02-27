#include <bits/stdc++.h> 
#include <unistd.h>
using namespace std; 

int main() {
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