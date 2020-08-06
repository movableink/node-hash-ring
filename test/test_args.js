const assert = require('assert');

const HashRing = require("../index");

describe("hash ring arguments", function() {
  it("instantiates with weights", function() {
    const h = new HashRing({ a: 5, b: 3 });

    assert.ok(h instanceof HashRing);
    assert.deepEqual(h.getBuckets(), ['a', 'b']);
  });

  it("instantiates with weights and precision", function() {
    const h = new HashRing({ a: 5, b: 3 }, 500);
    assert.ok(h instanceof HashRing);
  });

  it("throws if no weights are passed", function() {
    assert.throws(() => {
      new HashRing();
    }, { name: 'TypeError' });
  });

  it("throws if bad weights are passed", function() {
    assert.throws(() => {
      new HashRing({ "foo": "bar" });
    }, { name: 'TypeError' });
  });
});
