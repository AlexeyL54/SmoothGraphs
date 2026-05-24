// MainWindow.hpp
#pragma once

#include <QApplication>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "../back/Graph.hpp"
#include "../back/Logger.hpp"
#include "GraphView.hpp"
#include "MenuBar.hpp"
#include "ThemeManager.hpp"

class MainWindow : public QWidget {
  Q_OBJECT

public:
  explicit MainWindow(ThemeManager &themeMng, QWidget *parent = nullptr);
  void updateStyle();
  ~MainWindow();
  MenuBar *getMenuBar() const { return menuBar_; }

private:
  QGraphicsScene *scene_;
  GraphView *graphView_ = nullptr;
  MenuBar *menuBar_ = nullptr;
  ThemeManager *themeMng_;
  Graph *graph_;
  Logger logger_;

  QString generateGlobalStyleSheet() const;
  QString generateMenuBarStyleSheet() const;
  QString generateGraphViewStyleSheet() const;
  void updateGraphColors();
  void showNotification(const QString &message, bool isError = false);
  bool saveGraphToFile(const QString &filepath);
  bool loadGraphFromFile(const QString &filepath);
  void findAndVisualizePath();
  bool saveSolutionToFile(const QString &filepath);
  bool showCyrillicWarning(const QString filepath);

protected:
  void resizeEvent(QResizeEvent *event) override;

private slots:
  void onThemeChanged();
  void onSaveGraph();
  void onSaveSolution();
  void onOpenGraph();
  void onFindPath();
  void onStopPath();
};
