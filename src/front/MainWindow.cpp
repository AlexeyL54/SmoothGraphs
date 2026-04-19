#include "MainWindow.hpp"
#include "MenuBar.hpp"
#include "qevent.h"
#include "qgraphicsscene.h"
#include "qgraphicsview.h"
#include "qwidget.h"
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QWidget(parent) {
  // 1. Создаем GraphView, который будет занимать всё окно
  QGraphicsScene *scene = new QGraphicsScene(this);
  scene->setSceneRect(-200, -200, 400, 400);

  graphView_ =
      new GraphView(scene, this); // Сохраните graphView_ как поле класса!
  graphView_->setRenderHint(QPainter::Antialiasing);
  graphView_->setDragMode(QGraphicsView::RubberBandDrag);

  // 2. Создаем MenuBar
  menuBar_ = new MenuBar(this); // Сохраните menuBar_ как поле класса!

  // 3. Настраиваем размеры и позиции
  // GraphView всегда занимает всё доступное место
  graphView_->setGeometry(0, 0, width(), height());

  // MenuBar позиционируем сверху.
  // Предположим, высота MenuBar около 60-80px (зависит от вашего MAX_HEIGHT_)
  // Вам нужно узнать желаемую высоту MenuBar.
  menuBar_->setGeometry(10, 10, width() - 20, menuBar_->maxHeight() + 40);

  // 4. Важно: поднимаем MenuBar на передний план, чтобы клики по нему работали
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
