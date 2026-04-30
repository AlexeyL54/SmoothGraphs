// GraphView.hpp
#pragma once

#include <QContextMenuEvent>
#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QPointF>

#include "Figures.hpp"

/**
 * @class GraphView
 * @brief Представление графа на основе QGraphicsView.
 *
 * Класс GraphView расширяет функциональность QGraphicsView для работы с
 * графами:
 * - Добавление узлов (фигур) через контекстное меню
 * - Создание рёбер между узлами в интерактивном режиме
 * - Очистка сцены
 * - Отслеживание перемещения мыши для отрисовки временных рёбер
 */
class GraphView : public QGraphicsView {
  Q_OBJECT

public:
  /**
   * @brief Конструктор класса GraphView.
   * @param scene Указатель на сцену QGraphicsScene.
   * @param parent Родительский виджет (по умолчанию nullptr).
   */
  explicit GraphView(QGraphicsScene *scene, QWidget *parent = nullptr);

  /**
   * @brief Начинает процесс создания ребра от указанного узла.
   * @param startNode Указатель на начальный узел (фигуру).
   *
   * Метод активирует режим создания ребра: устанавливает курсор,
   * создаёт временное пунктирное ребро и отслеживает перемещение мыши.
   */
  void startEdgeCreation(SmoothNode *startNode);

  /**
   * @brief Обновляет стиль всех элементов на сцене.
   * @param colors Структура с цветами текущей темы.
   */
  void updateAllElementsTheme(const ThemeColors &colors);

  /**
   * @brief Очищает сцену с правильным порядком удаления элементов.
   *
   * Сначала удаляет все рёбра (отвязывая их от узлов),
   * затем удаляет все узлы. Это предотвращает ошибки при удалении.
   */
  void clearScene();

signals:
  /**
   * @brief Сигнал о добавлении нового узла на сцену.
   * @param node Указатель на добавленный узел.
   */
  void nodeAdded(SmoothNode *node);

  /**
   * @brief Сигнал о добавлении нового ребра на сцену.
   * @param edge Указатель на добавленное ребро.
   */
  void edgeAdded(SmoothEdge *edge);

  /**
   * @brief Сигнал об удалении узла со сцены.
   * @param node Указатель на удаляемый узел.
   */
  void nodeRemoved(SmoothNode *node);

  /**
   * @brief Сигнал об удалении ребра со сцены.
   * @param edge Указатель на удаляемое ребро.
   */
  void edgeRemoved(SmoothEdge *edge);

protected:
  /**
   * @brief Обработчик события контекстного меню.
   * @param event Указатель на событие контекстного меню.
   *
   * Если клик произошёл на пустом месте сцены, отображает меню с опциями:
   * - "Добавить узел" — создаёт новую фигуру
   * - "Очистить все" — удаляет все элементы со сцены
   */
  void contextMenuEvent(QContextMenuEvent *event) override;

  /**
   * @brief Обработчик события нажатия кнопки мыши.
   * @param event Указатель на событие мыши.
   *
   * В режиме создания ребра:
   * - ЛКМ: завершает создание ребра, если курсор над целевым узлом
   * - ПКМ: отменяет создание ребра
   */
  void mousePressEvent(QMouseEvent *event) override;

  /**
   * @brief Обработчик события перемещения мыши.
   * @param event Указатель на событие мыши.
   *
   * В режиме создания ребра обновляет положение временного ребра
   * от начального узла до текущей позиции курсора.
   */
  void mouseMoveEvent(QMouseEvent *event) override;

private:
  QGraphicsScene *scene_;           // Указатель на управляемую сцену
  bool isCreatingEdge_ = false;     // Флаг активного режима создания ребра
  SmoothNode *startNode_ = nullptr; // Начальный узел для создаваемого ребра
  QGraphicsLineItem *tempEdge_ = nullptr; // Временное ребро для визуализации
  QPointF startPos_; // Стартовая позиция для отрисовки ребра

private slots:
  /**
   * @brief Слот для добавления новой фигуры на сцену.
   *
   * Создаёт экземпляр Figure, разрешает перемещение
   * и добавляет его в сцену. Испускает сигнал nodeAdded.
   */
  void addFigure();

  /**
   * @brief Слот для очистки сцены.
   *
   * Удаляет все элементы со сцены и сбрасывает внутренние состояния,
   * связанные с созданием рёбер. Испускает сигналы nodeRemoved и edgeRemoved.
   */
  // void clear();
};
