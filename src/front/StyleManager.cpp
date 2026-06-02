#include "StyleManager.hpp"

// ============================================================================
// Публичные методы
// ============================================================================

/**
 * @brief Генерирует глобальную таблицу стилей приложения
 * @param colors Цветовая схема текущей темы
 * @return QString Полная таблица стилей для всех виджетов
 */
QString StyleManager::generateGlobalStyleSheet(const ThemeColors &colors) {
  return generateBaseWidgetStyles(colors) + generateButtonStyles(colors) +
         generateMenuStyles(colors) + generateInputStyles(colors) +
         generateListStyles(colors) + generateScrollBarStyles(colors) +
         generateStatusAndTooltipStyles(colors);
}

/**
 * @brief Генерирует таблицу стилей для панели меню
 * @param colors Цветовая схема текущей темы
 * @return QString Стили для меню-бара и его элементов
 */
QString StyleManager::generateMenuBarStyleSheet(const ThemeColors &colors) {
  return generateMenuBarContainerStyles(colors) +
         generateMenuBarButtonStyles(colors) +
         generateMenuBarMenuStyles(colors) +
         generateMenuBarWrapButtonStyles(colors);
}

/**
 * @brief Генерирует таблицу стилей для области отображения графа
 * @param colors Цветовая схема текущей темы
 * @return QString Стили для QGraphicsView
 */
QString StyleManager::generateGraphViewStyleSheet(const ThemeColors &colors) {
  return QString(R"(
        QGraphicsView {
            background-color: %1;
            border: none;
            outline: none;
        }
    )")
      .arg(formatColor(colors.background));
}

/**
 * @brief Генерирует таблицу стилей для плавающих кнопок
 * @return QString Стили для кнопок масштабирования и справки
 */
QString StyleManager::generateFloatingButtonsStyleSheet() {
  return R"(
        QPushButton {
            background-color: rgba(60, 60, 70, 200);
            color: white;
            border: none;
            border-radius: 20px;
            font-size: 20px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: rgba(80, 80, 100, 220);
        }
        QPushButton:pressed {
            background-color: rgba(50, 50, 60, 220);
        }
    )";
}

/**
 * @brief Генерирует таблицу стилей для диалога справки
 * @param colors Цветовая схема текущей темы
 * @return QString Стили для QMessageBox со справкой
 */
QString StyleManager::generateHelpDialogStyleSheet(const ThemeColors &colors) {
  return QString(R"(
        QMessageBox {
            background-color: %1;
            color: %2;
        }
        QMessageBox QPushButton {
            background-color: %3;
            color: %2;
            border: none;
            border-radius: 6px;
            padding: 8px 20px;
            font-size: 13px;
        }
        QMessageBox QPushButton:hover {
            background-color: %4;
        }
        QLabel {
            color: %2;
        }
    )")
      .arg(formatColor(colors.background))
      .arg(formatColor(colors.textPrimary))
      .arg(formatColor(colors.primary))
      .arg(formatColor(colors.hover));
}

// ============================================================================
// Приватные методы - Глобальные стили
// ============================================================================

/**
 * @brief Генерирует базовые стили для основных виджетов
 * @param colors Цветовая схема текущей темы
 * @return QString Стили для QWidget, QDialog, QMessageBox
 */
QString StyleManager::generateBaseWidgetStyles(const ThemeColors &colors) {
  return QString(R"(
        QWidget {
            background-color: %1;
            color: %2;
            font-family: 'Segoe UI', 'Microsoft YaHei', sans-serif;
        }
        
        QDialog {
            background-color: %1;
        }
        
        QMessageBox {
            background-color: %1;
        }
    )")
      .arg(formatColor(colors.background))
      .arg(formatColor(colors.textPrimary));
}

/**
 * @brief Генерирует стили для кнопок
 * @param colors Цветовая схема текущей темы
 * @return QString Стили для QPushButton в различных состояниях
 */
QString StyleManager::generateButtonStyles(const ThemeColors &colors) {
  return QString(R"(
        QPushButton {
            background-color: %1;
            color: %2;
            border: 1px solid %3;
            border-radius: 6px;
            padding: 6px 12px;
            font-size: 13px;
            font-weight: 500;
        }
        
        QPushButton:hover {
            background-color: %4;
            border-color: %5;
        }
        
        QPushButton:pressed {
            background-color: %6;
        }
        
        QPushButton:disabled {
            background-color: %7;
            color: %8;
            border-color: %3;
        }
    )")
      .arg(formatColor(colors.primary))
      .arg(formatColor(colors.textPrimary))
      .arg(formatColor(colors.border))
      .arg(formatColor(colors.hover))
      .arg(formatColor(colors.primaryLight))
      .arg(formatColor(colors.pressed))
      .arg(formatColor(colors.surface))
      .arg(formatColor(colors.textDisabled));
}

/**
 * @brief Генерирует стили для меню
 * @param colors Цветовая схема текущей темы
 * @return QString Стили для QMenu и QMenu::item
 */
QString StyleManager::generateMenuStyles(const ThemeColors &colors) {
  return QString(R"(
        QMenu {
            background-color: %1;
            color: %2;
            border: 1px solid %3;
            border-radius: 6px;
            padding: 5px;
        }
        
        QMenu::item {
            padding: 6px 30px 6px 20px;
            border-radius: 4px;
        }
        
        QMenu::item:selected {
            background-color: %4;
            color: %2;
        }
        
        QMenu::separator {
            height: 1px;
            background-color: %3;
            margin: 5px 10px;
        }
    )")
      .arg(formatColor(colors.background))
      .arg(formatColor(colors.textPrimary))
      .arg(formatColor(colors.border))
      .arg(formatColor(colors.hover));
}

/**
 * @brief Генерирует стили для полей ввода
 * @param colors Цветовая схема текущей темы
 * @return QString Стили для QLineEdit, QTextEdit, QPlainTextEdit
 */
QString StyleManager::generateInputStyles(const ThemeColors &colors) {
  return QString(R"(
        QLineEdit, QTextEdit, QPlainTextEdit {
            background-color: %1;
            color: %2;
            border: 1px solid %3;
            border-radius: 4px;
            padding: 5px;
            selection-background-color: %4;
        }
        
        QLineEdit:focus, QTextEdit:focus, QPlainTextEdit:focus {
            border-color: %4;
            outline: none;
        }
    )")
      .arg(formatColor(colors.surface))
      .arg(formatColor(colors.textPrimary))
      .arg(formatColor(colors.border))
      .arg(formatColor(colors.primaryLight));
}

/**
 * @brief Генерирует стили для списков и таблиц
 * @param colors Цветовая схема текущей темы
 * @return QString Стили для QListWidget, QTreeWidget, QTableWidget
 */
QString StyleManager::generateListStyles(const ThemeColors &colors) {
  return QString(R"(
        QListWidget, QTreeWidget, QTableWidget {
            background-color: %1;
            color: %2;
            border: 1px solid %3;
            border-radius: 4px;
            outline: none;
        }
        
        QListWidget::item:selected, 
        QTreeWidget::item:selected, 
        QTableWidget::item:selected {
            background-color: %4;
            color: %2;
        }
        
        QListWidget::item:hover, 
        QTreeWidget::item:hover, 
        QTableWidget::item:hover {
            background-color: %5;
        }
    )")
      .arg(formatColor(colors.surface))
      .arg(formatColor(colors.textPrimary))
      .arg(formatColor(colors.border))
      .arg(formatColor(colors.hover))
      .arg(formatColor(colors.hover));
}

/**
 * @brief Генерирует стили для скроллбаров
 * @param colors Цветовая схема текущей темы
 * @return QString Стили для QScrollBar
 */
QString StyleManager::generateScrollBarStyles(const ThemeColors &colors) {
  return QString(R"(
        QScrollBar:vertical {
            background-color: %1;
            width: 12px;
            border-radius: 6px;
            margin: 0px;
        }
        
        QScrollBar::handle:vertical {
            background-color: %2;
            border-radius: 6px;
            min-height: 20px;
        }
        
        QScrollBar::handle:vertical:hover {
            background-color: %3;
        }
        
        QScrollBar:horizontal {
            background-color: %1;
            height: 12px;
            border-radius: 6px;
            margin: 0px;
        }
        
        QScrollBar::handle:horizontal {
            background-color: %2;
            border-radius: 6px;
            min-width: 20px;
        }
        
        QScrollBar::handle:horizontal:hover {
            background-color: %3;
        }
    )")
      .arg(formatColor(colors.surface))
      .arg(formatColor(colors.primary))
      .arg(formatColor(colors.hover));
}

/**
 * @brief Генерирует стили для статусной строки и всплывающих подсказок
 * @param colors Цветовая схема текущей темы
 * @return QString Стили для QStatusBar и QToolTip
 */
QString
StyleManager::generateStatusAndTooltipStyles(const ThemeColors &colors) {
  return QString(R"(
        QStatusBar {
            background-color: %1;
            color: %2;
            border-top: 1px solid %3;
        }
        
        QToolTip {
            background-color: %4;
            color: %2;
            border: 1px solid %3;
            border-radius: 4px;
            padding: 4px;
        }
    )")
      .arg(formatColor(colors.surface))
      .arg(formatColor(colors.textPrimary))
      .arg(formatColor(colors.border))
      .arg(formatColor(colors.background));
}

// ============================================================================
// Приватные методы - Стили меню-бара
// ============================================================================

/**
 * @brief Генерирует стили контейнера панели меню
 * @param colors Цветовая схема текущей темы
 * @return QString Стили для #menubar
 */
QString
StyleManager::generateMenuBarContainerStyles(const ThemeColors &colors) {
  return QString(R"(
        #menubar {
            background-color: %1;
            border-radius: 15px;
            padding: 5px;
            border: 1px solid %2;
        }
    )")
      .arg(formatColor(colors.surface))
      .arg(formatColor(colors.border));
}

/**
 * @brief Генерирует стили кнопок панели меню
 * @param colors Цветовая схема текущей темы
 * @return QString Стили для QPushButton и QToolButton в меню-баре
 */
QString StyleManager::generateMenuBarButtonStyles(const ThemeColors &colors) {
  return QString(R"(
        QPushButton {
            background-color: transparent;
            color: %1;
            border: none;
            padding: 8px 15px;
            font-size: 14px;
            border-radius: 8px;
        }
        
        QPushButton:hover {
            background-color: %2;
        }
        
        QPushButton:pressed {
            background-color: %3;
            color: %1;
        }
        
        QToolButton {
            background-color: transparent;
            color: %1;
            border: none;
            padding: 8px 15px;
            font-size: 14px;
            border-radius: 8px;
        }
        
        QToolButton:hover {
            background-color: %2;
        }
        
        QToolButton:pressed {
            background-color: %3;
            color: %1;
        }
        
        QToolButton::menu-indicator {
            image: none;
        }
    )")
      .arg(formatColor(colors.textPrimary))
      .arg(formatColor(colors.hover))
      .arg(formatColor(colors.primary));
}

/**
 * @brief Генерирует стили выпадающих меню панели меню
 * @param colors Цветовая схема текущей темы
 * @return QString Стили для QMenu в меню-баре
 */
QString StyleManager::generateMenuBarMenuStyles(const ThemeColors &colors) {
  return QString(R"(
        QMenu {
            background-color: %1;
            color: %2;
            border: 1px solid %3;
            border-radius: 8px;
            padding: 5px;
        }
        
        QMenu::item {
            padding: 8px 30px 8px 15px;
            border-radius: 4px;
        }
        
        QMenu::item:selected {
            background-color: %4;
            color: %2;
        }
        
        QMenu::separator {
            height: 1px;
            background-color: %3;
            margin: 5px 10px;
        }
    )")
      .arg(formatColor(colors.background))
      .arg(formatColor(colors.textPrimary))
      .arg(formatColor(colors.border))
      .arg(formatColor(colors.hover));
}

/**
 * @brief Генерирует стили для кнопки обертки (wrap button)
 * @param colors Цветовая схема текущей темы
 * @return QString Стили для #wrapBtn
 */
QString
StyleManager::generateMenuBarWrapButtonStyles(const ThemeColors &colors) {
  return QString(R"(
        #wrapBtn {
            background-color: %1;
            border: 2px solid %2;
            border-radius: 15px;
        }
        
        #wrapBtn:hover {
            background-color: %2;
        }
    )")
      .arg(formatColor(colors.background))
      .arg(formatColor(colors.border));
}

// ============================================================================
// Утилиты
// ============================================================================

/**
 * @brief Преобразует QColor в строковый формат CSS
 * @param color Цвет для форматирования
 * @return QString Цвет в формате "#RRGGBB"
 */
QString StyleManager::formatColor(const QColor &color) { return color.name(); }
