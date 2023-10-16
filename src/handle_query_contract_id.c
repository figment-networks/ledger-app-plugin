#include "figment_plugin.h"

// Sets the first screen to display.
void handle_query_contract_id(void *parameters) {
    ethQueryContractID_t *msg = (ethQueryContractID_t *) parameters;
    const context_t *context = (const context_t *) msg->pluginContext;
    // msg->name will be the upper sentence displayed on the screen.
    // msg->version will be the lower sentence displayed on the screen.

    // For the first screen, display the plugin name.
    size_t length = strlcpy(msg->name, PLUGIN_NAME, msg->nameLength);
    if (length < strlen(PLUGIN_NAME)) {
        PRINTF("Screen title truncated: %s\n", msg->name);
    }

    if (context->selectorIndex == FIGMENT_DEPOSIT) {
        const char *message = "Stake ETH";

        size_t length = strlcpy(msg->version, message, msg->versionLength);
        if (length < strlen(message)) {
            PRINTF("Screen message truncated: %s\n", msg->version);
        }

        msg->result = ETH_PLUGIN_RESULT_OK;
    } else {
        PRINTF("Selector index: %d not supported\n", context->selectorIndex);
        msg->result = ETH_PLUGIN_RESULT_ERROR;
    }
}
