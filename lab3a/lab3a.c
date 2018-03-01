//
//  main.c
//  lab3a
//
//  Created by Tom Zhang on 11/19/17.
//  Copyright © 2017 CS_111. All rights reserved.
//

#include <stdio.h>
#include "ext2_fs.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

void superblock_summary(int fd, struct ext2_super_block *sb) {
    if (pread(fd, sb, sizeof(struct ext2_super_block), 1024) < 0) {
        fprintf(stderr,
                "Reading FS image fail: %s\n", strerror(errno));
        exit(2);
    }
    size_t bsize = EXT2_MIN_BLOCK_SIZE << sb->s_log_block_size;
    if (bsize != 1024) {
        fprintf(stderr, "Support only 1024 bytes block size.\n");
        exit(2);
    }
    char read_in[1024];
    //int snprintf(char *restrict buf, size_t n, const char * restrict  format, ...);
    //Using snprintf to make sure only 1023 bytes are read and the end
    //of read_in buffer is bull bit '\0'
    int n = snprintf(read_in, 1024, "SUPERBLOCK,%d,%d,%d,%d,%d,%d,%d\n",
                     sb->s_blocks_count,
                     sb->s_inodes_count,
                     (int)bsize,
                     sb->s_inode_size,
                     sb->s_blocks_per_group,
                     sb->s_inodes_per_group,
                     sb->s_first_ino);
    
    if (write(STDOUT_FILENO, read_in, n) < 0) {
        fprintf(stderr,
                "Writing superblock summary to STDOUT fail: %s\n", strerror(errno));
        exit(2);
    }
}

void time_convert(unsigned itime, char *buffer) {
    time_t time = itime;
    struct tm *tmp = gmtime(&time);
    
    if(tmp == NULL) {
        fprintf(stderr, "Converting time fail.\n");
        exit(2);
    }
    
    if(strftime(buffer, 18, "%m/%d/%y %H:%M:%S", tmp) == 0) {
        fprintf(stderr, "Setting time fail.\n");
        exit(2);
    }
}

void directory_entries(int fd, unsigned inode_index, struct ext2_inode *inode) {
    struct ext2_dir_entry direc_entry;
    char read_in[1024];
    int i, n;
    for (i=0; i<EXT2_NDIR_BLOCKS; i++) {
        int block_offset = inode->i_block[i]*1024;
        int next = block_offset + 1024;
        int curr = block_offset;
        
        while(curr < next) {
            if (pread(fd, &direc_entry, sizeof(struct ext2_dir_entry), curr) < 0) {
                fprintf(stderr,
                        "Reading directory entries fail: %s\n", strerror(errno));
                exit(2);
            }
            if(direc_entry.inode == 0) {
                break;
            }
            n = snprintf(read_in, 1024, "DIRENT,%d,%d,%d,%d,%d,'%s'\n",
                         inode_index,
                         curr - block_offset,
                         direc_entry.inode,
                         direc_entry.rec_len,
                         direc_entry.name_len,
                         direc_entry.name);
            if (write(STDOUT_FILENO, read_in, n) < 0) {
                fprintf(stderr,
                        "Writing directory entries to STDOUT fail: %s\n", strerror(errno));
                exit(2);
            }
            curr += direc_entry.rec_len;
        }
    }
}

void indirect_block_references(int fd, off_t offset, int level, int inode_index, int block_index) {
    __u32 indirect_blocks[256];
    if(block_index != 0) {
        if (pread(fd, indirect_blocks, 1024, block_index*1024) < 0) {
            fprintf(stderr,
                    "Reading indirect block references fail: %s\n", strerror(errno));
            exit(2);
        }
        int i, n;
        for (i=0; i<256; i++) {
            off_t curr_offset = offset + i;
            char read_in[1024];
            if(indirect_blocks[i] != 0) {
                n = snprintf(read_in, 1024, "INDIRECT,%d,%d,%d,%d,%d\n",
                             inode_index,
                             level,
                             (int)curr_offset,
                             block_index,
                             indirect_blocks[i]);
                if (write(STDOUT_FILENO, read_in, n) < 0) {
                    fprintf(stderr,
                            "Writing indirect block references to STDOUT fail: %s\n", strerror(errno));
                    exit(2);
                }
            }
            if(level > 1) {
                indirect_block_references(fd, curr_offset, level - 1,
                                          inode_index,
                                          indirect_blocks[i]);
            }
        }
    }
}

void indirect_block_references_wrapper(int fd, int level, int inode_index, int block_index) {
    off_t offset;
    switch(level)
    {
        case 1:
            offset = EXT2_IND_BLOCK;
            break;
        case 2:
            offset = EXT2_IND_BLOCK + 256;
            break;
        case 3:
            offset = EXT2_IND_BLOCK + 256 +
            (256 << 8);
            break;
        default:
            fprintf(stderr, "Invalid level for indirect block references.\nValid usage: [123].\n");
            exit(2);
    }
    indirect_block_references(fd, offset, level, inode_index, block_index);
}

char file_type(__u16 i_mode) {
    switch(i_mode & 0xF000) {
        case 0x8000:
            return 'f';
        case 0x4000:
            return 'd';
        case 0xA000:
            return 's';
        default:
            return '?';
    }
}

void group_summary(int fd, struct ext2_super_block *sb) {
    int group_size = ((sb->s_blocks_count-1) / sb->s_blocks_per_group)+1;
    struct ext2_group_desc groups[group_size];
    
    if (pread(fd, groups, group_size * sizeof(struct ext2_group_desc), 2048) < 0) {
        fprintf(stderr,
                "Reading FS image fail: %s\n", strerror(errno));
        exit(2);
    }
    
    
    int i;
    for (i=0; i<group_size; i++) {
        int blocks_number, inodes_number;
        if (i == group_size-1) {
            blocks_number = sb->s_blocks_count - (sb->s_blocks_per_group * i);
            inodes_number = sb->s_inodes_count - (sb->s_inodes_per_group * i);
        }
        else {
            blocks_number = sb->s_blocks_per_group;
            inodes_number = sb->s_inodes_per_group;
        }
        
        char read_in[1024];
        int n = snprintf(read_in, 1024, "GROUP,%d,%d,%d,%d,%d,%d,%d,%d\n",
                         i,
                         blocks_number,
                         inodes_number,
                         groups[i].bg_free_blocks_count,
                         groups[i].bg_free_inodes_count,
                         groups[i].bg_block_bitmap,
                         groups[i].bg_inode_bitmap,
                         groups[i].bg_inode_table);
        if (write(STDOUT_FILENO, read_in, n) < 0) {
            fprintf(stderr,
                    "Writing group summary to STDOUT fail: %s\n", strerror(errno));
            exit(2);
        }
        
        //---------------------------------------------------------------
        //Free block entries:
        char block_bitmap[1024];
        char bfree_buffer[1024];
        //Left shift 10 == multiply by 1024
        int bitmap_offset = groups[i].bg_block_bitmap*1024;
        if (pread(fd, block_bitmap, 1024, bitmap_offset) < 0) {
            fprintf(stderr,
                    "Reading block bitmap fail: %s\n", strerror(errno));
            exit(2);
        }
        
        int j, k;
        //8 bits per byte
        for (j=0; j<blocks_number; j+=8) {
            //            Each bit represent the current state of a block within that
            //            block group, where 1 means "used" and 0 "free/available".
            //            The first block of this block group is represented by bit 0
            //            of byte 0, the second by bit 1 of byte 0. The 8th block
            //            is represented by bit 7 (most significant bit) of byte 0
            //            while the 9th block is represented by bit 0 (least
            //            significant bit) of byte 1.
            char curr_byte = block_bitmap[j>>3];
            for (k=1; k<=8; k++) {
                //Use AND op to check the current bit
                if ((curr_byte & 1) == 0) {
                    int n = snprintf(bfree_buffer, 1024, "BFREE,%d\n", j+k+i*(sb->s_blocks_per_group));
                    if (write(STDOUT_FILENO, bfree_buffer, n) < 0) {
                        fprintf(stderr,
                                "Writing group summary to STDOUT fail: %s\n", strerror(errno));
                        exit(2);
                    }
                }
                //Check next bit
                curr_byte = curr_byte >> 1;
            }
        }
        
        //---------------------------------------------------------------
        //Free I-node entries:
        char inode_bitmap[1024];
        char ifree_buffer[1024];
        int inode_offset = groups[i].bg_inode_bitmap*1024;
        if (pread(fd, inode_bitmap, 1024, inode_offset) < 0) {
            fprintf(stderr,
                    "Reading inode bitmap fail: %s\n", strerror(errno));
            exit(2);
        }
        
        //Same as operations to free block entries
        for (j=0; j<inodes_number; j+=8) {
            char curr_byte = inode_bitmap[j>>3];
            for (k=1; k<=8; k++) {
                //Use AND op to check the current bit
                if ((curr_byte & 1) == 0) {
                    int n = snprintf(ifree_buffer, 1024, "IFREE,%d\n", j+k+i*(sb->s_blocks_per_group));
                    if (write(STDOUT_FILENO, ifree_buffer, n) < 0) {
                        fprintf(stderr,
                                "Writing group summary to STDOUT fail: %s\n", strerror(errno));
                        exit(2);
                    }
                }
                //Check next bit
                curr_byte >>= 1;
            }
        }
        
        //---------------------------------------------------------------
        //I-node summary
        struct ext2_inode inodes[inodes_number];
        char atime[18];
        char ctime[18];
        char mtime[18];
        char inode_read[1024];
        
        if (pread(fd, inodes, inodes_number*sizeof(struct ext2_inode), (groups[i].bg_inode_table)*1024) < 0) {
            fprintf(stderr,
                    "Reading inode table fail: %s\n", strerror(errno));
            exit(2);
        }
        
        
        int p;
        for(p = 0; p < inodes_number; ++p)
        {
            int inode_index = p + 1 + i * sb->s_inodes_per_group;
            
            if(inodes[p].i_mode != 0 && inodes[p].i_links_count != 0)
            {
                time_convert(inodes[p].i_atime, atime);
                time_convert(inodes[p].i_ctime, ctime);
                time_convert(inodes[p].i_mtime, mtime);
                
                n = snprintf(inode_read, 1024, "INODE,%d,%c,%o,%d,%d,%d,%s,%s,%s,%d,%d,",
                             inode_index,
                             file_type(inodes[p].i_mode),
                             inodes[p].i_mode & 0x0FFF,
                             inodes[p].i_uid,
                             inodes[p].i_gid,
                             inodes[p].i_links_count,
                             ctime,
                             mtime,
                             atime,
                             inodes[p].i_size,
                             inodes[p].i_blocks);
                
                if (write(STDOUT_FILENO, inode_read, n) < 0) {
                    fprintf(stderr,
                            "Writing inode summary to STDOUT fail: %s\n", strerror(errno));
                    exit(2);
                }
                
                int num_blocks = EXT2_N_BLOCKS;
                int tmp = num_blocks - 1;
                int q;
                for(q = 0; q < num_blocks; ++q)
                {
                    if(inodes[p].i_blocks == 0 && inodes[p].i_block[q] != 0)
                    {
                        n = snprintf(inode_read, 1024,
                                     "%d\n", inodes[p].i_block[q]);
                        if (write(STDOUT_FILENO, inode_read, n) < 0) {
                            fprintf(stderr,
                                    "Writing inode summary to STDOUT fail: %s\n", strerror(errno));
                            exit(2);
                        }
                        break;
                    }
                    else
                    {
                        if(q == tmp)
                        {
                            n = snprintf(inode_read, 1024,
                                         "%d\n", inodes[p].i_block[q]);
                        }
                        else
                        {
                            n = snprintf(inode_read, 1024,
                                         "%d,", inodes[p].i_block[q]);
                        }
                    }
                    
                    if (write(STDOUT_FILENO, inode_read, n) < 0) {
                        fprintf(stderr,
                                "Writing inode summary to STDOUT fail: %s\n", strerror(errno));
                        exit(2);
                    }
                    
                }
            }
            
            //Directory entries
            if(file_type(inodes[p].i_mode) == 'd') {
                directory_entries(fd, inode_index, &inodes[p]);
            }
            
            //Indirect block references
            indirect_block_references_wrapper(fd, 1, inode_index, inodes[p].i_block[EXT2_IND_BLOCK]);
            
            indirect_block_references_wrapper(fd, 2, inode_index, inodes[p].i_block[EXT2_DIND_BLOCK]);
            
            indirect_block_references_wrapper(fd, 3, inode_index, inodes[p].i_block[EXT2_TIND_BLOCK]);
        }
    }
}


int main(int argc, const char * argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Wrong number of arguments.\nValid usage: ./lab3a FSImageName");
        exit(1);
    }
    int fd;
    if((fd = open(argv[1], O_RDONLY)) < 0) {
        fprintf(stderr, "Reading file system image fail: %s\n", strerror(errno));
        exit(1);
    }
    struct ext2_super_block sb;
    superblock_summary(fd, &sb);
    group_summary(fd, &sb);
}
