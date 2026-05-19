#include "Figures.hpp"
#include "GraphView.hpp"
#include "ThemeManager.hpp"
#include "qlist.h"

#include <QApplication>
#include <QDebug>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsView>
#include <QMenu>
#include <QMouseEvent>
#include <QObject>
#include <QPainterPath>
#include <cmath>
#include <utility>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ==================== SmoothEdge Implementation ====================

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
    qDebug() << "Nodes doesn't exist in scene";
    return;
  }

  QPointF startCenter = startNode_->pos();
  QPointF endCenter = endNode_->pos();

  if (startNode_ == endNode_) {
    setLine(QLineF(startCenter.x() + 40, startCenter.y(), startCenter.x() + 80,
                   startCenter.y() - 40));
  } else {
    setLine(QLineF(startCenter, endCenter));
  }
}

void SmoothEdge::updateThemeStyle(const ThemeColors &colors) {
  defaultColor_ = colors.edgeDefault;
  highlightColor_ = colors.pathEdge;
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

std::pair<QPointF, QPointF> SmoothEdge::computeArrowPos(QLineF &line) {
  double angle = std::atan2(-line.dy(), line.dx());
  QPointF endPt = line.p2();
  qreal arrowSize = 20.0;

  QPointF arrowP1 = endPt - QPointF(std::sin(angle + M_PI / 3) * arrowSize,
                                    std::cos(angle + M_PI / 3) * arrowSize);
  QPointF arrowP2 =
      endPt - QPointF(std::sin(angle + M_PI - M_PI / 3) * arrowSize,
                      std::cos(angle + M_PI - M_PI / 3) * arrowSize);

  return std::make_pair(arrowP1, arrowP2);
}

void SmoothEdge::paintArrow(QPainter *painter, QLineF &line, QPointF p1,
                            QPointF p2) {
  painter->setPen(Qt::NoPen);
  painter->setBrush(pen().color());

  QPainterPath arrowPath;
  arrowPath.moveTo(line.p2());
  arrowPath.lineTo(p1);
  arrowPath.lineTo(p2);
  arrowPath.closeSubpath();

  painter->drawPath(arrowPath);
}

void SmoothEdge::paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) {
  Q_UNUSED(option);
  Q_UNUSED(widget);

  QLineF line = this->line();

  if (line.length() < 1e-5)
    return;

  painter->setPen(pen());
  painter->drawLine(line);

  std::pair<QPointF, QPointF> arrowP = computeArrowPos(line);
  paintArrow(painter, line, arrowP.first, arrowP.second);
}

void SmoothEdge::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
  QMenu menu;
  QAction *deleteAction = menu.addAction("Удалить ребро");
  QAction *weightAction = menu.addAction("Изменить вес");

  QObject::connect(deleteAction, &QAction::triggered, [this]() {
    if (scene())
      scene()->removeItem(this);
  });

  menu.exec(event->screenPos());
  event->accept();
}

// ==================== SmoothNode Implementation ====================

SmoothNode::SmoothNode(qreal centerX, qreal centerY, qreal radius,
                       QGraphicsItem *parent)
    : QGraphicsEllipseItem(centerX - radius, centerY - radius, radius * 2,
                           radius * 2, parent),
      id_(0) {
  setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
  setAcceptHoverEvents(true);
  setFlag(QGraphicsItem::ItemIsMovable);
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
  const auto &views = scene()->views();
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

void SmoothNode::updateThemeStyle(const ThemeColors &colors) {
  defaultColor_ = colors.nodeDefault;
  hoverColor_ = colors.nodeHover;
  borderColor_ = colors.border;
  startNodeColor_ = colors.startNode;
  endNodeColor_ = colors.endNode;
  pathNodeColor_ = colors.pathNode;

  setBrush(defaultColor_);
  setPen(QPen(borderColor_, 1));
  updateNodeColor();
  update();
}

QList<SmoothEdge *> SmoothNode::getIncomingEdges() { return incomingEdges_; }

QList<SmoothEdge *> SmoothNode::getOutcomingEdges() { return outgoingEdges_; }
