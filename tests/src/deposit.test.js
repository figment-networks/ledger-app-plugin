import "core-js/stable";
import "regenerator-runtime/runtime";

import { waitForAppScreen, zemu, genericTx, nano_models } from './test.fixture';
import { ethers } from "ethers";
import { parseEther } from "ethers/lib/utils";

const contractAddr = "0xf0075b3cf8953d3e23b0ef65960913fd97eb5227";
const pluginName = "figment";
const testNetwork = "ethereum";
const abiPath = `../networks/${testNetwork}/${pluginName}/abis/` + contractAddr + '.json';
const abi = require(abiPath);

// Test from constructed transaction
nano_models.forEach(function(model) {
  jest.setTimeout(20000);

  test('[Nano ' + model.letter + '] Stake ETH', zemu(model, async (sim, eth) => {
    const contract = new ethers.Contract(contractAddr, abi);

    // Signature: deposit(bytes[], bytes[], bytes[], bytes32[])
    const { data } = await contract.populateTransaction.deposit([], [], [], []);

    // Get the generic transaction template
    let unsignedTx = genericTx;
    // Modify `to` to make it interact with the contract
    unsignedTx.to = contractAddr;
    // Modify the attached data
    unsignedTx.data = data;
    // Modify the number of ETH sent
    unsignedTx.value = parseEther("64");

    // Create serializedTx and remove the "0x" prefix
    const serializedTx = ethers.utils.serializeTransaction(unsignedTx).slice(2);

    const tx = eth.signTransaction("44'/60'/0'/0", serializedTx);

    // Wait for the application to actually load and parse the transaction
    await waitForAppScreen(sim);

    // Navigate the display by pressing the right button 4 times, then pressing both buttons to accept the transaction.
    const rightClicks = 4;
    await sim.navigateAndCompareSnapshots('.', model.name + '_deposit', [rightClicks, 0]);

    await tx;
  }));
});

