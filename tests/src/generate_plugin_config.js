// You will need to create a folder with this name, and put in the abis of your contracts in `abis/`.
// You will also need to create a `b2c.json` file that will hold the methodIDs and location of
// the erc20 tokens that should get displayed.
const pluginFolder = "figment";

function serializeData(pluginName, contractAddress, selector) {
  const length = Buffer.from([pluginName.length]);
  const name = Buffer.from(pluginName);
  const address = Buffer.from(contractAddress.slice(2), "hex");
  const methodId = Buffer.from(selector.slice(2), "hex");

  // Taking .slice(2) to remove the "0x" prefix
  return Buffer.concat([length, name, address, methodId]);
}

function assert(condition, message) {
  if (!condition) {
    throw message || "Assertion failed";
  }
}

// Function to generate the plugin configuration.
function generatePluginConfig(network = "ethereum") {
  const fs = require("fs");
  const files = fs.readdirSync(`networks/${network}/${pluginFolder}/abis/`);

  // `contractsToABIs` holds a mapping of contract addresses to ABIs
  let contractsToABIs = {};
  for (let abiFileName of files) {
    assert(
      abiFileName.toLocaleLowerCase() == abiFileName,
      `FAILED: File ${abiFileName} should be lower case.`,
    );

    // Strip ".json" suffix
    let contractAddress = abiFileName.slice(
      0,
      abiFileName.length - ".json".length,
    );
    // Load the ABI
    let abi = require(
      `../networks/${network}/${pluginFolder}/abis/${abiFileName}`,
    );
    // Add it to contracts
    contractsToABIs[contractAddress] = abi;
  }

  // Load the `b2c.json` file
  const b2c = require(`../networks/${network}/${pluginFolder}/b2c.json`);

  let result = {};

  const PLACE_HOLDER_SIGNATURE =
    "3045022100f6e1a922c745e244fa3ed9a865491672808ef93f492ee0410861d748c5de201f0220160d6522499f3a84fa3e744b3b81e49e129e997b28495e58671a1169b16fa777";

  // Iterate through contracts in the `b2c.json` file
  for (let contract of b2c["contracts"]) {
    let methodsInfo = {};
    const contractAddress = contract["address"];

    assert(
      contractAddress.toLowerCase() == contractAddress,
      `FAILED: Contract Address ${contractAddress} should be lower case`,
    );

    for (let [selector, values] of Object.entries(contract["selectors"])) {
      assert(
        selector.toLowerCase() == selector,
        `FAILED: Selector ${selector} should be lower case`,
      );

      // Gather up the info needed for the end result
      const pluginName = values["plugin"];
      const serializedData = serializeData(
        pluginName,
        contractAddress,
        selector,
      );
      const signature = PLACE_HOLDER_SIGNATURE;

      const erc20OfInterest = values["erc20OfInterest"];
      assert(
        erc20OfInterest.length <= 2,
        `Maximum of 2 erc20OfInterest allowed. Got ${erc20OfInterest.length}`,
      );

      // Put them in `methodsInfo`
      methodsInfo[selector] = {
        erc20OfInterest: values["erc20OfInterest"],
        plugin: pluginName,
        serialized_data: serializedData,
        signature: signature,
      };
    }
    // Add the ABI to `methodsInfo`
    methodsInfo["abi"] = contractsToABIs[contractAddress];
    // Add the `methodsInfo` to the end result
    result[contractAddress] = methodsInfo;
  }

  assert(
    result.length == contractsToABIs.length,
    `FAILED: ${result.length} contracts in b2c.json and ${contractsToABIs.length} contracts in abis/ folder`,
  );

  return result;
}

module.exports = {
  generatePluginConfig,
};
