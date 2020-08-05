const assert = require('assert');
const chiSquaredTest = require('chi-squared-test');

const HashRing = require("../index");

const iterations = 10000;
const runs = 10;
const acceptablePVal = 0.10;

describe("hash ring distribution", function() {
  it('randomly distributes even weights', function () {
    const weights = {'a': 1, 'b': 1, 'c': 1};
    const fn = () => genCode(10);

    const pval = trialPVals(runs, weights, iterations, fn, 1000);
    assert.ok(pval > acceptablePVal, `expect pval ${pval} to be > ${acceptablePVal}`);
  });

  it('semi-randomly distributes even weights when no precision passed', function() {
    const weights = {'a': 1, 'b': 1, 'c': 1};
    const fn = () => genCode(10);

    const pval = trialPVals(runs, weights, iterations, fn);
    assert.ok(pval > 0, `expect pval ${pval} to be > 0`);
  });

  it('randomly distributes different weights', function () {
    const weights = {'a': 1, 'b': 1, 'c': 4};
    const fn = () => genCode(10);

    const pval = trialPVals(runs, weights, iterations, fn, 1000);
    assert.ok(pval > acceptablePVal, `expect pval ${pval} to be > ${acceptablePVal}`);
  });

  it('uses full value when hashing', function () {
    const weights = {'a': 1, 'b': 1, 'c': 4};
    const fn = () => '00000000000000' + genCode(10) + '0000000000000';

    const pval = trialPVals(runs, weights, iterations, fn, 1000);
    assert.ok(pval > acceptablePVal, `expect pval ${pval} to be > ${acceptablePVal}`);
  });

  it('consistently distributes identical values', function() {
    const weights = {'a': 1, 'b': 1, 'c': 1};

    const trial = trialPVal(weights, 3000, () => 'hello', 1000);
    assert.equal(trial.counts['b'], 3000, 'all values bucketed the same');
  });

  it('keeps existing bucket choices when removing buckets', function() {
    const weights = {'a': 1, 'b': 1, 'c': 1};
    const ring = new HashRing(weights, 1000);

    const bValues = [];
    for (let i = 0; i < iterations; i++) {
      const word = genCode(10);
      const value = ring.getNode(word);
      if (value === 'b') {
        bValues.push(word);
      }
    }

    const fewerWeights = {'a': 1, 'b': 1};
    const fewerRing = new HashRing(weights, 1000);
    const stillBValues = bValues.filter(word => {
      return fewerRing.getNode(word) === 'b';
    });

    assert.equal(bValues.length, stillBValues.length, "all 'b' bucket values are still in 'b'");

    const moreWeights = {'a': 1, 'b': 1, 'c': 1, 'd': 1};
    const moreRing = new HashRing(moreWeights, 1000);
    const stillBorDValues = bValues.filter(word => {
      const value = moreRing.getNode(word);
      return value === 'b' || value === 'd';
    });

    assert.equal(bValues.length, stillBorDValues.length, "all 'b' bucket values are either still in 'b' or are now in 'd'");
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

function trialPVals(runs, weights, iterations, valueFn, precision) {
  let pvals = [];
  for (let i = 0; i < runs; i++) {
    pvals.push(trialPVal(weights, iterations, valueFn, precision));
  }

  return average(pvals.map(p => p.probability));
}

function trialPVal(weights, iterations, valueFn, precision) {
  const totalWeight = Object.values(weights).reduce((sum, w) => sum += w, 0);
  const percents = Object.values(weights).map(w => w / totalWeight);
  const expected = percents.map(p => iterations * p);

  const ring = new HashRing(weights, precision);

  let counts = {};
  for (let i = 0; i < iterations; i++) {
    const value = ring.getNode(valueFn());
    if (!counts[value]) {
      counts[value] = 0;
    }
    counts[value]++;
  }

  const countList = Object.keys(weights).map(k => counts[k]);

  return {
    probability: chiSquaredTest(countList, expected, 1).probability,
    counts
  };
}

function average(arr) {
  return arr.reduce((sum, a) => sum += a, 0) / arr.length;
}

function genCode (length) {
  length = length || 10;
  var chars = "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm1234567890",
      numChars = chars.length;
  var ret = "";
  for (var i = 0; i < length; i++) {
    ret += chars[parseInt(Math.random() * numChars, 10)];
  }
  return ret;
};
