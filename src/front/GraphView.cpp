#include "GraphView.hpp"

GraphView::GraphView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent) {}

void GraphView::contextMenuEvent(QContextMenuEvent *event) {
  // Проверяем, кликнули ли мы по какому-то элементу.
  // Если itemUnderMouse() возвращает nullptr, значит клик по фону.
  if (itemAt(event->pos()) == nullptr) {
    QMenu menu;
    menu.addAction("Добавить узел");
    menu.addAction("Очистить все");
    menu.exec(event->globalPos());
  } else {
    // Если клик по элементу, передаем событие дальше (элемент сам покажет свое
    // меню)
    QGraphicsView::contextMenuEvent(event);
  }
}
