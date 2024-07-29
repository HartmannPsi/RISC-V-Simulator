#include "RoB.hpp"
#include "headers.hpp"

bool ReorderBuffer::push(int32_t _src, bool _busy) {
  if (buf_queue.full()) {
    return false;
  }

  RoBUnit tmp;
  tmp.busy = _busy;
  tmp.src = _src;
  buf_queue.push(tmp);

  return true;
}

bool ReorderBuffer::empty() const { return buf_queue.empty(); }

bool ReorderBuffer::full() const { return buf_queue.full(); }

void ReorderBuffer::clear() {
  while (!buf_queue.empty()) {
    buf_queue.top() = RoBUnit();
    buf_queue.pop();
  }
}

bool ReorderBuffer::pop() {
  if (buf_queue.empty()) {
    return false;
  }
  if (buf_queue.top().busy == true) {
    return false;
  }

  cdb.broadcast(buf_queue.top().src, buf_queue.top().val);

  buf_queue.top().busy = true;
  buf_queue.top().src = 0;
  buf_queue.pop();

  return true;
}

void ReorderBuffer::submit(int32_t _src, int32_t _val) {

  for (int i = 0; i != 100; ++i) {
    if (buf_queue.arr[i].src == _src) {
      buf_queue.arr[i].busy = false;
      buf_queue.arr[i].val = _val;
    }
  }
}

void ReorderBuffer::print_first() {

  std::cout << "RoB FIRST: ";

  if (empty()) {
    std::cout << "NONE\n";
    return;
  }

  auto &tmp = buf_queue.top();

  std::cout << "SER: " << tmp.src << " BUSY: " << (tmp.busy ? "TRUE" : "FALSE")
            << " VAL: " << tmp.val << '\n';
}