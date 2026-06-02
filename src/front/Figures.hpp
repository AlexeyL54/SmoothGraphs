#pragma once

#include <QColor>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsScale>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsView>
#include <QList>
#include <QMenu>
#include <QObject>
#include <QPainter>
#include <QPen>
#include <QPointF>

#include <cstddef>

class SmoothNode;

/**
 * @brief Вспомогательный класс для связи узлов с GraphView
 *
 * Реализует паттерн Singleton для передачи сигналов выбора узлов между
 * компонентами.
 */
class NodeSelectionBridge : public QObject {
  Q_OBJECT
public:
  /**
   * @brief Возвращает единственный экземпляр класса
   * @return NodeSelectionBridge* Указатель на экземпляр NodeSelectionBridge
   */
  static NodeSelectionBridge *instance() {
    static NodeSelectionBridge bridge;
    return &bridge;
  }

signals:
  /**
   * @brief Сигнал запроса установки стартового узла
   * @param node Указатель на узел, который нужно сделать стартовым
   */
  void setStartNodeRequested(SmoothNode *node);

  /**
   * @brief Сигнал запроса установки конечного узла
   * @param node Указатель на узел, который нужно сделать конечным
   */
  void setEndNodeRequested(SmoothNode *node);

  /**
   * @brief Сигнал запроса снятия статуса стартового узла
   * @param node Указатель на узел, у которого нужно снять статус стартового
   */
  void clearStartNodeRequested(SmoothNode *node);

  /**
   * @brief Сигнал запроса снятия статуса конечного узла
   * @param node Указатель на узел, у которого нужно снять статус конечного
   */
  void clearEndNodeRequested(SmoothNode *node);
};

/**
 * @class SmoothEdge
 * @brief Класс, представляющий ребро графа между двумя узлами.
 *
 * SmoothEdge отображает направленное ребро между двумя узлами графа,
 * позволяет настраивать вес ребра, цветовую схему и подсветку.
 */
class SmoothEdge : public QObject, public QGraphicsLineItem {
  Q_OBJECT
public:
  /**
   * @brief Конструктор ребра
   * @param start SmoothNode* Указатель на начальный узел
   * @param end SmoothNode* Указатель на конечный узел
   * @param parent QGraphicsItem* Родительский графический элемент (по умолчанию
   * nullptr)
   */
  SmoothEdge(SmoothNode *start, SmoothNode *end,
             QGraphicsItem *parent = nullptr);

  /**
   * @brief Деструктор ребра
   */
  ~SmoothEdge();

  /**
   * @brief Обновляет позицию ребра в соответствии с положением узлов
   */
  void updatePosition();

  /**
   * @brief Возвращает указатель на начальный узел
   * @return SmoothNode* Указатель на начальный узел
   */
  SmoothNode *getStartNode() const { return startNode_; }

  /**
   * @brief Возвращает указатель на конечный узел
   * @return SmoothNode* Указатель на конечный узел
   */
  SmoothNode *getEndNode() const { return endNode_; }

  /**
   * @brief Устанавливает вес ребра
   * @param weight float Новый вес ребра
   */
  void setWeight(float weight);

  /**
   * @brief Возвращает вес ребра
   * @return float Текущий вес ребра
   */
  float getWeight() const { return weight_; }

  /**
   * @brief Обновляет стиль ребра в соответствии с текущей темой
   * @param colors const ThemeColors& Ссылка на цветовую схему темы
   */
  void updateThemeStyle(const class ThemeColors &colors);

  /**
   * @brief Устанавливает или снимает подсветку ребра
   * @param highlight bool true - включить подсветку, false - выключить
   */
  void setHighlighted(bool highlight);

  /**
   * @brief Проверяет, подсвечено ли ребро
   * @return bool true - ребро подсвечено, false - нет
   */
  bool isHighlighted() const { return isHighlighted_; }

  /**
   * @brief Возвращает ограничивающий прямоугольник ребра
   * @return QRectF Ограничивающий прямоугольник
   */
  QRectF boundingRect() const override;

protected:
  /**
   * @brief Обработчик контекстного меню ребра
   * @param event QGraphicsSceneContextMenuEvent* Событие контекстного меню
   */
  void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

  /**
   * @brief Переопределённый метод отрисовки ребра
   * @param painter QPainter* Указатель на объект рисования
   * @param option const QStyleOptionGraphicsItem* Опции стиля графического
   * элемента
   * @param widget QWidget* Указатель на виджет-родитель
   */
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *widget) override;

private:
  SmoothNode *startNode_;      ///< Указатель на начальный узел
  SmoothNode *endNode_;        ///< Указатель на конечный узел
  float weight_;               ///< Вес ребра
  QColor defaultColor_;        ///< Цвет ребра по умолчанию
  QColor highlightColor_;      ///< Цвет подсвеченного ребра
  QColor textColor_;           ///< Цвет текста веса
  bool isHighlighted_ = false; ///< Флаг подсветки

  /**
   * @brief Вычисляет позиции для отрисовки стрелки
   * @param line QLineF& Ссылка на линию ребра (будет модифицирована)
   * @return std::pair<QPointF, QPointF> Пара точек для отрисовки стрелки
   */
  std::pair<QPointF, QPointF> computeArrowPos(QLineF &line);

  /**
   * @brief Отрисовывает стрелку на конце ребра
   * @param painter QPainter* Указатель на объект рисования
   * @param line QLineF& Ссылка на линию ребра
   * @param p1 QPointF Первая точка стрелки
   * @param p2 QPointF Вторая точка стрелки
   */
  void paintArrow(QPainter *painter, QLineF &line, QPointF p1, QPointF p2);

  /**
   * @brief Отрисовывает текст веса ребра
   * @param painter QPainter* Указатель на объект рисования
   * @param line const QLineF& Ссылка на линию ребра
   */
  void paintWeight(QPainter *painter, const QLineF &line);

  /**
   * @brief Показывает диалог ввода веса ребра
   */
  void showWeightDialog();

  mutable QPolygonF cachedArrowHead_; ///< Кэшированная форма стрелки
  mutable QLineF cachedLine_;         ///< Кэшированная линия

signals:
  /**
   * @brief Сигнал об удалении ребра
   * @param self SmoothEdge* Указатель на удаляемое ребро
   */
  void edgeAboutToBeDeleted(SmoothEdge *self);
};

///////////////////////////////////////////////////////////////////////////

/**
 * @brief Перечисление ролей узла
 */
enum class NodeRole {
  Normal, ///< Обычный узел
  Start,  ///< Стартовый узел
  End     ///< Конечный узел
};

/**
 * @class SmoothNode
 * @brief Класс, представляющий узел графа.
 *
 * SmoothNode отображает узел графа в виде круга, поддерживает перемещение,
 * контекстное меню, роли (стартовый/конечный) и управление инцидентными
 * рёбрами.
 */
class SmoothNode : public QObject, public QGraphicsEllipseItem {
  Q_OBJECT
public:
  /**
   * @brief Конструктор узла
   * @param centerX qreal X-координата центра узла
   * @param centerY qreal Y-координата центра узла
   * @param radius qreal Радиус узла
   * @param parent QGraphicsItem* Родительский графический элемент (по умолчанию
   * nullptr)
   */
  SmoothNode(qreal centerX, qreal centerY, qreal radius,
             QGraphicsItem *parent = nullptr);

  /**
   * @brief Добавляет входящее ребро
   * @param edge SmoothEdge* Указатель на добавляемое входящее ребро
   */
  void addIncomingEdge(SmoothEdge *edge);

  /**
   * @brief Добавляет исходящее ребро
   * @param edge SmoothEdge* Указатель на добавляемое исходящее ребро
   */
  void addOutgoingEdge(SmoothEdge *edge);

  /**
   * @brief Удаляет входящее ребро
   * @param edge SmoothEdge* Указатель на удаляемое входящее ребро
   */
  void removeIncomingEdge(SmoothEdge *edge);

  /**
   * @brief Удаляет исходящее ребро
   * @param edge SmoothEdge* Указатель на удаляемое исходящее ребро
   */
  void removeOutgoingEdge(SmoothEdge *edge);

  /**
   * @brief Очищает все входящие рёбра
   */
  void clearIncomingEdges();

  /**
   * @brief Очищает все исходящие рёбра
   */
  void clearOutcomingEdges();

  /**
   * @brief Возвращает центр узла
   * @return QPointF Координаты центра узла
   */
  QPointF getCenter() const;

  /**
   * @brief Устанавливает цвет при наведении
   * @param color const QColor& Новый цвет при наведении
   */
  void setHoverColor(const QColor &color);

  /**
   * @brief Обновляет стиль узла в соответствии с текущей темой
   * @param colors const ThemeColors& Ссылка на цветовую схему темы
   */
  void updateThemeStyle(const class ThemeColors &colors);

  /**
   * @brief Восстанавливает цвет узла по умолчанию
   */
  void restoreDefaultColor();

  /**
   * @brief Обновляет цвет узла в зависимости от его роли
   */
  void updateNodeColor();

  /**
   * @brief Возвращает список входящих рёбер
   * @return QList<SmoothEdge*> Список указателей на входящие рёбра
   */
  QList<SmoothEdge *> getIncomingEdges();

  /**
   * @brief Возвращает список исходящих рёбер
   * @return QList<SmoothEdge*> Список указателей на исходящие рёбра
   */
  QList<SmoothEdge *> getOutcomingEdges();

  /**
   * @brief Устанавливает идентификатор узла
   * @param id size_t Новый идентификатор
   */
  void setId(size_t id) { id_ = id; }

  /**
   * @brief Возвращает идентификатор узла
   * @return size_t Текущий идентификатор узла
   */
  size_t getId() const { return id_; }

  /**
   * @brief Устанавливает роль узла
   * @param role NodeRole Новая роль узла
   */
  void setRole(NodeRole role);

  /**
   * @brief Возвращает роль узла
   * @return NodeRole Текущая роль узла
   */
  NodeRole getRole() const { return role_; }

  /**
   * @brief Сбрасывает подсветку пути
   */
  void resetPathHighlight();

  /**
   * @brief Возвращает радиус узла
   * @return qreal Текущий радиус узла
   */
  qreal getRadius() const { return rect().width() / 2; }

protected:
  /**
   * @brief Обработчик контекстного меню узла
   * @param event QGraphicsSceneContextMenuEvent* Событие контекстного меню
   */
  void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

  /**
   * @brief Обработчик входа курсора в область узла
   * @param event QGraphicsSceneHoverEvent* Событие наведения курсора
   */
  void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

  /**
   * @brief Обработчик выхода курсора из области узла
   * @param event QGraphicsSceneHoverEvent* Событие наведения курсора
   */
  void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

  /**
   * @brief Обработчик изменения свойств узла (например, позиции)
   * @param change GraphicsItemChange Тип изменения
   * @param value const QVariant& Новое значение свойства
   * @return QVariant Результат обработки изменения
   */
  QVariant itemChange(GraphicsItemChange change,
                      const QVariant &value) override;

  /**
   * @brief Переопределённый метод отрисовки узла
   * @param painter QPainter* Указатель на объект рисования
   * @param option const QStyleOptionGraphicsItem* Опции стиля графического
   * элемента
   * @param widget QWidget* Указатель на виджет-родитель
   */
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *widget) override;

private:
  QList<SmoothEdge *> incomingEdges_; ///< Список входящих рёбер
  QList<SmoothEdge *> outgoingEdges_; ///< Список исходящих рёбер

  QColor defaultColor_;   ///< Цвет узла по умолчанию
  QColor hoverColor_;     ///< Цвет узла при наведении
  QColor borderColor_;    ///< Цвет границы узла
  QColor startNodeColor_; ///< Цвет стартового узла
  QColor endNodeColor_;   ///< Цвет конечного узла
  QColor pathNodeColor_;  ///< Цвет узла на пути
  QColor textColor_;      ///< Цвет текста ID

  size_t id_;                        ///< Уникальный идентификатор узла
  NodeRole role_ = NodeRole::Normal; ///< Роль узла
  bool isOnPath_ = false;            ///< Нахождения узла на выделенном пути

  /**
   * @brief Добавляет ребро от данного узла
   */
  void addEdge();

  /**
   * @brief Устанавливает узел как стартовый
   */
  void setAsStart();

  /**
   * @brief Устанавливает узел как конечный
   */
  void setAsEnd();

  /**
   * @brief Снимает статус стартового узла
   */
  void clearStart();

  /**
   * @brief Снимает статус конечного узла
   */
  void clearEnd();

  /**
   * @brief Возвращает родительское представление (GraphView)
   * @return QGraphicsView* Указатель на родительское представление или nullptr
   */
  QGraphicsView *getParentView() const;

signals:
  /**
   * @brief Сигнал об удалении узла
   * @param self SmoothNode* Указатель на удаляемый узел
   */
  void nodeAboutToBeDeleted(SmoothNode *self);
};
