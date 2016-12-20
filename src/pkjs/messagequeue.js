//Copyright (C) 2014 Meiguro

//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

//The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

var fifo = require('./fifo');

/**
 * MessageQueue is an app message queue that guarantees delivery and order.
 */
var MessageQueue = function() {
  this._queue = fifo.create();
  this._sending = false;

  this._consume = this.consume.bind(this);
  this._cycle = this.cycle.bind(this);
};

MessageQueue.prototype.stop = function() {
  // TODO add a way to filter old message and restart
  this._sending = false;
};

MessageQueue.prototype.consume = function() {
  this._queue.shift();
  if (this._queue.empty()) {
    return this.stop();
  }
  this.cycle();
};

MessageQueue.prototype.checkSent = function(message, fn) {
  return function() {
    if (message === this._sent) {
      fn();
    }
  }.bind(this);
};

MessageQueue.prototype.cycle = function() {
  if (!this._sending) {
    return;
  }
  var head = this._queue.peek();
  if (!head) {
    return this.stop();
  }
  this._sent = head;
  var success = this.checkSent(head, this._consume);
  var failure = this.checkSent(head, this._cycle);
  Pebble.sendAppMessage(head, success, failure);
};

MessageQueue.prototype.send = function(message) {
  this._queue.push(message);
  if (this._sending) {
    return;
  }
  this._sending = true;
  this.cycle();
};

function create ( ) {
	return new MessageQueue();
}

module.exports.create = create ;