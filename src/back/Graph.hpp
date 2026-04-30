// Graph.hpp
#pragma once

#include "../front/Figures.hpp"

#include <QGraphicsScene>
#include <QList>
#include <cstddef>
#include <list>
#include <string>
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
class Graph {
public:
  /**
   * @brief Конструктор класса Graph.
   */
  Graph();

  /**
   * @brief Деструктор класса Graph.
   */
  ~Graph();

  /**
   * @brief Добавляет узел в граф.
   * @param node Указатель на добавляемый узел.
   * @return ID присвоенный узлу.
   */
  ID addNode(SmoothNode *node);

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
  bool saveToFile(const std::string &filepath);

  /**
   * @brief Загружает граф из файла.
   * @param filepath Путь к файлу для загрузки.
   * @param scene Сцена QGraphicsScene для отрисовки узлов и рёбер.
   * @return true если загрузка успешна, false в противном случае.
   */
  bool loadFromFile(const std::string &filepath, QGraphicsScene *scene);

  /**
   * @brief Устанавливает путь к текущему файлу графа.
   * @param filepath Путь к файлу.
   */
  void setCurrentFilePath(const std::string &filepath) {
    currentFilePath_ = filepath;
  }

  /**
   * @brief Возвращает путь к текущему файлу графа.
   * @return Путь к файлу.
   */
  std::string getCurrentFilePath() const { return currentFilePath_; }

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
   * @param from Начальный узел.
   * @param to Конечный узел.
   * @return Вектор узлов, составляющих кратчайший путь.
   */
  std::vector<SmoothNode *> findShortestPath(SmoothNode *from, SmoothNode *to);

private:
  std::unordered_map<ID, SmoothNode *> nodes_;    // Узлы графа
  std::unordered_map<ID, std::list<ID>> adjList_; // Список смежности
  std::unordered_map<ID, std::unordered_map<ID, float>>
      edgeWeights_;             // Веса рёбер
  std::string currentFilePath_; // Путь к текущему файлу
  bool isModified_;             // Флаг модификации
  ID nextNodeId_;               // Следующий доступный ID

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
};
