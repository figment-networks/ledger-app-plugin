#pragma once

#include "eth_internals.h"
#include "eth_plugin_interface.h"
#include <string.h>

// Number of selectors defined in this plugin. Should match the enum `selector_t`.
#define NUM_SELECTORS 1

// Name of the plugin.
#define PLUGIN_NAME "Figment"

// Enumeration of the different selectors possible.
// Should follow the exact same order as the array declared in main.c
typedef enum {
    FIGMENT_DEPOSIT = 0,
} selector_t;

extern const uint32_t FIGMENT_SELECTORS[NUM_SELECTORS];

// Enumeration used to parse the smart contract data.
typedef enum {
    VALIDATOR_PUBKEYS_ARRAY = 0,
    WITHDRAWAL_CREDENTIALS_ARRAY,
    WITHDRAWAL_CREDENTIALS_ARRAY_SIZE,
    WITHDRAWAL_CREDENTIALS_OFFSET,
    WITHDRAWAL_CREDENTIALS_SIZE,
    WITHDRAWAL_CREDENTIALS,
    IGNORED_PARAMETERS,
} parameter;

// Shared global memory with Ethereum app. Must be at most 5 * 32 bytes.
typedef struct context_t {
    // For display.
    uint8_t withdrawal_address[ADDRESS_LENGTH];

    // For parsing data.
    uint8_t next_param; // Set to be the next param we expect to parse.
    uint16_t offset;    // Offset at which the array or struct starts.
    bool go_to_offset;  // If set, will force the parsing to iterate through parameters until
                        // `offset` is reached.

    // For both parsing and display.
    selector_t selectorIndex;
} context_t;

// Piece of code that will check that the above structure is not bigger than 5 * 32. Do not remove
// this check.
_Static_assert(sizeof(context_t) <= 5 * 32, "Structure of parameters too big.");

void handle_provide_parameter(void *parameters);
void handle_query_contract_ui(void *parameters);
void handle_init_contract(void *parameters);
void handle_finalize(void *parameters);
void handle_query_contract_id(void *parameters);
