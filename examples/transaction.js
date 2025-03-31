const yuma123 = require('yuma123');

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

yuma123.yuma.init();
const [res, output] = yuma123.yuma.val_make_serialized_string(reply, 5);
console.log(output)