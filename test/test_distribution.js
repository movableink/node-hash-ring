var assert = require('assert');

var HashRing = require("../index");

var nodes = {
    "127.0.0.1:8080": 1,
    "127.0.0.2:8080": 1,
    "127.0.0.3:8080": 1
};

var ring = new HashRing(nodes);

var iterations = 100000;

var genCode = function (length) {
    length = length || 10;
    var chars = "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm1234567890",
        numChars = chars.length;
    var ret = "";
    for (var i = 0; i < length; i++) {
        ret += chars[parseInt(Math.random() * numChars, 10)];
    }
    return ret;
};

describe("hash ring", function() {
  it('should be randomly distributed', function () {
    var counts = {},
        node, i, len, word;
    for (i = 0, len = nodes.length; i < len; i++) {
        node = nodes[i];
        counts[node] = 0;
    }
    for (i = 0, len = iterations; i < len; i++) {
      word = genCode(10);
      node = ring.getNode(word);
      counts[node] = counts[node] || 0;
      counts[node]++;
    }
    var total = Object.keys(counts).reduce( function (sum, node) {
      return sum += counts[node];
    }, 0.0);
    var delta = 0.05
      , lower = 1.0 / 3 - 0.05
      , upper = 1.0 / 3 + 0.05;
    for (node in counts) {
      var result = (counts[node] / total);
      assert(result > lower);
      assert(result < upper);
    }
  });


  it('is deterministic', function() {
    const nodes = {
      "127.0.0.1:8080": 1,
      "127.0.0.2:8080": 1,
      "127.0.0.3:8080": 1
    };

    const ring = new HashRing(nodes, 40);

    assert.equal(ring.getNode("a"), "127.0.0.2:8080");
    assert.equal(ring.getNode("b"), "127.0.0.2:8080");
    assert.equal(ring.getNode("c"), "127.0.0.3:8080");
    assert.equal(ring.getNode("d"), "127.0.0.1:8080");
    assert.equal(ring.getNode("e"), "127.0.0.2:8080");
    assert.equal(ring.getNode("f"), "127.0.0.2:8080");
    assert.equal(ring.getNode("g"), "127.0.0.2:8080");
    assert.equal(ring.getNode("a!b"), "127.0.0.2:8080");
    assert.equal(ring.getNode("c!d"), "127.0.0.1:8080");
    assert.equal(ring.getNode("e!123"), "127.0.0.2:8080");
    assert.equal(ring.getNode("f!456"), "127.0.0.3:8080");
  });
});
