const HashRing = require("../index");

const nodes = {
  "127.0.0.1:8080": 1,
  "127.0.0.2:8080": 1,
  "127.0.0.3:8080": 1
};

const time = new Date().getTime();

for(let i = 0; i < 100000; i++) {
  const ring = new HashRing(nodes);
  ring.getNode('a'+i+'b'+i);
}

console.log('Time: ' + (new Date().getTime() - time) / 100 + ' s');

console.log("Cool down...");

setTimeout(function() {
  console.log("Done.");
}, 15000);
