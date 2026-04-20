#include "GraphView.hpp"
#include "Figures.hpp"

GraphView::GraphView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent), scene_(scene) {
  setMouseTracking(true);
}

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

void GraphView::startEdgeCreation(Figure *startNode) {
  isCreatingEdge_ = true;
  startNode_ = startNode;
  setCursor(Qt::CrossCursor);

  // Временное ребро (начало и конец совпадают)
  tempEdge_ = new Edge(startNode, startNode);
  tempEdge_->setPen(QPen(Qt::gray, 2, Qt::DashLine));
  tempEdge_->setFlag(QGraphicsItem::ItemIsSelectable, false);
  scene_->addItem(tempEdge_);
}

void GraphView::mouseMoveEvent(QMouseEvent *event) {
  if (isCreatingEdge_ && tempEdge_) {
    // Тянем конец линии за курсором в локальных координатах startNode
    QPointF mouseLocalPos = startNode_->mapFromScene(mapToScene(event->pos()));
    tempEdge_->setLine(QLineF(0, 0, mouseLocalPos.x(), mouseLocalPos.y()));
  }
  QGraphicsView::mouseMoveEvent(event);
}

void GraphView::mousePressEvent(QMouseEvent *event) {
  if (isCreatingEdge_ && event->button() == Qt::LeftButton) {
    QGraphicsItem *item = itemAt(event->pos());
    Figure *endNode = dynamic_cast<Figure *>(item);

    if (endNode) {
      // Удаляем временную линию
      if (tempEdge_) {
        scene_->removeItem(tempEdge_);
        delete tempEdge_;
        tempEdge_ = nullptr;
      }

      // Создаём постоянное ребро
      Edge *finalEdge = new Edge(startNode_, endNode);
      scene_->addItem(finalEdge);

      // Регистрируем ребро в целевом узле
      endNode->addIncomingEdge(finalEdge);
    } else {
      // Кликнули в пустоту -> отмена
      if (tempEdge_) {
        scene_->removeItem(tempEdge_);
        delete tempEdge_;
        tempEdge_ = nullptr;
      }
    }

    // Сброс состояния
    isCreatingEdge_ = false;
    startNode_ = nullptr;
    setCursor(Qt::ArrowCursor);
    return; // Не передаём событие дальше, чтобы не выделять объекты
  }

  // Правая кнопка во время создания -> отмена
  if (isCreatingEdge_ && event->button() == Qt::RightButton) {
    if (tempEdge_) {
      scene_->removeItem(tempEdge_);
      delete tempEdge_;
      tempEdge_ = nullptr;
    }
    isCreatingEdge_ = false;
    startNode_ = nullptr;
    setCursor(Qt::ArrowCursor);
    return;
  }

  QGraphicsView::mousePressEvent(event);
}

void GraphView::addFigure() {
  Figure *fig = new Figure(0, 0, 100, 100);
  fig->setBrush(Qt::red);
  fig->setFlag(QGraphicsItem::ItemIsMovable);
  fig->setAcceptHoverEvents(true);

  if (scene_)
    scene_->addItem(fig);
}

void GraphView::clear() {
  if (scene_)
    scene_->clear();
  isCreatingEdge_ = false;
  startNode_ = nullptr;
  tempEdge_ = nullptr;
}
