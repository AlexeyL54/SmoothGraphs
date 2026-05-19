// Graph.cpp
#include "Graph.hpp"
#include "Graphviz.hpp"

#include <QDebug>
#include <QMessageBox>
#include <algorithm>
#include <limits>

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

#include <limits>
#include <set>
#include <stack>

// Вспомогательная функция для топологической сортировки (DFS)
static void topologicalSortUtil(SmoothNode *node,
                                std::unordered_map<SmoothNode *, bool> &visited,
                                std::vector<SmoothNode *> &stack) {
  visited[node] = true;

  // Рекурсивно посещаем всех соседей (исходящие ребра)
  for (SmoothEdge *edge : node->getOutcomingEdges()) {
    SmoothNode *neighbor = edge->getEndNode();
    if (!visited[neighbor]) {
      topologicalSortUtil(neighbor, visited, stack);
    }
  }

  // Добавляем текущий узел в стек после обработки всех соседей
  stack.push_back(node);
}

// Получение топологически отсортированного списка узлов
static std::vector<SmoothNode *>
getTopologicalOrder(const std::vector<SmoothNode *> &allNodes) {
  std::unordered_map<SmoothNode *, bool> visited;
  std::vector<SmoothNode *> stack;

  for (SmoothNode *node : allNodes) {
    visited[node] = false;
  }

  for (SmoothNode *node : allNodes) {
    if (!visited[node]) {
      topologicalSortUtil(node, visited, stack);
    }
  }

  // stack содержит узлы в обратном топологическом порядке (последний
  // обработанный - первый в списке) Для прямого прохождения (от источника к
  // стоку) нам нужен обычный топологический порядок. Но для обратного прохода
  // (от стока к источнику) удобнее использовать обратный порядок. Вернем как
  // есть, будем обрабатывать с конца.
  return stack;
}

std::vector<SmoothNode *> Graph::findShortestPath(SmoothNode *from,
                                                  SmoothNode *to) {
  if (!from || !to) {
    return {};
  }

  if (from == to) {
    return {from};
  }

  // 1. Получаем все узлы графа
  std::vector<SmoothNode *> allNodes = getNodes();

  // 2. Топологическая сортировка
  // Важно: этот алгоритм работает ТОЛЬКО для DAG. Если есть циклы, поведение не
  // определено.
  std::vector<SmoothNode *> topoOrder = getTopologicalOrder(allNodes);

  // topoOrder сейчас содержит узлы в порядке финиша DFS.
  // Чтобы получить классический топологический порядок (источники первыми),
  // нужно развернуть. Но мы будем делать ОБРАТНЫЙ проход (от to к from),
  // поэтому нам удобно обрабатывать узлы в порядке, обратном топологическому
  // (стоки первыми). Так как getTopologicalOrder возвращает узлы в порядке
  // "пост-порядка" (потомки перед родителями в стеке), то итерация с начала
  // вектора даст нам узлы, близкие к стокам, первыми. Это то, что нам нужно для
  // обратного прохода.

  // 3. Инициализация структур данных для обратного прохода
  // dist[node] - кратчайшее расстояние от node до to
  std::unordered_map<SmoothNode *, float> dist;
  // nextNode[node] - следующий узел на кратчайшем пути от node к to
  std::unordered_map<SmoothNode *, SmoothNode *> nextNode;

  const float INF = std::numeric_limits<float>::max();

  for (SmoothNode *node : allNodes) {
    dist[node] = INF;
    nextNode[node] = nullptr;
  }

  dist[to] = 0.0f; // Расстояние от to до to равно 0

  // 4. Обратный проход ДП
  // Обрабатываем узлы в топологическом порядке (так, чтобы при обработке узла
  // u, все его потомки (куда ведут исходящие ребра) уже были обработаны). В
  // нашем topoOrder (результат DFS post-order) узлы идут в порядке: потомки,
  // затем предки. Поэтому простая итерация по topoOrder подходит для обратного
  // прохода (от стоков к источникам).

  for (SmoothNode *u : topoOrder) {
    // Если до текущего узла u еще не удалось добраться от to (dist[u] == INF),
    // то он не лежит на пути к to, пропускаем.
    // Исключение: сам to, который мы инициализировали.
    if (dist[u] == INF) {
      continue;
    }

    // Рассматриваем все входящие ребра в u: (v -> u)
    // Мы хотим обновить dist[v] используя dist[u]
    for (SmoothEdge *inEdge : u->getIncomingEdges()) {
      SmoothNode *v = inEdge->getStartNode();
      float weight = inEdge->getWeight();

      // Если путь от v до to через u короче, чем известный путь от v до to
      if (dist[v] > weight + dist[u]) {
        dist[v] = weight + dist[u];
        nextNode[v] = u; // Запоминаем, что из v лучше идти в u
      }
    }
  }

  // 5. Проверка достижимости
  if (dist[from] == INF) {
    // Пути нет
    return {};
  }

  // 6. Прямой проход: восстановление пути от from к to
  std::vector<SmoothNode *> path;
  SmoothNode *current = from;

  while (current != nullptr) {
    path.push_back(current);
    if (current == to) {
      break;
    }
    current = nextNode[current];
  }

  // Если путь не завершился в to (например, цикл или ошибка), возвращаем пустой
  // вектор
  if (path.back() != to) {
    return {};
  }

  return path;
}

/**
 * @brief Находит кратчайший путь между двумя узлами.
 * @param from Начальный узел.
 * @param to Конечный узел.
 * @return Вектор узлов, составляющих кратчайший путь.
 */
/*std::vector<SmoothNode *> Graph::findShortestPath(SmoothNode *from,
                                                  SmoothNode *to) {
  std::vector<std::unordered_map<SmoothNode *, Rout>> vRev;
  std::vector<SmoothNode *> shortestPath = {from};
  vRev.push_back({{to, {0, nullptr}}});
  size_t k = 0;

  // пока не дошли до отправного узла
  while (vRev[k].find(from) == vRev[k].end()) {
    // создать новый пояс
    vRev.push_back({});

    // для каждого узла k-го пояса
    // определить соседние узлы, из которых есть путь в текущий
    for (std::pair<SmoothNode *const, Rout> node : vRev[k]) {
      QList<SmoothEdge *> tempIncomingEdges = node.first->getIncomingEdges();

      // для каждой входящей грани определить начало
      for (SmoothEdge *inEdge : tempIncomingEdges) {
        SmoothNode *tempNode = inEdge->getStartNode();
        QList<SmoothEdge *> tempOutcomingEdges = tempNode->getIncomingEdges();
        float minBellmanVal = 0;

        // для каждого соседнего узла расчитать значение функции Беллмана
        // и определить оптимальный следующий узел
        for (SmoothEdge *outEdge : tempOutcomingEdges) {

          if (minBellmanVal > vRev[k][outEdge->getEndNode()].bellmanValue) {
            minBellmanVal = vRev[k][outEdge->getEndNode()].bellmanValue;

            vRev[k + 1][tempNode].bellmanValue =
                outEdge->getWeight() + minBellmanVal;
            vRev[k + 1][tempNode].nextBestNode = outEdge->getEndNode();
          }
        }
      }
    }
    k++;
  }

  SmoothNode *tempNode = from;
  for (size_t i = vRev.size() - 2; i >= 0; i--) {
    shortestPath.push_back(vRev[i - 1][tempNode].nextBestNode);
    tempNode = vRev[i - 1][tempNode].nextBestNode;
  }

  return shortestPath;
}*/
