#include "plugin.h"

// Type 1 withdrawal credentials prefix
#define ETH1_ADDRESS_WITHDRAWAL_PREFIX 0x01

static bool withdrawal_address_matches_sender(ethPluginFinalize_t *msg) {
    context_t *context = (context_t *) msg->pluginContext;

    return memcmp(context->withdrawal_address, msg->address, ADDRESS_LENGTH) == 0;
}

void handle_finalize(ethPluginFinalize_t *msg) {
    if (msg == NULL) {
        PRINTF("handle_finalize: msg is NULL\n");
        return;
    }

    context_t *context = (context_t *) msg->pluginContext;

    msg->uiType = ETH_UI_TYPE_GENERIC;
    msg->numScreens = 1;

    // Copy the withdrawal address (if present)
    if (context->withdrawal_credentials[0] == ETH1_ADDRESS_WITHDRAWAL_PREFIX) {
        copy_address(context->withdrawal_address,
                     context->withdrawal_credentials,
                     sizeof(context->withdrawal_address));
    }
    // Show a warning screen when:
    // - withdrawal credentials differ across validators
    // - the withdrawal address does not match the sender address
    if (context->withdrawal_credentials_differ || !withdrawal_address_matches_sender(msg)) {
        msg->numScreens += 1;
    }

    msg->result = ETH_PLUGIN_RESULT_OK;
}
