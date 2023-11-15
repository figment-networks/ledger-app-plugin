#include "figment_plugin.h"

static void set_screen_title(ethQueryContractUI_t *msg, const char *title) {
    if (strlcpy(msg->title, title, msg->titleLength) < strlen(title)) {
        PRINTF("Screen title truncated: %s\n", msg->title);
    }
}

// Set UI for the "Amount" screen.
static bool set_amount_ui(ethQueryContractUI_t *msg) {
    set_screen_title(msg, "Amount");

    const uint8_t *eth_amount = msg->pluginSharedRO->txContent->value.value;
    uint8_t eth_amount_size = msg->pluginSharedRO->txContent->value.length;

    // Converts the uint256 number located in `eth_amount` to its string representation and
    // copies this to `msg->msg`.
    return amountToString(eth_amount, eth_amount_size, WEI_TO_ETHER, "ETH", msg->msg,
        msg->msgLength);
}

// Set UI for the "Withdrawal Address" screen.
static bool set_withdrawal_address_ui(ethQueryContractUI_t *msg, context_t *context) {
    set_screen_title(msg, "Withdrawal Address");

    // Prefix the address with `0x`.
    msg->msg[0] = '0';
    msg->msg[1] = 'x';

    // We need a random chainID for legacy reasons with `getEthAddressStringFromBinary`.
    // Setting it to `0` will make it work with every chainID.
    uint64_t chain_id = 0;

    // Get the string representation of the address stored in `context->withdrawal_address`.
    // Put it in `msg->msg`.
    return getEthAddressStringFromBinary(
        context->withdrawal_address,
        msg->msg + 2, // +2 here because we've already prefixed with '0x'.
        msg->pluginSharedRW->sha3,
        chain_id);
}

void handle_query_contract_ui(void *parameters) {
    ethQueryContractUI_t *msg = (ethQueryContractUI_t *) parameters;
    context_t *context = (context_t *) msg->pluginContext;

    // msg->title is the upper line displayed on the device.
    // msg->msg is the lower line displayed on the device.

    // Clean the display fields.
    memset(msg->title, 0, msg->titleLength);
    memset(msg->msg, 0, msg->msgLength);

    bool result = false;

    switch (msg->screenIndex) {
        case 0:
            result = set_amount_ui(msg);
            break;

        case 1:
            result = set_withdrawal_address_ui(msg, context);
            break;

        // Keep this
        default:
            PRINTF("Received an invalid screenIndex\n");
    }

    msg->result = result ? ETH_PLUGIN_RESULT_OK : ETH_PLUGIN_RESULT_ERROR;
}
