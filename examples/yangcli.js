import { yuma123, yangcli } from "../index.js";

const server = "sandbox0.lightside-instruments.com";
const port = 22001;
const username = "pi";
const password = "grimbadgerassault";

const connection = yuma123.yangrpc.connect(server, port, username, password, null, null, null);

let res = yangcli(connection, "xge", yuma123.NCX_DISPLAY_MODE_XML_NONS);

console.log(res);