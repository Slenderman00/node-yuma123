import { yuma123, yangcli } from "../index.js";

const server = "127.0.0.1";
const port = 830;
const username = "user";
const password = "pass";

const connection = yuma123.yangrpc.connect(server, port, username, password, null, null, null);

let res = yangcli(connection, "xget /", yuma123.NCX_DISPLAY_MODE_XML_NONS);

console.log(res);