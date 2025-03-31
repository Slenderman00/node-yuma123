const yuma123 = require('../build/Release/yang');

const server = "127.0.0.1";
const port = 830;
const username = "user";
const password = "pass";

const connection = yuma123.yangrpc.connect(server, port, username, password, null, null, null);

if (connection) {
  console.log("Connection successful");
} else {
  console.log("Connection failed");
}
