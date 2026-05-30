#ifndef E2E_TESTS_HPP
#define E2E_TESTS_HPP

#include <QObject>

class ThemeManager;
class MainWindow;
class Graph;
class QGraphicsScene;
class GraphView;

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
  // void testNodeMovement();

  // Тесты создания и управления рёбрами
  void testEdgeCreation();
  void testEdgeDeletion();

  // Тесты работы с графом
  void testShortestPathFinding();
  void testCycleDetection();

  // Тесты сохранения и загрузки
  void testSaveGraphToFile();

  // Тесты темы
  void testThemeChange();

  Graph *getGraph() const;

private:
  ThemeManager *themeManager;
  MainWindow *mainWindow;
  Graph *graph;
  QGraphicsScene *scene;
  GraphView *graphView;
};

#endif // E2E_TESTS_HPP
