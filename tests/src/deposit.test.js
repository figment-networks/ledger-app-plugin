import "core-js/stable";
import "regenerator-runtime/runtime";

import { ethers, Transaction } from "ethers";
import { ledgerService } from "@ledgerhq/hw-app-eth";
import {
  waitForAppScreen,
  zemu,
  genericTx,
  nano_models,
  generateRandomBytes,
} from "./test.fixture";

const contractAddress = "0xf0075b3cf8953d3e23b0ef65960913fd97eb5227";
const pluginName = "figment";
const testNetwork = "ethereum";
const abiPath =
  `../networks/${testNetwork}/${pluginName}/abis/` + contractAddress + ".json";
const abi = require(abiPath);

const withdrawalAddress =
  "0x100000000000000000000000c146bbfede2786d56047d90b8a5805da5732c0b9";

nano_models.forEach(function (model) {
  jest.setTimeout(20000);

  test(
    `[Nano ${model.letter}] Stake 32 ETH`,
    zemu(model, async (sim, eth) => {
      await simulateTransaction(model, sim, eth, {
        testName: "single_validator",
        validatorsCount: 1,
        rightClicks: model.letter === "S" ? 7 : 5,
      });
    }),
  );

  test(
    `[Nano ${model.letter}] Stake 128 ETH`,
    zemu(model, async (sim, eth) => {
      await simulateTransaction(model, sim, eth, {
        testName: "multiple_validators",
        validatorsCount: 4,
        rightClicks: model.letter === "S" ? 7 : 5,
      });
    }),
  );
});

async function simulateTransaction(
  model,
  sim,
  eth,
  { testName, validatorsCount, rightClicks },
) {
  // Prepare a transaction
  const tx = prepareTransaction(eth, { validatorsCount });

  // Wait for the application to actually load and parse the transaction
  await waitForAppScreen(sim);

  // Navigate the display by pressing the right button `rightClicks` times, then pressing both buttons to accept the transaction.
  await sim.navigateAndCompareSnapshots(
    ".",
    `${model.name}_deposit/${testName}`,
    [rightClicks, 0],
  );

  await tx;
}

async function prepareTransaction(eth, { validatorsCount = 1 }) {
  const contract = new ethers.Contract(contractAddress, abi);

  // Signature: deposit(bytes[], bytes[], bytes[], bytes32[])
  const { data } = await contract.deposit.populateTransaction(
    Array.from({ length: validatorsCount }, () => generateRandomBytes(48)), // `pubkeys`
    Array.from({ length: validatorsCount }, () => withdrawalAddress), // `withdrawal_credentials`
    Array.from({ length: validatorsCount }, () => generateRandomBytes(96)), // `signatures`
    Array.from({ length: validatorsCount }, () => generateRandomBytes(32)), // `deposit_data_roots`
  );

  // Get the generic transaction template
  let unsignedTx = genericTx;
  // Modify `to` to make it interact with the contract
  unsignedTx.to = contractAddress;
  // Modify the attached data
  unsignedTx.data = data;
  // Modify the number of ETH sent
  unsignedTx.value = ethers.parseEther((32 * validatorsCount).toString());

  // Create `serializedTx` and remove the "0x" prefix
  const serializedTx = Transaction.from(unsignedTx).unsignedSerialized.slice(2);

  // Resolve transaction metadata
  const resolution = await ledgerService.resolveTransaction(
    serializedTx,
    eth.loadConfig,
    {
      externalPlugins: true,
    },
  );

  // Sign the transaction
  return eth.signTransaction("44'/60'/0'/0", serializedTx, resolution);
}
