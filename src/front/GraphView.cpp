#include "GraphView.hpp"
#include "Figures.hpp"
#include "HelpText.hpp"
#include "StyleManager.hpp"

#include <QMenu>
#include <QMessageBox>
#include <QVBoxLayout>

/**
 * @brief Конструктор класса GraphView
 * @param scene Указатель на графическую сцену
 * @param parent Родительский виджет (по умолчанию nullptr)
 */
GraphView::GraphView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent), scene_(scene) {
  setMouseTracking(true);
  setFocusPolicy(Qt::StrongFocus);
  setupNodeSelectionBridge();
  setupZoomButtons();
  setupHelpButton();
}

/**
 * @brief Настраивает кнопки масштабирования
 */
void GraphView::setupZoomButtons() {
  zoomInBtn_ = new QPushButton("+", this);
  zoomOutBtn_ = new QPushButton("-", this);

  zoomInBtn_->setFixedSize(40, 40);
  zoomOutBtn_->setFixedSize(40, 40);

  QString btnStyle = StyleManager::generateFloatingButtonsStyleSheet();

  zoomInBtn_->setStyleSheet(btnStyle);
  zoomOutBtn_->setStyleSheet(btnStyle);

  connect(zoomInBtn_, &QPushButton::clicked, this, &GraphView::zoomIn);
  connect(zoomOutBtn_, &QPushButton::clicked, this, &GraphView::zoomOut);

  zoomInBtn_->raise();
  zoomOutBtn_->raise();
}

/**
 * @brief Настраивает кнопку справки
 */
void GraphView::setupHelpButton() {
  helpBtn_ = new QPushButton("?", this);
  helpBtn_->setFixedSize(40, 40);
  helpBtn_->setStyleSheet(StyleManager::generateFloatingButtonsStyleSheet());

  connect(helpBtn_, &QPushButton::clicked, this, &GraphView::showHelpDialog);
  helpBtn_->raise();
}

/**
 * @brief Отображает диалоговое окно справки
 */
void GraphView::showHelpDialog() {
  QMessageBox *msgBox = new QMessageBox(this);
  msgBox->setWindowTitle("Справка - Редактор графов");
  msgBox->setTextFormat(Qt::RichText);
  msgBox->setText(HelpText::getHelpText());
  msgBox->setStandardButtons(QMessageBox::Ok);
  msgBox->setDefaultButton(QMessageBox::Ok);

  if (themeMng_) {
    ThemeColors colors = themeMng_->getThemeColors();
    msgBox->setStyleSheet(StyleManager::generateHelpDialogStyleSheet(colors));
  }

  msgBox->exec();
  delete msgBox;
}

/**
 * @brief Обработчик изменения размера виджета
 * @param event Событие изменения размера
 */
void GraphView::resizeEvent(QResizeEvent *event) {
  QGraphicsView::resizeEvent(event);
  updateButtonsPosition();
}

/**
 * @brief Обновляет позиции кнопок на форме
 */
void GraphView::updateButtonsPosition() {
  if (!zoomInBtn_ || !zoomOutBtn_ || !helpBtn_)
    return;

  int margin = 20;
  int buttonSpacing = 10;

  // Кнопка "-" в правом нижнем углу
  zoomOutBtn_->setGeometry(width() - zoomOutBtn_->width() - margin,
                           height() - zoomOutBtn_->height() - margin,
                           zoomOutBtn_->width(), zoomOutBtn_->height());

  // Кнопка "+" слева от "-"
  zoomInBtn_->setGeometry(
      zoomOutBtn_->x() - zoomInBtn_->width() - buttonSpacing, zoomOutBtn_->y(),
      zoomInBtn_->width(), zoomInBtn_->height());

  // Кнопка "?" в левом нижнем углу
  helpBtn_->setGeometry(margin, height() - helpBtn_->height() - margin,
                        helpBtn_->width(), helpBtn_->height());
}

/**
 * @brief Увеличивает масштаб сцены
 */
void GraphView::zoomIn() {
  if (currentZoom_ * ZOOM_STEP <= MAX_ZOOM) {
    currentZoom_ *= ZOOM_STEP;
    scale(ZOOM_STEP, ZOOM_STEP);
  }
}

/**
 * @brief Уменьшает масштаб сцены
 */
void GraphView::zoomOut() {
  if (currentZoom_ / ZOOM_STEP >= MIN_ZOOM) {
    currentZoom_ /= ZOOM_STEP;
    scale(1.0 / ZOOM_STEP, 1.0 / ZOOM_STEP);
  }
}

/**
 * @brief Обработчик нажатия клавиш клавиатуры
 * @param event Событие клавиатуры
 */
void GraphView::keyPressEvent(QKeyEvent *event) {
  if (event->modifiers() == Qt::ControlModifier) {
    if (event->key() == Qt::Key_Plus || event->key() == Qt::Key_Equal) {
      zoomIn();
      event->accept();
      return;
    } else if (event->key() == Qt::Key_Minus) {
      zoomOut();
      event->accept();
      return;
    } else if (event->key() == Qt::Key_0) {
      if (currentZoom_ != 1.0) {
        qreal resetScale = 1.0 / currentZoom_;
        scale(resetScale, resetScale);
        currentZoom_ = 1.0;
      }
      event->accept();
      return;
    }
  }
  QGraphicsView::keyPressEvent(event);
}

/**
 * @brief Обработчик контекстного меню
 * @param event Событие контекстного меню
 */
void GraphView::contextMenuEvent(QContextMenuEvent *event) {
  if (itemAt(event->pos()) == nullptr) {
    QMenu menu;
    QAction *addFigAction = menu.addAction("Добавить узел");
    QAction *clearAction = menu.addAction("Очистить все");

    connect(clearAction, &QAction::triggered, this, &GraphView::clearScene);
    connect(addFigAction, &QAction::triggered, this,
            [this, event]() { this->addFigure(mapToScene(event->pos())); });

    menu.exec(event->globalPos());
  } else {
    QGraphicsView::contextMenuEvent(event);
  }
}

/**
 * @brief Начинает процесс создания ребра от указанного узла
 * @param startNode Начальный узел, от которого создаётся ребро
 */
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

/**
 * @brief Обновляет тему оформления для всех элементов на сцене
 * @param colors Цветовая схема темы
 */
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

/**
 * @brief Обработчик движения мыши
 * @param event Событие движения мыши
 */
void GraphView::mouseMoveEvent(QMouseEvent *event) {
  if (isCreatingEdge_ && tempEdge_) {
    QPointF currentPos = mapToScene(event->pos());
    QLineF line(startPos_, currentPos);
    tempEdge_->setLine(line);
  }
  QGraphicsView::mouseMoveEvent(event);
}

/**
 * @brief Обработчик нажатия кнопки мыши
 * @param event Событие мыши
 */
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
        connect(finalEdge, &SmoothEdge::edgeAboutToBeDeleted, this,
                [this](SmoothEdge *n) { emit edgeRemoved(n); });

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

/**
 * @brief Добавляет новый узел в указанной позиции
 * @param pos Позиция для добавления узла
 */
void GraphView::addFigure(const QPointF &pos) {
  SmoothNode *fig = new SmoothNode(pos.x(), pos.y(), 50);

  connect(fig, &SmoothNode::nodeAboutToBeDeleted, this,
          [this](SmoothNode *n) { emit nodeRemoved(n); });

  if (themeMng_) {
    fig->updateThemeStyle(themeMng_->getThemeColors());
  }

  if (scene_) {
    scene_->addItem(fig);
    emit nodeAdded(fig);
    qDebug() << "Figure added to scene, signal emitted";
  }
}

/**
 * @brief Очищает всю сцену, удаляя все узлы и рёбра
 */
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

      fig->clearIncomingEdges();
      fig->clearOutcomingEdges();
      scene_->removeItem(fig);
      delete fig;
    }
  }

  scene_->clear();
  emit sceneCleared();

  tempStartNode_ = nullptr;
  if (tempEdge_) {
    tempEdge_ = nullptr;
  }

  setCursor(Qt::ArrowCursor);
  qDebug() << "Clear finished";
}

/**
 * @brief Настраивает мост для выбора узлов через сигналы
 */
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

/**
 * @brief Устанавливает указанный узел как стартовый для поиска пути
 * @param node Узел, который станет стартовым
 */
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

/**
 * @brief Устанавливает указанный узел как конечный для поиска пути
 * @param node Узел, который станет конечным
 */
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

/**
 * @brief Сбрасывает статус стартового узла
 * @param node Узел, у которого снимается статус стартового
 */
void GraphView::clearStartNode(SmoothNode *node) {
  qDebug() << "clearStartNode called for node:" << (node ? node->getId() : 0);
  if (pathStartNode_ == node) {
    pathStartNode_->setRole(NodeRole::Normal);
    pathStartNode_ = nullptr;
    emit startNodeChanged(nullptr);
    clearPathHighlight();
  }
}

/**
 * @brief Сбрасывает статус конечного узла
 * @param node Узел, у которого снимается статус конечного
 */
void GraphView::clearEndNode(SmoothNode *node) {
  qDebug() << "clearEndNode called for node:" << (node ? node->getId() : 0);
  if (pathEndNode_ == node) {
    pathEndNode_->setRole(NodeRole::Normal);
    pathEndNode_ = nullptr;
    emit endNodeChanged(nullptr);
    clearPathHighlight();
  }
}

/**
 * @brief Подсвечивает указанный путь на графе
 * @param path Вектор указателей на узлы, составляющие путь
 */
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

/**
 * @brief Снимает подсветку с текущего пути
 */
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
