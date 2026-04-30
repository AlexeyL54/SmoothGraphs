// Graph.cpp
#include "Graph.hpp"
#include "Graphviz.hpp"

#include <QDebug>
#include <QMessageBox>
#include <algorithm>

/**
 * @brief Конструктор класса Graph.
 */
Graph::Graph() : isModified_(false), nextNodeId_(1) {}

/**
 * @brief Деструктор класса Graph.
 */
Graph::~Graph() { clear(); }

/**
 * @brief Добавляет узел в граф.
 * @param node Указатель на добавляемый узел.
 * @return ID присвоенный узлу.
 */
ID Graph::addNode(SmoothNode *node) {
  if (!node)
    return 0;

  // Проверяем, не добавлен ли уже узел
  if (getNodeId(node) != 0) {
    qDebug() << "Node already in graph";
    return getNodeId(node);
  }

  ID id = nextNodeId_++;
  nodes_[id] = node;
  node->setId(id);
  isModified_ = true;
  qDebug() << "Node added to graph with ID:" << id;
  return id;
}

/**
 * @brief Добавляет ребро в граф.
 * @param edge Указатель на добавляемое ребро.
 */
void Graph::addEdge(SmoothEdge *edge) {
  if (!edge)
    return;

  SmoothNode *start = edge->getStartNode();
  SmoothNode *end = edge->getEndNode();

  if (!start || !end)
    return;

  ID from = getNodeId(start);
  ID to = getNodeId(end);

  if (from == 0 || to == 0) {
    qDebug() << "Cannot add edge: nodes not in graph";
    return;
  }

  // Проверяем, не существует ли уже такое ребро
  for (ID existingTo : adjList_[from]) {
    if (existingTo == to) {
      qDebug() << "Edge already exists";
      return;
    }
  }

  adjList_[from].push_back(to);
  edgeWeights_[from][to] = edge->getWeight();
  isModified_ = true;
  qDebug() << "Edge added to graph:" << from << "->" << to;
}

/**
 * @brief Удаляет узел из графа по ID.
 * @param id ID удаляемого узла.
 */
void Graph::deleteNode(ID id) {
  auto it = nodes_.find(id);
  if (it == nodes_.end()) {
    qDebug() << "Node not found:" << id;
    return;
  }

  SmoothNode *node = it->second;

  // Удаляем все связанные рёбра из сцены
  for (SmoothEdge *edge : node->getOutcomingEdges()) {
    if (edge && edge->scene()) {
      edge->scene()->removeItem(edge);
    }
  }
  for (SmoothEdge *edge : node->getIncomingEdges()) {
    if (edge && edge->scene()) {
      edge->scene()->removeItem(edge);
    }
  }

  // Удаляем узел из сцены
  if (node && node->scene()) {
    node->scene()->removeItem(node);
  }

  nodes_.erase(id);
  adjList_.erase(id);
  edgeWeights_.erase(id);

  for (auto &[nodeId, neighbours] : adjList_) {
    neighbours.remove(id);
  }

  for (auto &[from, toMap] : edgeWeights_) {
    toMap.erase(id);
  }

  isModified_ = true;
  qDebug() << "Node deleted from graph:" << id;
}

/**
 * @brief Удаляет ребро из графа.
 * @param from ID начального узла.
 * @param to ID конечного узла.
 */
void Graph::deleteEdge(ID from, ID to) {
  auto iter = std::find(adjList_[from].begin(), adjList_[from].end(), to);
  if (iter != adjList_[from].end()) {
    adjList_[from].erase(iter);
    edgeWeights_[from].erase(to);
    isModified_ = true;
    qDebug() << "Edge deleted from graph:" << from << "->" << to;
  } else {
    qDebug() << "Edge not found:" << from << "->" << to;
  }
}

/**
 * @brief Очищает граф (удаляет все узлы и рёбра).
 */
void Graph::clear() {
  nodes_.clear();
  adjList_.clear();
  edgeWeights_.clear();
  currentFilePath_.clear();
  isModified_ = false;
  nextNodeId_ = 1;
  qDebug() << "Graph cleared";
}

/**
 * @brief Возвращает список всех узлов графа.
 * @return Вектор указателей на узлы.
 */
std::vector<SmoothNode *> Graph::getNodes() const {
  std::vector<SmoothNode *> result;
  result.reserve(nodes_.size());

  for (const auto &pair : nodes_) {
    result.push_back(pair.second);
  }

  return result;
}

/**
 * @brief Возвращает список всех рёбер графа.
 * @return Вектор указателей на рёбра.
 */
std::vector<SmoothEdge *> Graph::getEdges() const {
  std::vector<SmoothEdge *> result;
  QList<SmoothEdge *> uniqueEdges; // Для избежания дубликатов

  for (const auto &pair : nodes_) {
    SmoothNode *node = pair.second;
    for (SmoothEdge *edge : node->getOutcomingEdges()) {
      if (!uniqueEdges.contains(edge)) {
        uniqueEdges.append(edge);
        result.push_back(edge);
      }
    }
  }

  return result;
}

/**
 * @brief Возвращает ID узла по указателю.
 * @param node Указатель на узел.
 * @return ID узла или 0 если не найден.
 */
ID Graph::getNodeId(SmoothNode *node) const {
  for (const auto &pair : nodes_) {
    if (pair.second == node) {
      return pair.first;
    }
  }
  return 0;
}

/**
 * @brief Создаёт узлы на сцене из загруженных данных.
 * @param nodesData Данные об узлах (ID, x, y).
 * @param scene Сцена для добавления узлов.
 * @return Карта соответствия ID -> SmoothNode*.
 */
std::unordered_map<ID, SmoothNode *> Graph::createNodesFromData(
    const std::vector<std::tuple<ID, double, double>> &nodesData,
    QGraphicsScene *scene) {
  std::unordered_map<ID, SmoothNode *> idToNode;

  for (const auto &nodeData : nodesData) {
    ID id = std::get<0>(nodeData);
    double x = std::get<1>(nodeData);
    double y = std::get<2>(nodeData);

    SmoothNode *node = new SmoothNode(x, y, 100, 100);
    node->setPos(x, y);
    node->setId(id);
    node->setFlag(QGraphicsItem::ItemIsMovable);
    node->setAcceptHoverEvents(true);

    if (scene) {
      scene->addItem(node);
    }

    nodes_[id] = node;
    idToNode[id] = node;

    if (id >= nextNodeId_) {
      nextNodeId_ = id + 1;
    }

    qDebug() << "Created node with ID:" << id << "at position (" << x << ","
             << y << ")";
  }

  return idToNode;
}

/**
 * @brief Создаёт рёбра на сцене из загруженных данных.
 * @param edgesData Данные о рёбрах (from, to, weight).
 * @param idToNode Карта соответствия ID -> SmoothNode*.
 * @param scene Сцена для добавления рёбер.
 */
void Graph::createEdgesFromData(
    const std::vector<std::tuple<ID, ID, float>> &edgesData,
    const std::unordered_map<ID, SmoothNode *> &idToNode,
    QGraphicsScene *scene) {
  for (const auto &edgeData : edgesData) {
    ID from = std::get<0>(edgeData);
    ID to = std::get<1>(edgeData);
    float weight = std::get<2>(edgeData);

    auto fromIt = idToNode.find(from);
    auto toIt = idToNode.find(to);

    if (fromIt != idToNode.end() && toIt != idToNode.end()) {
      SmoothEdge *edge = new SmoothEdge(fromIt->second, toIt->second);
      edge->setWeight(weight);

      if (scene) {
        scene->addItem(edge);
      }

      fromIt->second->addOutgoingEdge(edge);
      toIt->second->addIncomingEdge(edge);

      adjList_[from].push_back(to);
      edgeWeights_[from][to] = weight;

      qDebug() << "Created edge:" << from << "->" << to << "weight:" << weight;
    } else {
      qDebug() << "Failed to create edge:" << from << "->" << to
               << "- nodes not found";
    }
  }
}

/**
 * @brief Сохраняет граф в файл.
 * @param filepath Путь к файлу для сохранения.
 * @return true если сохранение успешно, false в противном случае.
 */
bool Graph::saveToFile(const std::string &filepath) {
  Graphviz gv;
  bool success = gv.saveToFile(filepath, getNodes(), getEdges());

  if (success) {
    currentFilePath_ = filepath;
    isModified_ = false;
    qDebug() << "Graph saved to:" << filepath.c_str();
  } else {
    qDebug() << "Failed to save graph to:" << filepath.c_str();
  }

  return success;
}

/**
 * @brief Загружает граф из файла.
 * @param filepath Путь к файлу для загрузки.
 * @param scene Сцена QGraphicsScene для отрисовки узлов и рёбер.
 * @return true если загрузка успешна, false в противном случае.
 */
bool Graph::loadFromFile(const std::string &filepath, QGraphicsScene *scene) {
  Graphviz gv;
  std::vector<std::tuple<ID, double, double>> nodesData;
  std::vector<std::tuple<ID, ID, float>> edgesData;

  if (!gv.loadFromFile(filepath, nodesData, edgesData)) {
    qDebug() << "Failed to load graph from:" << filepath.c_str();
    return false;
  }

  // Очищаем текущий граф
  clear();

  // Создаём узлы
  std::unordered_map<ID, SmoothNode *> idToNode =
      createNodesFromData(nodesData, scene);

  // Создаём рёбра
  createEdgesFromData(edgesData, idToNode, scene);

  currentFilePath_ = filepath;
  isModified_ = false;

  qDebug() << "Graph loaded from:" << filepath.c_str();
  return true;
}

/**
 * @brief Находит кратчайший путь между двумя узлами.
 * @param from Начальный узел.
 * @param to Конечный узел.
 * @return Вектор узлов, составляющих кратчайший путь.
 */
std::vector<SmoothNode *> Graph::findShortestPath(SmoothNode *from,
                                                  SmoothNode *to) {
  // TODO: Реализация алгоритма поиска кратчайшего пути
  std::vector<SmoothNode *> shortestPath;
  return shortestPath;
}

/*std::vector<SmoothNode *> Graph::findShortestPath(SmoothNode *from,
                                                  SmoothNode *to) {
  std::vector<SmoothNode *> shortestPath;
  std::vector<std::unordered_map<SmoothNode *, float>> vRev;
  QList<SmoothEdge *> tempIncomingEdges;

  SmoothNode *temp = to;
  vRev.push_back({{to, 0}});
  size_t k = 0;

  while (vRev[k].find(from) == vRev[k].end()) {
    vRev.push_back({});

    for (std::pair<SmoothNode *const, float> node : vRev[k]) {
      tempIncomingEdges = node.first->getIncomingEdges();

      for (SmoothEdge *edge : tempIncomingEdges) {
        vRev[k + 1][edge->getStartNode()] =
            edge->getWeight() + vRev[k][edge->getEndNode()];
      }
    }
    k++;
  }

  // TODO: прямой проход, вес грани

  return shortestPath;
}*/
