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

  /**
   * @brief Предварительная обработка содержимого файла (удаление комментариев).
   * @param content Исходное содержимое файла.
   * @return Очищенное содержимое.
   */
  QString preprocessContent(const QString &content) const;

  /**
   * @brief Парсит узлы из содержимого.
   * @param content Очищенное содержимое файла.
   * @param nodes Выходной параметр: вектор узлов.
   * @return true если парсинг прошёл успешно.
   */
  bool parseNodes(const QString &content,
                  std::vector<std::tuple<size_t, double, double>> &nodes) const;

  /**
   * @brief Парсит рёбра из содержимого.
   * @param content Очищенное содержимое файла.
   * @param edges Выходной параметр: вектор рёбер.
   * @return true если парсинг прошёл успешно.
   */
  bool parseEdges(const QString &content,
                  std::vector<std::tuple<size_t, size_t, float>> &edges) const;

  /**
   * @brief Парсит один узел из регулярного выражения.
   * @param match Регулярное выражение с данными узла.
   * @return tuple (id, x, y) или std::nullopt если парсинг не удался.
   */
  std::optional<std::tuple<size_t, double, double>>
  parseNodeMatch(const QRegularExpressionMatch &match) const;

  /**
   * @brief Парсит одно ребро из регулярного выражения.
   * @param match Регулярное выражение с данными ребра.
   * @return tuple (from, to, weight) или std::nullopt если парсинг не удался.
   */
  std::optional<std::tuple<size_t, size_t, float>>
  parseEdgeMatch(const QRegularExpressionMatch &match) const;
};
