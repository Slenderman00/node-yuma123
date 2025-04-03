const fs = require("fs");

if(!fs.existsSync("/usr/include/yuma")) {
    console.error("Yuma123 not found at path: /usr/include/yuma")
    process.exit(1);
}