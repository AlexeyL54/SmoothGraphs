#pragma once

#include <QColor>
#include <QObject>

/**
 * @brief Перечисление доступных тем оформления
 */
enum Theme {
  Light, ///< Светлая тема оформления
  Dark   ///< Тёмная тема оформления
};

/**
 * @brief Структура, содержащая все цвета для темы оформления
 *
 * ThemeColors объединяет все цветовые параметры для консистентного
 * оформления приложения, включая цвета фона, текста, границ,
 * а также специфические цвета для узлов и рёбер графа.
 */
struct ThemeColors {
  // Основные цвета
  QColor background;   ///< Цвет фона приложения
  QColor surface;      ///< Цвет поверхностей (панели, карточки)
  QColor primary;      ///< Основной акцентный цвет
  QColor primaryDark;  ///< Тёмный вариант акцентного цвета
  QColor primaryLight; ///< Светлый вариант акцентного цвета

  // Цвета текста
  QColor textPrimary;   ///< Цвет основного текста
  QColor textSecondary; ///< Цвет второстепенного текста
  QColor textDisabled;  ///< Цвет неактивного текста

  // Цвета границ и разделителей
  QColor border;  ///< Цвет границ элементов
  QColor divider; ///< Цвет разделителей

  // Состояния
  QColor hover;    ///< Цвет элемента при наведении курсора
  QColor pressed;  ///< Цвет элемента при нажатии
  QColor selected; ///< Цвет выбранного элемента

  // Специфические цвета для графа
  QColor nodeDefault; ///< Цвет обычного узла графа
  QColor nodeHover;   ///< Цвет узла при наведении курсора
  QColor edgeDefault; ///< Цвет ребра графа по умолчанию

  // Цвета статусов
  QColor success; ///< Цвет для обозначения успешного действия
  QColor error;   ///< Цвет для обозначения ошибки
  QColor warning; ///< Цвет для обозначения предупреждения

  // Цвета для визуализации пути
  QColor startNode; ///< Цвет стартового узла на пути
  QColor endNode;   ///< Цвет конечного узла на пути
  QColor pathNode;  ///< Цвет обычного узла, входящего в путь
  QColor pathEdge;  ///< Цвет ребра, входящего в путь
};

/**
 * @class ThemeManager
 * @brief Менеджер для управления темами оформления приложения
 *
 * ThemeManager реализует паттерн для переключения между светлой и тёмной
 * темами, а также предоставляет доступ к цветовым схемам для всех компонентов
 * приложения.
 */
class ThemeManager : public QObject {
  Q_OBJECT
public:
  /**
   * @brief Конструктор менеджера тем
   * @param parent QObject* Родительский объект (по умолчанию nullptr)
   */
  explicit ThemeManager(QObject *parent = nullptr);

  /**
   * @brief Устанавливает текущую тему оформления
   * @param theme Theme Выбранная тема (Light или Dark)
   */
  void setTheme(Theme theme);

  /**
   * @brief Возвращает текущую активную тему
   * @return Theme Текущая тема оформления
   */
  Theme getTheme();

  /**
   * @brief Возвращает цветовую схему текущей темы
   * @return ThemeColors Структура со всеми цветами текущей темы
   */
  ThemeColors getThemeColors();

signals:
  /**
   * @brief Сигнал об изменении темы оформления
   *
   * Испускается после успешной смены темы для уведомления
   * всех заинтересованных компонентов о необходимости обновить своё оформление.
   */
  void themeChanged();

private:
  Theme currentTheme_;        ///< Текущая активная тема
  ThemeColors currentColors_; ///< Цветовая схема текущей темы

  /**
   * @brief Загружает цветовую схему для указанной темы
   * @param theme Theme Тема, для которой нужно загрузить цвета
   * @return ThemeColors Готовая цветовая схема для запрошенной темы
   */
  ThemeColors loadTheme(Theme theme) const;
};
