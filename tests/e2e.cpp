/**
 * @file e2e.cpp
 * @brief Реализация E2E тестов для приложения SmoothGraphs
 */

#include "e2e.hpp"
#include "../src/back/Graph.hpp"
#include "../src/front/Figures.hpp"
#include "../src/front/GraphView.hpp"
#include "../src/front/MainWindow.hpp"
#include "../src/front/MenuBar.hpp"
#include "../src/front/ThemeManager.hpp"
#include "qlogging.h"

#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMenu>
#include <QTemporaryFile>
#include <QTimer>
#include <QtTest/QSignalSpy>
#include <QtTest/QTest>

// ============================================================================
// ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ
// ============================================================================

/** @brief Ожидает обработки событий в течение указанного времени */
void E2ETests::waitForEvents(int ms) {
  QEventLoop loop;
  QTimer::singleShot(ms, &loop, SLOT(quit()));
  loop.exec();
}

/** @brief Создаёт узел в указанной позиции */
SmoothNode *E2ETests::createNode(double x, double y) {
  graphView->addFigure(QPointF(x, y));
  waitForEvents();
  return findNodeByPosition(x, y);
}

/** @brief Создаёт несколько узлов по указанным позициям */
std::vector<SmoothNode *>
E2ETests::createNodes(const std::vector<QPointF> &positions) {
  std::vector<SmoothNode *> nodes;
  for (const auto &pos : positions) {
    nodes.push_back(createNode(pos.x(), pos.y()));
  }
  return nodes;
}

/** @brief Находит узел по его координатам */
SmoothNode *E2ETests::findNodeByPosition(double x, double y) {
  for (QGraphicsItem *item : scene->items()) {
    if (SmoothNode *node = dynamic_cast<SmoothNode *>(item)) {
      if (std::abs(node->getCenter().x() - x) < 1 &&
          std::abs(node->getCenter().y() - y) < 1) {
        return node;
      }
    }
  }
  return nullptr;
}

/** @brief Находит узел по его идентификатору */
SmoothNode *E2ETests::findNodeById(size_t id) {
  for (SmoothNode *node : graph->getNodes()) {
    if (node->getId() == id) {
      return node;
    }
  }
  return nullptr;
}

/** @brief Создаёт ребро между двумя узлами */
SmoothEdge *E2ETests::createEdge(SmoothNode *from, SmoothNode *to,
                                 float weight) {
  graphView->startEdgeCreation(from);
  QTest::mouseClick(graphView->viewport(), Qt::LeftButton, Qt::NoModifier,
                    graphView->mapFromScene(to->scenePos() + QPointF(25, 25)));
  waitForEvents();

  for (SmoothEdge *edge : graph->getEdges()) {
    if (edge->getStartNode() == from && edge->getEndNode() == to) {
      edge->setWeight(weight);
      return edge;
    }
  }
  return nullptr;
}

/** @brief Создаёт ребро между узлами по их координатам */
SmoothEdge *E2ETests::createEdgeByPositions(double fromX, double fromY,
                                            double toX, double toY,
                                            float weight) {
  SmoothNode *from = findNodeByPosition(fromX, fromY);
  SmoothNode *to = findNodeByPosition(toX, toY);
  if (!from || !to)
    return nullptr;
  return createEdge(from, to, weight);
}

/** @brief Создаёт простой граф из двух узлов и одного ребра */
void E2ETests::createTwoNodeGraph() {
  createNode(100, 100);
  createNode(300, 100);
  createEdgeByPositions(100, 100, 300, 100, 2.5f);
}

/** @brief Создаёт граф-цепочку из трёх узлов */
void E2ETests::createThreeNodeChain() {
  createNode(0, 0);
  createNode(200, 0);
  createNode(400, 0);
  createEdgeByPositions(0, 0, 200, 0, 1.0f);
  createEdgeByPositions(200, 0, 400, 0, 1.0f);
}

/** @brief Создаёт циклический граф из трёх узлов */
void E2ETests::createThreeNodeCycle() {
  createNode(0, 0);
  createNode(200, 0);
  createNode(100, 173);
  createEdgeByPositions(0, 0, 200, 0, 1.0f);
  createEdgeByPositions(200, 0, 100, 173, 1.0f);
  createEdgeByPositions(100, 173, 0, 0, 1.0f);
}

/** @brief Полностью очищает сцену и модель графа */
void E2ETests::clearGraph() {
  graphView->clearScene();
  waitForEvents();
}

/** @brief Сохраняет текущий граф во временный файл */
QString E2ETests::saveGraphToTempFile() {
  // Create a temporary file that persists
  QTemporaryFile tempFile(QDir::tempPath() + "/graph_test_XXXXXX.gphz");
  tempFile.setAutoRemove(false); // Don't auto-delete

  if (!tempFile.open()) {
    qDebug() << "Failed to create temporary file...";
    return QString();
  }

  QString filepath = tempFile.fileName();
  tempFile.close(); // Close but keep the file

  bool success = graph->saveToFile(filepath);
  if (!success) {
    qDebug() << "Failed to save graph to file...";
    QFile::remove(filepath); // Clean up on failure
    return QString();
  }

  return filepath;
}

/** @brief Загружает граф из файла через главное окно */
bool E2ETests::loadGraphFromFile(const QString &filepath) {
  bool result = false;
  QMetaObject::invokeMethod(mainWindow, "loadGraphFromFile",
                            Qt::DirectConnection, Q_RETURN_ARG(bool, result),
                            Q_ARG(const QString &, filepath));
  waitForEvents();
  return result;
}

/** @brief Проверяет существование узла по координатам */
void E2ETests::assertNodeExists(double x, double y, size_t expectedId) {
  SmoothNode *node = findNodeByPosition(x, y);
  QVERIFY2(
      node != nullptr,
      QString("Node at position (%1, %2) not found").arg(x).arg(y).toUtf8());
  if (expectedId != 0) {
    QCOMPARE(node->getId(), expectedId);
  }
}

/** @brief Проверяет существование ребра между узлами */
void E2ETests::assertEdgeExists(size_t fromId, size_t toId,
                                float expectedWeight) {
  bool found = false;
  for (SmoothEdge *edge : graph->getEdges()) {
    if (edge->getStartNode()->getId() == fromId &&
        edge->getEndNode()->getId() == toId) {
      found = true;
      QCOMPARE(edge->getWeight(), expectedWeight);
      break;
    }
  }
  QVERIFY2(
      found,
      QString("Edge from %1 to %2 not found").arg(fromId).arg(toId).toUtf8());
}

// ============================================================================
// ТЕСТЫ
// ============================================================================

/** @brief Инициализация перед запуском всех тестов */
void E2ETests::initTestCase() {
  QVERIFY(QCoreApplication::instance() != nullptr);
}

/** @brief Очистка после выполнения всех тестов */
void E2ETests::cleanupTestCase() {}

/** @brief Инициализация перед каждым тестом */
void E2ETests::init() {
  themeManager = new ThemeManager();
  mainWindow = new MainWindow(*themeManager);
  graphView = mainWindow->findChild<GraphView *>();
  QVERIFY(graphView != nullptr);
  scene = graphView->scene();
  QVERIFY(scene != nullptr);
  graph = mainWindow->getGraph();
  QVERIFY(graph != nullptr);
}

/** @brief Очистка после каждого теста */
void E2ETests::cleanup() {
  delete mainWindow;
  delete themeManager;
  mainWindow = nullptr;
  themeManager = nullptr;
  graphView = nullptr;
  scene = nullptr;
  graph = nullptr;
}

/** @brief Тест создания узла графа */
void E2ETests::testNodeCreation() {
  createNode(100, 200);

  QCOMPARE(graph->getNodes().size(), (size_t)1);
  assertNodeExists(100, 200, 1);
}

/** @brief Тест удаления узла графа */
void E2ETests::testNodeDeletion() {
  createNode(50, 50);
  QCOMPARE(graph->getNodes().size(), (size_t)1);

  clearGraph();
  QCOMPARE(graph->getNodes().size(), (size_t)0);
}

/** @brief Тест создания ребра между узлами */
void E2ETests::testEdgeCreation() {
  createTwoNodeGraph();

  QCOMPARE(graph->getNodes().size(), (size_t)2);
  QCOMPARE(graph->getEdges().size(), (size_t)1);
  assertEdgeExists(1, 2, 2.5f);
}

/** @brief Тест удаления ребра */
void E2ETests::testEdgeDeletion() {
  createTwoNodeGraph();
  QCOMPARE(graph->getEdges().size(), (size_t)1);

  clearGraph();
  QCOMPARE(graph->getEdges().size(), (size_t)0);
}

/** @brief Тест поиска кратчайшего пути */
void E2ETests::testShortestPathFinding() {
  createThreeNodeChain();

  SmoothNode *start = findNodeByPosition(0, 0);
  SmoothNode *end = findNodeByPosition(400, 0);
  QVERIFY(start && end);

  graphView->setStartNode(start);
  graphView->setEndNode(end);

  MenuBar *menuBar = mainWindow->getMenuBar();
  QTest::mouseClick(menuBar->getRunBtn(), Qt::LeftButton);
  waitForEvents(200);

  bool pathHighlighted = false;
  for (QGraphicsItem *item : scene->items()) {
    if (SmoothEdge *e = dynamic_cast<SmoothEdge *>(item)) {
      if (e->isHighlighted()) {
        pathHighlighted = true;
        break;
      }
    }
  }
  QVERIFY(pathHighlighted);
}

/** @brief Тест обнаружения цикла в графе */
void E2ETests::testCycleDetection() {
  createThreeNodeCycle();

  QSignalSpy loopSpy(graph, SIGNAL(loopFound()));

  SmoothNode *start = findNodeByPosition(0, 0);
  SmoothNode *end = findNodeByPosition(200, 0);
  graphView->setStartNode(start);
  graphView->setEndNode(end);

  MenuBar *menuBar = mainWindow->getMenuBar();
  QTest::mouseClick(menuBar->getRunBtn(), Qt::LeftButton);
  waitForEvents(200);

  QCOMPARE(loopSpy.count(), 1);
}

/** @brief Тест сохранения графа в файл */
void E2ETests::testSaveGraphToFile() {
  createTwoNodeGraph();

  QString filepath = saveGraphToTempFile();
  QVERIFY(QFile::exists(filepath));
  QVERIFY(QFileInfo(filepath).size() > 0);

  QFile::remove(filepath);
}

/** @brief Тест загрузки графа из файла */
void E2ETests::testLoadGraphFromFile() {
  createTwoNodeGraph();
  QString filepath = saveGraphToTempFile();

  clearGraph();
  QCOMPARE(graph->getNodes().size(), (size_t)0);

  bool result = loadGraphFromFile(filepath);

  QVERIFY(result);
  QCOMPARE(graph->getNodes().size(), (size_t)2);
  QCOMPARE(graph->getEdges().size(), (size_t)1);
  assertEdgeExists(1, 2, 2.5f);

  QFile::remove(filepath);
}

/** @brief Тест смены темы оформления */
void E2ETests::testThemeChange() {
  QSignalSpy spy(themeManager, SIGNAL(themeChanged()));

  MenuBar *menuBar = mainWindow->getMenuBar();
  QToolButton *themeBtn = menuBar->getThemeBtn();
  QVERIFY(themeBtn != nullptr);

  QTest::mouseClick(themeBtn, Qt::LeftButton);
  waitForEvents();

  QMenu *menu = themeBtn->menu();
  QVERIFY(menu != nullptr);

  QAction *darkAction = nullptr;
  for (QAction *action : menu->actions()) {
    if (action->text().contains("☁️ Тёмная", Qt::CaseInsensitive)) {
      darkAction = action;
      break;
    }
  }
  QVERIFY(darkAction != nullptr);

  darkAction->trigger();
  waitForEvents();

  QCOMPARE(spy.count(), 1);
}

QTEST_MAIN(E2ETests)
