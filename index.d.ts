export interface Weights {
  [key: string]: number;
}

export class HashRing {
  constructor(weights: Weights, precision?: number)
  getNode(value: string): string
  getBuckets(): Array<string>
}
