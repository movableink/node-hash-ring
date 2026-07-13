## node-hash-ring - Consistent Hashing C++ Add-on for node.js
---

See this [blog post](http://ngchi.wordpress.com/2010/08/23/towards-auto-sharding-in-your-node-js-app/) for background on the algorithm.

### Installation

Via npm:

    npm install @movable/hash_ring

The native addon is compiled on install via `node-gyp` (invoked automatically), so a C++ toolchain is required.

From source:

    git clone https://github.com/movableink/node-hash-ring.git
    cd node-hash-ring
    npm install

### Example

```javascript
const { HashRing } = require("@movable/hash_ring");

// Create a cluster of 3 servers weighted so that 127.0.0.2:8080 stores twice as many
// keys as 127.0.0.1:8080, and 127.0.0.3:8080 stores 4x as many keys as 127.0.0.1:8080
// and 2x as many keys as 127.0.0.2:8080
const ring = new HashRing({ "127.0.0.1:8080": 1, "127.0.0.2:8080": 2, "127.0.0.3:8080": 4 });

console.log(ring.getNode("users:102")); // Should be 127.0.0.3:8080
console.log(ring.getBuckets());         // ['127.0.0.1:8080', '127.0.0.2:8080', '127.0.0.3:8080']
```

An optional second argument controls the number of replica points per bucket (precision):

```javascript
const ring = new HashRing({ a: 5, b: 3 }, 500);
```

TypeScript definitions are bundled ([index.d.ts](index.d.ts)).

### Tests

Tests run with [mocha](https://mochajs.org/):

    npm test

### License
MIT License

---
### Author
Brian Noguchi

Maintained by [Movable Ink](https://github.com/movableink).
