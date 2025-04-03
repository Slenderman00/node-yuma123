# Node-Yuma123

Node-Yuma123 is a Node.js module providing bindings for the Yuma123 library, enabling interaction with YANG models and NETCONF protocols. This project facilitates the integration of Yuma123 functionalities into Node.js applications, allowing developers to leverage YANG and NETCONF capabilities in a JavaScript environment.

## Features

- Connect to NETCONF servers using Yuma123.
- Execute RPC commands and parse CLI commands.
- Load and manage YANG modules and configurations.
- Interact with YANG data structures and serialize them.

## Installation

To install Node-Yuma123, ensure you have yuma123 installed, then run:

```bash
npm i node-yuma123
```

This will build the native module using Node-gyp.

## Usage

Below is a basic example of how to use Node-Yuma123 in your Node.js application:

```javascript
const yuma123 = require('node-yuma123');

const server = "127.0.0.1";
const port = 830;
const username = "user";
const password = "pass";

const connection = yuma123.yangrpc.connect(server, port, username, password, null, null, null);
console.log("Connection:", connection ? "successful" : "failed");

const [cliStatus, rpcData] = yuma123.yangrpc.parse_cli(connection, "xget /");
console.log("Command parsed:", cliStatus === 0 ? "yes" : "no");

const [rpcStatus, reply] = yuma123.yangrpc.rpc(connection, rpcData);
console.log("Command executed:", rpcStatus === 0 ? "successfully" : "failed");

const res = yuma123.yuma.init();
console.log(res);
```

## API

### yangrpc

- `connect(server, port, username, password, public_key, private_key, other_args)`: Connects to a NETCONF server.
- `rpc(connection, rpcData)`: Executes an RPC command.
- `parse_cli(connection, command)`: Parses a CLI command into an RPC structure.
- `close(connection)`: Closes the connection to the NETCONF server.

### yuma

- `init()`: Initializes the Yuma environment.
- `schema_module_load(moduleName)`: Loads a YANG module.
- `cfg_load(configFile)`: Loads a configuration file.
- `val_find_child(parent, namespace, name)`: Finds a child value in a YANG data structure.
- `val_string(value)`: Retrieves the string representation of a value.
- `val_dump_value(value, flag)`: Dumps the value for debugging purposes.
- `val_dump_value_ex(value, flag, display_mode)`: Dumps the value for debugging purposes allows user to specify display mode.
- `val_make_serialized_string(value, mode)`: Serializes a value into a string.
- `val_free_value(value)`: Frees a value from memory.

## Dependencies

- Node.js
- Node-gyp
- Yuma123 library

## TODO
- [x] fix: val_make_serialized_string

## License

This project is licensed under the ISC License.

## Author

Joar Heimonen

## Contributing

Contributions are welcome! Please submit issues or pull requests for any improvements or bug fixes.

## Support

Please open a ticket on the github page.
