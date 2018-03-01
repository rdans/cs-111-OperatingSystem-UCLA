//
//  lab1b-server.c
//  lab1b
//
//  Created by Reinaldo Daniswara on 10/16/17.
//  Copyright © 2017 Reinaldo Daniswara. All rights reserved.
//
//
//  main.c
//  lab1a
//
//  Created by Reinaldo Daniswara on 10/8/17.
//  Copyright © 2017 Reinaldo Daniswara. All rights reserved.
//
//#define _POSIX_SOURCE
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
#include <sys/socket.h>
#include <netdb.h>
#include <mcrypt.h>

#define BUFFER_SIZE 256

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
}

 void  ChildProcess(void){
 close(pipe_1[1]);
 close(pipe_2[0]);
 dup2(pipe_1[0], 0);     //input
 dup2((pipe_2[1]), 1);   //output
 dup2((pipe_2[1]), 2);   //error
 char* argv_exe[2];
 argv_exe[0] = "/bin/bash";
 argv_exe[1] = NULL;
 int tes = execvp(argv_exe[0], argv_exe);
 if (tes == -1){
     fprintf(stderr, "execvp error\n");
     exit(1);
     }
 }

int sock_fd, n;
int newsock_fd;

int main(int argc, char* argv[]) {
    
    int opt;
    int encrypt_flag = 0;
    int port_flag = 0;
    int port_num;
    int keysize = 16; //128 bits
   
    
    
    char* encrypt_file = NULL;
    char* my_key = (char*)malloc(17*sizeof(char));
    char* IV = NULL;
    char* decrypt_IV = NULL;
    
    struct sockaddr_in serv_address;
     struct sockaddr_in client_address;
    
    
    pid_t pid, end_ID;

   
    struct option long_opts[] =
    {
        {"port", required_argument, NULL, 'p'},
        {"encrypt", required_argument, NULL, 'e'},
        {0, 0 , 0, 0}
    };
    
    while((opt = getopt_long(argc, argv, "p:e", long_opts, NULL)) >= 0)
    {
        switch (opt) {
            case 'p':
                port_flag = 1;
                port_num = atoi(optarg);
                break;
            case 'e':
                encrypt_flag = 1;
                encrypt_file = (char*)optarg;
                FILE* efd;
                
                efd = fopen(encrypt_file, "r");
                if (efd == NULL) {
                    n = read(fileno(efd), my_key, keysize);
                    if (n < 0) {
                        fprintf(stderr, "Error reading key file.\n");
                        exit(1);
                    }
                    close(fileno(efd));
                }
                break;
            default:
                fprintf(stderr, "Error! Unrecognize Argument!\nCorrect argument is --port --log --encrypt\n");
                exit(1);
        }
    }
    
    if (!port_flag){
        fprintf(stderr, "Error! Please input port number.\n");
        exit(1);
    }
    
    
    MCRYPT encrypt_td, decrypt_td;
    
    if (encrypt_flag){
        encrypt_td = mcrypt_module_open("twofish", NULL, "cfb", NULL);
        if (encrypt_td == MCRYPT_FAILED) {
            fprintf(stderr, "Error! Cannot open module!");
            exit(1);
        }
        IV = malloc(mcrypt_enc_get_iv_size(encrypt_td));
        memset(IV, 0, sizeof(char)* mcrypt_enc_get_iv_size(encrypt_td));
        unsigned en_size = mcrypt_enc_get_iv_size(encrypt_td);
        if (sizeof(my_key) < en_size)
            mcrypt_generic_init(encrypt_td, my_key, keysize, IV);
        
        decrypt_td = mcrypt_module_open("twofish", NULL, "cfb", NULL);
        if (decrypt_td == MCRYPT_FAILED) {
            fprintf(stderr, "Error! Cannot open module!");
            exit(1);
        }
        decrypt_IV = malloc(mcrypt_enc_get_iv_size(decrypt_td));
        memset(IV, 0, sizeof(char)* mcrypt_enc_get_iv_size(decrypt_td));
        unsigned de_size = mcrypt_enc_get_iv_size(decrypt_td);
        if (sizeof(my_key) < de_size)
            mcrypt_generic_init(decrypt_td, my_key, keysize, decrypt_IV);
    }
    
    
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
    else {
        //parent process
             // close(pipe_1[0]);
             // close(pipe_2[1]);
            sock_fd = socket(AF_INET, SOCK_STREAM, 0);
            if (sock_fd < 0){
                fprintf(stderr, "Error opening socket!\n");
                kill(pid, SIGINT);
                exit(1);
            }
            
                serv_address.sin_family = AF_INET;
                serv_address.sin_port = htons(port_num);
                serv_address.sin_addr.s_addr = htonl(INADDR_ANY);
    
                    
                    if (bind(sock_fd, (struct sockaddr *) &serv_address,
                             sizeof(serv_address)) < 0){
                    fprintf(stderr, "ERROR on binding to port %d \n", port_num);
                    kill(pid, SIGINT);
                    exit(1);
              }
                    listen(sock_fd, 2);
                    socklen_t clilen = sizeof(client_address);
                    newsock_fd = accept(sock_fd, (struct sockaddr *) &client_address, &clilen);
                    if (newsock_fd < 0){
                        fprintf(stderr, "Error on accept %d. \n", port_num);
                        kill(pid, SIGINT);
                        exit(1);
                    }
                    else
                    close(sock_fd);
    
                    
    pfds[0].fd = newsock_fd;
    pfds[0].events = POLLIN;
    pfds[1].fd = pipe_2[0];
    pfds[1].events = POLLIN;
    
    close(pipe_1[0]);
    close(pipe_2[1]);
    
    while(1){
        if (poll(pfds, 2, 0) == -1){
            printf("ERROR! Polling error: %s\n", strerror(errno));
            kill(pid, SIGKILL);
            exit(1);
        }
        
        if (poll(pfds, 2, 0) > 0) {
            if (pfds[0].revents & (POLLHUP + POLLERR)) {
                //reset_input_mode();
                fprintf(stderr, "Error! Can't exit from client");
                close(newsock_fd);
                kill(pid, SIGINT);
                exit(1);
            }
            
            if(pfds[1].revents & (POLLERR + POLLHUP)){
                close(pipe_1[1]);
                close(pipe_2[0]);
                int status;
                end_ID = waitpid(pid, &status, WNOHANG);
                if (end_ID == -1){
                    perror("waitpid error");
                    exit(1);
                }
                else if (end_ID == pid){
                    termination_info(status);
                    if(encrypt_flag){
                        mcrypt_generic_deinit(encrypt_td);
                        mcrypt_module_close(encrypt_td);
                        mcrypt_generic_deinit(decrypt_td);
                        mcrypt_module_close(decrypt_td);
                    }
                    exit(0);
                }
            }
            
            
            if (pfds[0].revents & POLLIN){
                char buffer_read[BUFFER_SIZE];
                int read_client = read(newsock_fd, buffer_read, 1);
                if (read_client < 0){
                    if (encrypt_flag) {
                        mcrypt_generic_deinit(encrypt_td);
                        mcrypt_module_close(encrypt_td);
                        mcrypt_generic_deinit(decrypt_td);
                        mcrypt_module_close(decrypt_td);
                    }
                    fprintf(stderr, "Error! Can't read client");
                    exit(1);
                }
                
                if (encrypt_flag)
                    mdecrypt_generic(encrypt_td, buffer_read, 1);
                if (buffer_read[0] == 4){
                    close(pipe_1[1]);
                    close(newsock_fd);
                    if (encrypt_flag) {
                        mcrypt_generic_deinit(encrypt_td);
                        mcrypt_module_close(encrypt_td);
                        mcrypt_generic_deinit(decrypt_td);
                        mcrypt_module_close(decrypt_td);
                    }
                    exit(0);
                }
                else if (buffer_read[0] == 3){
                    kill(pid, SIGINT);
                    close(newsock_fd);
                }
                else {
                    write(pipe_1[1], buffer_read, 1);
                    //fprintf(stdout, "Sent to shell\n");
                }
            }
            
            
            if (pfds[1].revents & POLLIN){
                char buffer[BUFFER_SIZE];
                ssize_t nchar = read(pipe_2[0], buffer, 1);
                /*
                if (nchar > 0)
                    fprintf(stdout, "Reveived form shell\n");
                 */
                if (nchar < 0){
                    if (encrypt_flag) {
                        mcrypt_generic_deinit(encrypt_td);
                        mcrypt_module_close(encrypt_td);
                        mcrypt_generic_deinit(decrypt_td);
                        mcrypt_module_close(decrypt_td);
                    }
                    fprintf(stderr, "Error! Can't read client");
                    exit(1);
                }
                
                for (int i = 0; i < nchar; i++){
                    if (buffer[0] == 4){
                        close(newsock_fd);
                        if (encrypt_flag){
                            mcrypt_generic_deinit(encrypt_td);
                            mcrypt_module_close(encrypt_td);
                            mcrypt_generic_deinit(decrypt_td);
                            mcrypt_module_close(decrypt_td);
                        }
                        exit(0);
                    }
                    if (encrypt_flag){
                        mcrypt_generic(encrypt_td, &buffer[i], 1);
                    }
                        write(newsock_fd, &buffer[i], 1);
                }
            }
        }
    }
}
    if (encrypt_flag){
        mcrypt_generic_deinit(encrypt_td);
        mcrypt_module_close(encrypt_td);
        mcrypt_generic_deinit(decrypt_td);
        mcrypt_module_close(decrypt_td);
    }
    exit(0);
}



