#include "figment_plugin.h"

static void handle_deposit(ethPluginProvideParameter_t *msg, context_t *context) {
    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset) {
            return;
        }
        context->go_to_offset = false;
    }

    switch (context->next_param) {
        case VALIDATOR_PUBKEYS_ARRAY:
            // Skip the parameter
            context->next_param = WITHDRAWAL_CREDENTIALS_ARRAY;
            break;

        case WITHDRAWAL_CREDENTIALS_ARRAY:
            context->offset =
                SELECTOR_SIZE + U2BE(msg->parameter, PARAMETER_LENGTH - sizeof(context->offset));

            context->go_to_offset = true;
            context->next_param = WITHDRAWAL_CREDENTIALS_ARRAY_SIZE;
            break;

        case WITHDRAWAL_CREDENTIALS_ARRAY_SIZE:
            // Skip the parameter
            context->next_param = WITHDRAWAL_CREDENTIALS_OFFSET;
            break;

        case WITHDRAWAL_CREDENTIALS_OFFSET:
            context->offset = msg->parameterOffset +
                              U2BE(msg->parameter, PARAMETER_LENGTH - sizeof(context->offset));

            context->go_to_offset = true;
            context->next_param = WITHDRAWAL_CREDENTIALS_SIZE;
            break;

        case WITHDRAWAL_CREDENTIALS_SIZE:
            // Skip the parameter
            context->next_param = WITHDRAWAL_CREDENTIALS;
            break;

        case WITHDRAWAL_CREDENTIALS:
            copy_parameter(context->withdrawal_credentials,
                           msg->parameter,
                           sizeof(context->withdrawal_credentials));

            // Copy the withdrawal address (if present)
            if (context->withdrawal_credentials[0] == 0x01) {
                copy_address(context->withdrawal_address,
                             msg->parameter,
                             sizeof(context->withdrawal_address));
            }

            context->next_param = IGNORED_PARAMETERS;
            break;

        case IGNORED_PARAMETERS:
            // Skip ignored parameters
            break;

        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

void handle_provide_parameter(void *parameters) {
    ethPluginProvideParameter_t *msg = (ethPluginProvideParameter_t *) parameters;
    context_t *context = (context_t *) msg->pluginContext;
    // We use `%.*H`: it's a utility function to print bytes. You first give
    // the number of bytes you wish to print (in this case, `PARAMETER_LENGTH`) and then
    // the address (here `msg->parameter`).
    PRINTF("plugin provide parameter: offset %d\nBytes: %.*H\n",
           msg->parameterOffset,
           PARAMETER_LENGTH,
           msg->parameter);

    msg->result = ETH_PLUGIN_RESULT_OK;

    switch (context->selectorIndex) {
        case FIGMENT_DEPOSIT:
            handle_deposit(msg, context);
            break;

        default:
            PRINTF("Selector Index not supported: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}
