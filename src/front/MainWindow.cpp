// MainWindow.cpp
#include "MainWindow.hpp"
#include "Figures.hpp"
#include "MenuBar.hpp"
#include "ThemeManager.hpp"

#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>

MainWindow::MainWindow(ThemeManager &themeMng, QWidget *parent)
    : QWidget(parent), graph_(new Graph()) {
  themeMng_ = &themeMng;

  QGraphicsScene *scene = new QGraphicsScene(this);
  scene->setSceneRect(-200, -200, 400, 400);

  graphView_ = new GraphView(scene, this);
  graphView_->setRenderHint(QPainter::Antialiasing);
  graphView_->setDragMode(QGraphicsView::RubberBandDrag);
  graphView_->setThemeManager(&themeMng);

  menuBar_ = new MenuBar(this);

  graphView_->setGeometry(0, 0, width(), height());
  menuBar_->setGeometry(10, 10, width() - 20, menuBar_->maxHeight() + 40);
  menuBar_->raise();

  connect(graphView_, &GraphView::nodeAdded, this, [this](SmoothNode *node) {
    if (node) {
      graph_->addNode(node);
      qDebug() << "Node added to graph with ID:" << node->getId();
    }
  });

  connect(graphView_, &GraphView::edgeAdded, this, [this](SmoothEdge *edge) {
    if (edge) {
      graph_->addEdge(edge);
      qDebug() << "Edge added to graph";
    }
  });

  connect(graphView_, &GraphView::nodeRemoved, this, [this](SmoothNode *node) {
    if (node) {
      ID id = graph_->getNodeId(node);
      if (id != 0) {
        graph_->deleteNode(id);
        qDebug() << "Node removed from graph with ID:" << id;
      }
    }
  });

  connect(graphView_, &GraphView::edgeRemoved, this, [this](SmoothEdge *edge) {
    if (edge) {
      SmoothNode *start = edge->getStartNode();
      SmoothNode *end = edge->getEndNode();
      if (start && end) {
        ID from = graph_->getNodeId(start);
        ID to = graph_->getNodeId(end);
        if (from != 0 && to != 0) {
          graph_->deleteEdge(from, to);
          qDebug() << "Edge removed from graph:" << from << "->" << to;
        }
      }
    }
  });

  if (menuBar_->getThemeBtn() && menuBar_->getThemeBtn()->menu()) {
    connect(menuBar_->getThemeBtn()->menu(), &QMenu::triggered,
            [this](QAction *action) {
              Theme theme = static_cast<Theme>(action->data().toInt());
              themeMng_->setTheme(theme);
            });
  }

  connect(menuBar_->getSaveBtn(), &QPushButton::clicked, this,
          &MainWindow::onSaveGraph);
  connect(menuBar_->getOpenBtn(), &QPushButton::clicked, this,
          &MainWindow::onOpenGraph);
  connect(menuBar_->getRunBtn(), &QPushButton::clicked, this,
          &MainWindow::onFindPath);
  connect(menuBar_->getStopBtn(), &QPushButton::clicked, this,
          &MainWindow::onStopPath);

  connect(themeMng_, &ThemeManager::themeChanged, this,
          &MainWindow::onThemeChanged);

  updateStyle();
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
    
    QToolButton::menu-indicator {
        image: none;
    }
    
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
  graphView_->updateAllElementsTheme(colors);
}

void MainWindow::updateStyle() {
  qApp->setStyleSheet(generateGlobalStyleSheet());

  if (menuBar_) {
    menuBar_->setStyleSheet(generateMenuBarStyleSheet());
  }

  if (graphView_) {
    graphView_->setStyleSheet(generateGraphViewStyleSheet());
    updateGraphColors();
  }
}

void MainWindow::showNotification(const QString &message, bool isError) {
  QMessageBox msgBox;
  msgBox.setWindowTitle(isError ? "Ошибка" : "Информация");
  msgBox.setText(message);
  msgBox.setIcon(isError ? QMessageBox::Critical : QMessageBox::Information);
  msgBox.exec();
}

bool MainWindow::saveGraphToFile(const QString &filepath) {
  std::string stdPath = filepath.toStdString();

  QGraphicsScene *scene = graphView_->scene();
  if (!scene)
    return false;

  bool success = graph_->saveToFile(stdPath);
  if (success) {
    showNotification(
        QString("Граф успешно сохранён в файл:\n%1").arg(filepath));
  } else {
    showNotification("Ошибка при сохранении графа!", true);
  }

  return success;
}

bool MainWindow::loadGraphFromFile(const QString &filepath) {
  std::string stdPath = filepath.toStdString();

  if (graphView_) {
    graphView_->clearScene();
  }

  QGraphicsScene *scene = graphView_->scene();
  bool success = graph_->loadFromFile(stdPath, scene);

  if (success) {
    showNotification(
        QString("Граф успешно загружен из файла:\n%1").arg(filepath));
    updateGraphColors();
  } else {
    showNotification("Ошибка при загрузке графа!", true);
  }

  return success;
}

void MainWindow::findAndVisualizePath() {
  SmoothNode *startNode = graphView_->getStartNode();
  SmoothNode *endNode = graphView_->getEndNode();

  if (!startNode) {
    showNotification("Не выбран стартовый узел! Назначьте стартовый узел через "
                     "контекстное меню.",
                     true);
    return;
  }

  if (!endNode) {
    showNotification("Не выбран конечный узел! Назначьте конечный узел через "
                     "контекстное меню.",
                     true);
    return;
  }

  if (startNode == endNode) {
    showNotification("Стартовый и конечный узлы не могут совпадать!", true);
    return;
  }

  std::vector<SmoothNode *> path = graph_->findShortestPath(startNode, endNode);

  if (path.size() <= 1) {
    showNotification("Путь между выбранными узлами не найден!", true);
    return;
  }

  graphView_->highlightPath(path);
  showNotification(
      QString("Кратчайший путь найден! Длина пути: %1 узлов").arg(path.size()));
}

void MainWindow::onSaveGraph() {
  QString filepath;

  if (!graph_->getCurrentFilePath().empty()) {
    saveGraphToFile(QString::fromStdString(graph_->getCurrentFilePath()));
    return;
  }

  filepath = QFileDialog::getSaveFileName(this, "Сохранить граф", QString(),
                                          "Graph Files (*.gphz)");

  if (!filepath.isEmpty()) {
    if (!filepath.endsWith(".gphz", Qt::CaseInsensitive)) {
      filepath += ".gphz";
    }
    saveGraphToFile(filepath);
  }
}

void MainWindow::onOpenGraph() {
  if (graph_->isModified() && !graph_->getCurrentFilePath().empty()) {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Несохранённые изменения",
        "У вас есть несохранённые изменения. Открыть другой файл?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No) {
      return;
    }
  }

  QString filepath = QFileDialog::getOpenFileName(
      this, "Открыть граф", QString(), "Graph Files (*.gphz)");

  if (!filepath.isEmpty()) {
    loadGraphFromFile(filepath);
  }
}

void MainWindow::onFindPath() { findAndVisualizePath(); }

void MainWindow::onStopPath() {
  graphView_->clearPathHighlight();
  showNotification("Выделение пути снято");
}

MainWindow::~MainWindow() { delete graph_; }
