#pragma once

#include "../front/Figures.hpp"

#include <cstddef>
#include <list>
#include <unordered_map>

typedef size_t ID;

class Graph {
private:
  std::unordered_map<size_t, SmoothNode *> nodes_;
  std::unordered_map<size_t, std::list<size_t>> adjList_;

public:
  Graph();
  // TODO: Graph(const std::string &configPath);
  // TODO: getAdjList;

  void addNode(const ID id, SmoothNode *node);
  void addEdge(const ID from, const ID to);
  void deleteNode(const ID id);
  void deleteEdge(const ID from, const ID to);
  void clear();
};
