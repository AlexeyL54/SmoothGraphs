#include "GraphView.hpp"
#include "Figures.hpp"
#include "qgraphicsitem.h"

GraphView::GraphView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent), scene_(scene) {
  setMouseTracking(true);
}

void GraphView::contextMenuEvent(QContextMenuEvent *event) {
  if (itemAt(event->pos()) == nullptr) {
    QMenu menu;
    QAction *addFigAction = menu.addAction("Добавить узел");
    QAction *clearAction = menu.addAction("Очистить все");

    connect(addFigAction, &QAction::triggered, this, &GraphView::addFigure);
    connect(clearAction, &QAction::triggered, this, &GraphView::clear);

    menu.exec(event->globalPos());
  } else {
    QGraphicsView::contextMenuEvent(event);
  }
}

void GraphView::startEdgeCreation(Figure *startNode) {
  isCreatingEdge_ = true;
  startNode_ = startNode;
  startPos_ = startNode->scenePos() + QPointF(50, 50);
  setCursor(Qt::CrossCursor);

  tempEdge_ = new QGraphicsLineItem();
  tempEdge_->setPen(QPen(Qt::gray, 2, Qt::DashLine));
  tempEdge_->setZValue(-1);
  scene_->addItem(tempEdge_);
}

void GraphView::mouseMoveEvent(QMouseEvent *event) {
  if (isCreatingEdge_ && tempEdge_) {
    QPointF currentPos = mapToScene(event->pos());
    QLineF line(startPos_, currentPos);
    tempEdge_->setLine(line);
  }
  QGraphicsView::mouseMoveEvent(event);
}

void GraphView::mousePressEvent(QMouseEvent *event) {
  if (isCreatingEdge_) {
    if (event->button() == Qt::LeftButton) {
      QPointF scenePos = mapToScene(event->pos());
      QGraphicsItem *item = scene_->itemAt(scenePos, QTransform());
      Figure *endNode = dynamic_cast<Figure *>(item);

      if (endNode && endNode != startNode_) {
        if (tempEdge_) {
          scene_->removeItem(tempEdge_);
          delete tempEdge_;
          tempEdge_ = nullptr;
        }

        Edge *finalEdge = new Edge(startNode_, endNode);
        scene_->addItem(finalEdge);

        // ВАЖНО: добавляем ребро в оба узла
        startNode_->addOutgoingEdge(finalEdge);
        endNode->addIncomingEdge(finalEdge);

        // Принудительно обновляем позицию
        finalEdge->updatePosition();

        qDebug() << "Edge created and added to both nodes";
      } else {
        if (tempEdge_) {
          scene_->removeItem(tempEdge_);
          delete tempEdge_;
          tempEdge_ = nullptr;
        }
      }

      isCreatingEdge_ = false;
      startNode_ = nullptr;
      setCursor(Qt::ArrowCursor);
      return;
    } else if (event->button() == Qt::RightButton) {
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
