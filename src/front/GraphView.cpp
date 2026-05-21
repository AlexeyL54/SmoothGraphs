// GraphView.cpp
#include "GraphView.hpp"
#include "Figures.hpp"

#include <QMenu>
#include <QMessageBox>
#include <QVBoxLayout>

GraphView::GraphView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent), scene_(scene) {
  setMouseTracking(true);
  setupNodeSelectionBridge();
  setupZoomButtons();
}

void GraphView::setupZoomButtons() {
  zoomInBtn_ = new QPushButton("+", this);
  zoomOutBtn_ = new QPushButton("-", this);

  zoomInBtn_->setFixedSize(40, 40);
  zoomOutBtn_->setFixedSize(40, 40);

  // Делаем кнопки круглыми
  zoomInBtn_->setStyleSheet(R"(
    QPushButton {
      background-color: rgba(60, 60, 70, 200);
      color: white;
      border: none;
      border-radius: 20px;
      font-size: 20px;
      font-weight: bold;
    }
    QPushButton:hover {
      background-color: rgba(80, 80, 100, 220);
    }
    QPushButton:pressed {
      background-color: rgba(50, 50, 60, 220);
    }
  )");

  zoomOutBtn_->setStyleSheet(R"(
    QPushButton {
      background-color: rgba(60, 60, 70, 200);
      color: white;
      border: none;
      border-radius: 20px;
      font-size: 24px;
      font-weight: bold;
    }
    QPushButton:hover {
      background-color: rgba(80, 80, 100, 220);
    }
    QPushButton:pressed {
      background-color: rgba(50, 50, 60, 220);
    }
  )");

  // Подключаем сигналы
  connect(zoomInBtn_, &QPushButton::clicked, this, &GraphView::zoomIn);
  connect(zoomOutBtn_, &QPushButton::clicked, this, &GraphView::zoomOut);

  // Поднимаем кнопки выше сцены
  zoomInBtn_->raise();
  zoomOutBtn_->raise();
}

void GraphView::resizeEvent(QResizeEvent *event) {
  QGraphicsView::resizeEvent(event);
  updateZoomButtonsPosition();
}

void GraphView::updateZoomButtonsPosition() {
  if (!zoomInBtn_ || !zoomOutBtn_)
    return;

  int margin = 20;
  int buttonSpacing = 10;

  // Позиционируем кнопку "-" в правом нижнем углу
  zoomOutBtn_->setGeometry(width() - zoomOutBtn_->width() - margin,
                           height() - zoomOutBtn_->height() - margin,
                           zoomOutBtn_->width(), zoomOutBtn_->height());

  // Позиционируем кнопку "+" слева от кнопки "-"
  zoomInBtn_->setGeometry(
      zoomOutBtn_->x() - zoomInBtn_->width() - buttonSpacing, zoomOutBtn_->y(),
      zoomInBtn_->width(), zoomInBtn_->height());
}

void GraphView::zoomIn() {
  if (currentZoom_ * ZOOM_STEP <= MAX_ZOOM) {
    currentZoom_ *= ZOOM_STEP;
    scale(ZOOM_STEP, ZOOM_STEP);
  }
}

void GraphView::zoomOut() {
  if (currentZoom_ / ZOOM_STEP >= MIN_ZOOM) {
    currentZoom_ /= ZOOM_STEP;
    scale(1.0 / ZOOM_STEP, 1.0 / ZOOM_STEP);
  }
}

void GraphView::contextMenuEvent(QContextMenuEvent *event) {
  if (itemAt(event->pos()) == nullptr) {
    QMenu menu;
    QAction *addFigAction = menu.addAction("Добавить узел");
    QAction *clearAction = menu.addAction("Очистить все");

    connect(addFigAction, &QAction::triggered, this, &GraphView::addFigure);
    connect(clearAction, &QAction::triggered, this, &GraphView::clearScene);

    menu.exec(event->globalPos());
  } else {
    QGraphicsView::contextMenuEvent(event);
  }
}

void GraphView::startEdgeCreation(SmoothNode *startNode) {
  isCreatingEdge_ = true;
  tempStartNode_ = startNode;
  startPos_ = startNode->scenePos() + QPointF(50, 50);
  setCursor(Qt::CrossCursor);

  tempEdge_ = new QGraphicsLineItem();
  tempEdge_->setPen(QPen(Qt::gray, 2, Qt::DashLine));
  tempEdge_->setZValue(-1);
  scene_->addItem(tempEdge_);
}

void GraphView::updateAllElementsTheme(const ThemeColors &colors) {
  if (!scene_)
    return;

  QList<QGraphicsItem *> items = scene_->items();
  for (QGraphicsItem *item : items) {
    SmoothNode *figure = dynamic_cast<SmoothNode *>(item);
    if (figure) {
      figure->updateThemeStyle(colors);
    }

    SmoothEdge *edge = dynamic_cast<SmoothEdge *>(item);
    if (edge) {
      edge->updateThemeStyle(colors);
    }
  }
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
      SmoothNode *endNode = dynamic_cast<SmoothNode *>(item);

      if (endNode && endNode != tempStartNode_) {
        if (tempEdge_) {
          scene_->removeItem(tempEdge_);
          delete tempEdge_;
          tempEdge_ = nullptr;
        }

        SmoothEdge *finalEdge = new SmoothEdge(tempStartNode_, endNode);
        if (themeMng_) {
          finalEdge->updateThemeStyle(themeMng_->getThemeColors());
        }
        scene_->addItem(finalEdge);
        emit edgeAdded(finalEdge);

        tempStartNode_->addOutgoingEdge(finalEdge);
        endNode->addIncomingEdge(finalEdge);
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
      tempStartNode_ = nullptr;
      setCursor(Qt::ArrowCursor);
      return;
    } else if (event->button() == Qt::RightButton) {
      if (tempEdge_) {
        scene_->removeItem(tempEdge_);
        delete tempEdge_;
        tempEdge_ = nullptr;
      }
      isCreatingEdge_ = false;
      tempStartNode_ = nullptr;
      setCursor(Qt::ArrowCursor);
      return;
    }
  }

  QGraphicsView::mousePressEvent(event);
}

void GraphView::addFigure() {
  SmoothNode *fig = new SmoothNode(0, 0, 50);

  if (themeMng_) {
    fig->updateThemeStyle(themeMng_->getThemeColors());
  }

  if (scene_) {
    scene_->addItem(fig);
    emit nodeAdded(fig);
    qDebug() << "Figure added to scene, signal emitted";
  }
}

void GraphView::clearScene() {
  if (!scene_)
    return;

  qDebug() << "Starting clear...";

  clearPathHighlight();
  pathStartNode_ = nullptr;
  pathEndNode_ = nullptr;
  isCreatingEdge_ = false;

  QList<QGraphicsItem *> items = scene_->items();

  for (QGraphicsItem *item : items) {
    SmoothEdge *edge = dynamic_cast<SmoothEdge *>(item);
    if (edge) {
      qDebug() << "Removing edge manually";
      emit edgeRemoved(edge);

      SmoothNode *start = edge->getStartNode();
      SmoothNode *end = edge->getEndNode();
      if (start) {
        start->removeOutgoingEdge(edge);
      }
      if (end) {
        end->removeIncomingEdge(edge);
      }
      scene_->removeItem(edge);
      delete edge;
    }
  }

  items = scene_->items();
  for (QGraphicsItem *item : items) {
    SmoothNode *fig = dynamic_cast<SmoothNode *>(item);
    if (fig) {
      qDebug() << "Removing figure manually";
      emit nodeRemoved(fig);

      fig->clearIncomingEdges();
      fig->clearOutcomingEdges();
      scene_->removeItem(fig);
      delete fig;
    }
  }

  scene_->clear();

  tempStartNode_ = nullptr;
  if (tempEdge_) {
    tempEdge_ = nullptr;
  }

  setCursor(Qt::ArrowCursor);
  qDebug() << "Clear finished";
}

void GraphView::setupNodeSelectionBridge() {
  NodeSelectionBridge *bridge = NodeSelectionBridge::instance();

  connect(bridge, &NodeSelectionBridge::setStartNodeRequested, this,
          &GraphView::setStartNode);
  connect(bridge, &NodeSelectionBridge::setEndNodeRequested, this,
          &GraphView::setEndNode);
  connect(bridge, &NodeSelectionBridge::clearStartNodeRequested, this,
          &GraphView::clearStartNode);
  connect(bridge, &NodeSelectionBridge::clearEndNodeRequested, this,
          &GraphView::clearEndNode);
}

void GraphView::setStartNode(SmoothNode *node) {
  if (!node)
    return;

  qDebug() << "setStartNode called for node:" << node->getId();

  if (pathStartNode_) {
    if (pathStartNode_ == node) {
      pathStartNode_->setRole(NodeRole::Normal);
      pathStartNode_ = nullptr;
      emit startNodeChanged(nullptr);
      return;
    }
    pathStartNode_->setRole(NodeRole::Normal);
  }

  if (pathEndNode_ == node) {
    QMessageBox::warning(
        nullptr, "Ошибка",
        "Узел уже является конечным. Нельзя назначить его стартовым.");
    return;
  }

  pathStartNode_ = node;
  pathStartNode_->setRole(NodeRole::Start);
  emit startNodeChanged(pathStartNode_);
  clearPathHighlight();
}

void GraphView::setEndNode(SmoothNode *node) {
  if (!node)
    return;

  qDebug() << "setEndNode called for node:" << node->getId();

  if (pathEndNode_) {
    if (pathEndNode_ == node) {
      pathEndNode_->setRole(NodeRole::Normal);
      pathEndNode_ = nullptr;
      emit endNodeChanged(nullptr);
      return;
    }
    pathEndNode_->setRole(NodeRole::Normal);
  }

  if (pathStartNode_ == node) {
    QMessageBox::warning(
        nullptr, "Ошибка",
        "Узел уже является стартовым. Нельзя назначить его конечным.");
    return;
  }

  pathEndNode_ = node;
  pathEndNode_->setRole(NodeRole::End);
  emit endNodeChanged(pathEndNode_);
  clearPathHighlight();
}

void GraphView::clearStartNode(SmoothNode *node) {
  qDebug() << "clearStartNode called for node:" << (node ? node->getId() : 0);
  if (pathStartNode_ == node) {
    pathStartNode_->setRole(NodeRole::Normal);
    pathStartNode_ = nullptr;
    emit startNodeChanged(nullptr);
    clearPathHighlight();
  }
}

void GraphView::clearEndNode(SmoothNode *node) {
  qDebug() << "clearEndNode called for node:" << (node ? node->getId() : 0);
  if (pathEndNode_ == node) {
    pathEndNode_->setRole(NodeRole::Normal);
    pathEndNode_ = nullptr;
    emit endNodeChanged(nullptr);
    clearPathHighlight();
  }
}

void GraphView::highlightPath(const std::vector<SmoothNode *> &path) {
  clearPathHighlight();

  if (path.empty())
    return;

  currentPath_ = path;

  for (size_t i = 0; i < path.size() - 1; ++i) {
    if (path[i] && path[i + 1]) {
      QList<SmoothEdge *> edges = path[i]->getOutcomingEdges();
      for (SmoothEdge *edge : edges) {
        if (edge->getEndNode() == path[i + 1]) {
          edge->setHighlighted(true);
          break;
        }
      }
    }
  }

  qDebug() << "Path highlighted with" << path.size() << "nodes";
}

void GraphView::clearPathHighlight() {
  if (!scene_)
    return;

  QList<QGraphicsItem *> items = scene_->items();
  for (QGraphicsItem *item : items) {
    SmoothEdge *edge = dynamic_cast<SmoothEdge *>(item);
    if (edge) {
      edge->setHighlighted(false);
    }
  }

  currentPath_.clear();
  qDebug() << "Path highlight cleared";
}
