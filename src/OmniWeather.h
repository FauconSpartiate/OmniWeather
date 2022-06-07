#ifndef OMNIWEATHER_H
#define OMNIWEATHER_H

#include <libssh/libssh.h>
#include <esp_event_legacy.h>
#include "authentication.h"

ssh_session connect_ssh(const char *hostname, const char *user, int verbosity);
esp_err_t event_cb(void *ctx, system_event_t *event);
void connectWiFI();
void disconnectWiFI();

#endif
