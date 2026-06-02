#pragma once

#include <QContextMenuEvent>
#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QPointF>
#include <QPushButton>

#include "Figures.hpp"
#include "ThemeManager.hpp"
#include "qtmetamacros.h"

/**
 * @brief Основной виджет для отображения и редактирования графа
 *
 * Класс GraphView предоставляет графический интерфейс для работы с графом,
 * включая создание узлов и рёбер, масштабирование, выделение пути и управление
 * темами.
 */
class GraphView : public QGraphicsView {
  Q_OBJECT

public:
  /**
   * @brief Конструктор класса GraphView
   * @param scene Указатель на графическую сцену
   * @param parent Родительский виджет (по умолчанию nullptr)
   */
  explicit GraphView(QGraphicsScene *scene, QWidget *parent = nullptr);

  /**
   * @brief Начинает процесс создания ребра от указанного узла
   * @param startNode Начальный узел, от которого создаётся ребро
   */
  void startEdgeCreation(SmoothNode *startNode);

  /**
   * @brief Обновляет тему оформления для всех элементов на сцене
   * @param colors Цветовая схема темы
   */
  void updateAllElementsTheme(const ThemeColors &colors);

  /**
   * @brief Очищает всю сцену, удаляя все узлы и рёбра
   */
  void clearScene();

  /**
   * @brief Устанавливает указанный узел как стартовый для поиска пути
   * @param node Узел, который станет стартовым
   */
  void setStartNode(SmoothNode *node);

  /**
   * @brief Устанавливает указанный узел как конечный для поиска пути
   * @param node Узел, который станет конечным
   */
  void setEndNode(SmoothNode *node);

  /**
   * @brief Сбрасывает статус стартового узла
   * @param node Узел, у которого снимается статус стартового
   */
  void clearStartNode(SmoothNode *node);

  /**
   * @brief Сбрасывает статус конечного узла
   * @param node Узел, у которого снимается статус конечного
   */
  void clearEndNode(SmoothNode *node);

  /**
   * @brief Возвращает указатель на текущий стартовый узел
   * @return Указатель на стартовый узел или nullptr
   */
  SmoothNode *getStartNode() const { return pathStartNode_; }

  /**
   * @brief Возвращает указатель на текущий конечный узел
   * @return Указатель на конечный узел или nullptr
   */
  SmoothNode *getEndNode() const { return pathEndNode_; }

  /**
   * @brief Подсвечивает указанный путь на графе
   * @param path Вектор указателей на узлы, составляющие путь
   */
  void highlightPath(const std::vector<SmoothNode *> &path);

  /**
   * @brief Снимает подсветку с текущего пути
   */
  void clearPathHighlight();

  /**
   * @brief Устанавливает менеджер тем для управления оформлением
   * @param manager Указатель на менеджер тем
   */
  void setThemeManager(ThemeManager *manager) { themeMng_ = manager; }

  /**
   * @brief Возвращает указатель на менеджер тем
   * @return Указатель на менеджер тем
   */
  ThemeManager *getThemeManager() const { return themeMng_; }

  /**
   * @brief Увеличивает масштаб сцены
   */
  void zoomIn();

  /**
   * @brief Уменьшает масштаб сцены
   */
  void zoomOut();

  /**
   * @brief Проверяет, активен ли режим создания ребра
   * @return true если режим активен, false в противном случае
   */
  bool isEdgeCreationActive() const { return isCreatingEdge_; }

signals:
  /**
   * @brief Сигнал, испускаемый при добавлении нового узла
   * @param node Добавленный узел
   */
  void nodeAdded(SmoothNode *node);

  /**
   * @brief Сигнал, испускаемый при добавлении нового ребра
   * @param edge Добавленное ребро
   */
  void edgeAdded(SmoothEdge *edge);

  /**
   * @brief Сигнал, испускаемый при удалении узла
   * @param node Удалённый узел
   */
  void nodeRemoved(SmoothNode *node);

  /**
   * @brief Сигнал, испускаемый при удалении ребра
   * @param edge Удалённое ребро
   */
  void edgeRemoved(SmoothEdge *edge);

  /**
   * @brief Сигнал об изменении стартового узла
   * @param node Новый стартовый узел или nullptr
   */
  void startNodeChanged(SmoothNode *node);

  /**
   * @brief Сигнал об изменении конечного узла
   * @param node Новый конечный узел или nullptr
   */
  void endNodeChanged(SmoothNode *node);

  /**
   * @brief Сигнал запроса на поиск пути между стартовым и конечным узлами
   */
  void findPathRequested();

  /**
   * @brief Сигнал об очистке сцены
   */
  void sceneCleared();

protected:
  /**
   * @brief Обработчик контекстного меню
   * @param event Событие контекстного меню
   */
  void contextMenuEvent(QContextMenuEvent *event) override;

  /**
   * @brief Обработчик нажатия кнопки мыши
   * @param event Событие мыши
   */
  void mousePressEvent(QMouseEvent *event) override;

  /**
   * @brief Обработчик движения мыши
   * @param event Событие движения мыши
   */
  void mouseMoveEvent(QMouseEvent *event) override;

  /**
   * @brief Обработчик изменения размера виджета
   * @param event Событие изменения размера
   */
  void resizeEvent(QResizeEvent *event) override;

  /**
   * @brief Обработчик нажатия клавиш клавиатуры
   * @param event Событие клавиатуры
   */
  void keyPressEvent(QKeyEvent *event) override;

private:
  QGraphicsScene *scene_;               ///< Указатель на графическую сцену
  bool isCreatingEdge_ = false;         ///< Флаг активного создания ребра
  SmoothNode *tempStartNode_ = nullptr; ///< Временный начальный узел
  QGraphicsLineItem *tempEdge_ =
      nullptr;       ///< Временная линия ребра при создании
  QPointF startPos_; ///< Начальная позиция при создании ребра

  SmoothNode *pathStartNode_ = nullptr;   ///< Стартовый узел для поиска пути
  SmoothNode *pathEndNode_ = nullptr;     ///< Конечный узел для поиска пути
  std::vector<SmoothNode *> currentPath_; ///< Текущий выделенный путь

  ThemeManager *themeMng_ = nullptr; ///< Менеджер тем оформления

  QPushButton *zoomInBtn_;  ///< Кнопка увеличения масштаба
  QPushButton *zoomOutBtn_; ///< Кнопка уменьшения масштаба
  QPushButton *helpBtn_;    ///< Кнопка вызова справки

  qreal currentZoom_ = 1.0;     ///< Текущий коэффициент масштаба
  const qreal ZOOM_STEP = 1.25; ///< Шаг изменения масштаба
  const qreal MIN_ZOOM = 0.2;   ///< Минимальный масштаб
  const qreal MAX_ZOOM = 5.0;   ///< Максимальный масштаб

  /**
   * @brief Настраивает кнопки масштабирования
   */
  void setupZoomButtons();

  /**
   * @brief Настраивает кнопку справки
   */
  void setupHelpButton();

  /**
   * @brief Обновляет позиции кнопок на форме
   */
  void updateButtonsPosition();

  /**
   * @brief Отображает диалоговое окно справки
   */
  void showHelpDialog();

public slots:
  /**
   * @brief Добавляет новый узел в указанной позиции
   * @param pos Позиция для добавления узла
   */
  void addFigure(const QPointF &pos);

  /**
   * @brief Настраивает мост для выбора узлов через сигналы
   */
  void setupNodeSelectionBridge();
};
