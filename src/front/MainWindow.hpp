#pragma once

#include <QApplication>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "GraphView.hpp"
#include "MenuBar.hpp"
#include "ThemeManager.hpp"

class MainWindow : public QWidget {
  Q_OBJECT

public:
  explicit MainWindow(ThemeManager &themeMng, QWidget *parent = nullptr);
  void updateStyle();
  ~MainWindow();

  // Геттер для доступа к menuBar (для подключения сигналов)
  MenuBar *getMenuBar() const { return menuBar_; }

private:
  QGraphicsScene *scene_;
  GraphView *graphView_ = nullptr;
  MenuBar *menuBar_ = nullptr;
  ThemeManager *themeMng_;

  QString generateGlobalStyleSheet() const;
  QString generateMenuBarStyleSheet() const;
  QString generateGraphViewStyleSheet() const;

  void updateGraphColors();

protected:
  void resizeEvent(QResizeEvent *event) override;

private slots:
  void onThemeChanged();
};
