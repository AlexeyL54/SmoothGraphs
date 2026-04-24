#include "MainWindow.hpp"
#include "MenuBar.hpp"
#include "qevent.h"
#include "qgraphicsscene.h"
#include "qgraphicsview.h"
#include "qwidget.h"
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QWidget(parent) {
  QGraphicsScene *scene = new QGraphicsScene(this);
  scene->setSceneRect(-200, -200, 400, 400);

  graphView_ = new GraphView(scene, this);
  graphView_->setRenderHint(QPainter::Antialiasing);
  graphView_->setDragMode(QGraphicsView::RubberBandDrag);

  menuBar_ = new MenuBar(this);

  graphView_->setGeometry(0, 0, width(), height());

  menuBar_->setGeometry(10, 10, width() - 20, menuBar_->maxHeight() + 40);

  //  Важно: поднимаем MenuBar на передний план, чтобы клики по нему работали
  menuBar_->raise();
}

void MainWindow::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);
  if (graphView_) {
    graphView_->setGeometry(0, 0, width(), height());
  }
  if (menuBar_) {
    menuBar_->setGeometry(10, 10, width() - 20, menuBar_->height());
  }
}

MainWindow::~MainWindow() {}
