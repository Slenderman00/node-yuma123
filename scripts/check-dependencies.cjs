const fs = require("fs");

if(!fs.existsSync("/usr/include/yuma")) {
    console.error("Yuma123 not found at path: /usr/include/yuma")
    process.exit(1);
}

if(!fs.existsSync("/usr/include/yuma/yangrpc")) {
    console.error("Yangrpc not found at path: /usr/include/yuma/yangrpc")
    process.exit(1);
}