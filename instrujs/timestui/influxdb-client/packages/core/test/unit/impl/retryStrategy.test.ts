import {expect} from 'chai'
import {
  RetryStrategyImpl,
  createRetryDelayStrategy,
} from '../../../src/impl/retryStrategy'
import {HttpError, DEFAULT_RetryDelayStrategyOptions} from '../../../src'

describe('RetryStrategyImpl', () => {
  it('has constructor that uses defaults on no options', () => {
    expect(() => new RetryStrategyImpl()).to.not.throw()
    expect(() => new RetryStrategyImpl({})).to.not.throw()
    expect(() => createRetryDelayStrategy()).to.not.throw()
    expect(new RetryStrategyImpl())
      .property('options')
      .is.deep.equal(DEFAULT_RetryDelayStrategyOptions)
    expect(new RetryStrategyImpl({}))
      .property('options')
      .is.deep.equal(DEFAULT_RetryDelayStrategyOptions)
  })
  it('generates exponential data from min to max for unknown delays', () => {
    const subject = new RetryStrategyImpl({
      minRetryDelay: 100,
      maxRetryDelay: 1000,
      retryJitter: 0,
      exponentialBase: 2,
    })
    const values = [1, 2, 3, 4, 5, 6].reduce((acc, _val) => {
      acc.push(subject.nextDelay())
      return acc
    }, [] as number[])
    expect(values).to.be.deep.equal([100, 200, 400, 800, 1000, 1000])
    subject.success()
    expect(subject.nextDelay()).equals(100)
  })
  it('generates exponential data from min to max for unknown delays', () => {
    const subject = new RetryStrategyImpl({
      minRetryDelay: 100,
      maxRetryDelay: 2000,
      retryJitter: 20,
      // exponentialBase: 5, 5 by default
    })
    const values = [1, 2, 3, 4, 5, 6].reduce((acc, _val, index) => {
      acc.push(subject.nextDelay(undefined, index + 1))
      return acc
    }, [] as number[])
    // truncate values to ignore jittering
    expect(values.map(x => Math.trunc(x / 100) * 100)).to.be.deep.equal([
      100,
      500,
      2000,
      2000,
      2000,
      2000,
    ])
    subject.success()
    expect(Math.trunc(subject.nextDelay() / 100) * 100).equals(100)
  })
  it('generates the delays according to errors', () => {
    const subject = new RetryStrategyImpl({
      minRetryDelay: 100,
      maxRetryDelay: 1000,
      retryJitter: 0,
    })
    const values = [1, 2, 3, 4, 5, 6].reduce((acc, val) => {
      acc.push(subject.nextDelay(new HttpError(503, '', '', String(val))))
      return acc
    }, [] as number[])
    expect(values).to.be.deep.equal([1, 2, 3, 4, 5, 6])
  })
  it('generates jittered delays according to error delay', () => {
    const subject = new RetryStrategyImpl({
      minRetryDelay: 100,
      maxRetryDelay: 1000,
      retryJitter: 10,
    })
    const values = [1, 2, 3, 4, 5, 6].reduce((acc, val) => {
      acc.push(subject.nextDelay(new HttpError(503, '', '', String(val))))
      return acc
    }, [] as number[])
    expect(values).to.have.length(6)
    values.forEach((x, i) => {
      expect(x).to.not.be.lessThan(i + 1)
      expect(x).to.not.be.greaterThan(1000)
    })
  })
  it('generates exponential delays with failedAttempts', () => {
    const subject = new RetryStrategyImpl({
      minRetryDelay: 100,
      maxRetryDelay: 1000,
      retryJitter: 10,
    })
    const values = [1, 2, 3, 4, 5, 6].reduce((acc, val) => {
      acc.push(subject.nextDelay(new Error(), val))
      return acc
    }, [] as number[])
    expect(values).to.have.length(6)
    values.forEach((x, i) => {
      if (i > 0) {
        expect(Math.max(Math.trunc(x / 100), 10)).to.not.be.lessThan(
          Math.max(Math.trunc(values[i - 1] / 100), 10)
        )
      }
      expect(x).to.not.be.greaterThan(1000 + 10)
    })
  })
  it('generates default jittered delays', () => {
    const subject = new RetryStrategyImpl({
      minRetryDelay: 100,
      maxRetryDelay: 1000,
      retryJitter: 10,
    })
    const values = [1, 2, 3, 4, 5, 6].reduce((acc, _val) => {
      acc.push(subject.nextDelay())
      return acc
    }, [] as number[])
    expect(values).to.have.length(6)
    values.forEach((x, i) => {
      expect(x).to.not.be.lessThan(i)
      expect(x).to.not.be.greaterThan(1000)
      if (i > 0) {
        expect(x).to.not.be.lessThan(values[i - 1])
      }
    })
  })
})
