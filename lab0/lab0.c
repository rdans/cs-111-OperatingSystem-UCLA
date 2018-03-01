// NAME: Reinaldo Daniswara
// EMAIL: rdaniswara@g.ucla.edu
// ID: 604840665

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#define MAX_SIZE 2048

void sigsegv_handler(int sig_fault)
{
  if(sig_fault == SIGSEGV)
    {
      fprintf(stderr, "Segmentation fault error!");
      exit(4);
    }
}

int main(int argc, char** argv)
{
    int ifd, ofd, opt;
    size_t ret_size;
    char buf[MAX_SIZE];
    int seg_flag = 0;
    
    static struct option long_opts[] =
    {
        {"input", required_argument, NULL, 'i'},
        {"output", required_argument, NULL, 'o'},
        {"segfault", no_argument, NULL, 's'},
        {"catch", no_argument, NULL, 'c'},
	{0, 0, 0, 0}
    };
    
    while ((opt = getopt_long(argc, argv, "i:o:sc", long_opts, NULL)) != -1)
    {
        switch(opt)
        {
            case 'i':
                ifd = open(optarg, O_RDONLY);
		if (optarg == NULL){
                    printf("Error: %s\n", strerror(errno));
                    fprintf(stderr, "Error! Can't open input file %s\n", optarg);
                    exit(2);
		}
                else if (ifd >= 0)
                {
                    close(0);
                    dup(ifd);
                    close(ifd);
                }
                else {
                    printf("Error: %s\n", strerror(errno));
                    fprintf(stderr, "Error! Can't open input file %s\n", optarg);
                    exit(2);
                }
                break;
            case 'o':
                ofd = creat(optarg, 0666);
                if (optarg == NULL){
                    printf("Error: %s\n", strerror(errno));
                    fprintf(stderr, "Error! Can't open output file %s\n", optarg);
                    exit(3);
		}
                else if (ofd >= 0) {
                    close(1);
                    dup(ofd);
                    close(ofd);
                }
                else {
                    printf("Error: %s\n", strerror(errno));
                    fprintf(stderr, "Error! Can't create output file %s\n", optarg);
                    exit(3);
                }
                break;
            case 's':
                seg_flag = 1;
                break;
            case 'c':
                signal(SIGSEGV, sigsegv_handler);
                break;
            default:
                exit(EXIT_FAILURE);
        }
    }
    
    if (seg_flag)
    {
        char *sf = NULL;
        *sf = 's';
    }
    
    while((ret_size = read(0, &buf, MAX_SIZE)) > 0)
        write(1, &buf, ret_size);
    exit(0);
}

