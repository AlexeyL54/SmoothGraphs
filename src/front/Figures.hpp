#pragma once

#include "qpoint.h"
#include <QColor>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsScale>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsView>
#include <QList>
#include <QMenu>
#include <QObject>
#include <QPainter>
#include <QPen>
#include <QPointF>
#include <cstddef>

class SmoothNode;

// Вспомогательный класс для связи узлов с GraphView
class NodeSelectionBridge : public QObject {
  Q_OBJECT
public:
  static NodeSelectionBridge *instance() {
    static NodeSelectionBridge bridge;
    return &bridge;
  }

signals:
  void setStartNodeRequested(SmoothNode *node);
  void setEndNodeRequested(SmoothNode *node);
  void clearStartNodeRequested(SmoothNode *node);
  void clearEndNodeRequested(SmoothNode *node);
};

/**
 * @class SmoothEdge
 * @brief Класс, представляющий ребро графа между двумя узлами.
 */
class SmoothEdge : public QGraphicsLineItem {
public:
  SmoothEdge(SmoothNode *start, SmoothNode *end,
             QGraphicsItem *parent = nullptr);
  ~SmoothEdge();

  void updatePosition();
  SmoothNode *getStartNode() const { return startNode_; }
  SmoothNode *getEndNode() const { return endNode_; }
  void setWeight(float weight);
  float getWeight() const { return weight_; }
  void updateThemeStyle(const class ThemeColors &colors);
  void setHighlighted(bool highlight);
  bool isHighlighted() const { return isHighlighted_; }
  QRectF boundingRect() const override;

protected:
  void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *widget) override;

private:
  SmoothNode *startNode_;
  SmoothNode *endNode_;
  float weight_;
  QColor defaultColor_;
  QColor highlightColor_;
  QColor textColor_;
  bool isHighlighted_ = false;

  std::pair<QPointF, QPointF> computeArrowPos(QLineF &line);
  void paintArrow(QPainter *painter, QLineF &line, QPointF p1, QPointF p2);
  void paintWeight(QPainter *painter, const QLineF &line);
  void showWeightDialog();

  mutable QPolygonF cachedArrowHead_;
  mutable QLineF cachedLine_;
};

///////////////////////////////////////////////////////////////////////////

enum class NodeRole { Normal, Start, End };

class SmoothNode : public QGraphicsEllipseItem {
public:
  SmoothNode(qreal x, qreal y, qreal radius, QGraphicsItem *parent = nullptr);

  void addIncomingEdge(SmoothEdge *edge);
  void addOutgoingEdge(SmoothEdge *edge);
  void removeIncomingEdge(SmoothEdge *edge);
  void removeOutgoingEdge(SmoothEdge *edge);
  void clearIncomingEdges();
  void clearOutcomingEdges();

  QPointF getCenter() const;

  void setHoverColor(const QColor &color);
  void updateThemeStyle(const class ThemeColors &colors);
  void restoreDefaultColor();
  void updateNodeColor();

  QList<SmoothEdge *> getIncomingEdges();
  QList<SmoothEdge *> getOutcomingEdges();

  void setId(size_t id) { id_ = id; }
  size_t getId() const { return id_; }

  void setRole(NodeRole role);
  NodeRole getRole() const { return role_; }
  void resetPathHighlight();
  qreal getRadius() const { return rect().width() / 2; }

protected:
  void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
  void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
  QVariant itemChange(GraphicsItemChange change,
                      const QVariant &value) override;

private:
  QList<SmoothEdge *> incomingEdges_;
  QList<SmoothEdge *> outgoingEdges_;

  QColor defaultColor_;
  QColor hoverColor_;
  QColor borderColor_;
  QColor startNodeColor_;
  QColor endNodeColor_;
  QColor pathNodeColor_;

  size_t id_;
  NodeRole role_ = NodeRole::Normal;
  bool isOnPath_ = false;

  void addEdge();
  void setAsStart();
  void setAsEnd();
  void clearStart();
  void clearEnd();
  QGraphicsView *getParentView() const;
};
