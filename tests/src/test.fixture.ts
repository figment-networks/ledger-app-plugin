import crypto from "crypto";
import Zemu, { DEFAULT_START_OPTIONS } from "@zondax/zemu";
import Eth from "@ledgerhq/hw-app-eth";
import { generatePluginConfig } from "./generate_plugin_config";
import { parseEther, parseUnits } from "ethers";

const transactionUploadDelay = 60000;

async function waitForAppScreen(sim) {
  await sim.waitUntilScreenIsNot(
    sim.getMainMenuSnapshot(),
    transactionUploadDelay,
  );
}

function generateRandomBytes(size: number): string {
  return "0x" + crypto.randomBytes(size).toString("hex");
}

const simOptionsNano = {
  ...DEFAULT_START_OPTIONS,
  logging: true,
  X11: true,
  startDelay: 5000,
  startText: "is ready",
};

const Resolve = require("path").resolve;

const nanoModels = [
  {
    name: "nanos",
    letter: "S",
    path: Resolve("elfs/plugin_nanos.elf"),
    ethPath: Resolve("elfs/ethereum_nanos.elf"),
  },
  {
    name: "nanosp",
    letter: "SP",
    path: Resolve("elfs/plugin_nanosp.elf"),
    ethPath: Resolve("elfs/ethereum_nanosp.elf"),
  },
  {
    name: "nanox",
    letter: "X",
    path: Resolve("elfs/plugin_nanox.elf"),
    ethPath: Resolve("elfs/ethereum_nanox.elf"),
  },
];

const figmentJSON = generatePluginConfig();

const SPECULOS_ADDRESS = "0xFE984369CE3919AA7BB4F431082D027B4F8ED70C";
const RANDOM_ADDRESS = "0xaaaabbbbccccddddeeeeffffgggghhhhiiiijjjj";

let genericTx = {
  nonce: Number(0),
  gasLimit: Number(21000),
  gasPrice: parseUnits("1", "gwei"),
  value: parseEther("1"),
  chainId: 1,
  to: RANDOM_ADDRESS,
  data: null,
};

const TIMEOUT = 1000000;

function zemu(device, func) {
  return async () => {
    jest.setTimeout(TIMEOUT);

    const sim = new Zemu(device.ethPath, { Figment: device.path });

    try {
      await sim.start({ ...simOptionsNano, model: device.name });

      const transport = await sim.getTransport();
      const eth = new Eth(transport);

      eth.setLoadConfig({
        pluginBaseURL: null,
        extraPlugins: figmentJSON,
      });

      await func(sim, eth);
    } finally {
      await sim.close();
    }
  };
}

module.exports = {
  zemu,
  waitForAppScreen,
  genericTx,
  nanoModels,
  SPECULOS_ADDRESS,
  RANDOM_ADDRESS,
  generateRandomBytes,
};
