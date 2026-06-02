#pragma once

#include "ThemeManager.hpp"
#include <QString>

/**
 * @brief Класс для генерации CSS стилей приложения
 *
 * StyleManager предоставляет статические методы для генерации
 * таблиц стилей для различных компонентов приложения на основе текущей темы.
 *
 * Основные секции стилей:
 * - Базовые стили для всех виджетов
 * - Стили для кнопок (QPushButton, QToolButton)
 * - Стили для меню (QMenu, QMenu::item)
 * - Стили для полей ввода (QLineEdit, QTextEdit)
 * - Стили для списков (QListWidget, QTreeWidget, QTableWidget)
 * - Стили для скроллбаров (QScrollBar)
 * - Стили для статусной строки и всплывающих подсказок (QStatusBar, QToolTip)
 */
class StyleManager {
public:
  /**
   * @brief Генерирует глобальную таблицу стилей приложения
   * @param colors Цветовая схема текущей темы
   * @return QString Полная таблица стилей для всех виджетов
   */
  static QString generateGlobalStyleSheet(const ThemeColors &colors);

  /**
   * @brief Генерирует таблицу стилей для панели меню
   * @param colors Цветовая схема текущей темы
   * @return QString Стили для меню-бара и его элементов
   */
  static QString generateMenuBarStyleSheet(const ThemeColors &colors);

  /**
   * @brief Генерирует таблицу стилей для области отображения графа
   * @param colors Цветовая схема текущей темы
   * @return QString Стили для QGraphicsView
   */
  static QString generateGraphViewStyleSheet(const ThemeColors &colors);

  /**
   * @brief Генерирует таблицу стилей для плавающих кнопок
   * @return QString Стили для кнопок масштабирования и справки
   */
  static QString generateFloatingButtonsStyleSheet();

  /**
   * @brief Генерирует таблицу стилей для диалога справки
   * @param colors Цветовая схема текущей темы
   * @return QString Стили для QMessageBox со справкой
   */
  static QString generateHelpDialogStyleSheet(const ThemeColors &colors);

private:
  /**
   * @brief Генерирует базовые стили для основных виджетов
   * @param colors Цветовая схема текущей темы
   * @return QString Стили для QWidget, QDialog, QMessageBox
   */
  static QString generateBaseWidgetStyles(const ThemeColors &colors);

  /**
   * @brief Генерирует стили для кнопок
   * @param colors Цветовая схема текущей темы
   * @return QString Стили для QPushButton в различных состояниях
   */
  static QString generateButtonStyles(const ThemeColors &colors);

  /**
   * @brief Генерирует стили для меню
   * @param colors Цветовая схема текущей темы
   * @return QString Стили для QMenu и QMenu::item
   */
  static QString generateMenuStyles(const ThemeColors &colors);

  /**
   * @brief Генерирует стили для полей ввода
   * @param colors Цветовая схема текущей темы
   * @return QString Стили для QLineEdit, QTextEdit, QPlainTextEdit
   */
  static QString generateInputStyles(const ThemeColors &colors);

  /**
   * @brief Генерирует стили для списков и таблиц
   * @param colors Цветовая схема текущей темы
   * @return QString Стили для QListWidget, QTreeWidget, QTableWidget
   */
  static QString generateListStyles(const ThemeColors &colors);

  /**
   * @brief Генерирует стили для скроллбаров
   * @param colors Цветовая схема текущей темы
   * @return QString Стили для QScrollBar
   */
  static QString generateScrollBarStyles(const ThemeColors &colors);

  /**
   * @brief Генерирует стили для статусной строки и всплывающих подсказок
   * @param colors Цветовая схема текущей темы
   * @return QString Стили для QStatusBar и QToolTip
   */
  static QString generateStatusAndTooltipStyles(const ThemeColors &colors);

  /**
   * @brief Генерирует стили контейнера панели меню
   * @param colors Цветовая схема текущей темы
   * @return QString Стили для #menubar
   */
  static QString generateMenuBarContainerStyles(const ThemeColors &colors);

  /**
   * @brief Генерирует стили кнопок панели меню
   * @param colors Цветовая схема текущей темы
   * @return QString Стили для QPushButton и QToolButton в меню-баре
   */
  static QString generateMenuBarButtonStyles(const ThemeColors &colors);

  /**
   * @brief Генерирует стили выпадающих меню панели меню
   * @param colors Цветовая схема текущей темы
   * @return QString Стили для QMenu в меню-баре
   */
  static QString generateMenuBarMenuStyles(const ThemeColors &colors);

  /**
   * @brief Генерирует стили для кнопки обертки (wrap button)
   * @param colors Цветовая схема текущей темы
   * @return QString Стили для #wrapBtn
   */
  static QString generateMenuBarWrapButtonStyles(const ThemeColors &colors);

  /**
   * @brief Преобразует QColor в строковый формат CSS
   * @param color Цвет для форматирования
   * @return QString Цвет в формате "#RRGGBB"
   */
  static QString formatColor(const QColor &color);
};
