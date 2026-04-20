#include "Figures.hpp"
#include "GraphView.hpp"

#include <QApplication>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsView>
#include <QMenu>
#include <QMouseEvent>
#include <QObject>
#include <utility>

Edge::Edge(Figure *start, Figure *end, QGraphicsItem *parent)
    : QGraphicsLineItem(parent), startNode_(start), endNode_(end) {
  setParentItem(start);
  setFlag(QGraphicsItem::ItemIsSelectable);
  setPen(QPen(Qt::black, 2));
  updateMicroFocus();
}

void Edge::updateEndPosition() {
  if (!startNode_ or !endNode_)
    return;

  // если петля
  if (startNode_ == endNode_) {
    setLine(QLineF(0, 0, 40, -40));

    // иначе ребро соединяет два разных узла
  } else {
    QPointF endLocalPos =
        endNode_->mapToItem(startNode_, endNode_->rect().center());
    setLine(QLineF(0, 0, endLocalPos.x(), endLocalPos.y()));
  }
}

std::pair<QPointF, QPointF> Edge::computeArrowPos(QLineF &line) {
  double angle = std::atan2(-line.dy(), line.dx());
  QPointF endPt = line.p2();
  qreal arrowSize = 10.0;

  QPointF arrowP1 = endPt + QPointF(std::sin(angle + M_PI / 3) * arrowSize,
                                    std::cos(angle + M_PI / 3) * arrowSize);
  QPointF arrowP2 =
      endPt + QPointF(std::sin(angle + M_PI - M_PI / 3) * arrowSize,
                      std::cos(angle + M_PI - M_PI / 3) * arrowSize);

  return std::make_pair(arrowP1, arrowP2);
}

void Edge::paintArrow(QPainter *painter, QLineF &line, QPointF p1, QPointF p2) {
  painter->setPen(Qt::NoPen);
  painter->setBrush(pen().color());

  QPainterPath arrowPath;
  arrowPath.moveTo(line.p2());
  arrowPath.lineTo(p1);
  arrowPath.lineTo(p2);
  arrowPath.closeSubpath();

  painter->drawPath(arrowPath);
}

void Edge::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                 QWidget *widget) {
  Q_UNUSED(option);
  Q_UNUSED(widget);

  QLineF line = this->line();

  if (line.length() < 1e-5)
    return;

  painter->setPen(pen());
  painter->drawLine(line);
}

void Edge::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
  QMenu menu;
  menu.addAction("Удалить");
  menu.addAction("Настроить");

  menu.exec(event->screenPos());

  event->accept();
}

Edge::~Edge() {
  if (endNode_)
    endNode_->removeIncomingEdge(this);
}

///////////////////////////////////////////////////////////////////////////////////////

void Figure::addIncomingEdge(Edge *edge) {
  if (!incomingEdges_.contains(edge))
    incomingEdges_.append(edge);
}

void Figure::removeIncomingEdge(Edge *edge) { incomingEdges_.removeOne(edge); }

void Figure::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
  setBrush(Qt::yellow);
  QGraphicsEllipseItem::hoverEnterEvent(event);
}

void Figure::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
  setBrush(Qt::red);
  QGraphicsEllipseItem::hoverLeaveEvent(event);
}

void Figure::addEdge() {
  if (scene()) {
    for (QGraphicsView *view : scene()->views()) {
      if (GraphView *gv = qobject_cast<GraphView *>(view)) {
        gv->startEdgeCreation(this);
        break;
      }
    }
  }
}

void Figure::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
  QMenu menu;
  QAction *addEdgeAction = menu.addAction("Добавить грань");

  QObject::connect(addEdgeAction, &QAction::triggered,
                   [this]() { this->addEdge(); });

  menu.exec(event->screenPos());

  event->accept();
}

QVariant Figure::itemChange(GraphicsItemChange change, const QVariant &value) {
  if (change == ItemPositionHasChanged) {
    for (Edge *edge : incomingEdges_) {
      edge->updateEndPosition();
    }
  }
  return QGraphicsEllipseItem::itemChange(change, value);
}
