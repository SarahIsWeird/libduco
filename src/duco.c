/*
 * libduco, Copyright (c) 2021 Sarah Klocke <sarah.klocke@outlook.com>
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See the
 * LICENSE.md file for more details.
 */

#include "../include/duco.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef _WIN32
# include <Winsock2.h>
# include <WS2tcpip.h>
# include <iphlpapi.h>
#else
# include <netdb.h>
# include <netinet/in.h>
# include <sys/socket.h>
# include <unistd.h>
#endif

static char _DUCO_BUFFER[256];
static char _DUCO_ERROR[256];

#ifdef _WIN32
# define write(sock, buffer, len) send(sock, buffer, len, 0)
# define read(sock, buffer, len) recv(sock, buffer, len, 0)
#endif

duco_connection_t* duco_connect(const char* pool_addr, int pool_port) {
    duco_connection_t* conn = (duco_connection_t*) malloc(sizeof(duco_connection_t));
    int recn;
    struct sockaddr_in addr;
    struct hostent* server;

#ifdef _WIN32
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        sprintf(_DUCO_ERROR, "Couldn't initialize Windows libraries.");
        free(conn);

        return NULL;
    }
#endif

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(pool_port);

    server = gethostbyname(pool_addr);
    memcpy(&addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);

    conn->sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (conn->sockfd < 0) {
        sprintf(_DUCO_ERROR, "Couldn't create socket.");
        free(conn);

        return NULL;
    }

    if (connect(conn->sockfd, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
        sprintf(_DUCO_ERROR, "Couldn't connect to the pool server");
        strcat(_DUCO_ERROR, strerror(errno));

    #ifdef _WIN32
        closesocket(conn->sockfd);
    #endif
        free(conn);

        return NULL;
    }

    memset(conn->buffer, 0, 10);
    recn = read(conn->sockfd, conn->version, 9);

    if (recn < 0) {
        sprintf(_DUCO_ERROR, "Couldn't read response.\n");
        free(conn);

        return NULL;
    }

    conn->version[recn] = 0;

    return conn;
}

void duco_disconnect(duco_connection_t *conn) {
#ifdef _WIN32
    shutdown(conn->sockfd, SD_BOTH);
    closesocket(conn->sockfd);
    WSACleanup();
#else
    close(conn->sockfd);
#endif

    free(conn);
    conn = NULL;
}

void duco_perror_nomsg() {
    fprintf(stderr, "%s\n", _DUCO_ERROR);
}

void duco_perror(const char *msg) {
    fprintf(stderr, "%s: %s\n", msg, _DUCO_ERROR);
}

int duco_register(duco_connection_t* conn, const char* username, const char* password, const char* email) {
    int recn;
    char* token;

    strcpy(_DUCO_BUFFER, "REGI,");
    strcat(_DUCO_BUFFER, username);
    strcat(_DUCO_BUFFER, ",");
    strcat(_DUCO_BUFFER, password);
    strcat(_DUCO_BUFFER, ",");
    strcat(_DUCO_BUFFER, email);

    write(conn->sockfd, _DUCO_BUFFER, strlen(_DUCO_BUFFER));

    /* 
     * Normally we wouldn't need to clear the buffer, but since we have the plain text password in there,
     * we should get rid of that. Who the fuck designed the API? Plain passwords, istg.
     */
    memset(_DUCO_BUFFER, 0, strlen(_DUCO_BUFFER));

    recn = read(conn->sockfd, _DUCO_BUFFER, 255);

    if (recn < 0) {
        sprintf(_DUCO_ERROR, "Couldn't read response.\n");

        return -1;
    }

    _DUCO_BUFFER[recn] = 0;

    token = strtok(_DUCO_BUFFER, ",");
    
    if (strncmp(token, "OK", 2) == 0) {
        strncpy(conn->username, username, 30);

        return 0;
    }

    /* Return code is "NO" => there was an error */

    sprintf(_DUCO_ERROR, "Couldn't register user: ");
    strcat(_DUCO_ERROR, strtok(NULL, ","));

    return 1;
}

int duco_login(duco_connection_t *conn, const char* username, const char* password) {
    int recn;
    char* token;

    strcpy(_DUCO_BUFFER, "LOGI,");
    strcat(_DUCO_BUFFER, username);
    strcat(_DUCO_BUFFER, ",");
    strcat(_DUCO_BUFFER, password);

    write(conn->sockfd, _DUCO_BUFFER, strlen(_DUCO_BUFFER));
   
    /*
     * Like with duco_register, we definitely don't want to keep a copy of the password in memory. Ever.
     */
    memset(_DUCO_BUFFER, 0, strlen(_DUCO_BUFFER));

    recn = read(conn->sockfd, _DUCO_BUFFER, 255);

    if (recn < 0) {
        sprintf(_DUCO_ERROR, "Couldn't read response.\n");

        return -1;
    }

    _DUCO_BUFFER[recn] = 0;

    token = strtok(_DUCO_BUFFER, ",");
    
    if (strncmp(token, "OK", 2) == 0) {
        strncpy(conn->username, username, 30);

        return 0;
    }

    /* Return code is "NO" => there was an error */

    strcat(_DUCO_ERROR, strtok(NULL, ","));

    return 1;
}

int duco_change_password(duco_connection_t *conn, const char *old_password, const char *new_password) {
    int recn;
    char* token;

    strcpy(_DUCO_BUFFER, "CHGP,");
    strcat(_DUCO_BUFFER, old_password);
    strcat(_DUCO_BUFFER, ",");
    strcat(_DUCO_BUFFER, new_password);
    
    write(conn->sockfd, _DUCO_BUFFER, strlen(_DUCO_BUFFER));
    recn = read(conn->sockfd, _DUCO_BUFFER, 255);

    if (recn < 0) {
        sprintf(_DUCO_ERROR, "Couldn't read response.\n");

        return -1;
    }

    if (recn < 0) {
        sprintf(_DUCO_ERROR, "Couldn't read response.\n");

        return -1;
    }

    _DUCO_BUFFER[recn] = 0;

    token = strtok(_DUCO_BUFFER, ",");
    
    if (strncmp(token, "OK", 2) == 0) {
        return 0;
    }

    /* Return code is "NO" => there was an error */

    strcpy(_DUCO_ERROR, strtok(NULL, ","));

    return 1;
}

double duco_get_balance(duco_connection_t *conn) {
    int recn;

    write(conn->sockfd, "BALA", 4);
    recn = read(conn->sockfd, _DUCO_BUFFER, 255);

    if (recn < 0) {
        sprintf(_DUCO_ERROR, "Couldn't read response.\n");

        return -1;
    }

    return atof(_DUCO_BUFFER);
}

int duco_send_balance(duco_connection_t *conn, const char *recipient, double amount) {
    int recn;
    char* token;

    strcpy(_DUCO_BUFFER, "SEND,-,");
    strcat(_DUCO_BUFFER, recipient);
    strcat(_DUCO_BUFFER, ",");
    sprintf(&_DUCO_BUFFER[strlen(_DUCO_BUFFER)], "%f", amount);

    write(conn->sockfd, _DUCO_BUFFER, strlen(_DUCO_BUFFER));
    recn = read(conn->sockfd, _DUCO_BUFFER, 255);

    if (recn < 0) {
        sprintf(_DUCO_ERROR, "Couldn't read response.\n");

        return -1;
    }

    _DUCO_BUFFER[recn] = 0;

    token = strtok(_DUCO_BUFFER, ",");
    
    if (strncmp(token, "OK", 2) == 0) {
        return 0;
    }

    /* Return code is "NO" => there was an error */

    strcpy(_DUCO_ERROR, strtok(NULL, ","));

    return 1;
}

char* duco_get_transactions(duco_connection_t *conn, unsigned int amount) {
    return duco_get_transactions_from(conn, conn->username, amount);
}

char* duco_get_transactions_from(duco_connection_t *conn, const char *username, unsigned int amount) {
    int recn;
    char* buf = (char*) malloc(1);
    size_t buf_i = 0;
    size_t buf_size = 1;

    strcpy(_DUCO_BUFFER, "GTXL,");
    strcat(_DUCO_BUFFER, username);
    sprintf(&_DUCO_BUFFER[strlen(_DUCO_BUFFER)], ",%u", amount);

    write(conn->sockfd, _DUCO_BUFFER, strlen(_DUCO_BUFFER));
    recn = read(conn->sockfd, _DUCO_BUFFER, 255);

    if (recn < 0) {
        sprintf(_DUCO_ERROR, "Couldn't read response.\n");
        free(buf);

        return NULL;
    }

    if (strncmp(_DUCO_BUFFER, "NO", 2) == 0) {
        strcat(_DUCO_ERROR, &_DUCO_BUFFER[3]);
        
        free(buf);

        return NULL;
    }

    buf = (char*) realloc(buf, buf_size + 256);
    buf_size += 256;

    memcpy(&buf[buf_i], _DUCO_BUFFER, recn);
    buf_i += recn;

    if (recn == 255) {
        while ((recn = read(conn->sockfd, _DUCO_BUFFER, 255)) > 0) {
            if (buf_size - buf_i < recn) {
                buf = (char*) realloc(buf, buf_size + 256);
                buf_size += 256;
            }

            memcpy(&buf[buf_i], _DUCO_BUFFER, recn);
            buf_i += recn;
        }
    }

    buf[buf_i] = 0;
    buf = (char*) realloc(buf, strlen(buf) + 1);

    return buf;
}