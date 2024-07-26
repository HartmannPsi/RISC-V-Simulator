#pragma once
#ifndef QUEUE_HPP
#define QUEUE_HPP

template <typename T, const int maxsize = 10> class Queue {
public:
  T arr[maxsize];
  int front = -1, rear = -1;

  Queue() {}

  ~Queue() {}

  bool empty() const { return front == -1; }

  bool full() const {
    return (front == rear + 1) || (front == 0 && rear == maxsize - 1);
  }

  bool push(const T &val) {
    if (full()) {
      return false;
    }

    if (front == -1) {
      front = 0;
    }
    rear = (rear + 1) % maxsize;
    arr[rear] = val;
    return true;
  }

  bool pop() {
    if (empty()) {
      return false;
    }

    if (front == rear) {
      front = rear = -1;
    } else {
      front = (front + 1) % maxsize;
    }
  }

  const T &top() const {
    if (empty()) {
      return T();
    }
    return arr[front];
  }

  T &top() {
    if (empty()) {
      return T();
    }
    return arr[front];
  }
};

#endif