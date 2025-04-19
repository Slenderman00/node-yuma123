import bindings from "bindings";
import { getErrorMessage } from "./error-codes.js";
const yuma123 = bindings('yuma123');

const NCX_DISPLAY_MODE_NONE = 0
const NCX_DISPLAY_MODE_PLAIN = 1
const NCX_DISPLAY_MODE_PREFIX = 2
const NCX_DISPLAY_MODE_MODULE = 3
const NCX_DISPLAY_MODE_XML = 4
const NCX_DISPLAY_MODE_XML_NONS = 5
const NCX_DISPLAY_MODE_JSON = 6

const safeConnect = (server, port, username, password, privateKeyPath=null, publicKeyPath=null, other_args=null) => {
    const [connectionStatus, connection] = yuma123.yangrpc.connect(server, port, username, password, privateKeyPath, publicKeyPath, other_args);

    if (connectionStatus != 0) {
        throw new Error(getErrorMessage(connectionStatus))
    }

    return connection;
}

const yangcli = (connection, cmd, displaymode = NCX_DISPLAY_MODE_XML_NONS) => {
    const [cliStatus, rpcData] = yuma123.yangrpc.parse_cli(connection, cmd);

    if (cliStatus != 0) {
        throw new Error(getErrorMessage(cliStatus))
    }

    const [rpcStatus, reply] = yuma123.yangrpc.rpc(connection, rpcData);

    if (rpcStatus != 0) {
        throw new Error(getErrorMessage(rpcStatus))
    }

    yuma123.yuma.init();
    const [res, output] = yuma123.yuma.val_make_serialized_string(reply, displaymode);
    yuma123.yuma.val_free_value(reply);
    yuma123.yuma.val_free_value(rpcData);

    if (res != 0) {
        throw new Error(getErrorMessage(res))
    }

    return output;
}

export {
    safeConnect,
    yangcli,
    yuma123,
    NCX_DISPLAY_MODE_NONE,
    NCX_DISPLAY_MODE_PLAIN,
    NCX_DISPLAY_MODE_PREFIX,
    NCX_DISPLAY_MODE_MODULE,
    NCX_DISPLAY_MODE_XML,
    NCX_DISPLAY_MODE_XML_NONS,
    NCX_DISPLAY_MODE_JSON,
    getErrorMessage
};