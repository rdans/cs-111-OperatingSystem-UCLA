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




int main(int argc, char* argv[]) {
    
    int opt;
    int encrypt_flag = 0;
    int port_flag = 0;
    int log_flag = 0;
    int port_num = 0;
    int keysize = 16; //128 bits
    int sock_fd;
    
    char* log_file = NULL;
    char* encrypt_file = NULL;
    char* my_key = malloc(17*sizeof(char));
    char* IV = NULL;
    char* decrypt_IV = NULL;
    
    struct pollfd pfds[2];
    
    struct termios original_attributes;
    struct sockaddr_in serv_address;
    struct hostent *server;
    
    struct termios tattr;
    char char_buffer[2] = {'\n', '\r'};
    
    struct option long_opts[] =
    {
        {"port", required_argument, NULL, 'p'},
        {"log", required_argument, NULL, 'l'},
        {"encrypt", required_argument, NULL, 'e'},
        {0, 0 , 0, 0}
    };

    while((opt = getopt_long(argc, argv, "p:l:e", long_opts, NULL)) >= 0)
    {
        switch (opt) {
            case 'p':
                port_flag = 1;
                port_num = atoi(optarg);
                break;
            case 'l':
                log_flag = 1;
                log_file = (char*)optarg;
                break;
            case 'e':
                encrypt_flag = 1;
                encrypt_file = (char*)optarg;
                FILE* efd;
                efd = fopen(encrypt_file, "r");
                if (efd == NULL) {
                    int n = read(fileno(efd), my_key, keysize);
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
    
    FILE* lfd;
    if (log_flag) {
        lfd = fopen(log_file, "w");
        if (lfd == NULL) {
            fprintf(stderr, "Error when open log file: %s\n", log_file);
            exit(1);
        }
    }
    
    //based on sample code of socket tutorial
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0){
        fprintf(stderr, "Error opening socket!\n");
        exit(1);
    }
    
    
    
    serv_address.sin_family = AF_INET;
    server = gethostbyname("localhost");
    /*if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(1);
    }*/
    bcopy((char*)server->h_addr,(char*)&serv_address.sin_addr.s_addr, server->h_length);
    serv_address.sin_port = htons(port_num);
    
    
    if (connect(sock_fd,(struct sockaddr*)&serv_address,sizeof(serv_address)) < 0){
        fprintf(stderr, "Error! Can't connect to port %d.\n", port_num);
        exit(1);
    }
    
    MCRYPT encrypt_td;
    MCRYPT decrypt_td;
    
    if (encrypt_flag){
        encrypt_td = mcrypt_module_open("twofish", NULL, "cfb", NULL);
        if (encrypt_td == MCRYPT_FAILED) {
            fprintf(stderr, "Error! Cannot open module!");
            exit(1);
        }
        IV = malloc(mcrypt_enc_get_iv_size(encrypt_td));
        memset(IV, 0, sizeof(char)* mcrypt_enc_get_iv_size(encrypt_td));
        unsigned un_ensize = mcrypt_enc_get_iv_size(encrypt_td);
        if (sizeof(my_key) < un_ensize)
            mcrypt_generic_init(encrypt_td, my_key, keysize, IV);
        
        decrypt_td = mcrypt_module_open("twofish", NULL, "cfb", NULL);
        if (decrypt_td == MCRYPT_FAILED) {
            fprintf(stderr, "Error! Cannot open module!");
            exit(1);
        }
        decrypt_IV = malloc(mcrypt_enc_get_iv_size(decrypt_td));
        memset(decrypt_IV, 0, sizeof(char)* mcrypt_enc_get_iv_size(decrypt_td));
        unsigned un_desize = mcrypt_enc_get_iv_size(decrypt_td);
        if (sizeof(my_key) < un_desize)
            mcrypt_generic_init(decrypt_td, my_key, keysize, decrypt_IV);
    }
        
    
    
    /*
    // Make sure stdin is a terminal.
    if (!isatty (STDIN_FILENO))
    {
        fprintf (stderr, "Not a terminal.\n");
        exit (EXIT_FAILURE);
    }
    
    if(tcgetattr(STDIN_FILENO, &original_attributes) == -1){
        fprintf(stderr, "Tcgetattr error!");
        exit(1);
    }
    */
    
    // Save the terminal attributes so we can restore them later. */
    //tcgetattr (STDIN_FILENO, &original_attributes);
    //atexit (reset_input_mode);
    
    // Set the funny terminal modes.
    tcgetattr (STDIN_FILENO, &original_attributes);
    tattr.c_iflag = ISTRIP;
    tattr.c_oflag = 0;
    tattr.c_lflag = 0;
    tcsetattr (STDIN_FILENO, TCSANOW, &tattr);
    
    pfds[0].fd = 0;
    pfds[0].events = POLLIN;
    pfds[1].fd = sock_fd;
    pfds[1].events = POLLIN;
    
    while(1){
        if (poll(pfds, 2, 0) == -1){
            printf("ERROR! Polling error: %s\n", strerror(errno));
            //kill(pid, SIGKILL);
            exit(1);
        }
        
        if (poll(pfds, 2, 0) > 0) {
            if (pfds[0].revents & (POLLHUP + POLLERR)) {
                    write(sock_fd, "\x04", 1);
                    tcsetattr(STDIN_FILENO, TCSANOW, &original_attributes);
                if (encrypt_flag){
                    mcrypt_generic_deinit(encrypt_td);
                    mcrypt_module_close(encrypt_td);
                    mcrypt_generic_deinit(decrypt_td);
                    mcrypt_module_close(decrypt_td);
                }
                exit(1);
            }
            
                if(pfds[1].revents & (POLLERR + POLLHUP)){
                    tcsetattr(STDIN_FILENO, TCSANOW, &original_attributes);
                    if (encrypt_flag){
                        mcrypt_generic_deinit(encrypt_td);
                        mcrypt_module_close(encrypt_td);
                        mcrypt_generic_deinit(decrypt_td);
                        mcrypt_module_close(decrypt_td);
                    }
                    
                    exit(0);
                    
                }
            
                
                if (pfds[0].revents & POLLIN){
                    char buffer_read[BUFFER_SIZE];
                    //char buffer_write[1] = {'\n'};
                    char newline[1] = "\n";
                    read(STDIN_FILENO, buffer_read, 1);
                    
                    if (buffer_read[0] == '\n' || buffer_read[0] == '\r'){
                        if (buffer_read[0] == '\n')
                        write(STDOUT_FILENO, buffer_read, 1);
                        
                        if (buffer_read[0] == '\r')
                        write(STDOUT_FILENO, buffer_read, 1); 
                        
                        if (encrypt_flag)
                            mcrypt_generic(encrypt_td, newline, 1);
                        
                        write(sock_fd, newline, 1);
                    }
                    
                        else {
                            write(STDOUT_FILENO, buffer_read, 1);
                            if (encrypt_flag)
                                mcrypt_generic(encrypt_td, buffer_read, 1);
                            write(sock_fd, buffer_read, 1);
                        }
                        if (log_flag) {
                            char write_log[] = "SENT 1 byte: ";
                            strcat(write_log, buffer_read);
                            strcat(write_log, "\n");
                            write(fileno(lfd), write_log, strlen(write_log));
                        }
                    }
            
                
                if (pfds[1].revents & POLLIN){
                    char buffer[BUFFER_SIZE];
                    int nchar = read(sock_fd, buffer, 1);
                    if (nchar < 1){
                        tcsetattr(STDIN_FILENO, TCSANOW, &original_attributes);
                        if (encrypt_flag){
                            mcrypt_generic_deinit(encrypt_td);
                            mcrypt_module_close(encrypt_td);
                            mcrypt_generic_deinit(decrypt_td);
                            mcrypt_module_close(decrypt_td);
                        }
                        exit(0);
                    }
                    
                    if (log_flag) {
                        char write_log[] = "RECEIVED 1 byte: ";
                        strcat(write_log, buffer);
                        strcat(write_log, "\n");
                        write(fileno(lfd), write_log, strlen(write_log));
                    }
                    
                    if (encrypt_flag)
                        mdecrypt_generic(decrypt_td, buffer, 1);
                    
                    for (int i = 0; i < nchar; i++){
                        if (buffer[i] == '\n' || buffer[i] == '\r' )
                            write(STDOUT_FILENO, char_buffer, 2);
                        else
                            write(STDOUT_FILENO, buffer, 1);
                    }
                }
            }
    }
    
    exit(0);
}

