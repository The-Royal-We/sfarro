//
// Created by se415011 on 10/13/15.
//

#ifndef SFARRO_LOG_H
#define SFARRO_LOG_H

#include <stdio.h>

// macro to log fields in structs

#define log_struct(st, field, format, typecast) \
    log_msg("   " #field " = " #format "\n", typecast st-> field)

FILE *log_open(void);
void log_conn(struct fuse_conn_info *conn);
void log_fi(struct fuse_file_info *fi);
void log_stat(struct stat *si);
void log_statsvfs(struct statvfs *sv);
void log_utime(struct utimbuf *buf);

void log_msg (const char *format, ...);

#endif //SFARRO_LOG_H
