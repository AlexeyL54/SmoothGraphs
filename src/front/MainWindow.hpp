#pragma once

#include <QApplication>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "GraphView.hpp"
#include "MenuBar.hpp"
#include "qevent.h"

class MainWindow : public QWidget {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:

private:
  QGraphicsScene *scene_;
  GraphView *graphView_ = nullptr;
  MenuBar *menuBar_ = nullptr;

protected:
  void resizeEvent(QResizeEvent *event) override;
};
