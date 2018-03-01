//
//  lab4c_tls.c
//  lab4c
//
//  Created by Reinaldo Daniswara on 12/7/17.
//  Copyright © 2017 Reinaldo Daniswara. All rights reserved.
//

#include <stdio.h>
#include "mraa.h"
#include "mraa/aio.h"
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include <poll.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#define BUFFER_SIZE 64

struct timespec begin, end;
mraa_aio_context temperature_sensor;
mraa_gpio_context button;
int time_addition = 0;
int log_on = 0;
int lfds;
int begin_counter = 1;
int period = 1;
char scale = 'F';
int stop_flag = 0;
const int B = 4275; // B value of the thermistor
const int R0 = 100000; // R0 = 100k

int sfds = 0;
char* num_id = "604604604";
char* name_host = "lever.cs.ucla.edu";
int port_number = 0;
SSL *client;

void aio_gpio_close(){
    mraa_aio_close(temperature_sensor);
    mraa_gpio_close(button);
    return;
}

void socket_starts(char* host, int port) {
    sfds = socket(AF_INET, SOCK_STREAM, 0);
    if (sfds < 0)
    {
        fprintf(stderr, "Error creating socket.\n");
        exit(2);
    }
    
    struct hostent *host_data = gethostbyname(host);
    if (host_data == NULL){
        fprintf(stderr,"Host not exist.\n");
        exit(1);
    }
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    bcopy((char *)host_data->h_addr, (char *)&serv_addr.sin_addr.s_addr, host_data->h_length);
    serv_addr.sin_port = htons(port);
    
    if (connect(sfds,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0 ){
        fprintf(stderr, "Error connecting to socket.\n");
        exit(2);
    }
    char buffer[BUFFER_SIZE];
    
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
    
    SSL_CTX *cxt = SSL_CTX_new(TLSv1_client_method());
    client = SSL_new(cxt);
    SSL_set_fd(client, sfds);
    if(SSL_connect(client) == -1){
        ERR_print_errors_fp(stderr);
        exit(2);
    }
    bzero(buffer, BUFFER_SIZE);
    sprintf(buffer, "ID=%s\n", num_id);
    SSL_write(client, buffer, strlen(buffer));
    if (log_on)
        dprintf(lfds, "ID=%s\n", num_id);
    
}


void shutdown_function() {
    int hour, minute, second;
    time_t t = time(NULL);
    struct tm time_t = *localtime(&t);
    hour = time_t.tm_hour;
    minute = time_t.tm_min;
    second = time_t.tm_sec;
    if (log_on){
        dprintf(lfds, "%02d:%02d:%02d SHUTDOWN\n", hour, minute, second);
    }
    
    char buffer[BUFFER_SIZE];
    sprintf(buffer, "%02d:%02d:%02d SHUTDOWN\n", hour, minute, second);
    SSL_write(client, buffer, strlen(buffer));
    
    aio_gpio_close();
    exit(0);
    return;
}

void get_temp() {
    int hour, minute, second;
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    long runtime = (end.tv_sec - begin.tv_sec) + time_addition;
    
    if ((begin_counter || runtime > 0) && (runtime % period) == 0) {
        //Convert to degree C, demo code from  Grove
        int a = mraa_aio_read(temperature_sensor);
        double R = 1023.0/a-1.0;
        R = R0*R;
        double temperature = 1.0/(log(R/R0)/B+1/298.15)-273.15;
        if (scale == 'F')
            temperature = temperature * 1.8 + 32;
        
        time_t t = time(NULL);
        struct tm time_t = *localtime(&t);
        hour = time_t.tm_hour;
        minute = time_t.tm_min;
        second = time_t.tm_sec;
        
        if (log_on){
            dprintf(lfds, "%02d:%02d:%02d %04.1f\n", hour, minute, second, temperature);
        }
        
        char buffer[BUFFER_SIZE];
        sprintf(buffer, "%02d:%02d:%02d %04.1f\n", hour, minute, second, temperature);
        SSL_write(client, buffer, strlen(buffer));
        
        clock_gettime(CLOCK_MONOTONIC, &begin);
        time_addition = 0;
        begin_counter = 0;
    }
    return;
}


int main(int argc, const char * argv[]) {
    int opt;
    int x = 0;
    
    const struct option long_options[] = {
        {"id", required_argument, NULL, 'i'},
        {"host", required_argument, NULL, 'h'},
        {"log", required_argument, NULL, 'l'},
        {0, 0, 0, 0}
    };
    
    while((opt = getopt_long(argc, (char* const*)argv, "i:h:l:", long_options, NULL)) >= 0) {
        switch(opt) {
            case 'i' :
                num_id = optarg;
                if (strlen(num_id) != 9){
                    fprintf(stderr, "Error: ID is not 9 digits.\n");
                    exit(1);
                }
                break;
            case 'h' :
                name_host = optarg;
                break;
            case 'l':
                log_on = 1;
                lfds = creat((char*)optarg, 00666);
                if (lfds <= 0){
                    fprintf(stderr, "Error when open log file: %s\n", (char*)optarg);
                    exit(1);
                }
                break;
            default:
                fprintf(stderr, "Invalid command! Correct format is:\n./lab4b --id= --host= --log=txtFilename portNumber\n");
                exit(1);
                break;
        }
    }
    
    if(argc-optind == 1){
        port_number = atoi(argv[optind]);
    }
    else{
        fprintf(stderr, "Too many argument\n");
        exit(1);
    }
    socket_starts(name_host, port_number);
    
    temperature_sensor = mraa_aio_init(1);
    button = mraa_gpio_init(62);
    mraa_gpio_dir(button, MRAA_GPIO_IN);  // not 0
    
    struct pollfd pfds[1];
    pfds[0].fd = sfds;
    pfds[0].events = POLLIN;
    
    char read_buffer[BUFFER_SIZE];
    int read_button;
    
    while(1){
        int ret = poll(pfds, 1, 0);
        if (ret < 0)
            x = 1;
        else
            x = 2;
        switch (x) {
            case 1:
                fprintf(stderr, "ERROR! Fail to poll.\n");
                aio_gpio_close();
                exit(1);
                break;
            case 2:
                read_button = mraa_gpio_read(button);
                if (read_button == 1){
                    shutdown_function();
                }
                else if (read_button < 0){
                    fprintf(stderr, "Button Error.\n");
                    aio_gpio_close();
                    exit(1);
                }
                if(stop_flag == 0){
                    get_temp();
                }
                if (ret > 0) {
                    if(pfds[0].revents & POLLHUP) {
                        break;
                    }
                    if(pfds[0].revents & POLLERR) {
                        fprintf(stderr, "Error! Fail to read stdin.\n");
                        aio_gpio_close();
                        exit(1);
                    }
                    if(pfds[0].revents & POLLIN) {
                        bzero(read_buffer, BUFFER_SIZE);
                        SSL_read(client, read_buffer, BUFFER_SIZE);
                        if (strcmp(read_buffer, "OFF\n") == 0){
                            if (log_on){
                                dprintf(lfds, "%s", read_buffer);
                            }
                            shutdown_function();
                        }
                        else if (strcmp(read_buffer, "STOP\n") == 0) {
                            if(log_on){
                                dprintf(lfds, "%s", read_buffer);
                            }
                            stop_flag = 1;
                            clock_gettime(CLOCK_MONOTONIC, &end);
                            time_addition = end.tv_sec - begin.tv_sec;
                        }
                        else if (strcmp(read_buffer, "START\n") == 0){
                            if (log_on){
                                dprintf(lfds, "%s", read_buffer);
                            }
                            stop_flag = 0;
                            clock_gettime(CLOCK_MONOTONIC, &begin);
                        }
                        else if (strncmp(read_buffer, "PERIOD=",7) == 0){
                            period = atoi(read_buffer + 7);
                            if (period <= 0){
                                fprintf(stderr, "Period has to be positive integer, exit.\n");
                                aio_gpio_close();
                                exit(1);
                            }
                            if (log_on){
                                dprintf(lfds, "%s", read_buffer);
                            }
                        }
                        else if (strcmp(read_buffer, "SCALE=C\n") == 0){
                            if (log_on){
                                dprintf(lfds, "%s", read_buffer);
                            }
                            scale = 'C';
                        }
                        else if (strcmp(read_buffer, "SCALE=F\n") == 0){
                            if (log_on){
                                dprintf(lfds, "%s", read_buffer);
                            }
                            scale = 'F';
                        }
                        else if (strncmp(read_buffer, "LOG ",4) == 0){
                            if (log_on){
                                dprintf(lfds, "%s", read_buffer+4);
                            }
                        }
                    }
                }
                break;
        }
    }
    aio_gpio_close();
    exit(0);
    
}

