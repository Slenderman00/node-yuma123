const binding = require('yuma123');

const server = "127.0.0.1";
const port = 830;
const username = "user";
const password = "pass";

const connection = binding.yangrpc.connect(server, port, username, password, null, null, null);
console.log("Connection:", connection ? "successful" : "failed");

const [cliStatus, rpcData] = binding.yangrpc.parse_cli(connection, "xget /");
console.log("Command parsed:", cliStatus === 0 ? "yes" : "no");

const [rpcStatus, reply] = binding.yangrpc.rpc(connection, rpcData);
console.log("Command executed:", rpcStatus === 0 ? "successfully" : "failed");

const res = binding.yuma.init();
console.log(res)