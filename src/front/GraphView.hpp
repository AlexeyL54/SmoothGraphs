#pragma once

#include <QContextMenuEvent>
#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QPointF>
#include <QPushButton>

#include "Figures.hpp"
#include "ThemeManager.hpp"

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
  void setThemeManager(ThemeManager *manager) { themeMng_ = manager; }
  ThemeManager *getThemeManager() const { return themeMng_; }

  void zoomIn();
  void zoomOut();

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
  void resizeEvent(QResizeEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;

private:
  QGraphicsScene *scene_;
  bool isCreatingEdge_ = false;
  SmoothNode *tempStartNode_ = nullptr;
  QGraphicsLineItem *tempEdge_ = nullptr;
  QPointF startPos_;

  SmoothNode *pathStartNode_ = nullptr;
  SmoothNode *pathEndNode_ = nullptr;
  std::vector<SmoothNode *> currentPath_;

  ThemeManager *themeMng_ = nullptr;

  // Кнопки масштабирования
  QPushButton *zoomInBtn_;
  QPushButton *zoomOutBtn_;

  QPushButton *helpBtn_;

  qreal currentZoom_ = 1.0;
  const qreal ZOOM_STEP = 1.25;
  const qreal MIN_ZOOM = 0.2;
  const qreal MAX_ZOOM = 5.0;

  void setupZoomButtons();
  void setupHelpButton();
  void updateButtonsPosition();
  void showHelpDialog();

private slots:
  void addFigure(const QPointF &pos);
  void setupNodeSelectionBridge();
};
