#pragma once

#include "qgraphicsitem.h"
#include <QApplication>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMenu>
#include <QMouseEvent>

class Figure : public QGraphicsEllipseItem {
public:
  using QGraphicsEllipseItem::QGraphicsEllipseItem;

protected:
  void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
};

class Edge {};
