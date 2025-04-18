import { NCX_DISPLAY_MODE_XML_NONS, yangcli, safeConnect, yuma123} from "../index.js";

const server = "127.0.0.1";
const port = 830;
const username = "user";
const password = "pass";

const connection = safeConnect(server, port, username, password, null, null, null);
let res = yangcli(connection, "xget /", NCX_DISPLAY_MODE_XML_NONS);

yuma123.yanrpc.close(connection)
