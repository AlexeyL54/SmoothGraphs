#pragma once

#include "qgraphicsscene.h"
#include "qtmetamacros.h"
#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMenu>
#include <QMouseEvent>

class GraphView : public QGraphicsView {
public:
  explicit GraphView(QGraphicsScene *scene, QWidget *parent = nullptr);

protected:
  void contextMenuEvent(QContextMenuEvent *event) override;

private:
  QGraphicsScene *scene_;

private slots:
  void addFigure();
  void clear();
};
