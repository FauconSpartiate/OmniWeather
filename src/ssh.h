#ifndef SSH_H
#define SSH_H
#include "Arduino.h"
#include "OmniWeather.h"
#include <libssh/libssh.h>
#include "libssh_esp32.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/reent.h>

int chdir(const char *path);
void error(ssh_session session);
int authenticate_console(ssh_session session);
void usage(const char *argv0);
int opts(int argc, char **argv);
void location_free(struct location *loc);
struct location *parse_location(char *ploc);
void close_location(struct location *loc);
int open_location(struct location *loc, int flag);
/** @brief copies files from source location to destination
 * @param src source location
 * @param dest destination location
 * @param recursive Copy also directories
 */
int do_copy(struct location *src, struct location *dest, int recursive);
ssh_session connect_ssh(const char *host, const char *user, int verbosity);
int ex_main(int argc, char **argv);

#endif