#include "Graphviz.hpp"
#include "qdebug.h"

#include <QDebug>
#include <QFile>
#include <QRegularExpression>
#include <QTextStream>
#include <optional>

/**
 * @brief Конструктор класса Graphviz.
 */
Graphviz::Graphviz() {}

/**
 * @brief Экранирует специальные символы для DOT формата.
 * @param str Строка для экранирования.
 * @return Экранированная строка.
 */
std::string Graphviz::escapeString(const std::string &str) const {
  std::string result = str;
  size_t pos = 0;
  while ((pos = result.find('"', pos)) != std::string::npos) {
    result.replace(pos, 1, "\\\"");
    pos += 2;
  }
  return result;
}

/**
 * @brief Сохраняет информацию об узлах в DOT формате.
 * @param out Выходной поток.
 * @param nodes Вектор узлов.
 */
void Graphviz::saveNodes(QTextStream &out,
                         const std::vector<SmoothNode *> &nodes) {
  out << "  // Nodes\n";
  out << "  node [margin=0 fontsize=12 width=0.5 shape=circle style=filled]\n";

  for (SmoothNode *node : nodes) {
    if (!node)
      continue;

    QPointF center = node->getCenter();
    out << "  n" << node->getId() << " [";
    out << "label=\"" << node->getId() << "\" ";
    out << "pos=\"" << center.x() << "," << center.y() << "!\" ";
    out << "];\n";
  }
}

/**
 * @brief Сохраняет информацию о рёбрах в DOT формате.
 * @param out Выходной поток.
 * @param edges Вектор рёбер.
 */
void Graphviz::saveEdges(QTextStream &out,
                         const std::vector<SmoothEdge *> &edges) {
  out << "\n  // Edges\n";
  for (SmoothEdge *edge : edges) {
    if (!edge)
      continue;

    SmoothNode *start = edge->getStartNode();
    SmoothNode *end = edge->getEndNode();

    if (!start || !end)
      continue;

    qDebug() << "Saving edge: start ID=" << start->getId()
             << "end ID=" << end->getId() << "weight=" << edge->getWeight();

    out << "  n" << start->getId() << " -> n" << end->getId();
    if (edge->getWeight() != 1.0f) {
      out << " [label=\"" << edge->getWeight() << "\"]";
    }
    out << ";\n";
  }
}

/**
 * @brief Сохраняет граф в файл формата .gphz.
 * @param filename Путь к файлу для сохранения.
 * @param nodes Вектор узлов графа.
 * @param edges Вектор рёбер графа.
 * @return true если сохранение прошло успешно, false в противном случае.
 */
bool Graphviz::saveToFile(const QString &filename,
                          const std::vector<SmoothNode *> &nodes,
                          const std::vector<SmoothEdge *> &edges) {
  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    return false;
  }

  QTextStream out(&file);

  out << "digraph G {\n";
  out << "  // Graph saved from Graph Editor Application\n";
  out << "  // Format: .gphz (Graphviz with extended attributes)\n";
  out << "  graph [bb=\"0,0,800,600\"];\n\n";

  saveNodes(out, nodes);
  out << "\n";
  saveEdges(out, edges);

  out << "}\n";

  file.close();
  qDebug() << "Graph saved successfully to:" << filename;
  return true;
}

/**
 * @brief Безопасно преобразует строку в float.
 * @param str Строка для преобразования.
 * @param defaultValue Значение по умолчанию при ошибке.
 * @return Преобразованное значение или defaultValue.
 */
static float safeStringToFloat(const QString &str, float defaultValue = 1.0f) {
  if (str.isEmpty()) {
    return defaultValue;
  }

  bool ok;
  float value = str.toFloat(&ok);
  if (ok) {
    return value;
  }

  QString cleaned;
  for (QChar c : str) {
    if (c.isDigit() || c == '.' || c == '-' || c == '+') {
      cleaned.append(c);
    } else if (c == ',') {
      cleaned.append('.');
    }
  }

  if (!cleaned.isEmpty()) {
    value = cleaned.toFloat(&ok);
    if (ok) {
      return value;
    }
  }

  qDebug() << "Failed to convert string to float:" << str;
  return defaultValue;
}

/**
 * @brief Предварительная обработка содержимого файла (удаление комментариев).
 * @param content Исходное содержимое файла.
 * @return Очищенное содержимое.
 */
QString Graphviz::preprocessContent(const QString &content) const {
  QString processed = content;

  // Удаляем однострочные комментарии
  QRegularExpression commentRegex(R"(//[^\n]*\n)");
  processed.replace(commentRegex, "\n");

  // Удаляем многострочные комментарии
  QRegularExpression multiCommentRegex(R"(/\*.*?\*/)");
  multiCommentRegex.setPatternOptions(
      QRegularExpression::DotMatchesEverythingOption);
  processed.replace(multiCommentRegex, "");

  return processed;
}

/**
 * @brief Парсит один узел из регулярного выражения.
 * @param match Регулярное выражение с данными узла.
 * @return tuple (id, x, y) или std::nullopt если парсинг не удался.
 */
std::optional<std::tuple<size_t, double, double>>
Graphviz::parseNodeMatch(const QRegularExpressionMatch &match) const {
  if (match.lastCapturedIndex() < 3) {
    return std::nullopt;
  }

  bool idOk, xOk, yOk;
  size_t id = match.captured(1).toULongLong(&idOk);
  double x = match.captured(2).toDouble(&xOk);
  double y = match.captured(3).toDouble(&yOk);

  if (idOk && xOk && yOk) {
    return std::make_tuple(id, x, y);
  }

  qDebug() << "Failed to parse node: invalid format";
  return std::nullopt;
}

/**
 * @brief Парсит узлы из содержимого.
 * @param content Очищенное содержимое файла.
 * @param nodes Выходной параметр: вектор узлов (id, x, y).
 * @return true если парсинг прошёл успешно, false в противном случае.
 */
bool Graphviz::parseNodes(
    const QString &content,
    std::vector<std::tuple<size_t, double, double>> &nodes) const {

  QRegularExpression nodeRegex(
      R"(n(\d+)\s*\[[^\]]*pos=\"([^,]+),([^!]+)!\"[^\]]*\])");
  QRegularExpressionMatchIterator nodeIt = nodeRegex.globalMatch(content);

  int parsedCount = 0;
  while (nodeIt.hasNext()) {
    auto nodeOpt = parseNodeMatch(nodeIt.next());
    if (nodeOpt.has_value()) {
      nodes.push_back(nodeOpt.value());
      parsedCount++;
    }
  }

  qDebug() << "Parsed" << parsedCount << "nodes";
  return !nodes.empty();
}

/**
 * @brief Парсит одно ребро из регулярного выражения.
 * @param match Регулярное выражение с данными ребра.
 * @return tuple (from, to, weight) или std::nullopt если парсинг не удался.
 */
std::optional<std::tuple<size_t, size_t, float>>
Graphviz::parseEdgeMatch(const QRegularExpressionMatch &match) const {
  if (match.lastCapturedIndex() < 2) {
    return std::nullopt;
  }

  bool fromOk, toOk;
  size_t from = match.captured(1).toULongLong(&fromOk);
  size_t to = match.captured(2).toULongLong(&toOk);

  if (!fromOk || !toOk) {
    qDebug() << "Failed to parse edge: invalid node IDs";
    return std::nullopt;
  }

  float weight = 1.0f;
  if (match.lastCapturedIndex() >= 3 && match.captured(3).length() > 0) {
    weight = safeStringToFloat(match.captured(3), 1.0f);
  }

  return std::make_tuple(from, to, weight);
}

/**
 * @brief Парсит рёбра из содержимого.
 * @param content Очищенное содержимое файла.
 * @param edges Выходной параметр: вектор рёбер (from, to, weight).
 * @return true если парсинг прошёл успешно, false в противном случае.
 */
bool Graphviz::parseEdges(
    const QString &content,
    std::vector<std::tuple<size_t, size_t, float>> &edges) const {

  QRegularExpression edgeRegex(
      R"(n(\d+)\s*->\s*n(\d+)\s*(?:\[label=\"([^\"]+)\"\])?;)");
  QRegularExpressionMatchIterator edgeIt = edgeRegex.globalMatch(content);

  int parsedCount = 0;
  while (edgeIt.hasNext()) {
    auto edgeOpt = parseEdgeMatch(edgeIt.next());
    if (edgeOpt.has_value()) {
      edges.push_back(edgeOpt.value());
      parsedCount++;
    }
  }

  qDebug() << "Parsed" << parsedCount << "edges";
  return true;
}

/**
 * @brief Загружает граф из файла формата .gphz.
 * @param filename Путь к файлу для загрузки.
 * @param nodes Выходной параметр: вектор узлов (id, x, y).
 * @param edges Выходной параметр: вектор рёбер (from, to, weight).
 * @return true если загрузка прошла успешно, false в противном случае.
 */
bool Graphviz::loadFromFile(
    const QString &filename,
    std::vector<std::tuple<size_t, double, double>> &nodes,
    std::vector<std::tuple<size_t, size_t, float>> &edges) {

  // Открытие файла
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qDebug() << "Failed to open file for reading:" << filename;
    return false;
  }

  // Чтение и предварительная обработка
  QTextStream stream(&file);
  QString content = stream.readAll();
  file.close();

  QString processed = preprocessContent(content);

  // Парсинг узлов и рёбер
  nodes.clear();
  edges.clear();

  bool nodesOk = parseNodes(processed, nodes);
  parseEdges(processed, edges);

  if (!nodesOk) {
    qDebug() << "No nodes found in file:" << filename;
    return false;
  }

  qDebug() << "Graph loaded successfully from:" << filename;
  qDebug() << "Total nodes:" << nodes.size() << "edges:" << edges.size();
  return true;
}
