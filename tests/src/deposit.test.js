import "core-js/stable";
import "regenerator-runtime/runtime";

import { ethers, Transaction } from "ethers";
import { ledgerService } from "@ledgerhq/hw-app-eth";
import {
  waitForAppScreen,
  zemu,
  genericTx,
  nanoModels,
  SPECULOS_ADDRESS,
  generateRandomBytes,
} from "./test.fixture";

const contractAddress = "0xf0075b3cf8953d3e23b0ef65960913fd97eb5227";
const pluginName = "figment";
const testNetwork = "ethereum";
const abiPath =
  `../networks/${testNetwork}/${pluginName}/abis/` + contractAddress + ".json";
const abi = require(abiPath);

const withdrawalAddress =
  "0x010000000000000000000000" + SPECULOS_ADDRESS.slice(2).toLowerCase();
const externalWithdrawalAddress =
  "0x010000000000000000000000c146bbfede2786d56047d90b8a5805da5732c0b9";
const blsCredentials =
  "0x00a9e22781d152844f5a8d6d7b30c06e8504f48cdb389ba63eaf85fa0710be92";

nanoModels.forEach(function (model) {
  jest.setTimeout(30000);

  test(
    `[Nano ${model.letter}] Deposit to a single validator`,
    zemu(model, async (sim, eth) => {
      await simulateTransaction(model, sim, eth, {
        testName: "single_validator",
        validatorsCount: 1,
        withdrawalCredentials: withdrawalAddress,
        rightClicks: 4,
      });
    }),
  );

  test(
    `[Nano ${model.letter}] Deposit to multiple validators`,
    zemu(model, async (sim, eth) => {
      await simulateTransaction(model, sim, eth, {
        testName: "multiple_validators",
        validatorsCount: 4,
        withdrawalCredentials: withdrawalAddress,
        rightClicks: 4,
      });
    }),
  );

  test(
    `[Nano ${model.letter}] Deposit with an external withdrawal address`,
    zemu(model, async (sim, eth) => {
      await simulateTransaction(model, sim, eth, {
        testName: "external_withdrawal_address",
        validatorsCount: 1,
        withdrawalCredentials: externalWithdrawalAddress,
        rightClicks: model.letter === "S" ? 7 : 5,
      });
    }),
  );

  test(
    `[Nano ${model.letter}] Deposit with differing withdrawal credentials`,
    zemu(model, async (sim, eth) => {
      await simulateTransaction(model, sim, eth, {
        testName: "differing_withdrawal_addresses",
        validatorsCount: 2,
        withdrawalCredentials: [withdrawalAddress, blsCredentials],
        rightClicks: model.letter === "S" ? 7 : 5,
      });
    }),
  );

  test(
    `[Nano ${model.letter}] Deposit with BLS credentials`,
    zemu(model, async (sim, eth) => {
      await simulateTransaction(model, sim, eth, {
        testName: "bls_credentials",
        validatorsCount: 1,
        withdrawalCredentials: blsCredentials,
        rightClicks: model.letter === "S" ? 7 : 5,
      });
    }),
  );
});

async function simulateTransaction(
  model,
  sim,
  eth,
  { testName, validatorsCount, withdrawalCredentials, rightClicks = 0 },
) {
  // Prepare a transaction
  const tx = prepareTransaction(eth, {
    validatorsCount,
    withdrawalCredentials,
  });

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

async function prepareTransaction(
  eth,
  { validatorsCount = 1, withdrawalCredentials = withdrawalAddress },
) {
  const contract = new ethers.Contract(contractAddress, abi);

  const withdrawalCredentialsArray = Array.isArray(withdrawalCredentials)
    ? withdrawalCredentials
    : Array.from({ length: validatorsCount }, () => withdrawalCredentials);

  // Signature: deposit(bytes[] pubkeys, bytes[] withdrawal_credentials, bytes[] signatures, bytes32[] deposit_data_roots)
  const { data } = await contract.deposit.populateTransaction(
    Array.from({ length: validatorsCount }, () => generateRandomBytes(48)),
    withdrawalCredentialsArray,
    Array.from({ length: validatorsCount }, () => generateRandomBytes(96)),
    Array.from({ length: validatorsCount }, () => generateRandomBytes(32)),
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
