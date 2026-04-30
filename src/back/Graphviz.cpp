// Graphviz.cpp
#include "Graphviz.hpp"

#include <QDebug>
#include <regex>
#include <sstream>

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
void Graphviz::saveNodes(std::ofstream &out,
                         const std::vector<SmoothNode *> &nodes) {
  out << "  // Nodes\n";
  out << "  node [margin=0 fontsize=12 width=0.5 shape=circle style=filled]\n";

  for (SmoothNode *node : nodes) {
    if (!node)
      continue;

    QPointF pos = node->scenePos();
    out << "  n" << node->getId() << " [";
    out << "label=\"" << node->getId() << "\" ";
    out << "pos=\"" << pos.x() << "," << pos.y() << "!\" ";
    out << "];\n";
  }
}

/**
 * @brief Сохраняет информацию о рёбрах в DOT формате.
 * @param out Выходной поток.
 * @param edges Вектор рёбер.
 */
void Graphviz::saveEdges(std::ofstream &out,
                         const std::vector<SmoothEdge *> &edges) {
  out << "\n  // Edges\n";
  for (SmoothEdge *edge : edges) {
    if (!edge)
      continue;

    SmoothNode *start = edge->getStartNode();
    SmoothNode *end = edge->getEndNode();

    if (!start || !end)
      continue;

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
bool Graphviz::saveToFile(const std::string &filename,
                          const std::vector<SmoothNode *> &nodes,
                          const std::vector<SmoothEdge *> &edges) {
  std::ofstream out(filename);
  if (!out.is_open()) {
    qDebug() << "Failed to open file for writing:" << filename.c_str();
    return false;
  }

  out << "digraph G {\n";
  out << "  // Graph saved from Graph Editor Application\n";
  out << "  // Format: .gphz (Graphviz with extended attributes)\n";
  out << "  graph [bb=\"0,0,800,600\"];\n\n";

  saveNodes(out, nodes);
  out << "\n";
  saveEdges(out, edges);

  out << "}\n";

  out.close();
  qDebug() << "Graph saved successfully to:" << filename.c_str();
  return true;
}

/**
 * @brief Безопасно преобразует строку в float.
 * @param str Строка для преобразования.
 * @param defaultValue Значение по умолчанию при ошибке.
 * @return Преобразованное значение или defaultValue.
 */
static float safeStringToFloat(const std::string &str,
                               float defaultValue = 1.0f) {
  if (str.empty()) {
    return defaultValue;
  }

  try {
    size_t pos;
    float value = std::stof(str, &pos);
    if (pos == str.length()) {
      return value;
    }
    // Если не вся строка распарсилась, пробуем другие варианты
    std::string cleaned;
    for (char c : str) {
      if (std::isdigit(c) || c == '.' || c == '-' || c == '+') {
        cleaned += c;
      } else if (c == ',') {
        cleaned += '.';
      }
    }
    if (!cleaned.empty()) {
      return std::stof(cleaned);
    }
  } catch (const std::exception &e) {
    qDebug() << "Failed to convert string to float:" << str.c_str()
             << "error:" << e.what();
  }

  return defaultValue;
}

/**
 * @brief Загружает граф из файла формата .gphz.
 * @param filename Путь к файлу для загрузки.
 * @param nodes Выходной параметр: вектор узлов (id, x, y).
 * @param edges Выходной параметр: вектор рёбер (from, to, weight).
 * @return true если загрузка прошла успешно, false в противном случае.
 */
bool Graphviz::loadFromFile(
    const std::string &filename,
    std::vector<std::tuple<size_t, double, double>> &nodes,
    std::vector<std::tuple<size_t, size_t, float>> &edges) {
  std::ifstream in(filename);
  if (!in.is_open()) {
    qDebug() << "Failed to open file for reading:" << filename.c_str();
    return false;
  }

  // Читаем весь файл в строку для удобства парсинга
  std::stringstream buffer;
  buffer << in.rdbuf();
  std::string content = buffer.str();
  in.close();

  // Удаляем комментарии
  std::regex commentRegex(R"(//[^\n]*\n)");
  content = std::regex_replace(content, commentRegex, "\n");

  // Удаляем многострочные комментарии
  std::regex multiCommentRegex(R"(/\*.*?\*/)");
  content = std::regex_replace(content, multiCommentRegex, "");

  // Парсинг узлов: n123 [label="123" pos="x,y!"];
  std::regex nodeRegex(R"(n(\d+)\s*\[[^\]]*pos=\"([^,]+),([^!]+)!\"[^\]]*\])");
  std::smatch match;
  std::string::const_iterator searchStart(content.cbegin());

  while (std::regex_search(searchStart, content.cend(), match, nodeRegex)) {
    if (match.size() >= 4) {
      try {
        size_t id = std::stoul(match[1].str());
        double x = std::stod(match[2].str());
        double y = std::stod(match[3].str());
        nodes.push_back(std::make_tuple(id, x, y));
        qDebug() << "Loaded node:" << id << "at (" << x << "," << y << ")";
      } catch (const std::exception &e) {
        qDebug() << "Failed to parse node:" << e.what();
      }
    }
    searchStart = match.suffix().first;
  }

  // Парсинг рёбер: n1 -> n2; или n1 -> n2 [label="weight"];
  // Более гибкое регулярное выражение
  std::regex edgeRegex(
      R"(n(\d+)\s*->\s*n(\d+)\s*(?:\[label=\"([^\"]+)\"\])?;)");
  searchStart = content.cbegin();

  while (std::regex_search(searchStart, content.cend(), match, edgeRegex)) {
    if (match.size() >= 3) {
      try {
        size_t from = std::stoul(match[1].str());
        size_t to = std::stoul(match[2].str());
        float weight = 1.0f;

        if (match.size() > 3 && match[3].matched) {
          weight = safeStringToFloat(match[3].str(), 1.0f);
        }

        edges.push_back(std::make_tuple(from, to, weight));
        qDebug() << "Loaded edge:" << from << "->" << to << "weight:" << weight;
      } catch (const std::exception &e) {
        qDebug() << "Failed to parse edge:" << e.what();
      }
    }
    searchStart = match.suffix().first;
  }

  if (nodes.empty()) {
    qDebug() << "No nodes found in file:" << filename.c_str();
    return false;
  }

  qDebug() << "Graph loaded successfully from:" << filename.c_str();
  qDebug() << "Total nodes:" << nodes.size() << "edges:" << edges.size();
  return true;
}
