#include "figment_plugin.h"

void handle_finalize(void *parameters) {
    ethPluginFinalize_t *msg = (ethPluginFinalize_t *) parameters;

    msg->uiType = ETH_UI_TYPE_GENERIC;
    msg->numScreens = 2;

    msg->result = ETH_PLUGIN_RESULT_OK;
}
