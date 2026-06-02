#include "Graph.hpp"
#include "Graphviz.hpp"

#include <QDebug>
#include <QMessageBox>
#include <algorithm>
#include <limits>

// ======================== КОНСТРУКТОР И ДЕСТРУКТОР ========================

/**
 * @brief Конструктор класса Graph.
 * @param logger Указатель на логгер для записи сообщений (опционально).
 */
Graph::Graph(Logger *logger)
    : isModified_(false), nextNodeId_(1), logger_(logger), revision_(0) {}

/**
 * @brief Деструктор класса Graph.
 *
 * Очищает все ресурсы графа.
 */
Graph::~Graph() { clear(); }

// ======================== ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ ========================

/**
 * @brief Уведомляет о изменении структуры графа.
 *
 * Увеличивает ревизию, устанавливает флаг модификации и генерирует сигнал.
 */
void Graph::notifyChange() {
  revision_++;
  isModified_ = true;
  emit graphStructureChanged();
}

// ======================== УПРАВЛЕНИЕ УЗЛАМИ ========================

/**
 * @brief Добавляет узел в граф.
 * @param node Указатель на добавляемый узел.
 * @return ID присвоенный узлу, или 0 если ошибка.
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
 * @brief Добавляет узел в граф с указанным ID.
 * @param node Указатель на добавляемый узел.
 * @param id ID для узла.
 * @return ID узла (тот же, что передан) или 0 если ошибка.
 */
ID Graph::addNodeWithId(SmoothNode *node, ID id) {
  if (!node) {
    qDebug() << "addNodeWithId: node is null";
    return 0;
  }

  if (nodes_.find(id) != nodes_.end()) {
    qDebug() << "addNodeWithId: Node with ID" << id << "already exists";
    return 0;
  }

  if (getNodeId(node) != 0) {
    qDebug() << "addNodeWithId: Node already in graph";
    return getNodeId(node);
  }

  node->setId(id);
  nodes_[id] = node;

  if (id >= nextNodeId_) {
    nextNodeId_ = id + 1;
  }

  isModified_ = true;
  qDebug() << "Node added to graph with ID:" << id;
  notifyChange();

  return id;
}

// ======================== УПРАВЛЕНИЕ РЁБРАМИ ========================

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

  // 1. Удаляем все связанные рёбра ИЗ ОБОИХ КОНЦОВ
  // Сначала копируем списки, так как они будут меняться при удалении
  QList<SmoothEdge *> outEdges = node->getOutcomingEdges();
  QList<SmoothEdge *> inEdges = node->getIncomingEdges();

  // Удаляем исходящие рёбра
  for (SmoothEdge *edge : outEdges) {
    SmoothNode *endNode = edge->getEndNode();
    if (endNode) {
      endNode->removeIncomingEdge(edge); // Удаляем из списка конечного узла
    }
    if (edge && edge->scene()) {
      edge->scene()->removeItem(edge);
    }
    delete edge; // Важно: удаляем объект ребра
  }

  // Удаляем входящие рёбра
  for (SmoothEdge *edge : inEdges) {
    SmoothNode *startNode = edge->getStartNode();
    if (startNode) {
      startNode->removeOutgoingEdge(edge); // Удаляем из списка начального узла
    }
    if (edge && edge->scene()) {
      edge->scene()->removeItem(edge);
    }
    delete edge;
  }

  // 2. Очищаем списки рёбер у удаляемого узла
  node->clearOutcomingEdges();
  node->clearIncomingEdges();

  // 3. Удаляем узел из сцены
  if (node && node->scene()) {
    node->scene()->removeItem(node);
  }
  delete node; // Важно: удаляем объект узла

  // 4. Удаляем из внутренних структур графа
  nodes_.erase(id);
  adjList_.erase(id);

  // 5. Удаляем ID из списков смежности других узлов
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

// ======================== ОЧИСТКА ГРАФА ========================

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

// ======================== ПОЛУЧЕНИЕ ДАННЫХ ГРАФА ========================

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
  QList<SmoothEdge *> uniqueEdges;

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

// ======================== РАБОТА С ФАЙЛАМИ ========================

/**
 * @brief Сохраняет граф в файл.
 * @param filepath Путь к файлу для сохранения.
 * @return true если сохранение успешно, false в противном случае.
 */
bool Graph::saveToFile(const QString &filepath) {
  Graphviz gv;
  bool success = gv.saveToFile(filepath, getNodes(), getEdges());

  if (success) {
    currentFilePath_ = filepath;
    isModified_ = false;
    qDebug() << "Graph saved to:" << filepath;
  } else {
    qDebug() << "Failed to save graph to:" << filepath;
  }

  return success;
}

/**
 * @brief Парсит файл и возвращает данные графа.
 *
 * Не создает визуальные элементы, только извлекает данные.
 * @param filepath Путь к файлу для парсинга.
 * @param nodes Вектор для заполнения данными узлов.
 * @param edges Вектор для заполнения данными рёбер.
 * @return true если парсинг успешен, false в противном случае.
 */
bool Graph::parseFile(const QString &filepath, std::vector<NodeData> &nodes,
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

// ======================== МЕТОДЫ ЗАГРУЗКИ ГРАФА ========================

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

      qDebug() << "Created edge:" << from << "->" << to << "weight:" << weight;
    } else {
      qDebug() << "Failed to create edge:" << from << "->" << to
               << "- nodes not found";
    }
  }
}

// ======================== ТОПОЛОГИЧЕСКАЯ СОРТИРОВКА ========================

/**
 * @brief Вспомогательная функция для топологической сортировки и проверки на
 * ацикличность.
 *
 * @param node Текущий узел для обработки.
 * @param state Состояние узлов (0=не посещён, 1=в обработке, 2=обработан).
 * @param stack Стек для хранения топологического порядка.
 * @param logger Логгер для записи сообщений.
 * @return true если обнаружен цикл, false в противном случае.
 */
static bool topologicalSortUtil(SmoothNode *node,
                                std::unordered_map<SmoothNode *, int> &state,
                                std::vector<SmoothNode *> &stack,
                                Logger *logger) {
  state[node] = 1; // Отмечаем узел как "в обработке"

  if (logger) {
    logger->addMessage(
        INFO,
        QString("Посещаем узел ID: %1 для топологической сортировки (DFS)")
            .arg(node->getId()));
  }

  // Рекурсивно посещаем всех соседей (исходящие ребра)
  for (SmoothEdge *edge : node->getOutcomingEdges()) {
    SmoothNode *neighbor = edge->getEndNode();

    // Если сосед находится в состоянии "visiting", значит найден цикл
    if (state[neighbor] == 1) {
      if (logger) {
        logger->addMessage(
            ERROR, QString("Обнаружен цикл! Узел ID %1 ссылается на узел ID "
                           "%2, который уже находится в стеке обработки.")
                       .arg(node->getId())
                       .arg(neighbor->getId()));
      }
      return true; // Цикл обнаружен
    }

    // Если сосед еще не посещен, рекурсивно обрабатываем его
    if (state[neighbor] == 0) {
      if (logger) {
        logger->addMessage(
            INFO, QString("  Переход по ребру: ID %1 -> ID %2 (вес: %3)")
                      .arg(node->getId())
                      .arg(neighbor->getId())
                      .arg(edge->getWeight()));
      }

      if (topologicalSortUtil(neighbor, state, stack, logger)) {
        return true;
      }
    }
  }

  state[node] = 2; // Отмечаем узел как "обработан"
  stack.push_back(node);

  if (logger) {
    logger->addMessage(
        INFO,
        QString("Добавляем узел ID: %1 в стек (после обработки всех потомков)")
            .arg(node->getId()));
  }

  return false;
}

/**
 * @brief Получение топологически отсортированного списка узлов.
 *
 * @param allNodes Все узлы графа.
 * @param logger Логгер для записи сообщений.
 * @return Вектор узлов в топологическом порядке или пустой вектор, если граф
 * содержит циклы.
 */
static std::vector<SmoothNode *>
getTopologicalOrder(const std::vector<SmoothNode *> &allNodes, Logger *logger) {
  std::unordered_map<SmoothNode *, int> state;
  std::vector<SmoothNode *> stack;

  if (logger) {
    logger->addMessage(INFO, "Начинаем топологическую сортировку графа и "
                             "проверку на ацикличность (алгоритм DFS)");
    logger->addMessage(INFO,
                       QString("Всего узлов в графе: %1").arg(allNodes.size()));
  }

  // Инициализируем состояние всех узлов
  for (SmoothNode *node : allNodes) {
    state[node] = 0; // 0 = не посещён
  }

  // Запускаем DFS для каждого непосещённого узла
  for (SmoothNode *node : allNodes) {
    if (state[node] == 0) {
      if (logger) {
        logger->addMessage(
            INFO, QString("Запускаем DFS с узла ID: %1").arg(node->getId()));
      }

      if (topologicalSortUtil(node, state, stack, logger)) {
        if (logger) {
          logger->addMessage(
              ERROR,
              "Граф содержит циклы. Топологическая сортировка невозможна.");
        }
        return {}; // Обнаружен цикл
      }
    }
  }

  if (logger) {
    logger->addMessage(
        SUCCESS,
        "Топологическая сортировка завершена успешно (граф ациклический)");
    QString orderStr = "Порядок узлов (от стоков к источникам): ";
    for (SmoothNode *node : stack) {
      orderStr += QString::number(node->getId()) + " ";
    }
    logger->addMessage(INFO, orderStr);
  }

  return stack;
}

// ======================== ПОИСК КРАТЧАЙШЕГО ПУТИ ========================

/**
 * @brief Находит кратчайший путь между двумя узлами.
 *
 * Использует алгоритм динамического программирования на DAG (Directed Acyclic
 * Graph). Требует, чтобы граф был ациклическим. В случае обнаружения цикла
 * генерируется сигнал loopFound() и возвращается пустой путь.
 *
 * @param from Начальный узел.
 * @param to Конечный узел.
 * @return Вектор узлов, составляющих кратчайший путь, или пустой вектор если
 * путь не найден.
 */
std::vector<SmoothNode *> Graph::findShortestPath(SmoothNode *from,
                                                  SmoothNode *to) {
  clearAndStartLogging();
  logPathStart(from, to);

  if (!validateNodes(from, to)) {
    return {};
  }

  if (isSameNode(from, to)) {
    logSameNodePath(from);
    return {from};
  }

  // Получаем топологический порядок (проверяем на циклы)
  std::vector<SmoothNode *> topoOrder;
  if (!isDag(topoOrder)) {
    return {};
  }

  // Инициализация DP структур
  std::unordered_map<SmoothNode *, float> dist;
  std::unordered_map<SmoothNode *, SmoothNode *> nextNode;

  if (!initializeDistanceMaps(topoOrder, to, dist, nextNode)) {
    return {};
  }

  // Обратный проход DP
  processTopologicalOrder(topoOrder, dist, nextNode);

  // Проверяем достижимость цели
  if (!isReachable(from, dist)) {
    logUnreachablePath(from, to);
    return {};
  }

  // Восстанавливаем путь
  float totalWeight = 0.0f;
  std::vector<SmoothNode *> path =
      reconstructPath(from, to, nextNode, totalWeight);

  if (path.empty() || path.back() != to) {
    logPathError("Ошибка при восстановлении пути");
    return {};
  }

  logPathSuccess(dist[from], path, totalWeight);
  return path;
}

/**
 * @brief Очищает логгер и добавляет заголовок поиска.
 */
void Graph::clearAndStartLogging() {
  if (logger_) {
    logger_->clear();
    logger_->addMessage(INFO, "=== НАЧАЛО ПОИСКА КРАТЧАЙШЕГО ПУТИ ===");
  }
}

/**
 * @brief Логирует начало поиска пути.
 * @param from Начальный узел.
 * @param to Конечный узел.
 */
void Graph::logPathStart(SmoothNode *from, SmoothNode *to) const {
  if (!logger_)
    return;

  logger_->addMessage(INFO,
                      QString("Стартовый узел: ID %1").arg(from->getId()));
  logger_->addMessage(INFO, QString("Целевой узел: ID %1").arg(to->getId()));
}

/**
 * @brief Логирует успешное нахождение пути.
 * @param distance Найденное расстояние.
 * @param path Найденный путь.
 * @param totalWeight Общий вес пути.
 */
void Graph::logPathSuccess(float distance,
                           const std::vector<SmoothNode *> &path,
                           float totalWeight) const {
  if (!logger_)
    return;

  logger_->addMessage(SUCCESS,
                      QString("Кратчайшее расстояние: %1").arg(distance));
  logger_->addMessage(
      SUCCESS, QString("Путь найден! Количество узлов: %1, общий вес: %2")
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

/**
 * @brief Логирует ошибку поиска пути.
 * @param error Текст ошибки.
 */
void Graph::logPathError(const QString &error) const {
  if (logger_) {
    logger_->addMessage(ERROR, error);
    logger_->addMessage(INFO, "=== КОНЕЦ ПОИСКА (ОШИБКА) ===");
  }
}

/**
 * @brief Проверяет валидность начального и конечного узлов.
 * @param from Начальный узел.
 * @param to Конечный узел.
 * @return true если оба узла существуют, false в противном случае.
 */
bool Graph::validateNodes(SmoothNode *from, SmoothNode *to) const {
  if (!from || !to) {
    logPathError("Ошибка: начальный или конечный узел не определён");
    return false;
  }
  return true;
}

/**
 * @brief Проверяет, совпадают ли начальный и конечный узлы.
 * @param from Начальный узел.
 * @param to Конечный узел.
 * @return true если узлы совпадают.
 */
bool Graph::isSameNode(SmoothNode *from, SmoothNode *to) const {
  return from == to;
}

/**
 * @brief Логирует случай, когда начальный и конечный узлы совпадают.
 * @param node Узел.
 */
void Graph::logSameNodePath(SmoothNode *node) const {
  if (logger_) {
    logger_->addMessage(
        SUCCESS, "Стартовый и конечный узлы совпадают. Путь найден: [0]");
    logger_->addMessage(INFO, "=== КОНЕЦ ПОИСКА (УСПЕХ) ===");
  }
}

/**
 * @brief Проверяет, является ли граф ациклическим (DAG) и получает
 * топологический порядок.
 * @param topoOrder Выходной параметр: вектор узлов в топологическом порядке.
 * @return true если граф ациклический, false если содержит циклы.
 */
bool Graph::isDag(std::vector<SmoothNode *> &topoOrder) const {
  std::vector<SmoothNode *> allNodes = getNodes();

  if (logger_) {
    logger_->addMessage(INFO, "Проверка графа на ацикличность...");
    logger_->addMessage(INFO,
                        QString("Получено узлов: %1").arg(allNodes.size()));
  }

  topoOrder = getTopologicalOrder(allNodes, logger_);

  if (topoOrder.empty() && !allNodes.empty()) {
    logPathError("Граф содержит циклы. Алгоритм применим только для DAG");
    emit const_cast<Graph *>(this)->loopFound();
    return false;
  }

  if (logger_) {
    logger_->addMessage(SUCCESS, "Граф ациклический, можно применять DP");
  }
  return true;
}

/**
 * @brief Инициализирует структуры расстояний для DP.
 * @param nodes Все узлы графа.
 * @param target Целевой узел (конечная точка пути).
 * @param dist Карта расстояний (выходной параметр).
 * @param nextNode Карта для восстановления пути (выходной параметр).
 * @return true если инициализация успешна.
 */
bool Graph::initializeDistanceMaps(
    const std::vector<SmoothNode *> &nodes, SmoothNode *target,
    std::unordered_map<SmoothNode *, float> &dist,
    std::unordered_map<SmoothNode *, SmoothNode *> &nextNode) const {
  const float INF = std::numeric_limits<float>::max();

  for (SmoothNode *node : nodes) {
    dist[node] = INF;
    nextNode[node] = nullptr;
  }

  dist[target] = 0.0f;

  if (logger_) {
    logger_->addMessage(INFO, "Инициализация DP структур");
    logger_->addMessage(INFO, QString("dist[ID %1] = 0").arg(target->getId()));
  }

  return true;
}

/**
 * @brief Обрабатывает топологический порядок для вычисления кратчайших
 * расстояний.
 * @param topoOrder Вектор узлов в топологическом порядке.
 * @param dist Карта расстояний (обновляется).
 * @param nextNode Карта для восстановления пути (обновляется).
 */
void Graph::processTopologicalOrder(
    const std::vector<SmoothNode *> &topoOrder,
    std::unordered_map<SmoothNode *, float> &dist,
    std::unordered_map<SmoothNode *, SmoothNode *> &nextNode) const {
  if (logger_) {
    logger_->addMessage(INFO, "Начинаем обратный проход (DP)...");
  }

  int processedCount = 0;

  for (SmoothNode *node : topoOrder) {
    if (dist[node] == std::numeric_limits<float>::max()) {
      continue;
    }

    processedCount++;
    if (logger_) {
      logger_->addMessage(INFO,
                          QString("Обработка узла ID: %1 (расст. до цели: %2)")
                              .arg(node->getId())
                              .arg(dist[node]));
    }

    processNodeInReverse(node, dist, nextNode);
  }

  if (logger_) {
    logger_->addMessage(INFO, QString("Обработано узлов: %1 из %2")
                                  .arg(processedCount)
                                  .arg(topoOrder.size()));
  }
}

/**
 * @brief Обрабатывает один узел при обратном проходе DP.
 * @param node Текущий обрабатываемый узел.
 * @param dist Карта расстояний (обновляется).
 * @param nextNode Карта для восстановления пути (обновляется).
 */
void Graph::processNodeInReverse(
    SmoothNode *node, std::unordered_map<SmoothNode *, float> &dist,
    std::unordered_map<SmoothNode *, SmoothNode *> &nextNode) const {
  // Рассматриваем все входящие ребра: prev -> node
  for (SmoothEdge *inEdge : node->getIncomingEdges()) {
    SmoothNode *prev = inEdge->getStartNode();
    float weight = inEdge->getWeight();
    float newDistance = weight + dist[node];

    if (dist[prev] > newDistance) {
      float oldDistance = dist[prev];
      dist[prev] = newDistance;
      nextNode[prev] = node;

      if (logger_) {
        logger_->addMessage(
            INFO, QString("  Обновление пути к узлу ID %1: %2 -> %3 (через ID "
                          "%4, вес: %5)")
                      .arg(prev->getId())
                      .arg(oldDistance == std::numeric_limits<float>::max()
                               ? "∞"
                               : QString::number(oldDistance))
                      .arg(newDistance)
                      .arg(node->getId())
                      .arg(weight));
      }
    }
  }
}

/**
 * @brief Проверяет, достижим ли целевой узел из начального.
 * @param from Начальный узел.
 * @param dist Карта расстояний.
 * @return true если узел достижим.
 */
bool Graph::isReachable(
    SmoothNode *from,
    const std::unordered_map<SmoothNode *, float> &dist) const {
  return dist.at(from) != std::numeric_limits<float>::max();
}

/**
 * @brief Логирует недостижимость целевого узла.
 * @param from Начальный узел.
 * @param to Конечный узел.
 */
void Graph::logUnreachablePath(SmoothNode *from, SmoothNode *to) const {
  if (logger_) {
    logger_->addMessage(ERROR,
                        QString("Путь от узла ID %1 до узла ID %2 не найден!")
                            .arg(from->getId())
                            .arg(to->getId()));
    logger_->addMessage(INFO, "=== КОНЕЦ ПОИСКА (ПУТЬ НЕ НАЙДЕН) ===");
  }
}

/**
 * @brief Восстанавливает путь по карте nextNode.
 * @param from Начальный узел.
 * @param to Конечный узел.
 * @param nextNode Карта, указывающая следующий узел на пути.
 * @param totalWeight Выходной параметр: общий вес восстановленного пути.
 * @return Вектор узлов, составляющих путь.
 */
std::vector<SmoothNode *> Graph::reconstructPath(
    SmoothNode *from, SmoothNode *to,
    const std::unordered_map<SmoothNode *, SmoothNode *> &nextNode,
    float &totalWeight) const {
  if (logger_) {
    logger_->addMessage(INFO, "Восстанавливаем путь...");
  }

  std::vector<SmoothNode *> path;
  SmoothNode *current = from;
  totalWeight = 0.0f;

  while (current != nullptr) {
    path.push_back(current);

    if (logger_) {
      logger_->addMessage(
          INFO, QString("  Добавлен узел ID: %1").arg(current->getId()));
    }

    if (current == to) {
      break;
    }

    auto it = nextNode.find(current);
    if (it == nextNode.end()) {
      break;
    }

    SmoothNode *next = it->second;
    if (next) {
      float weight = getEdgeWeight(current, next);
      totalWeight += weight;

      if (logger_) {
        logger_->addMessage(INFO, QString("    Ребро ID %1 -> ID %2 (вес: %3)")
                                      .arg(current->getId())
                                      .arg(next->getId())
                                      .arg(weight));
      }
    }

    current = next;
  }

  return path;
}

/**
 * @brief Получает вес ребра между двумя узлами.
 * @param from Начальный узел ребра.
 * @param to Конечный узел ребра.
 * @return Вес ребра или 0, если ребро не найдено.
 */
float Graph::getEdgeWeight(SmoothNode *from, SmoothNode *to) const {
  for (SmoothEdge *edge : from->getOutcomingEdges()) {
    if (edge->getEndNode() == to) {
      return edge->getWeight();
    }
  }
  return 0.0f;
}
