/*
 * libduco, Copyright (c) 2021 Sarah Klocke <sarah.klocke@outlook.com>
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See the
 * LICENSE.md file for more details.
 */

#ifndef DUCO_API
#define DUCO_API

#include <stddef.h>

#ifdef _WIN32
# include <WinSock2.h>
#else
# include <sys/socket.h>
#endif

#ifndef DUCO_POOL_ADDR
# define DUCO_POOL_ADDR "51.15.127.80"
#endif /* DUCO_POOL_ADDR */

#ifndef DUCO_POOL_PORT
# define DUCO_POOL_PORT 2811
#endif /* DUCO_POOL_PORT */

typedef struct duco_connection_t {
#ifdef _WIN32
    SOCKET sockfd;
#else
    int sockfd;
#endif
    char buffer[256];
    char version[10];
    char username[30];
} duco_connection_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Connect to the duco master server at pool_addr:port. Needs to be called in order for other API calls to work.
 * @param pool_addr The IP address of the master server
 * @param port The port of the master server
 * @return Zero on success, non-zero otherwise
 */
extern duco_connection_t* duco_connect(const char* pool_addr, int port);

/**
 * Disconnect from the server. Also frees up the memory used and sets conn to NULL.
 * @param conn The present connection
 */
extern void duco_disconnect(duco_connection_t* conn);

/**
 * Prints the current error message to stderr with msg prepended.
 * @param msg The message to prepend
 */
extern void duco_perror(const char* msg);

/**
 * Works exactly like duco_perror, but doesn't prepend a message.
 */
extern void duco_perror_nomsg();

/**
 * Registers a new user to Duinocoin. The API guarantees to delete the password from internal buffers after sending it.
 * This will also set the username field in the connection struct.
 * @param conn The present connection
 * @param username The username of the new user
 * @param password The password of the new user
 * @param email The email of the new user
 * @return Zero on success, non-zero otherwise
 */
extern int duco_register(duco_connection_t* conn, const char* username, const char* password, const char* email);

/**
 * Logs an user in. The API guarantees to delete the password from internal buffers after sending it.
 * This will also set the username field in the connection struct.
 * @param conn The present connection
 * @param username The username of the new user
 * @param password The password of the new user
 * @return Zero on success, non-zero otherwise
 */
extern int duco_login(duco_connection_t* conn, const char* username, const char* password);

/**
 * Changes the password of the connected user. The API guarantees to delete the passwords from internal buffers after sending it.
 * @param conn The present connection
 * @param old_password The old password of the user
 * @param new_password The new password of the user
 * @return Zero on success, non-zero otherwise
 */
extern int duco_change_password(duco_connection_t* conn, const char* old_password, const char* new_password);

/**
 * Gets the current balance of the logged in user.
 * @param conn The present connection
 * @return -1, if the query failed, the balance otherwise
 */
extern double duco_get_balance(duco_connection_t* conn);

/**
 * Sends Duinocoin to another account.
 * @param conn The present connection
 * @return Zero on success, non-zero otherwise
 */
extern int duco_send_balance(duco_connection_t* conn, const char* recipient, double amount);

/**
 * Gets a list of past transactions from the currently logged in user.
 * @param conn The present connection
 * @param amount The amount of transactions to query
 * @return A newly allocated JSON string with the transactions in it, or NULL on failure.
 */
extern char* duco_get_transactions(duco_connection_t* conn, unsigned int amount);

/**
 * Gets a list of past transactions from another user.
 * @param conn The present connection
 * @param username The user to query
 * @param amount The amount of transactions to query
 * @return A newly allocated JSON string with the transactions in it, or NULL on failure.
 */
extern char* duco_get_transactions_from(duco_connection_t* conn, const char* username, unsigned int amount);

#ifdef __cplusplus
}
#endif

#endif /* DUCO_API */