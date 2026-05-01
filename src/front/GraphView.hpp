// GraphView.hpp
#pragma once

#include <QContextMenuEvent>
#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QPointF>

#include "Figures.hpp"

/**
 * @class GraphView
 * @brief Представление графа на основе QGraphicsView.
 */
class GraphView : public QGraphicsView {
  Q_OBJECT

public:
  explicit GraphView(QGraphicsScene *scene, QWidget *parent = nullptr);
  void startEdgeCreation(SmoothNode *startNode);
  void updateAllElementsTheme(const ThemeColors &colors);
  void clearScene();
  void setStartNode(SmoothNode *node);
  void setEndNode(SmoothNode *node);
  void clearStartNode(SmoothNode *node);
  void clearEndNode(SmoothNode *node);
  SmoothNode *getStartNode() const { return pathStartNode_; }
  SmoothNode *getEndNode() const { return pathEndNode_; }
  void highlightPath(const std::vector<SmoothNode *> &path);
  void clearPathHighlight();

signals:
  void nodeAdded(SmoothNode *node);
  void edgeAdded(SmoothEdge *edge);
  void nodeRemoved(SmoothNode *node);
  void edgeRemoved(SmoothEdge *edge);
  void startNodeChanged(SmoothNode *node);
  void endNodeChanged(SmoothNode *node);
  void findPathRequested();

protected:
  void contextMenuEvent(QContextMenuEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;

private:
  QGraphicsScene *scene_;
  bool isCreatingEdge_ = false;
  SmoothNode *tempStartNode_ = nullptr;
  QGraphicsLineItem *tempEdge_ = nullptr;
  QPointF startPos_;

  SmoothNode *pathStartNode_ = nullptr;
  SmoothNode *pathEndNode_ = nullptr;
  std::vector<SmoothNode *> currentPath_;

private slots:
  void addFigure();
  void setupNodeSelectionBridge();
};
