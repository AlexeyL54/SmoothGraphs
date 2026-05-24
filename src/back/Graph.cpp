#include "Graph.hpp"
#include "Graphviz.hpp"

#include <QDebug>
#include <QMessageBox>
#include <algorithm>
#include <limits>

/**
 * @brief Конструктор класса Graph.
 */
Graph::Graph(Logger *logger)
    : isModified_(false), nextNodeId_(1), logger_(logger), revision_(0) {}
/**
 * @brief Деструктор класса Graph.
 */
Graph::~Graph() { clear(); }

void Graph::notifyChange() {
  revision_++;
  isModified_ = true;
  emit graphStructureChanged();
}

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
  notifyChange();
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
  isModified_ = true;
  notifyChange();
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
  // edgeWeights_.erase(id);

  for (auto &[nodeId, neighbours] : adjList_) {
    neighbours.remove(id);
  }

  isModified_ = true;
  notifyChange();
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
    isModified_ = true;
    notifyChange();
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
  currentFilePath_.clear();
  isModified_ = false;
  nextNodeId_ = 1;
  notifyChange();
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
    double centerX = std::get<1>(nodeData);
    double centerY = std::get<2>(nodeData);

    // Теперь конструктор принимает центр и радиус
    SmoothNode *node = new SmoothNode(centerX, centerY, 50);
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

    qDebug() << "Created node with ID:" << id << "at center (" << centerX << ","
             << centerY << ")";
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

      edge->updatePosition();

      fromIt->second->addOutgoingEdge(edge);
      toIt->second->addIncomingEdge(edge);

      adjList_[from].push_back(to);
      // edgeWeights_[from][to] = weight;

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

bool Graph::parseFile(const std::string &filepath, std::vector<NodeData> &nodes,
                      std::vector<EdgeData> &edges) {
  Graphviz gv;

  std::vector<std::tuple<ID, double, double>> nodesData;
  std::vector<std::tuple<ID, ID, float>> edgesData;

  if (!gv.loadFromFile(filepath, nodesData, edgesData)) {
    return false;
  }

  for (const auto &n : nodesData) {
    nodes.push_back({std::get<0>(n), std::get<1>(n), std::get<2>(n)});
  }
  for (const auto &e : edgesData) {
    edges.push_back({std::get<0>(e), std::get<1>(e), std::get<2>(e)});
  }

  return true;
}

/**
 * @brief Загружает граф из файла.
 * @param filepath Путь к файлу для загрузки.
 * @param scene Сцена QGraphicsScene для отрисовки узлов и рёбер.
 * @return true если загрузка успешна, false в противном случае.
 */
/*bool Graph::loadFromFile(const std::string &filepath, QGraphicsScene *scene) {
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

  notifyChange();
  qDebug() << "Graph loaded from:" << filepath.c_str();
  return true;
}*/

// Вспомогательная функция для топологической сортировки (DFS)
static void topologicalSortUtil(SmoothNode *node,
                                std::unordered_map<SmoothNode *, bool> &visited,
                                std::vector<SmoothNode *> &stack,
                                Logger *logger) {
  if (logger) {
    logger->addMessage(
        INFO,
        QString("Посещаем узел ID: %1 для топологической сортировки (DFS)")
            .arg(node->getId()));
  }

  visited[node] = true;

  // Рекурсивно посещаем всех соседей (исходящие ребра)
  for (SmoothEdge *edge : node->getOutcomingEdges()) {
    SmoothNode *neighbor = edge->getEndNode();
    if (!visited[neighbor]) {
      if (logger) {
        logger->addMessage(
            INFO, QString("  Переход по ребру: ID %1 -> ID %2 (вес: %3)")
                      .arg(node->getId())
                      .arg(neighbor->getId())
                      .arg(edge->getWeight()));
      }
      topologicalSortUtil(neighbor, visited, stack, logger);
    }
  }

  // Добавляем текущий узел в стек после обработки всех соседей
  stack.push_back(node);
  if (logger) {
    logger->addMessage(
        INFO,
        QString("Добавляем узел ID: %1 в стек (после обработки всех потомков)")
            .arg(node->getId()));
  }
}

// Получение топологически отсортированного списка узлов
static std::vector<SmoothNode *>
getTopologicalOrder(const std::vector<SmoothNode *> &allNodes, Logger *logger) {
  std::unordered_map<SmoothNode *, bool> visited;
  std::vector<SmoothNode *> stack;

  if (logger) {
    logger->addMessage(
        INFO, "Начинаем топологическую сортировку графа (алгоритм DFS)");
    logger->addMessage(INFO,
                       QString("Всего узлов в графе: %1").arg(allNodes.size()));
  }

  for (SmoothNode *node : allNodes) {
    visited[node] = false;
  }

  for (SmoothNode *node : allNodes) {
    if (!visited[node]) {
      if (logger) {
        logger->addMessage(
            INFO, QString("Запускаем DFS с узла ID: %1").arg(node->getId()));
      }
      topologicalSortUtil(node, visited, stack, logger);
    }
  }

  if (logger) {
    logger->addMessage(SUCCESS, "Топологическая сортировка завершена");
    QString orderStr = "Порядок узлов (от стоков к источникам): ";
    for (SmoothNode *node : stack) {
      orderStr += QString::number(node->getId()) + " ";
    }
    logger->addMessage(INFO, orderStr);
  }

  return stack;
}

std::vector<SmoothNode *> Graph::findShortestPath(SmoothNode *from,
                                                  SmoothNode *to) {
  // Очищаем логгер перед новым поиском
  if (logger_) {
    logger_->clear();
    logger_->addMessage(INFO, "=== НАЧАЛО ПОИСКА КРАТЧАЙШЕГО ПУТИ ===");
  }

  if (!from || !to) {
    if (logger_) {
      logger_->addMessage(ERROR,
                          "Ошибка: начальный или конечный узел не определён");
      logger_->addMessage(INFO, "=== КОНЕЦ ПОИСКА (ОШИБКА) ===");
    }
    return {};
  }

  if (logger_) {
    logger_->addMessage(INFO,
                        QString("Стартовый узел: ID %1").arg(from->getId()));
    logger_->addMessage(INFO, QString("Целевой узел: ID %1").arg(to->getId()));
  }

  if (from == to) {
    if (logger_) {
      logger_->addMessage(
          SUCCESS, "Стартовый и конечный узлы совпадают. Путь найден: [0]");
      logger_->addMessage(INFO, "=== КОНЕЦ ПОИСКА (УСПЕХ) ===");
    }
    return {from};
  }

  // 1. Получаем все узлы графа
  std::vector<SmoothNode *> allNodes = getNodes();
  if (logger_) {
    logger_->addMessage(
        INFO, QString("Получено узлов для обработки: %1").arg(allNodes.size()));
  }

  // 2. Топологическая сортировка
  if (logger_) {
    logger_->addMessage(INFO, "Проверка графа на ацикличность...");
  }
  std::vector<SmoothNode *> topoOrder = getTopologicalOrder(allNodes, logger_);

  // 3. Инициализация структур данных для обратного прохода
  std::unordered_map<SmoothNode *, float> dist;
  std::unordered_map<SmoothNode *, SmoothNode *> nextNode;

  const float INF = std::numeric_limits<float>::max();

  for (SmoothNode *node : allNodes) {
    dist[node] = INF;
    nextNode[node] = nullptr;
  }

  dist[to] = 0.0f;
  if (logger_) {
    logger_->addMessage(
        INFO, QString("Инициализация: dist[ID %1] = 0").arg(to->getId()));
  }

  // 4. Обратный проход ДП
  if (logger_) {
    logger_->addMessage(
        INFO, "Начинаем обратный проход динамического программирования...");
  }
  int processedCount = 0;

  for (SmoothNode *u : topoOrder) {
    if (dist[u] == INF) {
      continue;
    }

    processedCount++;
    if (logger_) {
      logger_->addMessage(
          INFO, QString("Обрабатываем узел ID: %1 (расстояние до цели: %2)")
                    .arg(u->getId())
                    .arg(dist[u] == INF ? "∞" : QString::number(dist[u])));
    }

    // Рассматриваем все входящие ребра в u: (v -> u)
    for (SmoothEdge *inEdge : u->getIncomingEdges()) {
      SmoothNode *v = inEdge->getStartNode();
      float weight = inEdge->getWeight();

      if (dist[v] > weight + dist[u]) {
        float oldDist = dist[v];
        dist[v] = weight + dist[u];
        nextNode[v] = u;

        if (logger_) {
          logger_->addMessage(
              INFO, QString("  Обновление пути к узлу ID %1: %2 -> %3 (через "
                            "ID %4, вес ребра: %5)")
                        .arg(v->getId())
                        .arg(oldDist == INF ? "∞" : QString::number(oldDist))
                        .arg(QString::number(dist[v]))
                        .arg(u->getId())
                        .arg(weight));
        }
      }
    }
  }

  if (logger_) {
    logger_->addMessage(INFO, QString("Обработано узлов: %1 из %2")
                                  .arg(processedCount)
                                  .arg(topoOrder.size()));
  }

  // 5. Проверка достижимости
  if (dist[from] == INF) {
    if (logger_) {
      logger_->addMessage(ERROR,
                          QString("Путь от узла ID %1 до узла ID %2 не найден!")
                              .arg(from->getId())
                              .arg(to->getId()));
      logger_->addMessage(INFO, "=== КОНЕЦ ПОИСКА (ПУТЬ НЕ НАЙДЕН) ===");
    }
    return {};
  }

  if (logger_) {
    logger_->addMessage(SUCCESS,
                        QString("Кратчайшее расстояние от ID %1 до ID %2: %3")
                            .arg(from->getId())
                            .arg(to->getId())
                            .arg(dist[from]));
  }

  // 6. Прямой проход: восстановление пути от from к to
  if (logger_) {
    logger_->addMessage(INFO, "Восстанавливаем путь...");
  }
  std::vector<SmoothNode *> path;
  SmoothNode *current = from;
  float totalWeight = 0.0f;

  while (current != nullptr) {
    path.push_back(current);
    if (logger_) {
      logger_->addMessage(
          INFO,
          QString("  Добавляем в путь узел ID: %1").arg(current->getId()));
    }

    if (current == to) {
      break;
    }

    SmoothNode *next = nextNode[current];
    if (next) {
      // Находим вес ребра для отображения
      for (SmoothEdge *edge : current->getOutcomingEdges()) {
        if (edge->getEndNode() == next) {
          totalWeight += edge->getWeight();
          if (logger_) {
            logger_->addMessage(INFO,
                                QString("    Ребро ID %1 -> ID %2 (вес: %3)")
                                    .arg(current->getId())
                                    .arg(next->getId())
                                    .arg(edge->getWeight()));
          }
          break;
        }
      }
    }
    current = next;
  }

  // Если путь не завершился в to, возвращаем пустой вектор
  if (path.back() != to) {
    if (logger_) {
      logger_->addMessage(ERROR, "Ошибка при восстановлении пути!");
      logger_->addMessage(INFO, "=== КОНЕЦ ПОИСКА (ОШИБКА) ===");
    }
    return {};
  }

  if (logger_) {
    logger_->addMessage(
        SUCCESS,
        QString("Путь найден! Количество узлов в пути: %1, общий вес: %2")
            .arg(path.size())
            .arg(totalWeight));

    QString pathStr = "Путь: ";
    for (size_t i = 0; i < path.size(); ++i) {
      if (i > 0)
        pathStr += " -> ";
      pathStr += QString::number(path[i]->getId());
    }
    logger_->addMessage(INFO, pathStr);
    logger_->addMessage(INFO, "=== КОНЕЦ ПОИСКА (УСПЕХ) ===");
  }

  return path;
}
