/**
 * @file e2e_tests.cpp
 * @brief E2E тесты для приложения SmoothGraphs с использованием QTest
 */

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTemporaryFile>
#include <QtTest/QSignalSpy>
#include <QtTest/QTest>

#include "../src/back/Graph.hpp"
#include "../src/back/Logger.hpp"
#include "../src/front/Figures.hpp"
#include "../src/front/GraphView.hpp"
#include "../src/front/MainWindow.hpp"
#include "../src/front/MenuBar.hpp"
#include "../src/front/ThemeManager.hpp"
#include "e2e.hpp"
#include "qpoint.h"

void E2ETests::initTestCase() {
  // Инициализация QApplication для тестов
  QVERIFY(QCoreApplication::instance() != nullptr);
}

void E2ETests::cleanupTestCase() {
  // Очистка после всех тестов
}

void E2ETests::init() {
  // Создание объектов перед каждым тестом
  themeManager = new ThemeManager();
  mainWindow = new MainWindow(*themeManager);
  scene = mainWindow->findChild<QGraphicsScene *>();
  if (!scene) {
    scene = new QGraphicsScene();
  }
}

void E2ETests::cleanup() {
  // Удаление объектов после каждого теста
  delete mainWindow;
  delete themeManager;
  mainWindow = nullptr;
  themeManager = nullptr;
}

/**
 * @brief Тест создания узла
 */
void E2ETests::testNodeCreation() {
  // Создаём узел через GraphView
  GraphView *graphView = mainWindow->findChild<GraphView *>();
  QVERIFY(graphView != nullptr);

  // Создаём узел программно
  SmoothNode *node = new SmoothNode(100, 100, 50);
  QVERIFY(node != nullptr);

  // Проверяем позицию центра
  QPointF center = node->getCenter();
  QCOMPARE(center.x(), 100.0);
  QCOMPARE(center.y(), 100.0);

  // Проверяем ID (должно быть 0 до добавления в граф)
  QCOMPARE(node->getId(), (size_t)0);

  // Добавляем узел в сцену
  scene->addItem(node);
  QVERIFY(scene->items().contains(node));

  delete node;
}

/**
 * @brief Тест удаления узла
 */
void E2ETests::testNodeDeletion() {
  Graph *testGraph = new Graph(new Logger(true));

  SmoothNode *node = new SmoothNode(50, 50, 50);
  scene->addItem(node);

  ID nodeId = testGraph->addNode(node);
  QVERIFY(nodeId > 0);
  QCOMPARE(testGraph->getNodes().size(), (size_t)1);

  // Удаляем узел
  testGraph->deleteNode(nodeId);
  QCOMPARE(testGraph->getNodes().size(), (size_t)0);

  delete testGraph;
}

/**
 * @brief Тест перемещения узла
 */
void E2ETests::testNodeMovement() {
  const float radius = 50;
  SmoothNode *node = new SmoothNode(0, 0, radius);
  scene->addItem(node);

  QPointF initialCenter = node->getCenter();
  QCOMPARE(initialCenter.x(), 0.0);
  QCOMPARE(initialCenter.y(), 0.0);

  // Перемещаем узел
  node->setPos(100 - radius, 100 - radius);
  QPointF newCenter = node->getCenter();
  QCOMPARE(newCenter.x(), 100.0);
  QCOMPARE(newCenter.y(), 100.0);

  delete node;
}

/**
 * @brief Тест создания ребра
 */
void E2ETests::testEdgeCreation() {
  Graph *testGraph = new Graph(new Logger(true));

  SmoothNode *node1 = new SmoothNode(0, 0, 50);
  SmoothNode *node2 = new SmoothNode(200, 0, 50);

  scene->addItem(node1);
  scene->addItem(node2);

  testGraph->addNode(node1);
  testGraph->addNode(node2);

  SmoothEdge *edge = new SmoothEdge(node1, node2);
  QVERIFY(edge != nullptr);

  QCOMPARE(edge->getStartNode(), node1);
  QCOMPARE(edge->getEndNode(), node2);

  scene->addItem(edge);
  testGraph->addEdge(edge);

  QCOMPARE(testGraph->getEdges().size(), (size_t)1);

  delete testGraph;
}

/**
 * @brief Тест удаления ребра
 */
void E2ETests::testEdgeDeletion() {
  Graph *testGraph = new Graph(new Logger(true));

  SmoothNode *node1 = new SmoothNode(0, 0, 50);
  SmoothNode *node2 = new SmoothNode(200, 0, 50);

  scene->addItem(node1);
  scene->addItem(node2);

  ID id1 = testGraph->addNode(node1);
  ID id2 = testGraph->addNode(node2);

  SmoothEdge *edge = new SmoothEdge(node1, node2);
  scene->addItem(edge);
  testGraph->addEdge(edge);

  QCOMPARE(testGraph->getEdges().size(), (size_t)1);

  // Удаляем ребро
  testGraph->deleteEdge(id1, id2);
  QCOMPARE(testGraph->getEdges().size(), (size_t)0);

  delete testGraph;
}

/**
 * @brief Тест изменения веса ребра
 */
void E2ETests::testEdgeWeightModification() {
  SmoothNode *node1 = new SmoothNode(0, 0, 50);
  SmoothNode *node2 = new SmoothNode(200, 0, 50);

  SmoothEdge *edge = new SmoothEdge(node1, node2);

  // Проверяем вес по умолчанию
  QCOMPARE(edge->getWeight(), 1.0f);

  // Изменяем вес
  edge->setWeight(5.5f);
  QCOMPARE(edge->getWeight(), 5.5f);

  edge->setWeight(0.5f);
  QCOMPARE(edge->getWeight(), 0.5f);

  delete edge;
  delete node1;
  delete node2;
}

/**
 * @brief Тест изменения структуры графа
 */
void E2ETests::testGraphStructureChange() {
  Logger *logger = new Logger(true);
  Graph *testGraph = new Graph(logger);

  QSignalSpy spy(testGraph, SIGNAL(graphStructureChanged()));

  SmoothNode *node = new SmoothNode(100, 100, 50);
  scene->addItem(node);

  testGraph->addNode(node);
  QCOMPARE(spy.count(), 1);

  SmoothNode *node2 = new SmoothNode(200, 200, 50);
  scene->addItem(node2);
  testGraph->addNode(node2);
  QCOMPARE(spy.count(), 2);

  SmoothEdge *edge = new SmoothEdge(node, node2);
  scene->addItem(edge);
  testGraph->addEdge(edge);
  QCOMPARE(spy.count(), 3);

  delete testGraph;
}

/**
 * @brief Тест очистки графа
 */
void E2ETests::testGraphClear() {
  Graph *testGraph = new Graph(new Logger(true));

  // Добавляем несколько узлов и рёбер
  SmoothNode *node1 = new SmoothNode(0, 0, 50);
  SmoothNode *node2 = new SmoothNode(200, 0, 50);
  scene->addItem(node1);
  scene->addItem(node2);

  testGraph->addNode(node1);
  testGraph->addNode(node2);

  SmoothEdge *edge = new SmoothEdge(node1, node2);
  scene->addItem(edge);
  testGraph->addEdge(edge);

  QCOMPARE(testGraph->getNodes().size(), (size_t)2);
  QCOMPARE(testGraph->getEdges().size(), (size_t)1);

  // Очищаем граф
  testGraph->clear();

  QCOMPARE(testGraph->getNodes().size(), (size_t)0);
  QCOMPARE(testGraph->getEdges().size(), (size_t)0);
  QCOMPARE(testGraph->isModified(), false);

  delete testGraph;
}

/**
 * @brief Тест поиска кратчайшего пути
 */
void E2ETests::testShortestPathFinding() {
  Logger *logger = new Logger(true);
  Graph *testGraph = new Graph(logger);

  // Создаём простой граф: A -> B -> C
  SmoothNode *nodeA = new SmoothNode(0, 0, 50);
  SmoothNode *nodeB = new SmoothNode(200, 0, 50);
  SmoothNode *nodeC = new SmoothNode(400, 0, 50);

  scene->addItem(nodeA);
  scene->addItem(nodeB);
  scene->addItem(nodeC);

  testGraph->addNode(nodeA);
  testGraph->addNode(nodeB);
  testGraph->addNode(nodeC);

  SmoothEdge *edgeAB = new SmoothEdge(nodeA, nodeB);
  edgeAB->setWeight(1.0f);
  SmoothEdge *edgeBC = new SmoothEdge(nodeB, nodeC);
  edgeBC->setWeight(2.0f);

  scene->addItem(edgeAB);
  scene->addItem(edgeBC);

  testGraph->addEdge(edgeAB);
  testGraph->addEdge(edgeBC);

  // Ищем путь от A к C
  std::vector<SmoothNode *> path = testGraph->findShortestPath(nodeA, nodeC);

  // Проверяем результат
  QCOMPARE(path.size(), (size_t)3);
  QCOMPARE(path[0], nodeA);
  QCOMPARE(path[1], nodeB);
  QCOMPARE(path[2], nodeC);

  delete testGraph;
}

/**
 * @brief Тест обнаружения цикла в графе
 */
void E2ETests::testCycleDetection() {
  Logger *logger = new Logger(true);
  Graph *testGraph = new Graph(logger);

  QSignalSpy loopSpy(testGraph, SIGNAL(loopFound()));

  // Создаём граф с циклом: A -> B -> C -> A
  SmoothNode *nodeA = new SmoothNode(0, 0, 50);
  SmoothNode *nodeB = new SmoothNode(200, 0, 50);
  SmoothNode *nodeC = new SmoothNode(400, 0, 50);

  scene->addItem(nodeA);
  scene->addItem(nodeB);
  scene->addItem(nodeC);

  testGraph->addNode(nodeA);
  testGraph->addNode(nodeB);
  testGraph->addNode(nodeC);

  SmoothEdge *edgeAB = new SmoothEdge(nodeA, nodeB);
  SmoothEdge *edgeBC = new SmoothEdge(nodeB, nodeC);
  SmoothEdge *edgeCA = new SmoothEdge(nodeC, nodeA);

  scene->addItem(edgeAB);
  scene->addItem(edgeBC);
  scene->addItem(edgeCA);

  testGraph->addEdge(edgeAB);
  testGraph->addEdge(edgeBC);
  testGraph->addEdge(edgeCA);

  // Пытаемся найти путь (должен обнаружить цикл)
  std::vector<SmoothNode *> path = testGraph->findShortestPath(nodeA, nodeC);

  // Путь должен быть пустым из-за цикла
  QCOMPARE(path.size(), (size_t)0);
  QCOMPARE(loopSpy.count(), 1);

  delete testGraph;
}

/**
 * @brief Тест сохранения графа в файл
 */
void E2ETests::testSaveGraphToFile() {
  Graph *testGraph = new Graph(new Logger(true));

  // Создаём простой граф
  SmoothNode *node1 = new SmoothNode(100, 100, 50);
  SmoothNode *node2 = new SmoothNode(300, 100, 50);

  scene->addItem(node1);
  scene->addItem(node2);

  testGraph->addNode(node1);
  testGraph->addNode(node2);

  SmoothEdge *edge = new SmoothEdge(node1, node2);
  edge->setWeight(5.0f);
  scene->addItem(edge);
  testGraph->addEdge(edge);

  // Создаём временный файл
  QTemporaryFile tempFile(QDir::tempPath() + "/graph_XXXXXX.dot");
  tempFile.open();
  QString filepath = tempFile.fileName();
  tempFile.close();

  // Сохраняем граф
  bool success = testGraph->saveToFile(filepath);
  QVERIFY(success);

  // Проверяем, что файл существует
  QFileInfo fileInfo(filepath);
  QVERIFY(fileInfo.exists());
  QVERIFY(fileInfo.size() > 0);

  delete testGraph;
}

/**
 * @brief Тест загрузки графа из файла
 */
void E2ETests::testLoadGraphFromFile() {
  // Сначала создаём и сохраняем граф
  Graph *saveGraph = new Graph(new Logger(true));

  SmoothNode *node1 = new SmoothNode(150, 150, 50);
  SmoothNode *node2 = new SmoothNode(350, 150, 50);

  saveGraph->addNode(node1);
  saveGraph->addNode(node2);

  SmoothEdge *edge = new SmoothEdge(node1, node2);
  edge->setWeight(3.5f);
  saveGraph->addEdge(edge);

  QTemporaryFile tempFile(QDir::tempPath() + "/graph_load_XXXXXX.dot");
  tempFile.open();
  QString filepath = tempFile.fileName();
  tempFile.close();

  saveGraph->saveToFile(filepath);
  delete saveGraph;

  // Теперь загружаем граф
  Graph *loadGraph = new Graph(new Logger(true));

  std::vector<Graph::NodeData> nodes;
  std::vector<Graph::EdgeData> edges;

  bool success = loadGraph->parseFile(filepath, nodes, edges);
  QVERIFY(success);

  // Проверяем данные
  QCOMPARE(nodes.size(), (size_t)2);
  QCOMPARE(edges.size(), (size_t)1);

  // Проверяем координаты первого узла (с учётом погрешности)
  QVERIFY(qAbs(nodes[0].x - 150.0) < 1.0);
  QVERIFY(qAbs(nodes[0].y - 150.0) < 1.0);

  // Проверяем вес ребра
  QCOMPARE(edges[0].weight, 3.5f);

  delete loadGraph;
}

/**
 * @brief Тест парсинга файла
 */
void E2ETests::testParseFile() {
  Graph *testGraph = new Graph(new Logger(true));

  QTemporaryFile tempFile(QDir::tempPath() + "/graph_parse_XXXXXX.dot");
  tempFile.open();

  // Записываем тестовые данные в формате DOT
  QTextStream out(&tempFile);
  out << "digraph G {\n";
  out << "  node1 [pos=\"100,200\"];\n";
  out << "  node2 [pos=\"300,400\"];\n";
  out << "  node1 -> node2 [weight=7.5];\n";
  out << "}\n";
  tempFile.close();

  std::vector<Graph::NodeData> nodes;
  std::vector<Graph::EdgeData> edges;

  bool success = testGraph->parseFile(tempFile.fileName(), nodes, edges);
  QVERIFY(success);

  QCOMPARE(nodes.size(), (size_t)2);
  QCOMPARE(edges.size(), (size_t)1);

  delete testGraph;
}

/**
 * @brief Тест смены темы
 */
void E2ETests::testThemeChange() {
  QSignalSpy spy(themeManager, SIGNAL(themeChanged()));

  // Меняем тему
  themeManager->setTheme(Theme::Dark);
  QCOMPARE(spy.count(), 1);

  ThemeColors colors = themeManager->getThemeColors();
  QVERIFY(colors.background.isValid());
  QVERIFY(colors.textPrimary.isValid());

  themeManager->setTheme(Theme::Light);
  QCOMPARE(spy.count(), 2);

  // Проверяем, что цвета изменились
  ThemeColors lightColors = themeManager->getThemeColors();
  QVERIFY(lightColors.background.isValid());
}

/**
 * @brief Тест добавления сообщений в логгер
 */
void E2ETests::testLoggerAddMessage() {
  Logger logger(true);

  logger.addMessage(INFO, "Test info message");
  logger.addMessage(WARNING, "Test warning message");
  logger.addMessage(ERROR, "Test error message");
  logger.addMessage(SUCCESS, "Test success message");

  QString log = logger.getLog();
  QVERIFY(!log.isEmpty());
  QVERIFY(log.contains("Test info message"));
  QVERIFY(log.contains("Test warning message"));
  QVERIFY(log.contains("Test error message"));
  QVERIFY(log.contains("Test success message"));
}

/**
 * @brief Тест сохранения лога в файл
 */
void E2ETests::testLoggerSaveToFile() {
  Logger logger(true);

  logger.addMessage(INFO, "Log entry 1");
  logger.addMessage(ERROR, "Log entry 2");
  logger.addMessage(SUCCESS, "Log entry 3");

  QTemporaryFile tempFile(QDir::tempPath() + "/log_XXXXXX.txt");
  tempFile.open();
  QString filepath = tempFile.fileName();
  tempFile.close();

  int count = logger.saveToFile(filepath);
  QCOMPARE(count, 3);

  // Проверяем файл
  QFile file(filepath);
  QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
  QByteArray content = file.readAll();
  file.close();

  QVERIFY(content.contains("Log entry 1"));
  QVERIFY(content.contains("Log entry 2"));
  QVERIFY(content.contains("Log entry 3"));
}

/**
 * @brief Тест фильтрации лога по статусу
 */
void E2ETests::testLoggerFilterByStatus() {
  Logger logger(false); // Без временных меток для простоты

  logger.addMessage(INFO, "Info 1");
  logger.addMessage(INFO, "Info 2");
  logger.addMessage(ERROR, "Error 1");
  logger.addMessage(WARNING, "Warning 1");
  logger.addMessage(SUCCESS, "Success 1");

  QString errorLog = logger.getLogByStatus(ERROR);
  QVERIFY(errorLog.contains("Error 1"));
  QVERIFY(!errorLog.contains("Info 1"));

  QString infoLog = logger.getLogByStatus(INFO);
  QVERIFY(infoLog.contains("Info 1"));
  QVERIFY(infoLog.contains("Info 2"));
  QVERIFY(!infoLog.contains("Error 1"));
}

/**
 * @brief Тест кнопок MenuBar
 */
void E2ETests::testMenuBarButtons() {
  MenuBar *menuBar = mainWindow->getMenuBar();
  QVERIFY(menuBar != nullptr);

  // Проверяем наличие кнопок
  QVERIFY(menuBar->getOpenBtn() != nullptr);
  QVERIFY(menuBar->getRunBtn() != nullptr);
  QVERIFY(menuBar->getStopBtn() != nullptr);
  QVERIFY(menuBar->getThemeBtn() != nullptr);

  // Проверяем, что кнопки включены
  QVERIFY(menuBar->getOpenBtn()->isEnabled());
  QVERIFY(menuBar->getRunBtn()->isEnabled());
}

QTEST_MAIN(E2ETests)
// #include "e2e_tests.moc"
