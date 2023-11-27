#include "plugin.h"

void handle_provide_token(ethPluginProvideInfo_t *msg) {
    if (msg == NULL) {
        PRINTF("handle_provide_token: msg is NULL\n");
        return;
    }

    msg->result = ETH_PLUGIN_RESULT_OK;
}
