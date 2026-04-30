// MainWindow.cpp
#include "MainWindow.hpp"
#include "Figures.hpp"
#include "MenuBar.hpp"
#include "ThemeManager.hpp"

#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>

/**
 * @brief Конструктор главного окна.
 * @param themeMng Менеджер тем для управления оформлением.
 * @param parent Родительский виджет (по умолчанию nullptr).
 */
MainWindow::MainWindow(ThemeManager &themeMng, QWidget *parent)
    : QWidget(parent), graph_(new Graph()) {
  themeMng_ = &themeMng;

  // Создаём сцену для графа
  QGraphicsScene *scene = new QGraphicsScene(this);
  scene->setSceneRect(-200, -200, 400, 400);

  // Создаём представление графа
  graphView_ = new GraphView(scene, this);
  graphView_->setRenderHint(QPainter::Antialiasing);
  graphView_->setDragMode(QGraphicsView::RubberBandDrag);

  // Создаём менюбар
  menuBar_ = new MenuBar(this);

  // Устанавливаем начальные размеры и позиции
  graphView_->setGeometry(0, 0, width(), height());
  menuBar_->setGeometry(10, 10, width() - 20, menuBar_->maxHeight() + 40);

  // Поднимаем менюбар на передний план
  menuBar_->raise();

  // ========== Подключаем сигналы от GraphView к Graph ==========
  // Сигнал добавления узла
  connect(graphView_, &GraphView::nodeAdded, this, [this](SmoothNode *node) {
    if (node) {
      graph_->addNode(node);
      qDebug() << "Node added to graph with ID:" << node->getId();
    }
  });

  // Сигнал добавления ребра
  connect(graphView_, &GraphView::edgeAdded, this, [this](SmoothEdge *edge) {
    if (edge) {
      graph_->addEdge(edge);
      qDebug() << "Edge added to graph";
    }
  });

  // Сигнал удаления узла
  connect(graphView_, &GraphView::nodeRemoved, this, [this](SmoothNode *node) {
    if (node) {
      ID id = graph_->getNodeId(node);
      if (id != 0) {
        graph_->deleteNode(id);
        qDebug() << "Node removed from graph with ID:" << id;
      }
    }
  });

  // Сигнал удаления ребра
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
  // ========== Конец подключения сигналов ==========

  // Подключаем сигнал изменения темы от меню
  if (menuBar_->getThemeBtn() && menuBar_->getThemeBtn()->menu()) {
    connect(menuBar_->getThemeBtn()->menu(), &QMenu::triggered,
            [this](QAction *action) {
              Theme theme = static_cast<Theme>(action->data().toInt());
              themeMng_->setTheme(theme);
            });
  }

  // Подключаем сигналы для кнопок сохранения и открытия
  connect(menuBar_->getSaveBtn(), &QPushButton::clicked, this,
          &MainWindow::onSaveGraph);
  connect(menuBar_->getOpenBtn(), &QPushButton::clicked, this,
          &MainWindow::onOpenGraph);

  // Подключаем сигнал изменения темы для обновления UI
  connect(themeMng_, &ThemeManager::themeChanged, this,
          &MainWindow::onThemeChanged);

  // Применяем начальную тему
  updateStyle();
}

/**
 * @brief Обработчик события изменения размера окна.
 * @param event Событие изменения размера.
 *
 * Адаптирует размеры GraphView и MenuBar при изменении размера окна.
 */
void MainWindow::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);
  if (graphView_) {
    graphView_->setGeometry(0, 0, width(), height());
  }
  if (menuBar_) {
    menuBar_->setGeometry(10, 10, width() - 20, menuBar_->height());
  }
}

/**
 * @brief Слот для обработки изменения темы.
 *
 * Вызывает updateStyle() для применения новой темы.
 */
void MainWindow::onThemeChanged() { updateStyle(); }

/**
 * @brief Генерирует глобальную таблицу стилей для всего приложения.
 * @return QString с CSS стилями.
 */
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

/**
 * @brief Генерирует таблицу стилей для менюбара.
 * @return QString с CSS стилями.
 */
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

/**
 * @brief Генерирует таблицу стилей для графового представления.
 * @return QString с CSS стилями.
 */
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

/**
 * @brief Обновляет цвета всех узлов и рёбер на сцене.
 */
void MainWindow::updateGraphColors() {
  if (!graphView_ || !graphView_->scene())
    return;

  ThemeColors colors = themeMng_->getThemeColors();

  QList<QGraphicsItem *> items = graphView_->scene()->items();
  for (QGraphicsItem *item : items) {
    SmoothNode *figure = dynamic_cast<SmoothNode *>(item);
    if (figure) {
      figure->updateThemeStyle(colors);
    }

    SmoothEdge *edge = dynamic_cast<SmoothEdge *>(item);
    if (edge) {
      edge->updateThemeStyle(colors);
    }
  }
}

/**
 * @brief Обновляет стиль всего окна и всех компонентов.
 */
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

/**
 * @brief Отображает уведомление пользователю.
 * @param message Текст уведомления.
 * @param isError Флаг ошибки (true) или информации (false).
 */
void MainWindow::showNotification(const QString &message, bool isError) {
  QMessageBox msgBox;
  msgBox.setWindowTitle(isError ? "Ошибка" : "Информация");
  msgBox.setText(message);
  msgBox.setIcon(isError ? QMessageBox::Critical : QMessageBox::Information);
  msgBox.exec();
}

/**
 * @brief Сохраняет граф в файл.
 * @param filepath Путь к файлу для сохранения.
 * @return true если сохранение успешно, false в противном случае.
 */
bool MainWindow::saveGraphToFile(const QString &filepath) {
  std::string stdPath = filepath.toStdString();

  // Собираем данные из сцены
  QGraphicsScene *scene = graphView_->scene();
  if (!scene)
    return false;

  // Очищаем граф (но сохраняем ID существующих узлов)
  graph_->clear();

  // Собираем узлы и назначаем ID
  QList<QGraphicsItem *> items = scene->items();
  std::unordered_map<SmoothNode *, size_t> nodeToId;
  size_t nextId = 1;

  for (QGraphicsItem *item : items) {
    SmoothNode *node = dynamic_cast<SmoothNode *>(item);
    if (node) {
      // Используем существующий ID или создаём новый
      size_t id = node->getId();
      if (id == 0) {
        id = nextId++;
      } else {
        if (id >= nextId)
          nextId = id + 1;
      }
      nodeToId[node] = id;
      node->setId(id);
      graph_->addNode(node);
    }
  }

  // Собираем рёбра
  for (QGraphicsItem *item : items) {
    SmoothEdge *edge = dynamic_cast<SmoothEdge *>(item);
    if (edge && nodeToId.count(edge->getStartNode()) &&
        nodeToId.count(edge->getEndNode())) {
      edge->setWeight(edge->getWeight()); // Сохраняем вес
      graph_->addEdge(edge);
    }
  }

  bool success = graph_->saveToFile(stdPath);
  if (success) {
    showNotification(
        QString("Граф успешно сохранён в файл:\n%1").arg(filepath));
  } else {
    showNotification("Ошибка при сохранении графа!", true);
  }

  return success;
}

/**
 * @brief Загружает граф из файла.
 * @param filepath Путь к файлу для загрузки.
 * @return true если загрузка успешна, false в противном случае.
 */
bool MainWindow::loadGraphFromFile(const QString &filepath) {
  std::string stdPath = filepath.toStdString();

  if (graphView_) {
    graphView_->clearScene();
  }

  // Загружаем граф из файла
  QGraphicsScene *scene = graphView_->scene();
  bool success = graph_->loadFromFile(stdPath, scene);

  if (success) {
    showNotification(
        QString("Граф успешно загружен из файла:\n%1").arg(filepath));
  } else {
    showNotification("Ошибка при загрузке графа!", true);
  }

  return success;
}

/**
 * @brief Слот для сохранения графа.
 *
 * Реализует умное сохранение:
 * - Если граф загружен из файла и не модифицирован - сохраняет в тот же файл
 * - Если граф новый или нужно сохранить как - вызывает диалог выбора файла
 * - После сохранения выводит уведомление с путём к файлу
 */
void MainWindow::onSaveGraph() {
  QString filepath;

  // Если граф уже был сохранён или загружен из файла
  if (!graph_->getCurrentFilePath().empty()) {
    // Сохраняем в тот же файл
    saveGraphToFile(QString::fromStdString(graph_->getCurrentFilePath()));
    return;
  }

  // Иначе запрашиваем новое имя файла
  filepath = QFileDialog::getSaveFileName(this, "Сохранить граф", QString(),
                                          "Graph Files (*.gphz)");

  if (!filepath.isEmpty()) {
    // Добавляем расширение если его нет
    if (!filepath.endsWith(".gphz", Qt::CaseInsensitive)) {
      filepath += ".gphz";
    }
    saveGraphToFile(filepath);
  }
}

/**
 * @brief Слот для открытия графа из файла.
 *
 * - Проверяет наличие несохранённых изменений
 * - Открывает диалог выбора файла (.gphz)
 * - Загружает и отображает граф
 */
void MainWindow::onOpenGraph() {
  // Проверяем, не потеряются ли несохранённые изменения
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

/**
 * @brief Деструктор главного окна.
 */
MainWindow::~MainWindow() { delete graph_; }
