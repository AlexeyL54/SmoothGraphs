#pragma once

#include <list>
#include <unordered_map>

template <typename ID, typename Value> class Node {
private:
  Value value_;
  ID id_;

public:
  Node();
  Node(ID id, Value v);
  ~Node();
  Value getValue();
  ID getId();
};

template <typename ID, typename Value> class Graph {
private:
  std::unordered_map<ID, Node<ID, Value>> nodes_;
  std::unordered_map<ID, std::list<ID>> adjList_;

public:
  Graph();
  // TODO: Graph(const std::string &configPath);
  // TODO: std::unordered_map<Node<ID, Value>, std::list<Node<ID, Value> &>>
  // getAdjList;

  void addNode(const ID id, const Value value);
  void addEdge(const ID from, const ID to);
  void deleteNode(const ID id);
  void deleteEdge(const ID from, const ID to);
  void clear();
};

//////////////////////////////////////////////////////////////////////////////

template <typename ID, typename Value> Node<ID, Value>::Node(){};

template <typename ID, typename Value>
Node<ID, Value>::Node(ID id, Value v) : value_(v), id_(id){};

template <typename ID, typename Value> Node<ID, Value>::~Node(){};

template <typename ID, typename Value> Value Node<ID, Value>::getValue() {
  return value_;
}

template <typename ID, typename Value> ID Node<ID, Value>::getId() {
  return id_;
}

//////////////////////////////////////////////////////////////////////////////

template <typename ID, typename Value> Graph<ID, Value>::Graph() {}

template <typename ID, typename Value>
void Graph<ID, Value>::addNode(ID id, Value value) {
  nodes_[id] = value;
}

template <typename ID, typename Value>
void Graph<ID, Value>::addEdge(const ID from, const ID to) {
  adjList_[from].push_back(to);
}

template <typename ID, typename Value>
void Graph<ID, Value>::deleteNode(const ID id) {
  nodes_.erase(id);
  adjList_.erase(id);

  for (auto edgeIter = adjList_.begin(); edgeIter != adjList_.end();
       edgeIter++) {

    for (auto iter = adjList_[*edgeIter].begin();
         iter != adjList_[*edgeIter].end(); iter++) {
      if (*iter == id)
        adjList_[*edgeIter].erase(iter);
    }
  }
}

template <typename ID, typename Value>
void Graph<ID, Value>::deleteEdge(ID from, ID to) {
  for (auto iter = adjList_[from].begin(); iter != adjList_[from].end();
       iter++) {
    if (*iter == to) {
      adjList_[from].erase(iter);
      break;
    }
  }
}

template <typename ID, typename Value> void Graph<ID, Value>::clear() {
  nodes_.clear();
  adjList_.clear();
}
