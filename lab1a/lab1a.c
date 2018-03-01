/*
NAME: Reinaldo Daniswara
EMAIL: rdaniswara@g.ucla.edu
ID: 604840665
*/

#define _POSIX_SOURCE
#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

#define BUFFER_SIZE 512

//to remember original terminal attributes
struct termios original_attributes;

int pipe_1[2], pipe_2[2];
struct pollfd pfds[2];

void reset_input_mode (void)
{
    tcsetattr(STDIN_FILENO, TCSANOW, &original_attributes);
}

void termination_info (int status)
{
    if (WIFSIGNALED(status))
            fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d \n", WTERMSIG(status), WEXITSTATUS(status));
        else
            fprintf(stderr, "SHELL EXIT STATUS=%d \n", WEXITSTATUS(status));
    exit(0);
}
void  ChildProcess(void){
    pid_t p_pid;
    p_pid = getpid();
    close(pipe_1[1]);
    close(pipe_2[0]);
    dup2(pipe_1[0], 0);     //input
    dup2((pipe_2[1]), 1);   //output
    dup2((pipe_2[1]), 2);   //error
    close(pipe_1[0]);
    close(pipe_2[1]);
    char* argv_exe[2];
    argv_exe[0] = "/bin/bash";
    argv_exe[1] = NULL;
    int tes = execvp(argv_exe[0], argv_exe);
    if (tes == -1){
        fprintf(stderr, "execvp error\n");
        kill(p_pid, SIGKILL);
        exit(1);
    }
}
void  RunPoll(void){
    pfds[0].fd = 0;
    pfds[0].events = POLLIN;
    pfds[1].fd = pipe_2[0];
    pfds[1].events = POLLIN;
    close(pipe_1[0]);
    close(pipe_2[1]);
}

int main(int argc, char* argv[]) {
   
    int opt;
    int shell_flag = 0;
    pid_t pid, end_ID;
    struct termios tattr;
    char char_buffer[2] = {'\r', '\n'};
    static struct option long_opts[] =
    {
        {"shell", no_argument, 0, 's'},
        {0, 0 , 0, 0}
    };
    
    while((opt = getopt_long(argc, argv, "s", long_opts, NULL)) >= 0)
          {
              switch (opt) {
                  case 's':
                      
                      shell_flag = 1;
                      break;
                      
                  default:
                      break;
              }
          }
    
    // Make sure stdin is a terminal. */
    if (!isatty (STDIN_FILENO))
    {
        fprintf (stderr, "Not a terminal.\n");
        exit (EXIT_FAILURE);
    }
    
    //    if(tcgetattr(STDIN_FILENO, &original_attributes) == -1){
    //   fprintf(stderr, "Tcgetattr error!");
    //    exit(1);
    // }
    
    // Save the terminal attributes so we can restore them later. */
    tcgetattr (STDIN_FILENO, &original_attributes);
    atexit (reset_input_mode);
    
    // Set the funny terminal modes.
    tcgetattr (STDIN_FILENO, &original_attributes);
    tattr.c_iflag = ISTRIP;
    tattr.c_oflag = 0;
    tattr.c_lflag = 0;
    tcsetattr (STDIN_FILENO, TCSANOW, &tattr);
    
    //forking
    if (shell_flag)
    {
        //create pipe_1 and pipe_2
        if (pipe(pipe_1) < 0 || pipe(pipe_2) < 0){
            perror("Can't create pipes");
            exit(1);
        }
        
        pid = fork();
        if (pid < 0){
            fprintf(stderr, "Fail to fork");
            exit(1);
        }
        else if (pid == 0)
                ChildProcess();
        else{
            RunPoll();
            while(1){
                if (poll(pfds, 2, 0) == -1){
                    
                        printf("ERROR! Polling error: %s\n", strerror(errno));
                        kill(pid, SIGKILL);
                        exit(1);
                    
                }
                if (pfds[0].revents & (POLLHUP + POLLERR)) {
                    fprintf(stderr, "Keyboard error");
                    reset_input_mode();
                    break;
                    
                }
                //Resource from README waitpid
                if(pfds[1].revents & (POLLERR + POLLHUP)){
                    close(pipe_1[1]);
                    close(pipe_2[0]);
                    int status;
                    end_ID = waitpid(pid, &status, WNOHANG|WUNTRACED);
                    if (end_ID == -1){
                        perror("waitpid error");
                        exit(1);
                    }
                    if (end_ID == pid){
                        reset_input_mode();
                        termination_info(status);
                        exit(0);
                    }
                }
                
                if (pfds[0].revents & POLLIN){
                    char buffer_read[BUFFER_SIZE];
                    char buffer_write[1] = {'\n'};
                    read(STDIN_FILENO, buffer_read, 1);
                    if (buffer_read[0] == 4){
                        close(pipe_1[1]);
                    }
                    else if (buffer_read[0] == 3)
                        kill(pid, SIGINT);
                    else if (buffer_read[0] == '\n' || buffer_read[0] == '\r'){
                        write(STDOUT_FILENO, char_buffer, 2);
                        write(pipe_1[1], buffer_write, 1);
                    }
                    else {
                        write(STDOUT_FILENO, buffer_read, 1);
                        write(pipe_1[1], buffer_read, 1);
                    }
                }
                
                if (pfds[1].revents & POLLIN){
                    char buffer[1];
                    ssize_t nchar = read(pipe_2[0], buffer, 1);
                    for (int i = 0; i < nchar; i++){
                        if (buffer[i] == '\n' || buffer[i] == '\r' )
                            write(STDOUT_FILENO, char_buffer, 2);
                        else
                            write(STDOUT_FILENO, buffer, 1);
                    }
                }
            }
        }
    }
    
    if (argc == 1) {
        while(1){
            char buffer[1];
            ssize_t nchar = read(0, buffer, 1);
            for (int i = 0; i < nchar; i++){
                if (buffer[i] == '\n' || buffer[i] == '\r' )
                    write(STDOUT_FILENO, char_buffer, 2);
                else if (buffer[i] == 4){
                    reset_input_mode();
                    exit(0);
                }
                else
                    write(STDOUT_FILENO, buffer, 1);
            }
        }
    }
    exit(0);
}
