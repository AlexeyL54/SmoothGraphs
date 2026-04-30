#pragma once

#include <QColor>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsScale>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneHoverEvent>
#include <QList>
#include <QMenu>
#include <QPainter>
#include <QPen>
#include <QPointF>
#include <cstddef>

class SmoothNode;

/**
 * @class SmoothEdge
 * @brief Класс, представляющий ребро графа между двумя узлами.
 *
 * Наследуется от QGraphicsLineItem и добавляет:
 * - Отрисовку стрелки на конце ребра
 * - Контекстное меню для удаления
 * - Автоматическое обновление позиции при перемещении узлов
 * - Управление связями с узлами (входящие/исходящие)
 */
class SmoothEdge : public QGraphicsLineItem {
public:
  /**
   * @brief Конструктор ребра.
   * @param start Указатель на начальный узел.
   * @param end Указатель на конечный узел.
   * @param parent Родительский элемент (по умолчанию nullptr).
   */
  SmoothEdge(SmoothNode *start, SmoothNode *end,
             QGraphicsItem *parent = nullptr);

  /**
   * @brief Деструктор ребра.
   *
   * Автоматически удаляет ссылки на данное ребро из связанных узлов.
   */
  ~SmoothEdge();

  /**
   * @brief Обновляет геометрическую позицию ребра.
   *
   * Вычисляет новые координаты линии на основе центров начального
   * и конечного узлов. Поддерживает отрисовку петель (ребро в себя).
   */
  void updatePosition();

  /**
   * @brief Возвращает начальный узел ребра.
   * @return Указатель на начальный узел (SmoothNode*).
   */
  SmoothNode *getStartNode() const { return startNode_; }

  /**
   * @brief Возвращает конечный узел ребра.
   * @return Указатель на конечный узел (SmoothNode*).
   */
  SmoothNode *getEndNode() const { return endNode_; }

  void setWeight(float weight) { weight_ = weight; }

  float getWeight() const { return weight_; }

  void updateThemeStyle(const class ThemeColors &colors);

protected:
  /**
   * @brief Обработчик контекстного меню ребра.
   * @param event Указатель на событие контекстного меню.
   *
   * Отображает меню с опцией "Удалить ребро".
   */
  void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

  /**
   * @brief Переопределённый метод отрисовки.
   * @param painter Указатель на QPainter для отрисовки.
   * @param option Опции стиля отрисовки.
   * @param widget Указатель на виджет (не используется).
   *
   * Отрисовывает линию ребра и стрелку на конце.
   */
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *widget) override;

private:
  SmoothNode *startNode_; // Указатель на начальный узел
  SmoothNode *endNode_;   // Указатель на конечный узел
  float weight_;

  QColor borderColor_;
  QColor defaultColor_;
  QColor hoverColor_;

  /**
   * @brief Вычисляет координаты точек для отрисовки стрелки.
   * @param line Ссылка на линию ребра.
   * @return Пара точек (QPointF), определяющих вершины стрелки.
   */
  std::pair<QPointF, QPointF> computeArrowPos(QLineF &line);

  /**
   * @brief Отрисовывает треугольную стрелку на конце ребра.
   * @param painter Указатель на QPainter.
   * @param line Ссылка на линию ребра.
   * @param p1 Первая вершина стрелки.
   * @param p2 Вторая вершина стрелки.
   */
  void paintArrow(QPainter *painter, QLineF &line, QPointF p1, QPointF p2);
};

///////////////////////////////////////////////////////////////////////////

/**
 * @class SmoothNode
 * @brief Класс, представляющий узел графа (визуальный элемент).
 *
 * Наследуется от QGraphicsEllipseItem и предоставляет:
 * - Управление входящими и исходящими рёбрами
 * - Обработку событий наведения и перемещения
 * - Контекстное меню для добавления рёбер и удаления узла
 * - Автоматическое обновление связанных рёбер при перемещении
 */
class SmoothNode : public QGraphicsEllipseItem {
public:
  /**
   * @brief Конструктор узла графа.
   * @param x Координата X левого верхнего угла.
   * @param y Координата Y левого верхнего угла.
   * @param width Ширина фигуры.
   * @param height Высота фигуры.
   * @param parent Родительский элемент (по умолчанию nullptr).
   */
  SmoothNode(qreal x, qreal y, qreal width, qreal height,
             QGraphicsItem *parent = nullptr);

  /**
   * @brief Добавляет входящее ребро к узлу.
   * @param edge Указатель на добавляемое ребро.
   *
   * Проверяет на дубликаты перед добавлением в список.
   */
  void addIncomingEdge(SmoothEdge *edge);

  /**
   * @brief Добавляет исходящее ребро от узла.
   * @param edge Указатель на добавляемое ребро.
   *
   * Проверяет на дубликаты перед добавлением в список.
   */
  void addOutgoingEdge(SmoothEdge *edge);

  /**
   * @brief Удаляет входящее ребро из списка узла.
   * @param edge Указатель на удаляемое ребро.
   */
  void removeIncomingEdge(SmoothEdge *edge);

  /**
   * @brief Удаляет исходящее ребро из списка узла.
   * @param edge Указатель на удаляемое ребро.
   */
  void removeOutgoingEdge(SmoothEdge *edge);

  /**
   * @brief Очищает список входящих ребер
   */
  void clearIncomingEdges();

  /**
   * @brief Очищает список исходящих ребер
   */
  void clearOutcomingEdges();

  /**
   * @brief Возвращает центр фигуры в координатах сцены.
   * @return QPointF с координатами центра.
   */
  QPointF getCenter() const { return scenePos() + QPointF(50, 50); }

  void setHoverColor(const QColor &color);
  void updateThemeStyle(const class ThemeColors &colors);
  void restoreDefaultColor();

  QList<SmoothEdge *> getIncomingEdges();
  QList<SmoothEdge *> getOutcomingEdges();

  /**
   * @brief Устанавливает ID узла.
   * @param id Уникальный идентификатор узла.
   */
  void setId(size_t id) { id_ = id; }

  /**
   * @brief Возвращает ID узла.
   * @return ID узла.
   */
  size_t getId() const { return id_; }

protected:
  /**
   * @brief Обработчик контекстного меню узла.
   * @param event Указатель на событие контекстного меню.
   *
   * Отображает меню с опциями:
   * - "Добавить грань" — начинает создание ребра от этого узла
   * - "Удалить узел" — удаляет узел и все связанные рёбра
   */
  void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

  /**
   * @brief Обработчик события наведения курсора на узел.
   * @param event Указатель на событие наведения.
   *
   * Изменяет цвет заливки на жёлтый для визуальной обратной связи.
   */
  void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

  /**
   * @brief Обработчик события ухода курсора с узла.
   * @param event Указатель на событие наведения.
   *
   * Возвращает исходный цвет заливки (красный).
   */
  void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

  /**
   * @brief Обработчик изменений свойств элемента.
   * @param change Тип изменяемого свойства.
   * @param value Новое значение свойства.
   * @return Возвращаемое значение для базового класса.
   *
   * При изменении позиции автоматически обновляет все связанные рёбра.
   */
  QVariant itemChange(GraphicsItemChange change,
                      const QVariant &value) override;

private:
  QList<SmoothEdge *> incomingEdges_; // Список входящих рёбер
  QList<SmoothEdge *> outgoingEdges_; // Список исходящих рёбер

  QColor defaultColor_;
  QColor hoverColor_;
  QColor borderColor_;

  size_t id_;

  /**
   * @brief Запускает процесс создания ребра от текущего узла.
   *
   * Находит родительский GraphView и вызывает его метод startEdgeCreation.
   */
  void addEdge();
};
