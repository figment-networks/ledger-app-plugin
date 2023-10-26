#include "figment_plugin.h"

void handle_finalize(ethPluginFinalize_t *msg) {
    msg->uiType = ETH_UI_TYPE_GENERIC;

    msg->numScreens = 1;
    msg->result = ETH_PLUGIN_RESULT_OK;
}
