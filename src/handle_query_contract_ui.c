#include "plugin.h"

static void set_screen_title(ethQueryContractUI_t *msg, const char *title) {
    if (strlcpy(msg->title, title, msg->titleLength) < strlen(title)) {
        PRINTF("Screen title truncated: %s\n", msg->title);
    }
}

static void set_screen_message(ethQueryContractUI_t *msg, const char *message) {
    if (strlcpy(msg->msg, message, msg->msgLength) < strlen(message)) {
        PRINTF("Screen message truncated: %s\n", msg->msg);
    }
}

// Set UI for the "Amount" screen.
static bool set_amount_ui(ethQueryContractUI_t *msg) {
    set_screen_title(msg, "Amount");

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

// Set UI for the "Withdrawal Address Warning" screen.
static bool set_withdrawal_address_warning_ui(ethQueryContractUI_t *msg) {
    set_screen_title(msg, "Funds at risk");
    set_screen_message(msg, "Withdrawal address does not match your wallet address.");

    return true;
}

void handle_query_contract_ui(ethQueryContractUI_t *msg) {
    if (msg == NULL) {
        PRINTF("handle_query_contract_ui: msg is NULL\n");
        return;
    }

    bool result = false;

    // Clean the display fields.
    memset(msg->title, 0, msg->titleLength);
    memset(msg->msg, 0, msg->msgLength);

    switch (msg->screenIndex) {
        case 0:
            result = set_amount_ui(msg);
            break;

        case 1:
            result = set_withdrawal_address_warning_ui(msg);
            break;

        // Keep this
        default:
            PRINTF("Received an invalid screenIndex\n");
    }

    msg->result = result ? ETH_PLUGIN_RESULT_OK : ETH_PLUGIN_RESULT_ERROR;
}
