# Node-Yuma123

[![Run Tests](https://github.com/Slenderman00/node-yuma123/actions/workflows/tests.yaml/badge.svg)](https://github.com/Slenderman00/node-yuma123/actions/workflows/tests.yaml)

Node-Yuma123 is a Node.js module providing bindings for the Yuma123 library, enabling interaction with YANG models and the NETCONF protocol. This project facilitates the integration of Yuma123 functionalities into Node.js applications, allowing developers to leverage YANG and NETCONF capabilities in a JavaScript environment.

## Features

- Connect to NETCONF servers using Yuma123 (synchronously or asynchronously)
- Execute RPC commands and parse yangcli commands
- Load and manage YANG modules and configurations
- Interact with YANG data structures and serialize them
- Comprehensive error handling with detailed error codes

## Installation

To install Node-Yuma123, ensure you have yuma123 installed, then run:

```bash
npm i node-yuma123
```

This will build the native module using Node-gyp.

## Usage

### Basic Connection Example

Below is a basic example of how to connect to a NETCONF server and execute a command:

```javascript
import { yuma123, NCX_DISPLAY_MODE_XML_NONS } from "node-yuma123";

const server = "127.0.0.1";
const port = 830;
const username = "user";
const password = "pass";

// Connect to the server
const [connectionStatus, connection] = yuma123.yangrpc.connect(server, port, username, password, null, null, null);
console.log("Connection status:", connectionStatus === 0 ? "successful" : "failed");

// Parse a CLI command
const [cliStatus, rpcData] = yuma123.yangrpc.parse_cli(connection, "xget /");
console.log("Command parsed:", cliStatus === 0 ? "yes" : "no");

// Execute the RPC command
const [rpcStatus, reply] = yuma123.yangrpc.rpc(connection, rpcData);
console.log("Command executed:", rpcStatus === 0 ? "successfully" : "failed");

// Initialize Yuma and serialize the response
yuma123.yuma.init();
const [res, output] = yuma123.yuma.val_make_serialized_string(reply, NCX_DISPLAY_MODE_XML_NONS);
console.log(output);

// Don't forget to close the connection when done
yuma123.yangrpc.close(connection);
```

### Using the Higher-Level API

The package provides convenient wrapper functions to simplify common operations:

```javascript
import { NCX_DISPLAY_MODE_XML_NONS, yangcli, safeConnect, yuma123 } from "node-yuma123";

const server = "127.0.0.1";
const port = 830;
const username = "user";
const password = "pass";

// Safe connection that throws errors on failure
const connection = safeConnect(server, port, username, password, null, null, null);

// Execute a command and get the result in one step
let result = yangcli(connection, "xget /", NCX_DISPLAY_MODE_XML_NONS);
console.log(result);

// Close the connection when done
yuma123.yangrpc.close(connection);
```

### Asynchronous Connection

Node-Yuma123 also supports asynchronous connections using Promises:

```javascript
import { NCX_DISPLAY_MODE_XML_NONS, yangcli, yuma123 } from "node-yuma123";

const server = "127.0.0.1";
const port = 830;
const username = "user";
const password = "pass";

// Connect asynchronously
yuma123.yangrpc.async_connect(server, port, username, password, null, null, null)
    .then((connection) => {
        // Execute command once connected
        let result = yangcli(connection, "xget /", NCX_DISPLAY_MODE_XML_NONS);
        console.log(result);
        
        // Close the connection
        yuma123.yangrpc.close(connection);
    })
    .catch((error) => {
        console.error("Connection failed with code:", error.code);
    });
```

## API

### yangrpc

- `connect(server, port, username, password, public_key, private_key, other_args)`: Connects to a NETCONF server. Returns `[status, connection]`.
- `async_connect(server, port, username, password, public_key, private_key, other_args)`: Connects to a NETCONF server asynchronously. Returns a Promise.
- `rpc(connection, rpcData)`: Executes an RPC command. Returns `[status, reply]`.
- `parse_cli(connection, command)`: Parses a CLI command into an RPC structure. Returns `[status, rpcData]`.
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

### High-level wrappers

- `safeConnect(server, port, username, password, public_key, private_key, other_args)`: Wrapper that connects to a NETCONF server and performs error checking. Throws an error if connection fails.
- `yangcli(connection, command, mode)`: Wrapper that parses a yangcli command and sends an RPC request. Returns the serialized response string.

For a complete list of supported yangcli commands, refer to the [Yuma123 yangcli Manual](https://yuma123.org/wiki/index.php/Yuma_yangcli_Manual#Commands).

### Display Modes

The following display modes are available for serializing output:

- `NCX_DISPLAY_MODE_NONE`: No display
- `NCX_DISPLAY_MODE_PLAIN`: Plain text format
- `NCX_DISPLAY_MODE_PREFIX`: With prefix format
- `NCX_DISPLAY_MODE_MODULE`: Module format
- `NCX_DISPLAY_MODE_XML`: XML format with namespaces
- `NCX_DISPLAY_MODE_XML_NONS`: XML format without namespaces
- `NCX_DISPLAY_MODE_JSON`: JSON format

## Error Handling

Node-Yuma123 provides detailed error codes for diagnosing issues. You can use the `getErrorMessage` function to translate numeric error codes into human-readable messages:

```javascript
import { getErrorMessage } from "node-yuma123/error-codes.js";

try {
    const connection = safeConnect(server, port, username, password, null, null, null);
    // ...
} catch (error) {
    console.error("Connection error:", error.message);
    // Error message will contain the human-readable description from error-codes.js
}
```

## Dependencies

- Node-gyp
- bindings
- node-addon-api
- Yuma123 library

## Testing

The module includes Jest-based tests that run in a containerized environment using Podman. This ensures consistent testing across different environments.

To run the tests:

```bash
chmod +x ./test.sh
./test.sh
```

This script builds a Podman container with all the necessary dependencies and runs the tests inside it. The container setup includes:

- Installing required dependencies (libyangrpc-dev, libyuma-dev, etc.)
- Setting up SSH with NETCONF subsystem
- Starting a netconfd instance for testing
- Running the Jest test suite

Tests are also automatically run in CI via GitHub Actions using the same containerized test approach.

## License

This project is licensed under the ISC License.

## Author

Joar Heimonen

## Contributing

Contributions are welcome! Please submit issues or pull requests for any improvements or bug fixes.

## Support

Please open a ticket on the GitHub page.