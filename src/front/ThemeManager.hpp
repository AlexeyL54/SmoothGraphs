#pragma once

#include <QColor>
#include <QObject>

enum Theme { Light, Dark };

struct ThemeColors {
  // Основные цвета
  QColor background;   // Фон приложения
  QColor surface;      // Поверхности (панели, карточки)
  QColor primary;      // Акцентный цвет
  QColor primaryDark;  // Темный вариант акцента
  QColor primaryLight; // Светлый вариант акцента

  // Цвета текста
  QColor textPrimary;   // Основной текст
  QColor textSecondary; // Второстепенный текст
  QColor textDisabled;  // Неактивный текст

  // Цвета границ и разделителей
  QColor border;  // Границы
  QColor divider; // Разделители

  // Состояния
  QColor hover;    // При наведении
  QColor pressed;  // При нажатии
  QColor selected; // Выбранный элемент

  // Специфические цвета
  QColor nodeDefault; // Узел графа
  QColor nodeHover;   // Узел при ховере
  QColor edgeDefault; // Ребро графа
  QColor success;     // Успешное действие
  QColor error;       // Ошибка
  QColor warning;     // Предупреждение
};

class ThemeManager : public QObject {
  Q_OBJECT
public:
  explicit ThemeManager(QObject *parent = nullptr);

  void setTheme(Theme theme);
  // void setTheme(ThemeColors customColors);
  Theme getTheme();
  ThemeColors getThemeColors();

signals:
  void themeChanged();

private:
  Theme currentTheme_;
  ThemeColors currentColors_;
  ThemeColors loadTheme(Theme theme) const;
};
