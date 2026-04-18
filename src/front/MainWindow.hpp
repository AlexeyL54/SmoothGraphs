#pragma once

#include <QStackedWidget>
#include <QWidget>

#include "Figures.hpp"
#include "GraphView.hpp"
#include "MenuBar.hpp"

class MainWindow : public QWidget {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:

private:
  QStackedWidget *stackedWidget = nullptr;
};
