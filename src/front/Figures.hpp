#pragma once

#include "qline.h"
#include "qpainter.h"
#include "qpoint.h"
#include <QApplication>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMenu>
#include <QMouseEvent>
#include <utility>

class Figure;

class Edge : public QGraphicsLineItem {
public:
  Edge(Figure *start, Figure *end, QGraphicsItem *parent = nullptr);
  ~Edge();

  void updatePosition();
  Figure *getStartNode() const { return startNode_; }
  Figure *getEndNode() const { return endNode_; }

protected:
  void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *widget) override;

private:
  Figure *startNode_;
  Figure *endNode_;

  std::pair<QPointF, QPointF> computeArrowPos(QLineF &line);
  void paintArrow(QPainter *painter, QLineF &line, QPointF p1, QPointF p2);
};

/////////////////////////////////////////////////////////////////////////////

class Figure : public QGraphicsEllipseItem {
public:
  Figure(qreal x, qreal y, qreal width, qreal height,
         QGraphicsItem *parent = nullptr)
      : QGraphicsEllipseItem(x, y, width, height, parent) {
    setRect(0, 0, width, height); // Важно: установить локальные координаты
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
  }

  void addIncomingEdge(Edge *edge);
  void addOutgoingEdge(Edge *edge);
  void removeIncomingEdge(Edge *edge);
  void removeOutgoingEdge(Edge *edge);

  QPointF getCenter() const { return scenePos() + QPointF(50, 50); }

protected:
  void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
  void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
  QVariant itemChange(GraphicsItemChange change,
                      const QVariant &value) override;

private:
  QList<Edge *> incomingEdges_;
  QList<Edge *> outgoingEdges_;

  void addEdge();
};
