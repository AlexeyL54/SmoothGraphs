#ifndef E2E_TESTS_HPP
#define E2E_TESTS_HPP

#include <QObject>

class ThemeManager;
class MainWindow;
class Graph;
class QGraphicsScene;

/**
 * @class E2ETests
 * @brief Класс энд-ту-енд тестов для проверки основного функционала приложения
 */
class E2ETests : public QObject {
  Q_OBJECT

private slots:
  // Инициализация и очистка
  void initTestCase();
  void cleanupTestCase();
  void init();
  void cleanup();

  // Тесты создания и управления узлами
  void testNodeCreation();
  void testNodeDeletion();
  void testNodeMovement();

  // Тесты создания и управления рёбрами
  void testEdgeCreation();
  void testEdgeDeletion();
  void testEdgeWeightModification();

  // Тесты работы с графом
  void testGraphStructureChange();
  void testGraphClear();
  void testShortestPathFinding();
  void testCycleDetection();

  // Тесты сохранения и загрузки
  void testSaveGraphToFile();
  void testLoadGraphFromFile();
  void testParseFile();

  // Тесты темы
  void testThemeChange();

  // Тесты логгера
  void testLoggerAddMessage();
  void testLoggerSaveToFile();
  void testLoggerFilterByStatus();

  // Тесты MenuBar
  void testMenuBarButtons();

private:
  ThemeManager *themeManager;
  MainWindow *mainWindow;
  Graph *graph;
  QGraphicsScene *scene;
};

#endif // E2E_TESTS_HPP
