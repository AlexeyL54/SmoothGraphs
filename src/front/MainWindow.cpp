#include "MainWindow.hpp"
#include "Figures.hpp"
#include "MenuBar.hpp"
#include "StyleManager.hpp"
#include "ThemeManager.hpp"

#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>

/**
 * @brief Конструктор главного окна.
 * @param themeMng Менеджер тем оформления (ссылка).
 * @param parent Родительский виджет (по умолчанию nullptr).
 */
MainWindow::MainWindow(ThemeManager &themeMng, QWidget *parent)
    : QWidget(parent), logger_(true), graph_(new Graph(&logger_)),
      themeMng_(&themeMng) {
  setupGraphicsView();
  setupMenuBar();
  setupConnections();
  setupInitialGeometry();
  updateStyle();
}

/**
 * @brief Инициализирует графическую сцену и представление.
 *
 * Создает QGraphicsScene с предустановленной областью отображения,
 * настраивает GraphView, включая антиалиасинг, режим перетаскивания
 * и менеджер тем.
 */
void MainWindow::setupGraphicsView() {
  QGraphicsScene *scene = new QGraphicsScene(this);
  scene->setSceneRect(-200, -200, 400, 400);

  graphView_ = new GraphView(scene, this);
  graphView_->setRenderHint(QPainter::Antialiasing);
  graphView_->setDragMode(QGraphicsView::RubberBandDrag);
  graphView_->setThemeManager(themeMng_);
  graphView_->setGeometry(0, 0, width(), height());
}

/**
 * @brief Инициализирует меню-бар.
 *
 * Создает панель меню, устанавливает её начальное положение
 * и поднимает над другими виджетами.
 */
void MainWindow::setupMenuBar() {
  menuBar_ = new MenuBar(this);
  menuBar_->setGeometry(10, 10, width() - 20, menuBar_->maxHeight() + 40);
  menuBar_->raise();
}

/**
 * @brief Настраивает соединения сигналов и слотов.
 *
 * Вызывает методы для настройки соединений, связанных с графом,
 * UI компонентами и управлением темой.
 */
void MainWindow::setupConnections() {
  setupGraphConnections();
  setupUIComponentsConnections();
  setupThemeConnections();
}

/**
 * @brief Настраивает соединения для работы с графом.
 *
 * Подключает сигналы от GraphView (добавление/удаление узлов и рёбер,
 * очистка сцены) к соответствующим методам Graph.
 * Также подключает сигналы изменения структуры графа и обнаружения цикла.
 */
void MainWindow::setupGraphConnections() {
  // Добавление узлов
  connect(graphView_, &GraphView::nodeAdded, this, [this](SmoothNode *node) {
    if (node) {
      graph_->addNode(node);
      qDebug() << "Node added to graph with ID:" << node->getId();
    }
  });

  // Добавление рёбер
  connect(graphView_, &GraphView::edgeAdded, this, [this](SmoothEdge *edge) {
    if (edge) {
      graph_->addEdge(edge);
      qDebug() << "Edge added to graph";
    }
  });

  // Удаление узлов
  connect(graphView_, &GraphView::nodeRemoved, this, [this](SmoothNode *node) {
    if (node) {
      ID id = graph_->getNodeId(node);
      if (id != 0) {
        graph_->deleteNode(id);
        qDebug() << "Node removed from graph with ID:" << id;
      }
    }
  });

  // Удаление рёбер
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

  // Очистка сцены
  connect(graphView_, &GraphView::sceneCleared, this,
          [this]() { graph_->clear(); });

  // Изменение структуры графа
  connect(graph_, &Graph::graphStructureChanged, this,
          &MainWindow::onGraphChanged);

  // Обнаружение цикла
  connect(graph_, &Graph::loopFound, this, &MainWindow::onLoopFound);
}

/**
 * @brief Настраивает соединения для UI компонентов.
 *
 * Подключает сигналы от MenuBar и его кнопок к соответствующим слотам
 * для сохранения/открытия графа, сохранения решения, поиска пути
 * и очистки подсветки.
 */
void MainWindow::setupUIComponentsConnections() {
  // Сохранение графа
  connect(menuBar_, &MenuBar::saveGraphRequested, this,
          &MainWindow::onSaveGraph);

  // Сохранение решения
  connect(menuBar_, &MenuBar::saveSolutionRequested, this,
          &MainWindow::onSaveSolution);

  // Открытие графа
  connect(menuBar_->getOpenBtn(), &QPushButton::clicked, this,
          &MainWindow::onOpenGraph);

  // Поиск пути
  connect(menuBar_->getRunBtn(), &QPushButton::clicked, this,
          &MainWindow::onFindPath);

  // Остановка подсветки пути
  connect(menuBar_->getStopBtn(), &QPushButton::clicked, this,
          &MainWindow::onStopPath);
}

/**
 * @brief Настраивает соединения для обновления темы.
 *
 * Подключает сигналы от меню выбора темы для изменения текущей темы
 * и сигнал ThemeManager о смене темы для обновления стиля интерфейса.
 */
void MainWindow::setupThemeConnections() {
  if (menuBar_->getThemeBtn() && menuBar_->getThemeBtn()->menu()) {
    connect(menuBar_->getThemeBtn()->menu(), &QMenu::triggered,
            [this](QAction *action) {
              Theme theme = static_cast<Theme>(action->data().toInt());
              themeMng_->setTheme(theme);
            });
  }

  connect(themeMng_, &ThemeManager::themeChanged, this,
          &MainWindow::onThemeChanged);
}

/**
 * @brief Инициализирует геометрию виджетов.
 *
 * Устанавливает начальные размеры и положение GraphView и MenuBar.
 */
void MainWindow::setupInitialGeometry() {
  if (graphView_) {
    graphView_->setGeometry(0, 0, width(), height());
  }
  if (menuBar_) {
    menuBar_->setGeometry(10, 10, width() - 20, menuBar_->height());
  }
}

/**
 * @brief Слот для обработки изменения структуры графа.
 *
 * Инвалидирует кэшированные данные о найденном пути,
 * так как граф был изменён.
 */
void MainWindow::onGraphChanged() {
  qDebug() << "Graph structure changed. Invalidating cached path data.";
  hasValidPath_ = false;
}

/**
 * @brief Обрабатывает событие изменения размера окна.
 * @param event Событие изменения размера.
 *
 * Пересчитывает геометрию graphView_ и menuBar_ при изменении размеров окна.
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
 * @brief Слот для обновления интерфейса при смене темы.
 *
 * Вызывает updateStyle() для применения новой цветовой схемы
 * ко всем элементам интерфейса.
 */
void MainWindow::onThemeChanged() { updateStyle(); }

/**
 * @brief Сохраняет лог решения (результат поиска пути) в файл.
 * @param filepath Путь к файлу для сохранения.
 * @return true, если сохранение прошло успешно, иначе false.
 */
bool MainWindow::saveSolutionToFile(const QString &filepath) {
  if (logger_.getLog().isEmpty()) {
    showNotification("Нет данных о решении! Сначала выполните поиск пути.",
                     true);
    return false;
  }

  int result = logger_.saveToFile(filepath);

  if (result >= 0) {
    showNotification(
        QString("Решение успешно сохранено в файл:\n%1\n(Сообщений: %2)")
            .arg(filepath)
            .arg(result));
  } else {
    showNotification("Ошибка при сохранении решения!", true);
  }

  return result >= 0;
}

/**
 * @brief Слот для сохранения решения (лога) в файл.
 *
 * Проверяет наличие актуального решения (hasValidPath_)
 * и соответствие версии графа. Открывает диалог выбора файла
 * и вызывает saveSolutionToFile().
 */
void MainWindow::onSaveSolution() {
  // Было ли вообще найдено решение?
  if (!hasValidPath_) {
    showNotification("Нет актуального решения! Сначала выполните поиск пути.",
                     true);
    return;
  }

  // Не изменился ли граф с момента поиска?
  if (graph_->getRevision() != lastValidPathRevision_) {
    showNotification(
        "Граф был изменен после последнего поиска пути.\n"
        "Пожалуйста, выполните поиск пути заново перед сохранением решения.",
        true);
    return;
  }

  QString path = QDir::homePath();
  if (!solutionFocusDir_.isEmpty())
    path = solutionFocusDir_;

  QString filepath = QFileDialog::getSaveFileName(
      this, "Сохранить решение", path,
      "Text Files (*.txt);;Log Files (*.log);;All Files (*)");

  if (!filepath.isEmpty()) {
    if (!filepath.endsWith(".txt", Qt::CaseInsensitive) &&
        !filepath.endsWith(".log", Qt::CaseInsensitive)) {
      filepath += ".txt";
    }
    saveSolutionToFile(filepath);
    solutionFocusDir_ = QFileInfo(filepath).absolutePath();
  }
}

/**
 * @brief Обновляет цвета всех элементов графа согласно текущей теме.
 *
 * Получает текущие цвета темы и вызывает updateAllElementsTheme()
 * у GraphView для обновления цветов всех узлов и рёбер.
 */
void MainWindow::updateGraphColors() {
  if (!graphView_ || !graphView_->scene())
    return;

  ThemeColors colors = themeMng_->getThemeColors();
  graphView_->updateAllElementsTheme(colors);
}

/**
 * @brief Обновляет стиль всего окна и дочерних элементов.
 *
 * Применяет текущую тему к глобальной таблице стилей приложения,
 * а также к меню-бару и области графа. Вызывает обновление цветов графа.
 */
void MainWindow::updateStyle() {
  ThemeColors colors = themeMng_->getThemeColors();

  qApp->setStyleSheet(StyleManager::generateGlobalStyleSheet(colors));

  if (menuBar_) {
    menuBar_->setStyleSheet(StyleManager::generateMenuBarStyleSheet(colors));
  }

  if (graphView_) {
    graphView_->setStyleSheet(
        StyleManager::generateGraphViewStyleSheet(colors));
    updateGraphColors();
  }
}

/**
 * @brief Отображает всплывающее уведомление (информационное или об ошибке).
 * @param message Текст сообщения.
 * @param isError true, если сообщение об ошибке, false — информация.
 */
void MainWindow::showNotification(const QString &message, bool isError) {
  QMessageBox msgBox;
  msgBox.setWindowTitle(isError ? "Ошибка" : "Информация");
  msgBox.setText(message);
  msgBox.setIcon(isError ? QMessageBox::Critical : QMessageBox::Information);
  msgBox.exec();
}

/**
 * @brief Сохраняет текущий граф в файл.
 * @param filepath Путь к файлу для сохранения.
 * @return true, если сохранение прошло успешно, иначе false.
 *
 * Вызывает метод saveToFile у графа и отображает уведомление
 * о результате операции.
 */
bool MainWindow::saveGraphToFile(const QString &filepath) {
  QGraphicsScene *scene = graphView_->scene();
  if (!scene)
    return false;

  bool success = graph_->saveToFile(filepath);
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
 * @return true, если загрузка прошла успешно, иначе false.
 *
 * Очищает текущее состояние сцены и графа, парсит файл,
 * создаёт узлы и рёбра на основе данных из файла.
 */
bool MainWindow::loadGraphFromFile(const QString &filepath) {
  // Очищаем текущее состояние
  if (graphView_) {
    graphView_->clearScene();
  }
  graph_->clear();

  // Парсим файл
  std::vector<Graph::NodeData> nodesData;
  std::vector<Graph::EdgeData> edgesData;

  if (!graph_->parseFile(filepath, nodesData, edgesData)) {
    showNotification("Ошибка при чтении файла!", true);
    return false;
  }

  // Создаем Узлы (UI + Model)
  std::unordered_map<ID, SmoothNode *> createdNodes;
  QGraphicsScene *scene = graphView_->scene();

  for (const Graph::NodeData &ndata : nodesData) {
    SmoothNode *node = new SmoothNode(ndata.x, ndata.y, 50);

    connect(node, &SmoothNode::nodeAboutToBeDeleted, graphView_,
            [this](SmoothNode *n) { emit graphView_->nodeRemoved(n); });

    scene->addItem(node);
    createdNodes[ndata.id] = node;
    graph_->addNodeWithId(node, ndata.id);
  }

  // Создаем Ребра (UI + Model)
  for (const Graph::EdgeData &edata : edgesData) {
    auto itFrom = createdNodes.find(edata.from);
    auto itTo = createdNodes.find(edata.to);

    if (itFrom != createdNodes.end() && itTo != createdNodes.end()) {
      SmoothNode *start = itFrom->second;
      SmoothNode *end = itTo->second;

      SmoothEdge *edge = new SmoothEdge(start, end);
      edge->setWeight(edata.weight);

      connect(edge, &SmoothEdge::edgeAboutToBeDeleted, graphView_,
              [this](SmoothEdge *e) { emit graphView_->edgeRemoved(e); });

      scene->addItem(edge);
      edge->updatePosition();
      start->addOutgoingEdge(edge);
      end->addIncomingEdge(edge);

      graph_->addEdge(edge);
    }
  }

  updateGraphColors();
  showNotification(
      QString("Граф успешно загружен из файла:\n%1").arg(filepath));

  return true;
}

/**
 * @brief Слот для обработки обнаружения цикла в графе.
 *
 * Отображает предупреждение о необходимости отсутствия циклов
 * перед поиском кратчайшего пути.
 */
void MainWindow::onLoopFound() {
  QMessageBox::warning(this, "Предупреждение",
                       "Перед поиском кратчайшего пути необходимо убедиться, "
                       "что в графе отсутствуют циклы!");
}

/**
 * @brief Выполняет поиск кратчайшего пути и визуализирует его.
 *
 * Использует выбранные стартовый и конечный узлы для поиска пути
 * через модель Graph. При успешном нахождении вызывает подсветку пути.
 *
 * Проверяет, что стартовый и конечный узлы выбраны и не совпадают.
 * При успешном поиске сохраняет revision графа и устанавливает флаг
 * hasValidPath_ в true.
 */
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
    hasValidPath_ = false;
    return;
  }

  lastValidPathRevision_ = graph_->getRevision();
  hasValidPath_ = true;

  graphView_->highlightPath(path);
  showNotification(
      QString("Кратчайший путь найден! Длина пути: %1 узлов").arg(path.size()));
}

/**
 * @brief Слот для сохранения графа в файл.
 *
 * Если у графа уже есть путь к файлу, сохраняет по нему.
 * Иначе открывает диалог выбора файла и сохраняет граф.
 */
void MainWindow::onSaveGraph() {
  QString filepath;

  if (!graph_->getCurrentFilePath().isEmpty()) {
    saveGraphToFile(graph_->getCurrentFilePath());
    return;
  }

  QString path = QDir::homePath();
  if (!graphSaveFocusDir_.isEmpty())
    path = graphSaveFocusDir_;

  filepath = QFileDialog::getSaveFileName(this, "Сохранить граф", QString(),
                                          "Graph Files (*.gphz)");

  if (!filepath.isEmpty()) {
    if (!filepath.endsWith(".gphz", Qt::CaseInsensitive)) {
      filepath += ".gphz";
    }
    saveGraphToFile(filepath);
    graphSaveFocusDir_ = QFileInfo(filepath).absolutePath();
  }
}

/**
 * @brief Слот для открытия графа из файла.
 *
 * Проверяет наличие несохранённых изменений.
 * Открывает диалог выбора файла и загружает граф.
 */
void MainWindow::onOpenGraph() {
  if (graph_->isModified() && !graph_->getCurrentFilePath().isEmpty()) {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Несохранённые изменения",
        "У вас есть несохранённые изменения. Открыть другой файл?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No) {
      return;
    }
  }

  QString path = QDir::homePath();
  if (!graphOpenFocusDir_.isEmpty())
    path = graphOpenFocusDir_;

  QString filepath = QFileDialog::getOpenFileName(this, "Открыть граф", path,
                                                  "Graph Files (*.gphz)");
  if (!filepath.isEmpty()) {
    loadGraphFromFile(filepath);
    graphOpenFocusDir_ = QFileInfo(filepath).absolutePath();
  }
}

/**
 * @brief Слот для запуска поиска кратчайшего пути.
 *
 * Вызывает findAndVisualizePath() для выполнения поиска
 * и визуализации результата.
 */
void MainWindow::onFindPath() { findAndVisualizePath(); }

/**
 * @brief Слот для очистки подсветки пути.
 *
 * Вызывает clearPathHighlight() у GraphView и показывает уведомление.
 */
void MainWindow::onStopPath() {
  graphView_->clearPathHighlight();
  showNotification("Выделение пути снято");
}

/**
 * @brief Деструктор. Освобождает память, занятую объектом Graph.
 */
MainWindow::~MainWindow() { delete graph_; }
