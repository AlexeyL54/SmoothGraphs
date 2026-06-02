#pragma once

#include <QApplication>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "../back/Graph.hpp"
#include "../back/Logger.hpp"
#include "GraphView.hpp"
#include "MenuBar.hpp"
#include "ThemeManager.hpp"

/**
 * @brief Главное окно приложения.
 *
 * Класс MainWindow является центральным виджетом приложения, объединяющим
 * графическое представление графа (GraphView), панель меню (MenuBar) и
 * логику (Graph, Logger). Обрабатывает пользовательские действия,
 * управляет темами оформления, загрузкой/сохранением графов и решений,
 * а также визуализацией поиска кратчайшего пути.
 */
class MainWindow : public QWidget {
  Q_OBJECT

public:
  /**
   * @brief Конструктор главного окна.
   * @param themeMng Менеджер тем оформления (ссылка).
   * @param parent Родительский виджет (по умолчанию nullptr).
   */
  explicit MainWindow(ThemeManager &themeMng, QWidget *parent = nullptr);

  /**
   * @brief Обновляет стиль всего окна и дочерних элементов.
   *
   * Применяет текущую тему к глобальной таблице стилей приложения,
   * меню-бару и области графа.
   */
  void updateStyle();

  /**
   * @brief Деструктор. Освобождает память, занятую объектом Graph.
   */
  ~MainWindow();

  /**
   * @brief Возвращает указатель на панель меню.
   * @return MenuBar* Указатель на объект MenuBar (не nullptr).
   */
  MenuBar *getMenuBar() const { return menuBar_; }

  Graph *getGraph() const { return graph_; }

private:
  QGraphicsScene *scene_;          ///< Графическая сцена для отображения графа.
  GraphView *graphView_ = nullptr; ///< Виджет сцены (playground).
  MenuBar *menuBar_ = nullptr;     ///< Верхняя панель с кнопками управления.
  ThemeManager *themeMng_;         ///< Управление темами оформления.
  Graph *graph_;                   ///< Модель графа (логика, хранение данных).
  Logger logger_;                  ///< Логгер для записи шагов поиска пути.

  unsigned long long lastValidPathRevision_ = 0; ///< Счетчик изменений графа.
  bool hasValidPath_ = false; ///< Флаг актуальности кратчайшего пути.

  QString graphOpenFocusDir_ = ""; ///< Последняя директория для открытия графа.
  QString graphSaveFocusDir_ = ""; ///< Последняя директория сохранения графа.
  QString solutionFocusDir_ = "";  ///< Последняя директория сохранения решения.

  /**
   *@brief Инициализирует графическую сцену и представление.
   */
  void setupGraphicsView();

  /**
   * @brief Инициализирует меню-бар.
   */
  void setupMenuBar();

  /**
   * @brief Настраивает соединения сигналов и слотов.
   */
  void setupConnections();

  /**
   * @brief Настраивает соединения для работы с графом.
   */
  void setupGraphConnections();

  /**
   * @brief Настраивает соединения для UI компонентов.
   */
  void setupUIComponentsConnections();

  /**
   * @brief Настраивает соединения для обновления темы.
   */
  void setupThemeConnections();

  /**
   * @brief Инициализирует геометрию виджетов.
   */
  void setupInitialGeometry();

  /**
   * @brief Обновляет цвета всех элементов графа согласно текущей теме.
   */
  void updateGraphColors();

  /**
   * @brief Отображает всплывающее уведомление (информационное или об ошибке).
   * @param message Текст сообщения.
   * @param isError true, если сообщение об ошибке, false — информация.
   */
  void showNotification(const QString &message, bool isError = false);

  /**
   * @brief Сохраняет текущий граф в файл.
   * @param filepath Путь к файлу для сохранения.
   * @return true, если сохранение прошло успешно, иначе false.
   */
  bool saveGraphToFile(const QString &filepath);

  /**
   * @brief Загружает граф из файла.
   * @param filepath Путь к файлу для загрузки.
   * @return true, если загрузка прошла успешно, иначе false.
   */
  Q_INVOKABLE bool loadGraphFromFile(const QString &filepath);

  /**
   * @brief Выполняет поиск кратчайшего пути и визуализирует его.
   *
   * Использует выбранные стартовый и конечный узлы для поиска пути
   * через модель Graph. При успешном нахождении вызывает подсветку пути.
   */
  void findAndVisualizePath();

  /**
   * @brief Сохраняет лог решения (результат поиска пути) в файл.
   * @param filepath Путь к файлу для сохранения.
   * @return true, если сохранение прошло успешно, иначе false.
   */
  bool saveSolutionToFile(const QString &filepath);

protected:
  /**
   * @brief Обрабатывает событие изменения размера окна.
   * @param event Событие изменения размера.
   *
   * Пересчитывает геометрию graphView_ и menuBar_ при изменении размеров окна.
   */
  void resizeEvent(QResizeEvent *event) override;

private slots:
  void onThemeChanged(); ///< Слот для обновления интерфейса при смене темы.
  void onSaveGraph();    ///< Слот для сохранения графа в файл.
  void onSaveSolution(); ///< Слот для сохранения решения (лога) в файл.
  void onOpenGraph();    ///< Слот для открытия графа из файла.
  void onFindPath();     ///< Слот для запуска поиска кратчайшего пути.
  void onStopPath();     ///< Слот для очистки подсветки пути.
  void onGraphChanged(); ///< Слот для обработки изменения структуры графа.
  void onLoopFound();    ///< Слот для обработки обнаружения цикла в графе.
};
