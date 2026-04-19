#include "Figures.hpp"
#include <QApplication>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsView>
#include <QMenu>
#include <QMouseEvent>

void Figure::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
  QMenu menu;
  menu.addAction("Удалить");
  menu.addAction("Изменить цвет");

  menu.exec(event->screenPos());

  event->accept();
}
