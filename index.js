import bindings from "bindings";
const yuma123 = bindings('yuma123');

const NCX_DISPLAY_MODE_NONE = 0
const NCX_DISPLAY_MODE_PLAIN = 1
const NCX_DISPLAY_MODE_PREFIX = 2
const NCX_DISPLAY_MODE_MODULE = 3
const NCX_DISPLAY_MODE_XML = 4
const NCX_DISPLAY_MODE_XML_NONS = 5
const NCX_DISPLAY_MODE_JSON = 6

const yangcli = (connection, cmd, displaymode = NCX_DISPLAY_MODE_XML_NONS) => {
    const [cliStatus, rpcData] = yuma123.yangrpc.parse_cli(connection, cmd);

    if (!cliStatus) {
        //Implement error handling
    }

    const [rpcStatus, reply] = yuma123.yangrpc.rpc(connection, rpcData);

    if (!rpcStatus) {
        //Implement error handling
    }

    yuma123.yuma.init();
    const [res, output] = yuma123.yuma.val_make_serialized_string(reply, displaymode);

    return output;
}

export {
    yangcli,
    yuma123,
    NCX_DISPLAY_MODE_NONE,
    NCX_DISPLAY_MODE_PLAIN,
    NCX_DISPLAY_MODE_PREFIX,
    NCX_DISPLAY_MODE_MODULE,
    NCX_DISPLAY_MODE_XML,
    NCX_DISPLAY_MODE_XML_NONS,
    NCX_DISPLAY_MODE_JSON
};