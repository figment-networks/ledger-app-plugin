/*******************************************************************************
 *   Ethereum 2 Deposit Application
 *   (c) 2020 Ledger
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ********************************************************************************/

#include <stdint.h>
#include "figment_plugin.h"

// List of selectors supported by this plugin.
static const uint32_t FIGMENT_DEPOSIT_SELECTOR = 0x4f498c73;

// Array of all the different selectors. Make sure this follows the same order as the
// enum defined in `figment_plugin.h`
const uint32_t FIGMENT_SELECTORS[NUM_SELECTORS] = {
    FIGMENT_DEPOSIT_SELECTOR,
};
