import { NCX_DISPLAY_MODE_XML_NONS, yangcli, yuma123} from "../index.js";

const server = "127.0.0.1";
const port = 830;
const username = "user";
const password = "pass";


yuma123.yangrpc.async_connect(server, port, username, password, null, null, null).then((connection) => {
    let res = yangcli(connection, "xget /", NCX_DISPLAY_MODE_XML_NONS);
    console.log(res);
    yuma123.yanrpc.close(connection)
}).catch((e) => {
    console.log(e.code);
});