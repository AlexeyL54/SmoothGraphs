#pragma once

#include "../front/Figures.hpp"
#include "Logger.hpp"

#include <QGraphicsScene>
#include <QList>
#include <QObject>
#include <cstddef>
#include <list>
#include <unordered_map>
#include <vector>

typedef size_t ID;

/**
 * @class Graph
 * @brief Класс для управления структурой графа.
 *
 * Предоставляет методы для:
 * - Хранения узлов и рёбер графа
 * - Сохранения и загрузки в/из файла
 * - Поиска кратчайшего пути
 * - Управления модификациями и текущим файлом
 */
class Graph : public QObject {
  Q_OBJECT
public:
  struct NodeData {
    ID id;
    double x;
    double y;
  };

  struct EdgeData {
    ID from;
    ID to;
    float weight;
  };

  /**
   * @brief Конструктор класса Graph.
   * @param logger Указатель на логгер для записи сообщений (опционально).
   */
  explicit Graph(Logger *logger = nullptr);

  /**
   * @brief Деструктор класса Graph.
   */
  ~Graph();

  /**
   * @brief Добавляет узел в граф.
   * @param node Указатель на добавляемый узел.
   * @return ID присвоенный узлу, или 0 если ошибка.
   */
  ID addNode(SmoothNode *node);

  /**
   * @brief Добавляет узел в граф с указанным ID.
   * @param node Указатель на добавляемый узел.
   * @param id ID для узла.
   * @return ID узла (тот же, что передан) или 0 если ошибка.
   */
  ID addNodeWithId(SmoothNode *node, ID id);

  /**
   * @brief Добавляет ребро в граф.
   * @param edge Указатель на добавляемое ребро.
   */
  void addEdge(SmoothEdge *edge);

  /**
   * @brief Удаляет узел из графа по ID.
   * @param id ID удаляемого узла.
   */
  void deleteNode(ID id);

  /**
   * @brief Удаляет ребро из графа.
   * @param from ID начального узла.
   * @param to ID конечного узла.
   */
  void deleteEdge(ID from, ID to);

  /**
   * @brief Очищает граф (удаляет все узлы и рёбра).
   */
  void clear();

  /**
   * @brief Сохраняет граф в файл.
   * @param filepath Путь к файлу для сохранения.
   * @return true если сохранение успешно, false в противном случае.
   */
  bool saveToFile(const QString &filepath);

  /**
   * @brief Парсит файл и возвращает данные графа.
   *
   * Не создает визуальные элементы, только извлекает данные.
   * @param filepath Путь к файлу для парсинга.
   * @param nodes Вектор для заполнения данными узлов.
   * @param edges Вектор для заполнения данными рёбер.
   * @return true если парсинг успешен, false в противном случае.
   */
  bool parseFile(const QString &filepath, std::vector<NodeData> &nodes,
                 std::vector<EdgeData> &edges);

  /**
   * @brief Устанавливает путь к текущему файлу графа.
   * @param filepath Путь к файлу.
   */
  void setCurrentFilePath(QString &filepath) { currentFilePath_ = filepath; }

  /**
   * @brief Возвращает путь к текущему файлу графа.
   * @return Путь к файлу.
   */
  QString getCurrentFilePath() const { return currentFilePath_; }

  /**
   * @brief Устанавливает флаг модификации графа.
   * @param modified Флаг модификации.
   */
  void setModified(bool modified) { isModified_ = modified; }

  /**
   * @brief Проверяет, был ли граф модифицирован.
   * @return true если граф модифицирован, false в противном случае.
   */
  bool isModified() const { return isModified_; }

  /**
   * @brief Возвращает список всех узлов графа.
   * @return Вектор указателей на узлы.
   */
  std::vector<SmoothNode *> getNodes() const;

  /**
   * @brief Возвращает список всех рёбер графа.
   * @return Вектор указателей на рёбра.
   */
  std::vector<SmoothEdge *> getEdges() const;

  /**
   * @brief Возвращает ID узла по указателю.
   * @param node Указатель на узел.
   * @return ID узла или 0 если не найден.
   */
  ID getNodeId(SmoothNode *node) const;

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
  std::vector<SmoothNode *> findShortestPath(SmoothNode *from, SmoothNode *to);

  /**
   * @brief Возвращает текущую ревизию графа.
   * @return Номер ревизии (увеличивается при каждом изменении структуры).
   */
  unsigned long long getRevision() { return revision_; }

private:
  std::unordered_map<ID, SmoothNode *> nodes_;    ///< Хранилище узлов графа
  std::unordered_map<ID, std::list<ID>> adjList_; ///< Список смежности
  QString currentFilePath_;     ///< Путь к текущему открытому файлу
  bool isModified_;             ///< Флаг несохранённых изменения
  ID nextNodeId_;               ///< Следующий доступный ID для нового узла
  Logger *logger_;              ///< Логгер для записи информации о поиске пути
  unsigned long long revision_; ///< Счётчик ревизий для отслеживания изменений

  /**
   * @brief Уведомляет о изменении структуры графа.
   *
   * Увеличивает ревизию, устанавливает флаг модификации и генерирует сигнал.
   */
  void notifyChange();

  // ======== МЕТОДЫ ПОИСКА КРАТЧАЙШЕГО ПУТИ ========

  /**
   * @brief Очищает логгер и добавляет заголовок поиска.
   */
  void clearAndStartLogging();

  /**
   * @brief Логирует начало поиска пути.
   * @param from Начальный узел.
   * @param to Конечный узел.
   */
  void logPathStart(SmoothNode *from, SmoothNode *to) const;

  /**
   * @brief Логирует успешное нахождение пути.
   * @param distance Найденное расстояние.
   * @param path Найденный путь.
   * @param totalWeight Общий вес пути.
   */
  void logPathSuccess(float distance, const std::vector<SmoothNode *> &path,
                      float totalWeight) const;

  /**
   * @brief Логирует ошибку поиска пути.
   * @param error Текст ошибки.
   */
  void logPathError(const QString &error) const;

  /**
   * @brief Проверяет валидность начального и конечного узлов.
   * @param from Начальный узел.
   * @param to Конечный узел.
   * @return true если оба узла существуют, false в противном случае.
   */
  bool validateNodes(SmoothNode *from, SmoothNode *to) const;

  /**
   * @brief Проверяет, совпадают ли начальный и конечный узлы.
   * @param from Начальный узел.
   * @param to Конечный узел.
   * @return true если узлы совпадают.
   */
  bool isSameNode(SmoothNode *from, SmoothNode *to) const;

  /**
   * @brief Логирует случай, когда начальный и конечный узлы совпадают.
   * @param node Узел.
   */
  void logSameNodePath(SmoothNode *node) const;

  /**
   * @brief Проверяет, является ли граф ациклическим (DAG) и получает
   * топологический порядок.
   * @param topoOrder Выходной параметр: вектор узлов в топологическом порядке.
   * @return true если граф ациклический, false если содержит циклы.
   */
  bool isDag(std::vector<SmoothNode *> &topoOrder) const;

  /**
   * @brief Инициализирует структуры расстояний для DP.
   * @param nodes Все узлы графа.
   * @param target Целевой узел (конечная точка пути).
   * @param dist Карта расстояний (выходной параметр).
   * @param nextNode Карта для восстановления пути (выходной параметр).
   * @return true если инициализация успешна.
   */
  bool initializeDistanceMaps(
      const std::vector<SmoothNode *> &nodes, SmoothNode *target,
      std::unordered_map<SmoothNode *, float> &dist,
      std::unordered_map<SmoothNode *, SmoothNode *> &nextNode) const;

  /**
   * @brief Обрабатывает топологический порядок для вычисления кратчайших
   * расстояний.
   * @param topoOrder Вектор узлов в топологическом порядке.
   * @param dist Карта расстояний (обновляется).
   * @param nextNode Карта для восстановления пути (обновляется).
   */
  void processTopologicalOrder(
      const std::vector<SmoothNode *> &topoOrder,
      std::unordered_map<SmoothNode *, float> &dist,
      std::unordered_map<SmoothNode *, SmoothNode *> &nextNode) const;

  /**
   * @brief Обрабатывает один узел при обратном проходе DP.
   * @param node Текущий обрабатываемый узел.
   * @param dist Карта расстояний (обновляется).
   * @param nextNode Карта для восстановления пути (обновляется).
   */
  void processNodeInReverse(
      SmoothNode *node, std::unordered_map<SmoothNode *, float> &dist,
      std::unordered_map<SmoothNode *, SmoothNode *> &nextNode) const;

  /**
   * @brief Проверяет, достижим ли целевой узел из начального.
   * @param from Начальный узел.
   * @param dist Карта расстояний.
   * @return true если узел достижим.
   */
  bool isReachable(SmoothNode *from,
                   const std::unordered_map<SmoothNode *, float> &dist) const;

  /**
   * @brief Логирует недостижимость целевого узла.
   * @param from Начальный узел.
   * @param to Конечный узел.
   */
  void logUnreachablePath(SmoothNode *from, SmoothNode *to) const;

  /**
   * @brief Восстанавливает путь по карте nextNode.
   * @param from Начальный узел.
   * @param to Конечный узел.
   * @param nextNode Карта, указывающая следующий узел на пути.
   * @param totalWeight Выходной параметр: общий вес восстановленного пути.
   * @return Вектор узлов, составляющих путь.
   */
  std::vector<SmoothNode *> reconstructPath(
      SmoothNode *from, SmoothNode *to,
      const std::unordered_map<SmoothNode *, SmoothNode *> &nextNode,
      float &totalWeight) const;

  /**
   * @brief Получает вес ребра между двумя узлами.
   * @param from Начальный узел ребра.
   * @param to Конечный узел ребра.
   * @return Вес ребра или 0, если ребро не найдено.
   */
  float getEdgeWeight(SmoothNode *from, SmoothNode *to) const;

  // ======== МЕТОДЫ ЗАГРУЗКИ ГРАФА ========

  /**
   * @brief Создаёт узлы на сцене из загруженных данных.
   * @param nodesData Данные об узлах (ID, x, y).
   * @param scene Сцена для добавления узлов.
   * @return Карта соответствия ID -> SmoothNode*.
   */
  std::unordered_map<ID, SmoothNode *> createNodesFromData(
      const std::vector<std::tuple<ID, double, double>> &nodesData,
      QGraphicsScene *scene);

  /**
   * @brief Создаёт рёбра на сцене из загруженных данных.
   * @param edgesData Данные о рёбрах (from, to, weight).
   * @param idToNode Карта соответствия ID -> SmoothNode*.
   * @param scene Сцена для добавления рёбер.
   */
  void
  createEdgesFromData(const std::vector<std::tuple<ID, ID, float>> &edgesData,
                      const std::unordered_map<ID, SmoothNode *> &idToNode,
                      QGraphicsScene *scene);

signals:
  /**
   * @brief Сигнал об изменении структуры графа.
   *
   * Испускается при добавлении/удалении узлов или рёбер.
   */
  void graphStructureChanged();

  /**
   * @brief Сигнал об обнаружении цикла в графе.
   *
   * Испускается при попытке найти путь в циклическом графе.
   */
  void loopFound();
};
