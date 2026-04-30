#include "Figures.hpp"
#include "GraphView.hpp"
#include "ThemeManager.hpp"
#include "qlist.h"

#include <QApplication>
#include <QDebug>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsView>
#include <QMenu>
#include <QMouseEvent>
#include <QObject>
#include <QPainterPath>
#include <cmath>
#include <utility>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * @brief Конструктор ребра.
 * @param start Указатель на начальный узел.
 * @param end Указатель на конечный узел.
 * @param parent Родительский элемент (по умолчанию nullptr).
 */

SmoothEdge::SmoothEdge(SmoothNode *start, SmoothNode *end,
                       QGraphicsItem *parent)
    : QGraphicsLineItem(parent), startNode_(start), endNode_(end) {
  setFlag(QGraphicsItem::ItemIsSelectable);
  // setPen(QPen(Qt::black, 2));
  updatePosition();
  qDebug() << "Edge created between" << start << "and" << end;
}

/**
 * @brief Деструктор ребра.
 *
 * Автоматически удаляет ссылки на данное ребро из связанных узлов.
 */
SmoothEdge::~SmoothEdge() {
  qDebug() << "Edge destroyed";
  if (endNode_)
    endNode_->removeIncomingEdge(this);
  if (startNode_)
    startNode_->removeOutgoingEdge(this);
}

/**
 * @brief Обновляет геометрическую позицию ребра.
 *
 * Вычисляет новые координаты линии на основе центров начального
 * и конечного узлов. Поддерживает отрисовку петель (ребро в себя).
 */
void SmoothEdge::updatePosition() {
  if (!startNode_ || !endNode_) {
    qDebug() << "updatePosition: null nodes";
    return;
  }

  if (!startNode_->scene() || !endNode_->scene()) {
    qDebug() << "Nodes doesn't exist in scene";
    return;
  }

  QPointF startCenter = startNode_->getCenter();
  QPointF endCenter = endNode_->getCenter();

  if (startNode_ == endNode_) {
    // Петля
    setLine(QLineF(startCenter.x() + 40, startCenter.y(), startCenter.x() + 80,
                   startCenter.y() - 40));
  } else {
    // Обычное ребро
    setLine(QLineF(startCenter, endCenter));
  }

  qDebug() << "Edge updated:" << line();
}

void SmoothEdge::updateThemeStyle(const ThemeColors &colors) {
  setPen(QPen(colors.edgeDefault, 2));
  update(); // Перерисовать
}

/**
 * @brief Вычисляет координаты точек для отрисовки стрелки.
 * @param line Ссылка на линию ребра.
 * @return Пара точек (QPointF), определяющих вершины стрелки.
 */
std::pair<QPointF, QPointF> SmoothEdge::computeArrowPos(QLineF &line) {
  double angle = std::atan2(-line.dy(), line.dx());
  QPointF endPt = line.p2();
  qreal arrowSize = 20.0;

  QPointF arrowP1 = endPt - QPointF(std::sin(angle + M_PI / 3) * arrowSize,
                                    std::cos(angle + M_PI / 3) * arrowSize);
  QPointF arrowP2 =
      endPt - QPointF(std::sin(angle + M_PI - M_PI / 3) * arrowSize,
                      std::cos(angle + M_PI - M_PI / 3) * arrowSize);

  return std::make_pair(arrowP1, arrowP2);
}

/**
 * @brief Отрисовывает треугольную стрелку на конце ребра.
 * @param painter Указатель на QPainter.
 * @param line Ссылка на линию ребра.
 * @param p1 Первая вершина стрелки.
 * @param p2 Вторая вершина стрелки.
 */
void SmoothEdge::paintArrow(QPainter *painter, QLineF &line, QPointF p1,
                            QPointF p2) {
  painter->setPen(Qt::NoPen);
  painter->setBrush(pen().color());

  QPainterPath arrowPath;
  arrowPath.moveTo(line.p2());
  arrowPath.lineTo(p1);
  arrowPath.lineTo(p2);
  arrowPath.closeSubpath();

  painter->drawPath(arrowPath);
}

/**
 * @brief Переопределённый метод отрисовки.
 * @param painter Указатель на QPainter для отрисовки.
 * @param option Опции стиля отрисовки.
 * @param widget Указатель на виджет (не используется).
 *
 * Отрисовывает линию ребра и стрелку на конце.
 */
void SmoothEdge::paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) {
  Q_UNUSED(option);
  Q_UNUSED(widget);

  QLineF line = this->line();

  if (line.length() < 1e-5)
    return;

  painter->setPen(pen());
  painter->drawLine(line);

  std::pair<QPointF, QPointF> arrowP = computeArrowPos(line);
  paintArrow(painter, line, arrowP.first, arrowP.second);
}

/**
 * @brief Обработчик контекстного меню ребра.
 * @param event Указатель на событие контекстного меню.
 *
 * Отображает меню с опцией "Удалить ребро".
 */
void SmoothEdge::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
  QMenu menu;
  QAction *deleteAction = menu.addAction("Удалить ребро");
  QAction *weightAction = menu.addAction("Изменить вес");

  QObject::connect(deleteAction, &QAction::triggered, [this]() {
    if (scene())
      scene()->removeItem(this);
  });

  menu.exec(event->screenPos());
  event->accept();
}

//////////////////////////////////////////////////////////////////////////

/**
 * @brief Конструктор узла графа.
 * @param x Координата X левого верхнего угла.
 * @param y Координата Y левого верхнего угла.
 * @param width Ширина фигуры.
 * @param height Высота фигуры.
 * @param parent Родительский элемент (по умолчанию nullptr).
 */
SmoothNode::SmoothNode(qreal x, qreal y, qreal width, qreal height,
                       QGraphicsItem *parent)
    : QGraphicsEllipseItem(x, y, width, height, parent) {
  setRect(x, y, width, height);
  setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
  /*defaultColor_ = Qt::red;
  hoverColor_ = Qt::yellow; */
  setBrush(defaultColor_);
}

/**
 * @brief Добавляет входящее ребро к узлу.
 * @param edge Указатель на добавляемое ребро.
 *
 * Проверяет на дубликаты перед добавлением в список.
 */
void SmoothNode::addIncomingEdge(SmoothEdge *edge) {
  if (!incomingEdges_.contains(edge)) {
    incomingEdges_.append(edge);
    qDebug() << "Added incoming edge to" << this
             << "total:" << incomingEdges_.size();
  }
}

/**
 * @brief Добавляет исходящее ребро от узла.
 * @param edge Указатель на добавляемое ребро.
 *
 * Проверяет на дубликаты перед добавлением в список.
 */
void SmoothNode::addOutgoingEdge(SmoothEdge *edge) {
  if (!outgoingEdges_.contains(edge)) {
    outgoingEdges_.append(edge);
    qDebug() << "Added outgoing edge from" << this
             << "total:" << outgoingEdges_.size();
  }
}

/**
 * @brief Удаляет входящее ребро из списка узла.
 * @param edge Указатель на удаляемое ребро.
 */
void SmoothNode::removeIncomingEdge(SmoothEdge *edge) {
  incomingEdges_.removeOne(edge);
  qDebug() << "Removed incoming edge from" << this;
}

/**
 * @brief Удаляет исходящее ребро из списка узла.
 * @param edge Указатель на удаляемое ребро.
 */
void SmoothNode::removeOutgoingEdge(SmoothEdge *edge) {
  outgoingEdges_.removeOne(edge);
  qDebug() << "Removed outgoing edge from" << this;
}

/**
 * @brief Очищает список входящих ребер
 */
void SmoothNode::clearIncomingEdges() {
  incomingEdges_.clear();
  qDebug() << "Removed all incoming edges" << this;
}

/**
 * @brief Очищает список исходящих ребер
 */
void SmoothNode::clearOutcomingEdges() {
  outgoingEdges_.clear();
  qDebug() << "Removed all outcoming edges" << this;
}

/**
 * @brief Обработчик события наведения курсора на узел.
 * @param event Указатель на событие наведения.
 *
 * Изменяет цвет заливки на жёлтый для визуальной обратной связи.
 */
void SmoothNode::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
  setBrush(hoverColor_);
  QGraphicsEllipseItem::hoverEnterEvent(event);
}

/**
 * @brief Обработчик события ухода курсора с узла.
 * @param event Указатель на событие наведения.
 *
 * Возвращает исходный цвет заливки (красный).
 */
void SmoothNode::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
  setBrush(defaultColor_);
  QGraphicsEllipseItem::hoverLeaveEvent(event);
}

/**
 * @brief Запускает процесс создания ребра от текущего узла.
 *
 * Находит родительский GraphView и вызывает его метод startEdgeCreation.
 */
void SmoothNode::addEdge() {
  if (scene()) {
    for (QGraphicsView *view : scene()->views()) {
      GraphView *gv = qobject_cast<GraphView *>(view);
      if (gv) {
        qDebug() << "Starting edge creation from" << this;
        gv->startEdgeCreation(this);
        break;
      }
    }
  }
}

/**
 * @brief Обработчик контекстного меню узла.
 * @param event Указатель на событие контекстного меню.
 *
 * Отображает меню с опциями:
 * - "Добавить грань" — начинает создание ребра от этого узла
 * - "Удалить узел" — удаляет узел и все связанные рёбра
 */
void SmoothNode::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
  QMenu menu;
  QAction *addEdgeAction = menu.addAction("Добавить грань");
  QAction *deleteNodeAction = menu.addAction("Удалить узел");

  QObject::connect(addEdgeAction, &QAction::triggered,
                   [this]() { this->addEdge(); });
  QObject::connect(deleteNodeAction, &QAction::triggered, [this]() {
    // Удаляем все связанные рёбра
    for (SmoothEdge *edge : incomingEdges_) {
      if (scene())
        scene()->removeItem(edge);
    }
    for (SmoothEdge *edge : outgoingEdges_) {
      if (scene())
        scene()->removeItem(edge);
    }
    if (scene())
      scene()->removeItem(this);
  });

  menu.exec(event->screenPos());
  event->accept();
}

/**
 * @brief Обработчик изменений свойств элемента.
 * @param change Тип изменяемого свойства.
 * @param value Новое значение свойства.
 * @return Возвращаемое значение для базового класса.
 *
 * При изменении позиции автоматически обновляет все связанные рёбра.
 */
QVariant SmoothNode::itemChange(GraphicsItemChange change,
                                const QVariant &value) {
  if (change == ItemPositionHasChanged) {
    qDebug() << "SmoothNode" << this << "moved to" << value.toPointF();
    // Обновляем все связанные рёбра
    for (SmoothEdge *edge : incomingEdges_) {
      qDebug() << "Updating incoming edge";
      edge->updatePosition();
    }
    for (SmoothEdge *edge : outgoingEdges_) {
      qDebug() << "Updating outgoing edge";
      edge->updatePosition();
    }
  }
  return QGraphicsEllipseItem::itemChange(change, value);
}

void SmoothNode::setHoverColor(const QColor &color) { hoverColor_ = color; }

void SmoothNode::restoreDefaultColor() { setBrush(defaultColor_); }

void SmoothNode::updateThemeStyle(const ThemeColors &colors) {
  defaultColor_ = colors.nodeDefault;
  hoverColor_ = colors.nodeHover;
  borderColor_ = colors.border;

  setBrush(defaultColor_);
  setPen(QPen(borderColor_, 1));
  update(); // Перерисовать
}

QList<SmoothEdge *> SmoothNode::getIncomingEdges() { return incomingEdges_; }

QList<SmoothEdge *> SmoothNode::getOutcomingEdges() { return incomingEdges_; }
