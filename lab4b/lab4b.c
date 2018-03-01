#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <getopt.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include <poll.h>
#include "mraa/aio.h"
#include "mraa.h"
#define BUF_SIZE 64
#define GPIO_51 62
#define AIN0 1

mraa_aio_context temperature_sensor;
mraa_gpio_context button;
int period = 1;
int log_write = 0;
int lfds;
int stop_flag = 0;
int begin_counter = 1;
char scale = 'F';
struct timespec begin, stop;
int time_addition = 0;
char in_buffer[BUF_SIZE];
const int B = 4275; // B value of the thermistor
const int R0 = 100000; // R0 = 100k

void aio_gpio_close(){
    mraa_aio_close(temperature_sensor);
    mraa_gpio_close(button);
    return;
}

void log_output(){
    if (log_write)
        dprintf(lfds, "%s", in_buffer);
    return;
}

void shutdown_function() {
    int hour, min, sec;
    time_t t = time(NULL);
    struct tm tt = *localtime(&t);
    hour = tt.tm_hour;
    min = tt.tm_min;
    sec = tt.tm_sec;
    if (log_write){
        dprintf(lfds, "%02d:%02d:%02d SHUTDOWN\n", hour, min, sec);
    }
    dprintf(STDOUT_FILENO, "%02d:%02d:%02d SHUTDOWN\n", hour, min, sec);
    
    aio_gpio_close();
    exit(0);
    return;
}

void get_temp() {
    int hour, min, sec;
    clock_gettime(CLOCK_MONOTONIC, &stop);
    int runtime = (stop.tv_sec - begin.tv_sec) + time_addition;
    
    if ((begin_counter || runtime > 0) && (runtime % period) == 0) {
        int a = mraa_aio_read(temperature_sensor);
        double R = 1023.0/a-1.0;
        R = R0*R;
        double temperature = 1.0/(log(R/R0)/B+1/298.15)-273.15; //in celcius
        //convert to Fahrenheit
        if (scale == 'F')
            temperature = temperature * 1.8 + 32;
        
        time_t t = time(NULL);
        struct tm tt = *localtime(&t);
        hour = tt.tm_hour;
        min = tt.tm_min;
        sec = tt.tm_sec;

        if (log_write){
            dprintf(lfds, "%02d:%02d:%02d %04.1f\n", hour, min, sec, temperature);
        }
        dprintf(STDOUT_FILENO, "%02d:%02d:%02d %04.1f\n", hour, min, sec, temperature);
        
        //start counting again
        clock_gettime(CLOCK_MONOTONIC, &begin);
        time_addition = 0;
        begin_counter = 0;
    }
    return;
}

int main(int argc, char *argv[])
{
    int opt;
    int ret;
    int x;
    int read_button;
    
    const struct option long_opts[] = {
        {"scale", required_argument, NULL, 's'},
        {"period", required_argument, NULL, 'p'},
        {"log", required_argument, NULL, 'l'},
        {0, 0, 0, 0}
    };
    
    while((opt = getopt_long(argc, argv, "s:p:l:", long_opts, NULL)) >= 0) {
        switch(opt) {
            case 's' :
                if (strcmp(optarg, "F") == 0)
                    scale = 'F';
                else if (strcmp(optarg, "C") == 0)
                    scale = 'C';
                else{
                    fprintf(stderr, "Invalid scale input! Please input F as fahrenheit or C as celcius\n");
                    exit(1);
                }
                break;
            case 'p' :
                period = atoi(optarg);
                break;
            case 'l':
                log_write = 1;
                lfds = creat((char*)optarg, 00666);
                if (lfds <= 0){
                    fprintf(stderr, "Error when open log file: %s\n", (char*)optarg);
                    exit(1);
                }
                break;
            default:
                fprintf(stderr, "Correct format:\n./lab4b --period=# --scale=[CF] --log=nameofthefile\n");
                exit(1);
        }
    }
    
    //declare temperature sensor as analog input/output
    temperature_sensor = mraa_aio_init(AIN0);
    
    //initialize MRAA pin 62 (gpio_51) for button
    button = mraa_gpio_init(GPIO_51);
    mraa_gpio_dir(button, MRAA_GPIO_IN);  // not 0
    
    struct pollfd pfds[1];
    pfds[0].fd = STDIN_FILENO;
    pfds[0].events = POLLIN;
    
    while(1){
        ret = poll(pfds, 1, 0);
        if (ret < 0)
            x = 1;
        else
            x = 2;
        
        switch (x) {
            case 1:
                fprintf(stderr, "Error! Poll failed.\n");
                aio_gpio_close();
                exit(1);
                break;
            case 2:
                read_button = mraa_gpio_read(button);
                if (read_button < 0){
                    fprintf(stderr, "Error! Read button failed.\n");
                    aio_gpio_close();
                    exit(1);
                }
                else if (read_button == 1)
                    shutdown_function();
                if(stop_flag == 0)
                    get_temp();
                if (ret > 0) {
                    if(pfds[0].revents & POLLHUP) {
                        break;
                    }
                    if(pfds[0].revents & POLLERR) {
                        fprintf(stderr, "Error! Can't read stdin.\n");
                        aio_gpio_close();
                        exit(1);
                    }
                    if(pfds[0].revents & POLLIN) {
                        //set socket structures with null values
                        bzero(in_buffer, BUF_SIZE);
                        fgets(in_buffer, BUF_SIZE, stdin);
                                if (strcmp(in_buffer, "OFF\n") == 0){
                                    log_output();
                                    shutdown_function();
                                }
                                else if (strcmp(in_buffer, "SCALE=C\n") == 0){
                                    log_output();
                                    scale = 'C';
                                }
                                else if (strcmp(in_buffer, "SCALE=F\n") == 0){
                                    log_output();
                                    scale = 'F';
                                }
                                else if (strcmp(in_buffer, "STOP\n") == 0) {
                                    log_output();
                                    stop_flag = 1;
                                    clock_gettime(CLOCK_MONOTONIC, &stop);
                                    time_addition = stop.tv_sec - begin.tv_sec;
                                }
                                else if (strcmp(in_buffer, "START\n") == 0){
                                    log_output();
                                    stop_flag = 0;
                                    clock_gettime(CLOCK_MONOTONIC, &begin);
                                }
                                else if (strncmp(in_buffer, "PERIOD=", 7) == 0){
                                    period = atoi(in_buffer + 7);
                                    if (period <= 0){
                                        fprintf(stderr, "ERROR! Please input larger period.\n");
                                        aio_gpio_close();
                                        exit(1);
                                    }
                                    log_output();
                                }
                        
                    }
                }
                break;
        }
    }
    aio_gpio_close();
    exit(0);
}
