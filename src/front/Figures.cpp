#include "Figures.hpp"
#include "GraphView.hpp"
#include "ThemeManager.hpp"
#include "qaction.h"

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

SmoothEdge::SmoothEdge(SmoothNode *start, SmoothNode *end,
                       QGraphicsItem *parent)
    : QGraphicsLineItem(parent), startNode_(start), endNode_(end),
      weight_(1.0f) {
  setFlag(QGraphicsItem::ItemIsSelectable);
  updatePosition();
  qDebug() << "Edge created between" << start << "and" << end;
}

SmoothEdge::~SmoothEdge() {
  qDebug() << "Edge destroyed";
  if (endNode_)
    endNode_->removeIncomingEdge(this);
  if (startNode_)
    startNode_->removeOutgoingEdge(this);
}

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

void SmoothEdge::setWeight(float weight) {
  weight_ = weight;
  update();
}

void SmoothEdge::updateThemeStyle(const ThemeColors &colors) {
  defaultColor_ = colors.edgeDefault;
  highlightColor_ = colors.pathEdge;
  textColor_ = colors.textPrimary;
  setPen(QPen(isHighlighted_ ? highlightColor_ : defaultColor_,
              isHighlighted_ ? 3 : 2));
  update();
}

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

  // Настройка валидатора для запрета ввода букв и отрицательных чисел
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

void SmoothEdge::paintWeight(QPainter *painter, const QLineF &line) {
  QPointF midPoint = (line.p1() + line.p2()) / 2;

  // Смещаем текст перпендикулярно ребру
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
  painter->restore();
}

QRectF SmoothEdge::boundingRect() const {
  QRectF rect = QGraphicsLineItem::boundingRect();
  rect.adjust(-30, -30, 30, 30);
  return rect;
}

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

  // Угол для стрелки
  double angle = std::atan2(direction.y(), direction.x());

  qreal arrowSize = 12.0;

  // Точки для стрелки (треугольник)
  QPointF arrowP1 = endPoint - QPointF(std::cos(angle - M_PI / 3) * arrowSize,
                                       std::sin(angle - M_PI / 3) * arrowSize);
  QPointF arrowP2 = endPoint - QPointF(std::cos(angle + M_PI / 3) * arrowSize,
                                       std::sin(angle + M_PI / 3) * arrowSize);

  line.setP1(startPoint);
  line.setP2(endPoint);

  return std::make_pair(arrowP1, arrowP2);
}

void SmoothEdge::paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) {
  Q_UNUSED(option);
  Q_UNUSED(widget);

  // Получаем линию с учётом границ узлов
  QLineF line = this->line();

  // Пересчитываем линию и стрелку
  QLineF tempLine = line;
  std::pair<QPointF, QPointF> arrowP = computeArrowPos(tempLine);

  // Используем пересчитанную линию
  line = tempLine;

  if (line.length() < 1e-5)
    return;

  painter->save();
  painter->setRenderHint(QPainter::Antialiasing, true);

  // Рисуем линию
  painter->setPen(pen());
  painter->drawLine(line);

  // Рисуем стрелку
  painter->setPen(Qt::NoPen);
  painter->setBrush(pen().color());

  QPolygonF arrowHead;
  arrowHead << line.p2() << arrowP.first << arrowP.second;
  painter->drawPolygon(arrowHead);

  paintWeight(painter, line);

  painter->restore();
}

QPointF SmoothNode::getCenter() const {
  return pos() + QPointF(rect().width() / 2, rect().height() / 2);
}

void SmoothEdge::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
  QMenu menu;
  QAction *deleteAction = menu.addAction("Удалить ребро");
  QAction *weightAction = menu.addAction("Изменить вес");

  QObject::connect(deleteAction, &QAction::triggered, [this]() {
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

//////////////////////////////////////////////////////////////////////////////////////////////

SmoothNode::SmoothNode(qreal centerX, qreal centerY, qreal radius,
                       QGraphicsItem *parent)
    : QGraphicsEllipseItem(0, 0, radius * 2, radius * 2, parent), id_(0) {

  setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
  setAcceptHoverEvents(true);
  setFlag(QGraphicsItem::ItemIsMovable);
  setPos(centerX - radius, centerY - radius);

  qDebug() << "SmoothNode created at center:" << centerX << "," << centerY;
}

void SmoothNode::addIncomingEdge(SmoothEdge *edge) {
  if (!incomingEdges_.contains(edge)) {
    incomingEdges_.append(edge);
    qDebug() << "Added incoming edge to" << this->getId()
             << "total:" << incomingEdges_.size();
  }
}

void SmoothNode::addOutgoingEdge(SmoothEdge *edge) {
  if (!outgoingEdges_.contains(edge)) {
    outgoingEdges_.append(edge);
    qDebug() << "Added outgoing edge from" << this->getId()
             << "total:" << outgoingEdges_.size();
  }
}

void SmoothNode::removeIncomingEdge(SmoothEdge *edge) {
  incomingEdges_.removeOne(edge);
  qDebug() << "Removed incoming edge from" << this->getId();
}

void SmoothNode::removeOutgoingEdge(SmoothEdge *edge) {
  outgoingEdges_.removeOne(edge);
  qDebug() << "Removed outgoing edge from" << this->getId();
}

void SmoothNode::clearIncomingEdges() {
  incomingEdges_.clear();
  qDebug() << "Removed all incoming edges" << this->getId();
}

void SmoothNode::clearOutcomingEdges() {
  outgoingEdges_.clear();
  qDebug() << "Removed all outcoming edges" << this->getId();
}

void SmoothNode::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
  setBrush(hoverColor_);
  QGraphicsEllipseItem::hoverEnterEvent(event);
}

void SmoothNode::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
  updateNodeColor();
  QGraphicsEllipseItem::hoverLeaveEvent(event);
}

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

void SmoothNode::setAsStart() {
  qDebug() << "Node" << this->getId() << "set as start requested";
  NodeSelectionBridge::instance()->setStartNodeRequested(this);
}

void SmoothNode::setAsEnd() {
  qDebug() << "Node" << this->getId() << "set as end requested";
  NodeSelectionBridge::instance()->setEndNodeRequested(this);
}

void SmoothNode::clearStart() {
  qDebug() << "Node" << this->getId() << "clear start requested";
  NodeSelectionBridge::instance()->clearStartNodeRequested(this);
}

void SmoothNode::clearEnd() {
  qDebug() << "Node" << this->getId() << "clear end requested";
  NodeSelectionBridge::instance()->clearEndNodeRequested(this);
}

QGraphicsView *SmoothNode::getParentView() const {
  if (!scene())
    return nullptr;
  const QList<QGraphicsView *> &views = scene()->views();
  if (views.isEmpty())
    return nullptr;
  return views.first();
}

void SmoothNode::setRole(NodeRole role) {
  if (role_ == role)
    return;

  role_ = role;
  updateNodeColor();
  qDebug() << "Node" << this->getId() << "role set to" << (int)role;
}

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

void SmoothNode::resetPathHighlight() {
  isOnPath_ = false;
  updateNodeColor();
}

void SmoothNode::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
  QMenu menu;

  QAction *addEdgeAction = menu.addAction("Добавить грань");
  QAction *deleteNodeAction = menu.addAction("Удалить узел");
  menu.addSeparator();

  QAction *startAction = nullptr;
  QAction *endAction = nullptr;

  if (role_ == NodeRole::Start) {
    startAction = menu.addAction("Снять как стартовый");
  } else {
    startAction = menu.addAction("Задать как стартовый");
  }

  if (role_ == NodeRole::End) {
    endAction = menu.addAction("Снять как конечный");
  } else {
    endAction = menu.addAction("Задать как конечный");
  }

  QObject::connect(addEdgeAction, &QAction::triggered,
                   [this]() { this->addEdge(); });
  QObject::connect(deleteNodeAction, &QAction::triggered, [this]() {
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

void SmoothNode::setHoverColor(const QColor &color) { hoverColor_ = color; }

void SmoothNode::restoreDefaultColor() { setBrush(defaultColor_); }

void SmoothNode::paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) {
  Q_UNUSED(option);
  Q_UNUSED(widget);

  // Рисуем круг
  painter->setRenderHint(QPainter::Antialiasing, true);
  painter->setBrush(brush());
  painter->setPen(pen());
  painter->drawEllipse(rect());

  // Рисуем ID узла в центре
  painter->setPen(QPen(textColor_, 1));
  QFont font = painter->font();
  font.setPointSize(10);
  font.setBold(true);
  painter->setFont(font);

  QString idText = QString::number(id_);
  QRectF textRect = rect();
  painter->drawText(textRect, Qt::AlignCenter, idText);
}

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

QList<SmoothEdge *> SmoothNode::getIncomingEdges() { return incomingEdges_; }

QList<SmoothEdge *> SmoothNode::getOutcomingEdges() { return outgoingEdges_; }
