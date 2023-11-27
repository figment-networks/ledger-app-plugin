#include "plugin.h"

// Sets the first screen to display.
void handle_query_contract_id(ethQueryContractID_t *msg) {
    if (msg == NULL) {
        PRINTF("handle_query_contract_id: msg is NULL\n");
        return;
    }

    const context_t *context = (const context_t *) msg->pluginContext;

    // For the first screen, display the plugin name.
    if (strlcpy(msg->name, APPNAME, msg->nameLength) < strlen(APPNAME)) {
        PRINTF("Screen title truncated: %s\n", msg->name);
    }

    if (context->selectorIndex == FIGMENT_DEPOSIT) {
        const char *message = "Stake ETH";

        if (strlcpy(msg->version, message, msg->versionLength) < strlen(message)) {
            PRINTF("Screen message truncated: %s\n", msg->version);
        }

        msg->result = ETH_PLUGIN_RESULT_OK;
    } else {
        PRINTF("Selector index: %d not supported\n", context->selectorIndex);
        msg->result = ETH_PLUGIN_RESULT_ERROR;
    }
}
