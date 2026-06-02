#include "Figures.hpp"
#include "GraphView.hpp"
#include "ThemeManager.hpp"
#include "qaction.h"
#include "qobject.h"

#include <QApplication>
#include <QDebug>
#include <QDoubleValidator>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsView>
#include <QInputDialog>
#include <QMenu>
#include <QMouseEvent>
#include <QObject>
#include <QPainterPath>

#include <cmath>
#include <utility>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ======================== SmoothEdge ========================

/**
 * @brief Конструктор ребра
 * @param start SmoothNode* Указатель на начальный узел
 * @param end SmoothNode* Указатель на конечный узел
 * @param parent QGraphicsItem* Родительский графический элемент (по умолчанию
 * nullptr)
 */
SmoothEdge::SmoothEdge(SmoothNode *start, SmoothNode *end,
                       QGraphicsItem *parent)
    : QObject(), QGraphicsLineItem(parent), startNode_(start), endNode_(end),
      weight_(1.0f) {
  setFlag(QGraphicsItem::ItemIsSelectable);
  updatePosition();
  qDebug() << "Edge created between" << start->getId() << "and" << end->getId();
}

/**
 * @brief Деструктор ребра
 */
SmoothEdge::~SmoothEdge() {
  qDebug() << "Edge destroyed";
  if (endNode_)
    endNode_->removeIncomingEdge(this);
  if (startNode_)
    startNode_->removeOutgoingEdge(this);
}

/**
 * @brief Обновляет позицию ребра в соответствии с положением узлов
 */
void SmoothEdge::updatePosition() {
  if (!startNode_ || !endNode_) {
    qDebug() << "updatePosition: null nodes";
    return;
  }

  if (!startNode_->scene() || !endNode_->scene()) {
    qDebug() << "Nodes don't exist in scene yet";
    return;
  }

  QPointF startCenter = startNode_->getCenter();
  QPointF endCenter = endNode_->getCenter();

  qDebug() << "updatePosition edge from" << startNode_->getId() << "at"
           << startCenter << "to" << endNode_->getId() << "at" << endCenter;

  if (startNode_ == endNode_) {
    setLine(QLineF(startCenter.x() + 40, startCenter.y(), startCenter.x() + 80,
                   startCenter.y() - 40));
  } else {
    setLine(QLineF(startCenter, endCenter));
  }
}

/**
 * @brief Устанавливает вес ребра
 * @param weight float Новый вес ребра
 */
void SmoothEdge::setWeight(float weight) {
  weight_ = weight;
  update();
}

/**
 * @brief Обновляет стиль ребра в соответствии с текущей темой
 * @param colors const ThemeColors& Ссылка на цветовую схему темы
 */
void SmoothEdge::updateThemeStyle(const ThemeColors &colors) {
  defaultColor_ = colors.edgeDefault;
  highlightColor_ = colors.pathEdge;
  textColor_ = colors.textPrimary;
  setPen(QPen(isHighlighted_ ? highlightColor_ : defaultColor_,
              isHighlighted_ ? 3 : 2));
  update();
}

/**
 * @brief Устанавливает или снимает подсветку ребра
 * @param highlight bool true - включить подсветку, false - выключить
 */
void SmoothEdge::setHighlighted(bool highlight) {
  if (isHighlighted_ == highlight)
    return;
  isHighlighted_ = highlight;

  if (defaultColor_.isValid()) {
    setPen(QPen(isHighlighted_ ? highlightColor_ : defaultColor_,
                isHighlighted_ ? 3 : 2));
  }
  update();
}

/**
 * @brief Показывает диалог ввода веса ребра
 */
void SmoothEdge::showWeightDialog() {
  QInputDialog dialog;
  dialog.setWindowTitle("Вес ребра");
  dialog.setLabelText("Введите вес ребра:");
  dialog.setInputMode(QInputDialog::DoubleInput);
  dialog.setDoubleMinimum(0.0);
  dialog.setDoubleMaximum(999999.0);
  dialog.setDoubleDecimals(3);
  dialog.setDoubleValue(weight_);
  dialog.setOkButtonText("OK");
  dialog.setCancelButtonText("Отмена");

  QLineEdit *lineEdit = dialog.findChild<QLineEdit *>();
  if (lineEdit) {
    QDoubleValidator *validator =
        new QDoubleValidator(0.0, 999999.0, 3, lineEdit);
    validator->setNotation(QDoubleValidator::StandardNotation);
    lineEdit->setValidator(validator);
  }

  if (dialog.exec() == QDialog::Accepted) {
    setWeight(static_cast<float>(dialog.doubleValue()));
  }
}

/**
 * @brief Отрисовывает текст веса ребра
 * @param painter QPainter* Указатель на объект рисования
 * @param line const QLineF& Ссылка на линию ребра
 */
void SmoothEdge::paintWeight(QPainter *painter, const QLineF &line) {
  QPointF midPoint = (line.p1() + line.p2()) / 2;

  QPointF direction = line.p2() - line.p1();
  QPointF perpendicular(-direction.y(), direction.x());

  if (perpendicular.x() != 0 || perpendicular.y() != 0) {
    double len = std::sqrt(perpendicular.x() * perpendicular.x() +
                           perpendicular.y() * perpendicular.y());
    if (len > 0) {
      perpendicular /= len;
    }
  }

  QPointF textPos = midPoint + perpendicular * 15.0;

  QString weightText = QString::number(weight_, 'f', 1);
  QFont font("Arial", 10, QFont::Bold);
  QFontMetrics fm(font);
  QRect textRect = fm.boundingRect(weightText);
  textRect.translate(textPos.x() - textRect.width() / 2,
                     textPos.y() - textRect.height() / 2);

  painter->setPen(QPen(textColor_, 1));
  painter->drawText(textRect, Qt::AlignCenter, weightText);
}

/**
 * @brief Возвращает ограничивающий прямоугольник ребра
 * @return QRectF Ограничивающий прямоугольник
 */
QRectF SmoothEdge::boundingRect() const {
  QRectF rect = QGraphicsLineItem::boundingRect();
  rect.adjust(-30, -30, 30, 30);
  return rect;
}

/**
 * @brief Вычисляет позиции для отрисовки стрелки
 * @param line QLineF& Ссылка на линию ребра (будет модифицирована)
 * @return std::pair<QPointF, QPointF> Пара точек для отрисовки стрелки
 */
std::pair<QPointF, QPointF> SmoothEdge::computeArrowPos(QLineF &line) {
  if (!startNode_ || !endNode_) {
    return std::make_pair(line.p2(), line.p2());
  }

  QPointF startCenter = startNode_->getCenter();
  QPointF endCenter = endNode_->getCenter();

  QPointF direction = endCenter - startCenter;
  double length =
      std::sqrt(direction.x() * direction.x() + direction.y() * direction.y());

  if (length < 1e-5) {
    return std::make_pair(endCenter, endCenter);
  }

  QPointF directionNormalized = direction / length;

  double startRadius = startNode_->getRadius();
  double endRadius = endNode_->getRadius();

  QPointF startPoint = startCenter + directionNormalized * startRadius;
  QPointF endPoint = endCenter - directionNormalized * endRadius;

  double angle = std::atan2(direction.y(), direction.x());

  qreal arrowSize = 12.0;

  QPointF arrowP1 = endPoint - QPointF(std::cos(angle - M_PI / 3) * arrowSize,
                                       std::sin(angle - M_PI / 3) * arrowSize);
  QPointF arrowP2 = endPoint - QPointF(std::cos(angle + M_PI / 3) * arrowSize,
                                       std::sin(angle + M_PI / 3) * arrowSize);

  line.setP1(startPoint);
  line.setP2(endPoint);

  return std::make_pair(arrowP1, arrowP2);
}

/**
 * @brief Переопределённый метод отрисовки ребра
 * @param painter QPainter* Указатель на объект рисования
 * @param option const QStyleOptionGraphicsItem* Опции стиля графического
 * элемента
 * @param widget QWidget* Указатель на виджет-родитель
 */
void SmoothEdge::paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) {
  Q_UNUSED(option);
  Q_UNUSED(widget);

  QLineF line = this->line();

  QLineF tempLine = line;
  std::pair<QPointF, QPointF> arrowP = computeArrowPos(tempLine);

  line = tempLine;

  if (line.length() < 1e-5)
    return;

  painter->save();
  painter->setRenderHint(QPainter::Antialiasing, true);

  painter->setPen(pen());
  painter->drawLine(line);

  painter->setPen(Qt::NoPen);
  painter->setBrush(pen().color());

  QPolygonF arrowHead;
  arrowHead << line.p2() << arrowP.first << arrowP.second;
  painter->drawPolygon(arrowHead);

  paintWeight(painter, line);

  painter->restore();
}

/**
 * @brief Обработчик контекстного меню ребра
 * @param event QGraphicsSceneContextMenuEvent* Событие контекстного меню
 */
void SmoothEdge::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
  QMenu menu;
  QAction *deleteAction = menu.addAction("Удалить ребро");
  QAction *weightAction = menu.addAction("Изменить вес");

  QObject::connect(deleteAction, &QAction::triggered, [this]() {
    emit edgeAboutToBeDeleted(this);
    if (scene())
      scene()->removeItem(this);
  });

  QObject::connect(weightAction, &QAction::triggered, [this]() {
    if (scene())
      showWeightDialog();
  });

  menu.exec(event->screenPos());
  event->accept();
}

// ======================== SmoothNode ========================

/**
 * @brief Конструктор узла
 * @param centerX qreal X-координата центра узла
 * @param centerY qreal Y-координата центра узла
 * @param radius qreal Радиус узла
 * @param parent QGraphicsItem* Родительский графический элемент (по умолчанию
 * nullptr)
 */
SmoothNode::SmoothNode(qreal centerX, qreal centerY, qreal radius,
                       QGraphicsItem *parent)
    : QObject(), QGraphicsEllipseItem(0, 0, radius * 2, radius * 2, parent),
      id_(0) {

  setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
  setAcceptHoverEvents(true);
  setFlag(QGraphicsItem::ItemIsMovable);
  setPos(centerX - radius, centerY - radius);

  qDebug() << "SmoothNode created at center:" << centerX << "," << centerY;
}

/**
 * @brief Добавляет входящее ребро
 * @param edge SmoothEdge* Указатель на добавляемое входящее ребро
 */
void SmoothNode::addIncomingEdge(SmoothEdge *edge) {
  if (!incomingEdges_.contains(edge)) {
    incomingEdges_.append(edge);
    qDebug() << "Added incoming edge to" << this->getId()
             << "total:" << incomingEdges_.size();
  }
}

/**
 * @brief Добавляет исходящее ребро
 * @param edge SmoothEdge* Указатель на добавляемое исходящее ребро
 */
void SmoothNode::addOutgoingEdge(SmoothEdge *edge) {
  if (!outgoingEdges_.contains(edge)) {
    outgoingEdges_.append(edge);
    qDebug() << "Added outgoing edge from" << this->getId()
             << "total:" << outgoingEdges_.size();
  }
}

/**
 * @brief Удаляет входящее ребро
 * @param edge SmoothEdge* Указатель на удаляемое входящее ребро
 */
void SmoothNode::removeIncomingEdge(SmoothEdge *edge) {
  incomingEdges_.removeOne(edge);
  qDebug() << "Removed incoming edge from" << this->getId();
}

/**
 * @brief Удаляет исходящее ребро
 * @param edge SmoothEdge* Указатель на удаляемое исходящее ребро
 */
void SmoothNode::removeOutgoingEdge(SmoothEdge *edge) {
  outgoingEdges_.removeOne(edge);
  qDebug() << "Removed outgoing edge from" << this->getId();
}

/**
 * @brief Очищает все входящие рёбра
 */
void SmoothNode::clearIncomingEdges() {
  incomingEdges_.clear();
  qDebug() << "Removed all incoming edges" << this->getId();
}

/**
 * @brief Очищает все исходящие рёбра
 */
void SmoothNode::clearOutcomingEdges() {
  outgoingEdges_.clear();
  qDebug() << "Removed all outcoming edges" << this->getId();
}

/**
 * @brief Возвращает центр узла
 * @return QPointF Координаты центра узла
 */
QPointF SmoothNode::getCenter() const {
  return pos() + QPointF(rect().width() / 2, rect().height() / 2);
}

/**
 * @brief Обработчик входа курсора в область узла
 * @param event QGraphicsSceneHoverEvent* Событие наведения курсора
 */
void SmoothNode::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
  setBrush(hoverColor_);
  QGraphicsEllipseItem::hoverEnterEvent(event);
}

/**
 * @brief Обработчик выхода курсора из области узла
 * @param event QGraphicsSceneHoverEvent* Событие наведения курсора
 */
void SmoothNode::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
  updateNodeColor();
  QGraphicsEllipseItem::hoverLeaveEvent(event);
}

/**
 * @brief Добавляет ребро от данного узла
 */
void SmoothNode::addEdge() {
  if (scene()) {
    for (QGraphicsView *view : scene()->views()) {
      GraphView *gv = qobject_cast<GraphView *>(view);
      if (gv) {
        qDebug() << "Starting edge creation from node" << this->getId();
        gv->startEdgeCreation(this);
        break;
      }
    }
  }
}

/**
 * @brief Устанавливает узел как стартовый
 */
void SmoothNode::setAsStart() {
  qDebug() << "Node" << this->getId() << "set as start requested";
  NodeSelectionBridge::instance()->setStartNodeRequested(this);
}

/**
 * @brief Устанавливает узел как конечный
 */
void SmoothNode::setAsEnd() {
  qDebug() << "Node" << this->getId() << "set as end requested";
  NodeSelectionBridge::instance()->setEndNodeRequested(this);
}

/**
 * @brief Снимает статус стартового узла
 */
void SmoothNode::clearStart() {
  qDebug() << "Node" << this->getId() << "clear start requested";
  NodeSelectionBridge::instance()->clearStartNodeRequested(this);
}

/**
 * @brief Снимает статус конечного узла
 */
void SmoothNode::clearEnd() {
  qDebug() << "Node" << this->getId() << "clear end requested";
  NodeSelectionBridge::instance()->clearEndNodeRequested(this);
}

/**
 * @brief Возвращает родительское представление (GraphView)
 * @return QGraphicsView* Указатель на родительское представление или nullptr
 */
QGraphicsView *SmoothNode::getParentView() const {
  if (!scene())
    return nullptr;
  const QList<QGraphicsView *> &views = scene()->views();
  if (views.isEmpty())
    return nullptr;
  return views.first();
}

/**
 * @brief Устанавливает роль узла
 * @param role NodeRole Новая роль узла
 */
void SmoothNode::setRole(NodeRole role) {
  if (role_ == role)
    return;

  role_ = role;
  updateNodeColor();
  qDebug() << "Node" << this->getId() << "role set to" << (int)role;
}

/**
 * @brief Обновляет цвет узла в зависимости от его роли
 */
void SmoothNode::updateNodeColor() {
  if (role_ == NodeRole::Start) {
    setBrush(startNodeColor_);
    qDebug() << "Node" << this->getId() << "color set to start color";
  } else if (role_ == NodeRole::End) {
    setBrush(endNodeColor_);
    qDebug() << "Node" << this->getId() << "color set to end color";
  } else if (isOnPath_) {
    setBrush(pathNodeColor_);
  } else {
    setBrush(defaultColor_);
  }
}

/**
 * @brief Сбрасывает подсветку пути
 */
void SmoothNode::resetPathHighlight() {
  isOnPath_ = false;
  updateNodeColor();
}

/**
 * @brief Обработчик контекстного меню узла
 * @param event QGraphicsSceneContextMenuEvent* Событие контекстного меню
 */
void SmoothNode::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
  QMenu menu;

  QAction *addEdgeAction = menu.addAction("Добавить грань");
  QAction *deleteNodeAction = menu.addAction("Удалить узел");
  menu.addSeparator();

  QAction *startAction = nullptr;
  QAction *endAction = nullptr;

  if (role_ == NodeRole::Start) {
    startAction = menu.addAction("Задать как простой");
  } else {
    startAction = menu.addAction("Задать как стартовый");
  }

  if (role_ == NodeRole::End) {
    endAction = menu.addAction("Задать как простой");
  } else {
    endAction = menu.addAction("Задать как конечный");
  }

  QObject::connect(addEdgeAction, &QAction::triggered,
                   [this]() { this->addEdge(); });
  QObject::connect(deleteNodeAction, &QAction::triggered, [this]() {
    emit nodeAboutToBeDeleted(this);
    for (SmoothEdge *edge : incomingEdges_) {
      if (scene())
        scene()->removeItem(edge);
    }
    for (SmoothEdge *edge : outgoingEdges_) {
      if (scene())
        scene()->removeItem(edge);
    }
    if (scene())
      scene()->removeItem(this);
  });

  QObject::connect(startAction, &QAction::triggered, [this, startAction]() {
    if (role_ == NodeRole::Start) {
      this->clearStart();
    } else {
      this->setAsStart();
    }
  });

  QObject::connect(endAction, &QAction::triggered, [this, endAction]() {
    if (role_ == NodeRole::End) {
      this->clearEnd();
    } else {
      this->setAsEnd();
    }
  });

  menu.exec(event->screenPos());
  event->accept();
}

/**
 * @brief Обработчик изменения свойств узла (например, позиции)
 * @param change GraphicsItemChange Тип изменения
 * @param value const QVariant& Новое значение свойства
 * @return QVariant Результат обработки изменения
 */
QVariant SmoothNode::itemChange(GraphicsItemChange change,
                                const QVariant &value) {
  if (change == ItemPositionHasChanged) {
    for (SmoothEdge *edge : incomingEdges_) {
      edge->updatePosition();
    }
    for (SmoothEdge *edge : outgoingEdges_) {
      edge->updatePosition();
    }
  }
  return QGraphicsEllipseItem::itemChange(change, value);
}

/**
 * @brief Устанавливает цвет при наведении
 * @param color const QColor& Новый цвет при наведении
 */
void SmoothNode::setHoverColor(const QColor &color) { hoverColor_ = color; }

/**
 * @brief Восстанавливает цвет узла по умолчанию
 */
void SmoothNode::restoreDefaultColor() { setBrush(defaultColor_); }

/**
 * @brief Переопределённый метод отрисовки узла
 * @param painter QPainter* Указатель на объект рисования
 * @param option const QStyleOptionGraphicsItem* Опции стиля графического
 * элемента
 * @param widget QWidget* Указатель на виджет-родитель
 */
void SmoothNode::paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) {
  Q_UNUSED(option);
  Q_UNUSED(widget);

  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setBrush(brush());
  painter->setPen(pen());
  painter->drawEllipse(rect());

  painter->setPen(QPen(textColor_, 1));
  QFont font = painter->font();
  font.setPointSize(10);
  font.setBold(true);
  painter->setFont(font);

  QString idText = QString::number(id_);
  QRectF textRect = rect();
  painter->drawText(textRect, Qt::AlignCenter, idText);
}

/**
 * @brief Обновляет стиль узла в соответствии с текущей темой
 * @param colors const ThemeColors& Ссылка на цветовую схему темы
 */
void SmoothNode::updateThemeStyle(const ThemeColors &colors) {
  defaultColor_ = colors.nodeDefault;
  hoverColor_ = colors.nodeHover;
  borderColor_ = colors.border;
  startNodeColor_ = colors.startNode;
  endNodeColor_ = colors.endNode;
  pathNodeColor_ = colors.pathNode;
  textColor_ = colors.textPrimary;

  setBrush(defaultColor_);
  setPen(QPen(borderColor_, 1));
  updateNodeColor();
  update();
}

/**
 * @brief Возвращает список входящих рёбер
 * @return QList<SmoothEdge*> Список указателей на входящие рёбра
 */
QList<SmoothEdge *> SmoothNode::getIncomingEdges() { return incomingEdges_; }

/**
 * @brief Возвращает список исходящих рёбер
 * @return QList<SmoothEdge*> Список указателей на исходящие рёбра
 */
QList<SmoothEdge *> SmoothNode::getOutcomingEdges() { return outgoingEdges_; }
