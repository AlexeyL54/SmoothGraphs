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
  void startEdgeCreation(Figure *startNode);

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
  QGraphicsScene *scene_;       // Указатель на управляемую сцену
  bool isCreatingEdge_ = false; // Флаг активного режима создания ребра
  Figure *startNode_ = nullptr; // Начальный узел для создаваемого ребра
  QGraphicsLineItem *tempEdge_ = nullptr; // Временное ребро для визуализации
  QPointF startPos_; // Стартовая позиция для отрисовки ребра

private slots:
  /**
   * @brief Слот для добавления новой фигуры на сцену.
   *
   * Создаёт экземпляр Figure красного цвета, разрешает перемещение
   * и добавляет его в сцену.
   */
  void addFigure();

  /**
   * @brief Слот для очистки сцены.
   *
   * Удаляет все элементы со сцены и сбрасывает внутренние состояния,
   * связанные с созданием рёбер.
   */
  void clear();
};
