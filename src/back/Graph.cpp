#include "Graph.hpp"

Graph::Graph() {};

void Graph::addNode(const ID id, SmoothNode *node) { nodes_[id] = node; }

void Graph::addEdge(const ID from, const ID to) {
  adjList_[from].push_back(to);
}

void Graph::deleteNode(const ID id) {
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

void Graph::deleteEdge(ID from, ID to) {
  for (auto iter = adjList_[from].begin(); iter != adjList_[from].end();
       iter++) {
    if (*iter == to) {
      adjList_[from].erase(iter);
      break;
    }
  }
}

void Graph::clear() {
  nodes_.clear();
  adjList_.clear();
}
