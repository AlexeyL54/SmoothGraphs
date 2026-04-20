#pragma once

#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMenu>
#include <QMouseEvent>

#include "Figures.hpp"
#include "qtmetamacros.h"

class GraphView : public QGraphicsView {
  Q_OBJECT
public:
  explicit GraphView(QGraphicsScene *scene, QWidget *parent = nullptr);

  void startEdgeCreation(Figure *startNode);

protected:
  void contextMenuEvent(QContextMenuEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;

private:
  QGraphicsScene *scene_;
  bool isCreatingEdge_ = false;
  Figure *startNode_ = nullptr;
  Edge *tempEdge_ = nullptr;

private slots:
  void addFigure();
  void clear();
};
