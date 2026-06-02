/**
 * @file e2e.hpp
 * @brief Заголовочный файл класса E2E тестов для приложения SmoothGraphs
 */

#ifndef E2E_TESTS_HPP
#define E2E_TESTS_HPP

#include <QObject>

class ThemeManager;
class MainWindow;
class Graph;
class QGraphicsScene;
class GraphView;
class SmoothNode;
class SmoothEdge;

/**
 * @class E2ETests
 * @brief Класс энд-ту-енд тестов для проверки основного функционала приложения
 *
 * Обеспечивает автоматическое тестирование ключевых возможностей приложения:
 * - Создание и удаление узлов и рёбер
 * - Поиск кратчайшего пути и обнаружение циклов
 * - Сохранение и загрузка графов
 * - Смена темы оформления
 */
class E2ETests : public QObject {
  Q_OBJECT

private slots:
  /**
   * @brief Инициализация перед запуском всех тестов
   * @note Выполняется один раз перед всеми тестами
   */
  void initTestCase();

  /**
   * @brief Очистка после выполнения всех тестов
   * @note Выполняется один раз после всех тестов
   */
  void cleanupTestCase();

  /**
   * @brief Инициализация перед каждым тестом
   * @note Создаёт чистое окружение для каждого теста
   */
  void init();

  /**
   * @brief Очистка после каждого теста
   * @note Удаляет созданные объекты после каждого теста
   */
  void cleanup();

  // ========== ТЕСТЫ ==========

  /**
   * @brief Тест создания узла графа
   * @test Создаёт узел и проверяет его наличие в сцене и модели графа
   */
  void testNodeCreation();

  /**
   * @brief Тест удаления узла графа
   * @test Удаляет узел и проверяет его отсутствие в сцене и модели
   */
  void testNodeDeletion();

  /**
   * @brief Тест создания ребра между узлами
   * @test Создаёт два узла и ребро между ними, проверяет корректность веса
   */
  void testEdgeCreation();

  /**
   * @brief Тест удаления ребра
   * @test Создаёт и удаляет ребро, проверяет его отсутствие
   */
  void testEdgeDeletion();

  /**
   * @brief Тест поиска кратчайшего пути
   * @test Создаёт цепочку из трёх узлов, находит путь и проверяет подсветку
   */
  void testShortestPathFinding();

  /**
   * @brief Тест обнаружения цикла в графе
   * @test Создаёт циклический граф, проверяет отправку сигнала о цикле
   */
  void testCycleDetection();

  /**
   * @brief Тест сохранения графа в файл
   * @test Сохраняет граф во временный файл и проверяет его существование
   */
  void testSaveGraphToFile();

  /**
   * @brief Тест загрузки графа из файла
   * @test Сохраняет граф, очищает сцену, загружает и проверяет восстановление
   */
  void testLoadGraphFromFile();

  /**
   * @brief Тест смены темы оформления
   * @test Переключает тему и проверяет отправку сигнала themeChanged
   */
  void testThemeChange();

private:
  ThemeManager *themeManager = nullptr; ///< Менеджер тем оформления
  MainWindow *mainWindow = nullptr;     ///< Главное окно приложения
  Graph *graph = nullptr;               ///< Модель графа
  QGraphicsScene *scene = nullptr;      ///< Графическая сцена
  GraphView *graphView = nullptr;       ///< Виджет отображения графа

  // ========== ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ ==========

  /**
   * @brief Ожидает обработки событий в течение указанного времени
   * @param ms Количество миллисекунд для ожидания (по умолчанию 100)
   */
  void waitForEvents(int ms = 100);

  /**
   * @brief Создаёт узел в указанной позиции
   * @param x Координата X центра узла
   * @param y Координата Y центра узла
   * @return Указатель на созданный узел
   */
  SmoothNode *createNode(double x, double y);

  /**
   * @brief Создаёт несколько узлов по указанным позициям
   * @param positions Вектор координат центров узлов
   * @return Вектор указателей на созданные узлы
   */
  std::vector<SmoothNode *> createNodes(const std::vector<QPointF> &positions);

  /**
   * @brief Находит узел по его координатам
   * @param x Координата X центра узла
   * @param y Координата Y центра узла
   * @return Указатель на найденный узел или nullptr
   */
  SmoothNode *findNodeByPosition(double x, double y);

  /**
   * @brief Находит узел по его идентификатору
   * @param id Идентификатор узла
   * @return Указатель на найденный узел или nullptr
   */
  SmoothNode *findNodeById(size_t id);

  /**
   * @brief Создаёт ребро между двумя узлами
   * @param from Указатель на начальный узел
   * @param to Указатель на конечный узел
   * @param weight Вес ребра (по умолчанию 1.0)
   * @return Указатель на созданное ребро
   */
  SmoothEdge *createEdge(SmoothNode *from, SmoothNode *to, float weight = 1.0f);

  /**
   * @brief Создаёт ребро между узлами по их координатам
   * @param fromX Координата X начального узла
   * @param fromY Координата Y начального узла
   * @param toX Координата X конечного узла
   * @param toY Координата Y конечного узла
   * @param weight Вес ребра (по умолчанию 1.0)
   * @return Указатель на созданное ребро
   */
  SmoothEdge *createEdgeByPositions(double fromX, double fromY, double toX,
                                    double toY, float weight = 1.0f);

  /**
   * @brief Создаёт простой граф из двух узлов и одного ребра
   * @details Узлы в позициях (100,100) и (300,100), ребро с весом 2.5
   */
  void createTwoNodeGraph();

  /**
   * @brief Создаёт граф-цепочку из трёх узлов
   * @details Узлы в позициях (0,0), (200,0), (400,0) с рёбрами весом 1.0
   */
  void createThreeNodeChain();

  /**
   * @brief Создаёт циклический граф из трёх узлов
   * @details Узлы образуют треугольник A->B->C->A с рёбрами весом 1.0
   */
  void createThreeNodeCycle();

  /**
   * @brief Полностью очищает сцену и модель графа
   */
  void clearGraph();

  /**
   * @brief Сохраняет текущий граф во временный файл
   * @return Путь к сохранённому временному файлу
   */
  QString saveGraphToTempFile();

  /**
   * @brief Загружает граф из файла через главное окно
   * @param filepath Путь к файлу для загрузки
   * @return true если загрузка успешна, false в противном случае
   */
  bool loadGraphFromFile(const QString &filepath);

  /**
   * @brief Проверяет существование узла по координатам
   * @param x Координата X центра узла
   * @param y Координата Y центра узла
   * @param expectedId Ожидаемый ID узла (0 - не проверять)
   */
  void assertNodeExists(double x, double y, size_t expectedId = 0);

  /**
   * @brief Проверяет существование ребра между узлами
   * @param fromId ID начального узла
   * @param toId ID конечного узла
   * @param expectedWeight Ожидаемый вес ребра
   */
  void assertEdgeExists(size_t fromId, size_t toId,
                        float expectedWeight = 1.0f);
};

#endif // E2E_TESTS_HPP
