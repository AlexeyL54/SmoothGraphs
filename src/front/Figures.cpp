#include "Figures.hpp"
#include "GraphView.hpp"
#include "qobject.h"
#include "qpoint.h"

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

// ======================== Edge Implementation ========================

Edge::Edge(Figure *start, Figure *end, QGraphicsItem *parent)
    : QGraphicsLineItem(parent), startNode_(start), endNode_(end) {
  setFlag(QGraphicsItem::ItemIsSelectable);
  setPen(QPen(Qt::black, 2));
  updatePosition();
  qDebug() << "Edge created between" << start << "and" << end;
}

Edge::~Edge() {
  qDebug() << "Edge destroyed";
  if (endNode_)
    endNode_->removeIncomingEdge(this);
  if (startNode_)
    startNode_->removeOutgoingEdge(this);
}

void Edge::updatePosition() {
  if (!startNode_ || !endNode_) {
    qDebug() << "updatePosition: null nodes";
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

  std::pair<QPointF, QPointF> arrowP = computeArrowPos(line);
  paintArrow(painter, line, arrowP.first, arrowP.second);
}

void Edge::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
  QMenu menu;
  QAction *deleteAction = menu.addAction("Удалить ребро");

  QObject::connect(deleteAction, &QAction::triggered, [this]() {
    if (scene())
      scene()->removeItem(this);
  });

  menu.exec(event->screenPos());
  event->accept();
}

// ======================== Figure Implementation ========================

void Figure::addIncomingEdge(Edge *edge) {
  if (!incomingEdges_.contains(edge)) {
    incomingEdges_.append(edge);
    qDebug() << "Added incoming edge to" << this
             << "total:" << incomingEdges_.size();
  }
}

void Figure::addOutgoingEdge(Edge *edge) {
  if (!outgoingEdges_.contains(edge)) {
    outgoingEdges_.append(edge);
    qDebug() << "Added outgoing edge from" << this
             << "total:" << outgoingEdges_.size();
  }
}

void Figure::removeIncomingEdge(Edge *edge) {
  incomingEdges_.removeOne(edge);
  qDebug() << "Removed incoming edge from" << this;
}

void Figure::removeOutgoingEdge(Edge *edge) {
  outgoingEdges_.removeOne(edge);
  qDebug() << "Removed outgoing edge from" << this;
}

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
      GraphView *gv = qobject_cast<GraphView *>(view);
      if (gv) {
        qDebug() << "Starting edge creation from" << this;
        gv->startEdgeCreation(this);
        break;
      }
    }
  }
}

void Figure::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
  QMenu menu;
  QAction *addEdgeAction = menu.addAction("Добавить грань");
  QAction *deleteNodeAction = menu.addAction("Удалить узел");

  QObject::connect(addEdgeAction, &QAction::triggered,
                   [this]() { this->addEdge(); });
  QObject::connect(deleteNodeAction, &QAction::triggered, [this]() {
    // Удаляем все связанные рёбра
    for (Edge *edge : incomingEdges_) {
      if (scene())
        scene()->removeItem(edge);
    }
    for (Edge *edge : outgoingEdges_) {
      if (scene())
        scene()->removeItem(edge);
    }
    if (scene())
      scene()->removeItem(this);
  });

  menu.exec(event->screenPos());
  event->accept();
}

QVariant Figure::itemChange(GraphicsItemChange change, const QVariant &value) {
  if (change == ItemPositionHasChanged) {
    qDebug() << "Figure" << this << "moved to" << value.toPointF();
    // Обновляем все связанные рёбра
    for (Edge *edge : incomingEdges_) {
      qDebug() << "Updating incoming edge";
      edge->updatePosition();
    }
    for (Edge *edge : outgoingEdges_) {
      qDebug() << "Updating outgoing edge";
      edge->updatePosition();
    }
  }
  return QGraphicsEllipseItem::itemChange(change, value);
}
