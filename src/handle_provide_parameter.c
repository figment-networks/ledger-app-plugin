#include "plugin.h"

static bool withdrawal_credentials_match(ethPluginProvideParameter_t *msg, context_t *context) {
    return memcmp(context->withdrawal_credentials, msg->parameter, PARAMETER_LENGTH) == 0;
}

static void handle_deposit(ethPluginProvideParameter_t *msg, context_t *context) {
    if (context->go_to_offset) {
        if (msg->parameterOffset != context->offset) {
            return;
        }
        context->go_to_offset = false;
    }

    switch (context->next_param) {
        case PUBKEYS_ARRAY:
            // Skip the parameter
            context->next_param = WITHDRAWAL_CREDENTIALS_ARRAY;
            break;

        case WITHDRAWAL_CREDENTIALS_ARRAY:
            context->offset =
                SELECTOR_SIZE + U2BE(msg->parameter, PARAMETER_LENGTH - sizeof(context->offset));

            context->go_to_offset = true;
            context->next_param = WITHDRAWAL_CREDENTIALS_ARRAY_LENGTH;
            break;

        case WITHDRAWAL_CREDENTIALS_ARRAY_LENGTH:
            context->validators_count =
                U2BE(msg->parameter, PARAMETER_LENGTH - sizeof(context->validators_count));

            context->next_param = WITHDRAWAL_CREDENTIALS_OFFSET;
            break;

        case WITHDRAWAL_CREDENTIALS_OFFSET:
            context->offset = msg->parameterOffset +
                              U2BE(msg->parameter, PARAMETER_LENGTH - sizeof(context->offset));

            context->go_to_offset = true;
            context->next_param = WITHDRAWAL_CREDENTIALS_LENGTH;
            break;

        case WITHDRAWAL_CREDENTIALS_LENGTH:
            if (U2BE(msg->parameter, PARAMETER_LENGTH - sizeof(uint16_t)) != 32) {
                PRINTF("Invalid withdrawal credentials length\n");
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                break;
            }

            context->next_param = WITHDRAWAL_CREDENTIALS;
            break;

        case WITHDRAWAL_CREDENTIALS:
            // Store the first withdrawal credentials
            if (!context->withdrawal_credentials_stored) {
                copy_parameter(context->withdrawal_credentials,
                               msg->parameter,
                               sizeof(context->withdrawal_credentials));

                context->withdrawal_credentials_stored = true;
            }
            // Check if other withdrawal credentials match the first one
            else if (!withdrawal_credentials_match(msg, context)) {
                context->withdrawal_credentials_differ = true;
            }

            context->validators_count--;

            if (context->validators_count != 0) {
                context->next_param = WITHDRAWAL_CREDENTIALS_LENGTH;
            } else {
                context->next_param = REMAINING_PARAMETERS;
            }
            break;

        case REMAINING_PARAMETERS:
            // Skip the remaining parameters
            break;

        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

void handle_provide_parameter(ethPluginProvideParameter_t *msg) {
    if (msg == NULL) {
        PRINTF("handle_provide_parameter: msg is NULL\n");
        return;
    }

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
