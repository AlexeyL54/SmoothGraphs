#pragma once

#include "qline.h"
#include "qpainter.h"
#include "qpoint.h"
#include "qtmetamacros.h"
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
  using QGraphicsLineItem::QGraphicsLineItem;

  explicit Edge(Figure *start, Figure *end, QGraphicsItem *parent = nullptr);
  ~Edge();

  void updateEndPosition();
  Figure *getStartNode() const;
  Figure *getEndNode() const;

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
  using QGraphicsEllipseItem::QGraphicsEllipseItem;

  void addIncomingEdge(Edge *edge);
  void removeIncomingEdge(Edge *edge);

protected:
  void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
  void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
  QVariant itemChange(GraphicsItemChange change,
                      const QVariant &value) override;

private:
  QList<Edge *> incomingEdges_;

  void addEdge();

private slots:
  // void setColor();
  // void setWeight();
};
