// Graphviz.hpp
#pragma once

#include "../front/Figures.hpp"
#include "qobject.h"

#include <string>
#include <tuple>
#include <vector>

/**
 * @class Graphviz
 * @brief Класс для экспорта и импорта графа в формате Graphviz DOT.
 *
 * Предоставляет методы для сохранения структуры графа в файл
 * и загрузки графа из файла с использованием синтаксиса Graphviz.
 */
class Graphviz {
public:
  /**
   * @brief Конструктор класса Graphviz.
   */
  Graphviz();

  /**
   * @brief Сохраняет граф в файл формата .gphz.
   * @param filename Путь к файлу для сохранения.
   * @param nodes Вектор узлов графа.
   * @param edges Вектор рёбер графа.
   * @return true если сохранение прошло успешно, false в противном случае.
   */
  bool saveToFile(const QString &filename,
                  const std::vector<SmoothNode *> &nodes,
                  const std::vector<SmoothEdge *> &edges);

  /**
   * @brief Загружает граф из файла формата .gphz.
   * @param filename Путь к файлу для загрузки.
   * @param nodes Выходной параметр: вектор узлов (id, x, y).
   * @param edges Выходной параметр: вектор рёбер (from, to, weight).
   * @return true если загрузка прошла успешно, false в противном случае.
   */
  bool loadFromFile(const QString &filename,
                    std::vector<std::tuple<size_t, double, double>> &nodes,
                    std::vector<std::tuple<size_t, size_t, float>> &edges);

private:
  /**
   * @brief Экранирует специальные символы для DOT формата.
   * @param str Строка для экранирования.
   * @return Экранированная строка.
   */
  std::string escapeString(const std::string &str) const;

  /**
   * @brief Сохраняет информацию об узлах в DOT формате.
   * @param out Выходной поток.
   * @param nodes Вектор узлов.
   */
  void saveNodes(QTextStream &out, const std::vector<SmoothNode *> &nodes);

  /**
   * @brief Сохраняет информацию о рёбрах в DOT формате.
   * @param out Выходной поток.
   * @param edges Вектор рёбер.
   */
  void saveEdges(QTextStream &out, const std::vector<SmoothEdge *> &edges);
};
