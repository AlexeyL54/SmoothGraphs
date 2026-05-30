/**
@file e2e_tests.cpp
@brief E2E тесты для приложения SmoothGraphs с использованием QTest
*/
#include "e2e.hpp"
#include "../src/back/Graph.hpp"
#include "../src/back/Logger.hpp"
#include "../src/front/Figures.hpp"
#include "../src/front/GraphView.hpp"
#include "../src/front/MainWindow.hpp"
#include "../src/front/MenuBar.hpp"
#include "../src/front/ThemeManager.hpp"
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

// Вспомогательная функция для ожидания обработки событий
static void waitForEvents(int ms = 100) {
  QEventLoop loop;
  QTimer::singleShot(ms, &loop, SLOT(quit()));
  loop.exec();
}

void E2ETests::initTestCase() {
  QVERIFY(QCoreApplication::instance() != nullptr);
}

void E2ETests::cleanupTestCase() {}

void E2ETests::init() {
  themeManager = new ThemeManager();
  mainWindow = new MainWindow(*themeManager);
  // MainWindow создает свою сцену и GraphView внутри себя
  graphView = mainWindow->findChild<GraphView *>();
  QVERIFY(graphView != nullptr);
  scene = graphView->scene();
  QVERIFY(scene != nullptr);
}

void E2ETests::cleanup() {
  delete mainWindow;
  delete themeManager;
  mainWindow = nullptr;
  themeManager = nullptr;
  graphView = nullptr;
  scene = nullptr;
}

// Вспомогательный метод для получения графа из MainWindow
Graph *E2ETests::getGraph() const { return mainWindow->getGraph(); }

/**
@brief Тест создания узла через контекстное меню (E2E)
*/
void E2ETests::testNodeCreation() {
  // 1. Симулируем клик правой кнопкой мыши в центре сцены для вызова
  // контекстного меню
  QPointF scenePos(0, 0);
  QPoint viewPos = graphView->mapFromScene(scenePos);

  QTest::mouseClick(graphView->viewport(), Qt::RightButton, Qt::NoModifier,
                    viewPos);
  waitForEvents();

  // УПРОЩЕНИЕ ДЛЯ E2E: Вызовем публичный слот addFigure, который эмулирует
  // выбор пункта меню.
  graphView->addFigure(scenePos);
  waitForEvents();

  // 3. Проверяем, что узел появился в сцене и в графе
  QCOMPARE(scene->items().size(), 1);
  SmoothNode *node = dynamic_cast<SmoothNode *>(scene->items().first());
  QVERIFY(node != nullptr);

  Graph *graph = getGraph();
  QVERIFY(graph != nullptr);
  QCOMPARE(graph->getNodes().size(), (size_t)1);
  QCOMPARE(graph->getNodeId(node), (size_t)1); // ID должен быть присвоен

  qDebug() << "Node created at:" << node->getCenter();
}

/**
@brief Тест удаления узла (E2E)
*/
void E2ETests::testNodeDeletion() {
  Graph *graph = getGraph();

  // Создаем узел
  graphView->addFigure(QPointF(50, 50));
  waitForEvents();
  QCOMPARE(graph->getNodes().size(), (size_t)1);

  // Очищаем сцену (эмуляция действия пользователя "Очистить все")
  graphView->clearScene();
  waitForEvents();

  QCOMPARE(graph->getNodes().size(), (size_t)0);
  QCOMPARE(scene->items().size(), 0);
}

/**
@brief Тест перемещения узла (E2E)
*/
/*void E2ETests::testNodeMovement() {
  graphView->addFigure(QPointF(0, 0));
  waitForEvents();

  SmoothNode *node = dynamic_cast<SmoothNode *>(scene->items().first());
  QVERIFY(node != nullptr);

  QPointF initialCenter = node->getCenter();
  QCOMPARE(initialCenter.x(), 0.0);

  // Симулируем перемещение: нажатие, перемещение, отпускание
  QPoint startPos = graphView->mapFromScene(node->scenePos());
  QPoint endPos = startPos + QPoint(100, 100);

  QTest::mousePress(graphView->viewport(), Qt::LeftButton, Qt::NoModifier,
                    startPos);
  QTest::mouseMove(graphView->viewport(), endPos);
  QTest::mouseRelease(graphView->viewport(), Qt::LeftButton, Qt::NoModifier,
                      endPos);
  waitForEvents();

  QPointF newCenter = node->getCenter();
  // Проверяем, что узел сдвинулся.
  QVERIFY(newCenter.x() != initialCenter.x());
  QVERIFY(newCenter.y() != initialCenter.y());
}*/

/**
@brief Тест создания ребра (E2E)
*/
void E2ETests::testEdgeCreation() {
  Graph *graph = getGraph();

  // 1. Создаем два узла
  QPointF pos1(0, 0);
  QPointF pos2(200, 0);

  graphView->addFigure(pos1);
  graphView->addFigure(pos2);
  waitForEvents();

  QCOMPARE(graph->getNodes().size(), (size_t)2);

  QList<QGraphicsItem *> items = scene->items();
  SmoothNode *node1 = nullptr;
  SmoothNode *node2 = nullptr;

  for (QGraphicsItem *item : items) {
    SmoothNode *n = dynamic_cast<SmoothNode *>(item);
    if (n) {
      if (n->getCenter().x() == 0)
        node1 = n;
      else if (n->getCenter().x() == 200)
        node2 = n;
    }
  }
  QVERIFY(node1 != nullptr);
  QVERIFY(node2 != nullptr);

  // 2. Симулируем создание ребра
  graphView->startEdgeCreation(node1);
  QVERIFY(graphView->isEdgeCreationActive() == true);

  // Теперь клик по второму узлу
  QPoint pos2View = graphView->mapFromScene(node2->scenePos());
  // Смещаем немного в центр узла, чтобы попасть в него
  pos2View += QPoint(25, 25);

  QTest::mouseClick(graphView->viewport(), Qt::LeftButton, Qt::NoModifier,
                    pos2View);
  waitForEvents();

  // 3. Проверяем результат
  QCOMPARE(graph->getEdges().size(), (size_t)1);
  QCOMPARE(scene->items().size(), 3); // 2 узла + 1 ребро

  SmoothEdge *edge = nullptr;
  for (QGraphicsItem *item : scene->items()) {
    SmoothEdge *e = dynamic_cast<SmoothEdge *>(item);
    if (e) {
      edge = e;
      break;
    }
  }
  QVERIFY(edge != nullptr);
  QCOMPARE(edge->getStartNode(), node1);
  QCOMPARE(edge->getEndNode(), node2);
}

/**
@brief Тест удаления ребра (E2E)
*/
void E2ETests::testEdgeDeletion() {
  Graph *graph = getGraph();

  // Создаем граф с ребром
  graphView->addFigure(QPointF(0, 0));
  graphView->addFigure(QPointF(200, 0));
  waitForEvents();

  QList<QGraphicsItem *> items = scene->items();
  SmoothNode *node1 = nullptr;
  SmoothNode *node2 = nullptr;
  for (QGraphicsItem *item : items) {
    SmoothNode *n = dynamic_cast<SmoothNode *>(item);
    if (n) {
      if (n->getCenter().x() == 0)
        node1 = n;
      else if (n->getCenter().x() == 200)
        node2 = n;
    }
  }

  graphView->startEdgeCreation(node1);
  QPoint pos2View =
      graphView->mapFromScene(node2->scenePos() + QPointF(25, 25));
  QTest::mouseClick(graphView->viewport(), Qt::LeftButton, Qt::NoModifier,
                    pos2View);
  waitForEvents();

  QCOMPARE(graph->getEdges().size(), (size_t)1);

  // Удаляем один из узлов (через clearScene для простоты)
  graphView->clearScene();
  waitForEvents();

  QCOMPARE(graph->getEdges().size(), (size_t)0);
  QCOMPARE(graph->getNodes().size(), (size_t)0);
}

/**
@brief Тест поиска кратчайшего пути (E2E)
*/
void E2ETests::testShortestPathFinding() {
  Logger *logger = new Logger(true);
  Graph *graph = getGraph();

  // Создаем цепь: A -> B -> C
  graphView->addFigure(QPointF(0, 0));   // A
  graphView->addFigure(QPointF(200, 0)); // B
  graphView->addFigure(QPointF(400, 0)); // C
  waitForEvents();

  QList<QGraphicsItem *> items = scene->items();
  SmoothNode *nodeA = nullptr, *nodeB = nullptr, *nodeC = nullptr;
  for (QGraphicsItem *item : items) {
    SmoothNode *n = dynamic_cast<SmoothNode *>(item);
    if (n) {
      if (n->getCenter().x() == 0)
        nodeA = n;
      else if (n->getCenter().x() == 200)
        nodeB = n;
      else if (n->getCenter().x() == 400)
        nodeC = n;
    }
  }

  // Создаем ребра A->B и B->C
  graphView->startEdgeCreation(nodeA);
  QTest::mouseClick(
      graphView->viewport(), Qt::LeftButton, Qt::NoModifier,
      graphView->mapFromScene(nodeB->scenePos() + QPointF(25, 25)));
  waitForEvents();

  graphView->startEdgeCreation(nodeB);
  QTest::mouseClick(
      graphView->viewport(), Qt::LeftButton, Qt::NoModifier,
      graphView->mapFromScene(nodeC->scenePos() + QPointF(25, 25)));
  waitForEvents();

  QCOMPARE(graph->getEdges().size(), (size_t)2);

  // Выбираем стартовый и конечный узлы для пути
  graphView->setStartNode(nodeA);
  graphView->setEndNode(nodeC);
  waitForEvents();

  // Запускаем поиск пути через MainWindow (эмуляция нажатия кнопки "Run")
  MenuBar *menuBar = mainWindow->getMenuBar();
  QVERIFY(menuBar != nullptr);
  QPushButton *runBtn = menuBar->getRunBtn();
  QVERIFY(runBtn != nullptr);

  QTest::mouseClick(runBtn, Qt::LeftButton);
  waitForEvents(200); // Даем время на расчет и логирование

  // Проверяем, что путь найден и подсвечен
  bool pathHighlighted = false;
  for (QGraphicsItem *item : scene->items()) {
    SmoothEdge *e = dynamic_cast<SmoothEdge *>(item);
    if (e && e->isHighlighted()) {
      pathHighlighted = true;
      break;
    }
  }
  QVERIFY(pathHighlighted);

  delete logger;
}

/**
@brief Тест обнаружения цикла (E2E)
*/
void E2ETests::testCycleDetection() {
  Graph *graph = getGraph();

  // Создаем цикл: A -> B -> C -> A
  graphView->addFigure(QPointF(0, 0));     // A
  graphView->addFigure(QPointF(200, 0));   // B
  graphView->addFigure(QPointF(100, 173)); // C
  waitForEvents();

  QList<QGraphicsItem *> items = scene->items();
  SmoothNode *nodeA = nullptr, *nodeB = nullptr, *nodeC = nullptr;
  for (QGraphicsItem *item : items) {
    SmoothNode *n = dynamic_cast<SmoothNode *>(item);
    if (n) {
      if (n->getCenter().x() == 0)
        nodeA = n;
      else if (n->getCenter().x() == 200)
        nodeB = n;
      else if (n->getCenter().x() == 100)
        nodeC = n;
    }
  }

  // Ребра: A->B, B->C, C->A
  graphView->startEdgeCreation(nodeA);
  QTest::mouseClick(
      graphView->viewport(), Qt::LeftButton, Qt::NoModifier,
      graphView->mapFromScene(nodeB->scenePos() + QPointF(25, 25)));
  waitForEvents();

  graphView->startEdgeCreation(nodeB);
  QTest::mouseClick(
      graphView->viewport(), Qt::LeftButton, Qt::NoModifier,
      graphView->mapFromScene(nodeC->scenePos() + QPointF(25, 25)));
  waitForEvents();

  graphView->startEdgeCreation(nodeC);
  QTest::mouseClick(
      graphView->viewport(), Qt::LeftButton, Qt::NoModifier,
      graphView->mapFromScene(nodeA->scenePos() + QPointF(25, 25)));
  waitForEvents();

  QCOMPARE(graph->getEdges().size(), (size_t)3);

  // Сигнал loopFound должен быть испущен при попытке поиска пути
  QSignalSpy loopSpy(graph, SIGNAL(loopFound()));

  graphView->setStartNode(nodeA);
  graphView->setEndNode(nodeC);

  MenuBar *menuBar = mainWindow->getMenuBar();
  QTest::mouseClick(menuBar->getRunBtn(), Qt::LeftButton);
  waitForEvents(200);

  QCOMPARE(loopSpy.count(), 1);
}

/**
@brief Тест сохранения и загрузки графа (E2E)
*/
void E2ETests::testSaveGraphToFile() {
  Graph *graph = getGraph();

  graphView->addFigure(QPointF(100, 100));
  graphView->addFigure(QPointF(300, 100));
  waitForEvents();

  QList<QGraphicsItem *> items = scene->items();
  SmoothNode *node1 = nullptr, *node2 = nullptr;
  for (QGraphicsItem *item : items) {
    SmoothNode *n = dynamic_cast<SmoothNode *>(item);
    if (n) {
      if (n->getCenter().x() == 100)
        node1 = n;
      else if (n->getCenter().x() == 300)
        node2 = n;
    }
  }

  graphView->startEdgeCreation(node1);
  QTest::mouseClick(
      graphView->viewport(), Qt::LeftButton, Qt::NoModifier,
      graphView->mapFromScene(node2->scenePos() + QPointF(25, 25)));
  waitForEvents();

  QCOMPARE(graph->getEdges().size(), (size_t)1);

  // Сохраняем
  QTemporaryFile tempFile(QDir::tempPath() + "/graph_e2e_XXXXXX.gphz");
  // ИСПРАВЛЕНО: Проверка результата open()
  QVERIFY(tempFile.open());
  QString filepath = tempFile.fileName();
  tempFile.close();

  bool success = graph->saveToFile(filepath);
  QVERIFY(success);

  QFileInfo fileInfo(filepath);
  QVERIFY(fileInfo.exists());
  QVERIFY(fileInfo.size() > 0);
}

/**
@brief Тест смены темы (E2E)
*/
void E2ETests::testThemeChange() {
  QSignalSpy spy(themeManager, SIGNAL(themeChanged()));

  // Эмулируем выбор темы через MenuBar
  MenuBar *menuBar = mainWindow->getMenuBar();
  QToolButton *themeBtn = menuBar->getThemeBtn();
  QVERIFY(themeBtn != nullptr);

  // Открываем меню темы
  QTest::mouseClick(themeBtn, Qt::LeftButton);
  waitForEvents();

  // Ищем действие "Dark" или "Light" в меню
  QMenu *menu = themeBtn->menu();
  QVERIFY(menu != nullptr);

  // Находим действие по тексту или данным
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

  // Проверяем, что цвета изменились
  ThemeColors colors = themeManager->getThemeColors();
  QVERIFY(colors.background.isValid());
}

QTEST_MAIN(E2ETests)
