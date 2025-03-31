const yuma123 = require('./build/Release/yang');

const yangcli = (connection, cmd) =>  {
    const [cliStatus, rpcData] = yuma123.yangrpc.parse_cli(connection, cmd);

    if(!cliStatus) {
        //Implement error handling
    }

    const [rpcStatus, reply] = yuma123.yangrpc.rpc(connection, rpcData);

    if(!rpcStatus) {
        //Implement error handling
    }

    yuma123.yuma.init();
    const [res, output] = yuma123.yuma.val_make_serialized_string(reply, 5);

    return output;
}

module.exports = {...yuma123, yangcli };