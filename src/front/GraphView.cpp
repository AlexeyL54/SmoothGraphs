// GraphView.cpp
#include "GraphView.hpp"
#include "Figures.hpp"

#include <QMenu>

/**
 * @brief Конструктор класса GraphView.
 * @param scene Указатель на сцену QGraphicsScene.
 * @param parent Родительский виджет (по умолчанию nullptr).
 */
GraphView::GraphView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent), scene_(scene) {
  setMouseTracking(true);
}

/**
 * @brief Обработчик события контекстного меню.
 * @param event Указатель на событие контекстного меню.
 *
 * Если клик произошёл на пустом месте сцены, отображает меню с опциями:
 * - "Добавить узел" — создаёт новую фигуру
 * - "Очистить все" — удаляет все элементы со сцены
 */
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

/**
 * @brief Начинает процесс создания ребра от указанного узла.
 * @param startNode Указатель на начальный узел (фигуру).
 *
 * Метод активирует режим создания ребра: устанавливает курсор,
 * создаёт временное пунктирное ребро и отслеживает перемещение мыши.
 */
void GraphView::startEdgeCreation(SmoothNode *startNode) {
  isCreatingEdge_ = true;
  startNode_ = startNode;
  startPos_ = startNode->scenePos() + QPointF(50, 50);
  setCursor(Qt::CrossCursor);

  tempEdge_ = new QGraphicsLineItem();
  tempEdge_->setPen(QPen(Qt::gray, 2, Qt::DashLine));
  tempEdge_->setZValue(-1);
  scene_->addItem(tempEdge_);
}

/**
 * @brief Обновляет стиль всех элементов на сцене.
 * @param colors Структура с цветами текущей темы.
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
 * @brief Обработчик события перемещения мыши.
 * @param event Указатель на событие мыши.
 *
 * В режиме создания ребра обновляет положение временного ребра
 * от начального узла до текущей позиции курсора.
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
 * @brief Обработчик события нажатия кнопки мыши.
 * @param event Указатель на событие мыши.
 *
 * В режиме создания ребра:
 * - ЛКМ: завершает создание ребра, если курсор над целевым узлом
 * - ПКМ: отменяет создание ребра
 */
void GraphView::mousePressEvent(QMouseEvent *event) {
  if (isCreatingEdge_) {
    if (event->button() == Qt::LeftButton) {
      QPointF scenePos = mapToScene(event->pos());
      QGraphicsItem *item = scene_->itemAt(scenePos, QTransform());
      SmoothNode *endNode = dynamic_cast<SmoothNode *>(item);

      if (endNode && endNode != startNode_) {
        if (tempEdge_) {
          scene_->removeItem(tempEdge_);
          delete tempEdge_;
          tempEdge_ = nullptr;
        }

        SmoothEdge *finalEdge = new SmoothEdge(startNode_, endNode);
        scene_->addItem(finalEdge);
        emit edgeAdded(finalEdge); // Сигнал о добавлении ребра

        // Добавляем ребро в оба узла
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

/**
 * @brief Слот для добавления новой фигуры на сцену.
 *
 * Создаёт экземпляр Figure, разрешает перемещение
 * и добавляет его в сцену. Испускает сигнал nodeAdded.
 */
void GraphView::addFigure() {
  SmoothNode *fig = new SmoothNode(0, 0, 100, 100);
  fig->setFlag(QGraphicsItem::ItemIsMovable);
  fig->setAcceptHoverEvents(true);

  if (scene_) {
    scene_->addItem(fig);
    emit nodeAdded(fig); // Сигнал о добавлении узла
    qDebug() << "Figure added to scene, signal emitted";
  }
}

/**
 * @brief Слот для очистки сцены.
 *
 * Удаляет все элементы со сцены и сбрасывает внутренние состояния,
 * связанные с созданием рёбер. Испускает сигналы nodeRemoved и edgeRemoved.
 */
void GraphView::clearScene() {
  if (!scene_)
    return;

  qDebug() << "Starting clear...";

  // Отключаем сигналы и флаги, чтобы избежать лишних обновлений
  isCreatingEdge_ = false;

  // Собираем все элементы
  QList<QGraphicsItem *> items = scene_->items();

  // Сначала удаляем все рёбра вручную (отвязывая их перед удалением)
  for (QGraphicsItem *item : items) {
    SmoothEdge *edge = dynamic_cast<SmoothEdge *>(item);
    if (edge) {
      qDebug() << "Removing edge manually";
      emit edgeRemoved(edge); // Сигнал об удалении ребра

      // Отвязываем ребро от узлов перед удалением
      SmoothNode *start = edge->getStartNode();
      SmoothNode *end = edge->getEndNode();
      if (start) {
        start->removeOutgoingEdge(edge);
      }
      if (end) {
        end->removeIncomingEdge(edge);
      }
      // Удаляем ребро
      scene_->removeItem(edge);
      delete edge;
    }
  }

  // Теперь удаляем все фигуры
  items = scene_->items(); // Обновляем список (рёбер уже нет)
  for (QGraphicsItem *item : items) {
    SmoothNode *fig = dynamic_cast<SmoothNode *>(item);
    if (fig) {
      qDebug() << "Removing figure manually";
      emit nodeRemoved(fig); // Сигнал об удалении узла

      // Очищаем списки (хотя они уже должны быть пусты)
      fig->clearIncomingEdges();
      fig->clearOutcomingEdges();
      scene_->removeItem(fig);
      delete fig;
    }
  }

  // Дополнительная очистка на всякий случай
  scene_->clear();

  // Сбрасываем указатели
  startNode_ = nullptr;
  if (tempEdge_) {
    tempEdge_ = nullptr;
  }

  setCursor(Qt::ArrowCursor);
  qDebug() << "Clear finished";
}
