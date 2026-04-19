#include "GraphView.hpp"
#include "Figures.hpp"

GraphView::GraphView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent), scene_(scene) {}

void GraphView::contextMenuEvent(QContextMenuEvent *event) {
  // Проверяем, кликнули ли мы по какому-то элементу.
  // Если itemUnderMouse() возвращает nullptr, значит клик по фону.
  if (itemAt(event->pos()) == nullptr) {
    QMenu menu;
    QAction *addFigAction = menu.addAction("Добавить узел");
    QAction *clearAction = menu.addAction("Очистить все");

    connect(addFigAction, &QAction::triggered, this, &GraphView::addFigure);
    connect(clearAction, &QAction::triggered, this, &GraphView::clear);

    menu.exec(event->globalPos());
  } else {
    // Если клик по элементу, передаем событие дальше (элемент сам покажет свое
    // меню)
    QGraphicsView::contextMenuEvent(event);
  }
}

void GraphView::addFigure() {
  Figure *fig = new Figure(0, 0, 100, 100);
  fig->setBrush(Qt::red);
  fig->setFlag(QGraphicsItem::ItemIsMovable);

  if (scene_)
    scene_->addItem(fig);
}

void GraphView::clear() {
  if (scene_)
    scene_->clear();
}
