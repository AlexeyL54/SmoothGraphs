#include "MainWindow.hpp"
#include "Figures.hpp"
#include "MenuBar.hpp"
#include "ThemeManager.hpp"
#include "qevent.h"
#include "qgraphicsscene.h"
#include "qgraphicsview.h"
#include "qwidget.h"
#include <QVBoxLayout>

MainWindow::MainWindow(ThemeManager &themeMng, QWidget *parent)
    : QWidget(parent) {
  themeMng_ = &themeMng;

  QGraphicsScene *scene = new QGraphicsScene(this);
  scene->setSceneRect(-200, -200, 400, 400);

  graphView_ = new GraphView(scene, this);
  graphView_->setRenderHint(QPainter::Antialiasing);
  graphView_->setDragMode(QGraphicsView::RubberBandDrag);

  menuBar_ = new MenuBar(this);

  graphView_->setGeometry(0, 0, width(), height());
  menuBar_->setGeometry(10, 10, width() - 20, menuBar_->maxHeight() + 40);

  menuBar_->raise();

  // Подключаем сигнал изменения темы от меню
  if (menuBar_->getThemeBtn() && menuBar_->getThemeBtn()->menu()) {
    connect(menuBar_->getThemeBtn()->menu(), &QMenu::triggered,
            [this](QAction *action) {
              Theme theme = static_cast<Theme>(action->data().toInt());
              themeMng_->setTheme(theme);
            });
  }

  // Подключаем сигнал изменения темы для обновления UI
  connect(themeMng_, &ThemeManager::themeChanged, this,
          &MainWindow::onThemeChanged);

  // Применяем начальную тему
  updateStyle();
}

// Остальные методы MainWindow остаются без изменений...
void MainWindow::onThemeChanged() { updateStyle(); }

QString MainWindow::generateGlobalStyleSheet() const {
  ThemeColors colors = themeMng_->getThemeColors();

  return QString(R"(
    QWidget {
        background-color: %1;
        color: %2;
        font-family: 'Segoe UI', 'Microsoft YaHei', sans-serif;
    }
    
    QPushButton {
        background-color: %3;
        color: %2;
        border: 1px solid %4;
        border-radius: 6px;
        padding: 6px 12px;
        font-size: 13px;
        font-weight: 500;
    }
    
    QPushButton:hover {
        background-color: %5;
        border-color: %6;
    }
    
    QPushButton:pressed {
        background-color: %7;
    }
    
    QPushButton:disabled {
        background-color: %8;
        color: %9;
        border-color: %4;
    }
    
    QMenu {
        background-color: %1;
        color: %2;
        border: 1px solid %4;
        border-radius: 6px;
        padding: 5px;
    }
    
    QMenu::item {
        padding: 6px 30px 6px 20px;
        border-radius: 4px;
    }
    
    QMenu::item:selected {
        background-color: %5;
        color: %2;
    }
    
    QMenu::separator {
        height: 1px;
        background-color: %4;
        margin: 5px 10px;
    }
    
    QDialog {
        background-color: %1;
    }
    
    QMessageBox {
        background-color: %1;
    }
    
    QLineEdit, QTextEdit, QPlainTextEdit {
        background-color: %8;
        color: %2;
        border: 1px solid %4;
        border-radius: 4px;
        padding: 5px;
        selection-background-color: %6;
    }
    
    QLineEdit:focus, QTextEdit:focus, QPlainTextEdit:focus {
        border-color: %6;
        outline: none;
    }
    
    QListWidget, QTreeWidget, QTableWidget {
        background-color: %8;
        color: %2;
        border: 1px solid %4;
        border-radius: 4px;
        outline: none;
    }
    
    QListWidget::item:selected, QTreeWidget::item:selected, QTableWidget::item:selected {
        background-color: %5;
        color: %2;
    }
    
    QListWidget::item:hover, QTreeWidget::item:hover, QTableWidget::item:hover {
        background-color: %10;
    }
    
    QScrollBar:vertical {
        background-color: %8;
        width: 12px;
        border-radius: 6px;
        margin: 0px;
    }
    
    QScrollBar::handle:vertical {
        background-color: %3;
        border-radius: 6px;
        min-height: 20px;
    }
    
    QScrollBar::handle:vertical:hover {
        background-color: %5;
    }
    
    QScrollBar:horizontal {
        background-color: %8;
        height: 12px;
        border-radius: 6px;
        margin: 0px;
    }
    
    QScrollBar::handle:horizontal {
        background-color: %3;
        border-radius: 6px;
        min-width: 20px;
    }
    
    QScrollBar::handle:horizontal:hover {
        background-color: %5;
    }
    
    QStatusBar {
        background-color: %8;
        color: %2;
        border-top: 1px solid %4;
    }
    
    QToolTip {
        background-color: %1;
        color: %2;
        border: 1px solid %4;
        border-radius: 4px;
        padding: 4px;
    }
  )")
      .arg(colors.background.name())
      .arg(colors.textPrimary.name())
      .arg(colors.primary.name())
      .arg(colors.border.name())
      .arg(colors.hover.name())
      .arg(colors.primaryLight.name())
      .arg(colors.pressed.name())
      .arg(colors.surface.name())
      .arg(colors.textDisabled.name())
      .arg(colors.hover.name());
}

QString MainWindow::generateMenuBarStyleSheet() const {
  ThemeColors colors = themeMng_->getThemeColors();

  return QString(R"(
    #menubar {
        background-color: %1;
        border-radius: 15px;
        padding: 5px;
        border: 1px solid %2;
    }
    
    QPushButton {
        background-color: transparent;
        color: %3;
        border: none;
        padding: 8px 15px;
        font-size: 14px;
        border-radius: 8px;
    }
    
    QPushButton:hover {
        background-color: %4;
    }
    
    QPushButton:pressed {
        background-color: %5;
        color: %3;
    }
    
    /* Стили для QToolButton */
    QToolButton {
        background-color: transparent;
        color: %3;
        border: none;
        padding: 8px 15px;
        font-size: 14px;
        border-radius: 8px;
    }
    
    QToolButton:hover {
        background-color: %4;
    }
    
    QToolButton:pressed {
        background-color: %5;
        color: %3;
    }
    
    /* Убираем стандартный индикатор меню */
    QToolButton::menu-indicator {
        image: none;
    }
    
    /* Стили для меню */
    QMenu {
        background-color: %6;
        color: %3;
        border: 1px solid %2;
        border-radius: 8px;
        padding: 5px;
    }
    
    QMenu::item {
        padding: 8px 30px 8px 15px;
        border-radius: 4px;
    }
    
    QMenu::item:selected {
        background-color: %4;
        color: %3;
    }
    
    QMenu::separator {
        height: 1px;
        background-color: %2;
        margin: 5px 10px;
    }
    
    #wrapBtn {
        background-color: %7;
        border: 2px solid %8;
        border-radius: 15px;
    }
    
    #wrapBtn:hover {
        background-color: %8;
    }
  )")
      .arg(colors.surface.name())
      .arg(colors.border.name())
      .arg(colors.textPrimary.name())
      .arg(colors.hover.name())
      .arg(colors.primary.name())
      .arg(colors.background.name())
      .arg(colors.background.name())
      .arg(colors.border.name());
}

QString MainWindow::generateGraphViewStyleSheet() const {
  ThemeColors colors = themeMng_->getThemeColors();

  return QString(R"(
    QGraphicsView {
        background-color: %1;
        border: none;
        outline: none;
    }
  )")
      .arg(colors.background.name());
}

void MainWindow::updateGraphColors() {
  if (!graphView_ || !graphView_->scene())
    return;

  ThemeColors colors = themeMng_->getThemeColors();

  QList<QGraphicsItem *> items = graphView_->scene()->items();
  for (QGraphicsItem *item : items) {
    Figure *figure = dynamic_cast<Figure *>(item);
    if (figure) {
      figure->setBrush(colors.nodeDefault);
      figure->setPen(QPen(colors.border, 1));
      figure->setHoverColor(colors.nodeHover);
    }

    Edge *edge = dynamic_cast<Edge *>(item);
    if (edge) {
      edge->setPen(QPen(colors.edgeDefault, 2));
    }
  }
}

void MainWindow::updateStyle() {
  // Применяем глобальный стиль к приложению
  qApp->setStyleSheet(generateGlobalStyleSheet());

  // Применяем специфичные стили к виджетам
  if (menuBar_) {
    menuBar_->setStyleSheet(generateMenuBarStyleSheet());
  }

  if (graphView_) {
    graphView_->setStyleSheet(generateGraphViewStyleSheet());
    updateGraphColors();
  }
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
