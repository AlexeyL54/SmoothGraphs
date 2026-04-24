#pragma once

#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneHoverEvent>
#include <QList>
#include <QPainter>
#include <QPen>
#include <QPointF>
#include <utility>

class Figure;

/**
 * @class Edge
 * @brief Класс, представляющий ребро графа между двумя узлами.
 *
 * Наследуется от QGraphicsLineItem и добавляет:
 * - Отрисовку стрелки на конце ребра
 * - Контекстное меню для удаления
 * - Автоматическое обновление позиции при перемещении узлов
 * - Управление связями с узлами (входящие/исходящие)
 */
class Edge : public QGraphicsLineItem {
public:
  /**
   * @brief Конструктор ребра.
   * @param start Указатель на начальный узел.
   * @param end Указатель на конечный узел.
   * @param parent Родительский элемент (по умолчанию nullptr).
   */
  Edge(Figure *start, Figure *end, QGraphicsItem *parent = nullptr);

  /**
   * @brief Деструктор ребра.
   *
   * Автоматически удаляет ссылки на данное ребро из связанных узлов.
   */
  ~Edge();

  /**
   * @brief Обновляет геометрическую позицию ребра.
   *
   * Вычисляет новые координаты линии на основе центров начального
   * и конечного узлов. Поддерживает отрисовку петель (ребро в себя).
   */
  void updatePosition();

  /**
   * @brief Возвращает начальный узел ребра.
   * @return Указатель на начальный узел (Figure*).
   */
  Figure *getStartNode() const { return startNode_; }

  /**
   * @brief Возвращает конечный узел ребра.
   * @return Указатель на конечный узел (Figure*).
   */
  Figure *getEndNode() const { return endNode_; }

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
  Figure *startNode_; // Указатель на начальный узел
  Figure *endNode_;   // Указатель на конечный узел

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

/////////////////////////////////////////////////////////////////////////////

/**
 * @class Figure
 * @brief Класс, представляющий узел графа (визуальный элемент).
 *
 * Наследуется от QGraphicsEllipseItem и предоставляет:
 * - Управление входящими и исходящими рёбрами
 * - Обработку событий наведения и перемещения
 * - Контекстное меню для добавления рёбер и удаления узла
 * - Автоматическое обновление связанных рёбер при перемещении
 */
class Figure : public QGraphicsEllipseItem {
public:
  /**
   * @brief Конструктор узла графа.
   * @param x Координата X левого верхнего угла.
   * @param y Координата Y левого верхнего угла.
   * @param width Ширина фигуры.
   * @param height Высота фигуры.
   * @param parent Родительский элемент (по умолчанию nullptr).
   */
  Figure(qreal x, qreal y, qreal width, qreal height,
         QGraphicsItem *parent = nullptr);

  /**
   * @brief Добавляет входящее ребро к узлу.
   * @param edge Указатель на добавляемое ребро.
   *
   * Проверяет на дубликаты перед добавлением в список.
   */
  void addIncomingEdge(Edge *edge);

  /**
   * @brief Добавляет исходящее ребро от узла.
   * @param edge Указатель на добавляемое ребро.
   *
   * Проверяет на дубликаты перед добавлением в список.
   */
  void addOutgoingEdge(Edge *edge);

  /**
   * @brief Удаляет входящее ребро из списка узла.
   * @param edge Указатель на удаляемое ребро.
   */
  void removeIncomingEdge(Edge *edge);

  /**
   * @brief Удаляет исходящее ребро из списка узла.
   * @param edge Указатель на удаляемое ребро.
   */
  void removeOutgoingEdge(Edge *edge);

  /**
   * @brief Возвращает центр фигуры в координатах сцены.
   * @return QPointF с координатами центра.
   */
  QPointF getCenter() const { return scenePos() + QPointF(50, 50); }

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
  QList<Edge *> incomingEdges_; // Список входящих рёбер
  QList<Edge *> outgoingEdges_; // Список исходящих рёбер

  /**
   * @brief Запускает процесс создания ребра от текущего узла.
   *
   * Находит родительский GraphView и вызывает его метод startEdgeCreation.
   */
  void addEdge();
};
