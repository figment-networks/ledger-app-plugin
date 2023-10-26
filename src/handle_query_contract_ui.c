#include <stdbool.h>
#include "figment_plugin.h"

// Set UI for the "Amount" screen.
static bool set_amount_ui(ethQueryContractUI_t *msg) {
    const char *title = "Amount";

    if (strlcpy(msg->title, title, msg->titleLength) < strlen(title)) {
        PRINTF("Screen title truncated: %s\n", msg->title);
    }

    const uint8_t *eth_amount = msg->pluginSharedRO->txContent->value.value;
    uint8_t eth_amount_size = msg->pluginSharedRO->txContent->value.length;

    // Converts the uint256 number located in `eth_amount` to its string representation and
    // copies this to `msg->msg`.
    return amountToString(eth_amount,
                          eth_amount_size,
                          WEI_TO_ETHER,
                          "ETH",
                          msg->msg,
                          msg->msgLength);
}

void handle_query_contract_ui(ethQueryContractUI_t *msg) {
    bool ret = false;

    // msg->title is the upper line displayed on the device.
    // msg->msg is the lower line displayed on the device.

    // Clean the display fields.
    memset(msg->title, 0, msg->titleLength);
    memset(msg->msg, 0, msg->msgLength);

    switch (msg->screenIndex) {
        case 0:
            ret = set_amount_ui(msg);
            break;

        // Keep this
        default:
            PRINTF("Received an invalid screenIndex\n");
            break;
    }
    msg->result = ret ? ETH_PLUGIN_RESULT_OK : ETH_PLUGIN_RESULT_ERROR;
}
