#include "ThemeManager.hpp"

/**
 * @brief Конструктор менеджера тем
 * @param parent QObject* Родительский объект (по умолчанию nullptr)
 */
ThemeManager::ThemeManager(QObject *parent) : QObject(parent) {
  setTheme(Light);
}

/**
 * @brief Устанавливает текущую тему оформления
 * @param theme Theme Выбранная тема (Light или Dark)
 */
void ThemeManager::setTheme(Theme theme) {
  if (currentTheme_ == theme)
    return;
  currentTheme_ = theme;
  currentColors_ = loadTheme(theme);
  emit themeChanged();
}

/**
 * @brief Возвращает текущую активную тему
 * @return Theme Текущая тема оформления
 */
Theme ThemeManager::getTheme() { return currentTheme_; }

/**
 * @brief Возвращает цветовую схему текущей темы
 * @return ThemeColors Структура со всеми цветами текущей темы
 */
ThemeColors ThemeManager::getThemeColors() { return currentColors_; }

/**
 * @brief Загружает цветовую схему для указанной темы
 * @param theme Theme Тема, для которой нужно загрузить цвета
 * @return ThemeColors Готовая цветовая схема для запрошенной темы
 */
ThemeColors ThemeManager::loadTheme(Theme theme) const {
  ThemeColors colors;
  if (theme == Dark) {
    // --- Тёмная тема ---
    colors.background = QColor(30, 30, 30);     // #1e1e1e
    colors.surface = QColor(37, 37, 38);        // #252526
    colors.primary = QColor(0, 122, 204);       // #007acc
    colors.primaryDark = QColor(0, 102, 179);   // #0066b3
    colors.primaryLight = QColor(51, 153, 255); // #3399ff

    colors.textPrimary = QColor(204, 204, 204);   // #cccccc
    colors.textSecondary = QColor(153, 153, 153); // #999999
    colors.textDisabled = QColor(85, 85, 85);     // #555555

    colors.border = QColor(62, 62, 66);  // #3e3e42
    colors.divider = QColor(45, 45, 48); // #2d2d30

    colors.hover = QColor(42, 45, 46);     // #2a2d2e
    colors.pressed = QColor(0, 92, 153);   // #005c99
    colors.selected = QColor(0, 102, 179); // #0066b3

    colors.nodeDefault = QColor(100, 100, 100); // #646464
    colors.nodeHover = QColor(130, 130, 130);   // #828282
    colors.edgeDefault = QColor(70, 70, 70);    // #464646

    colors.success = QColor(39, 174, 96);  // #27ae60
    colors.error = QColor(231, 76, 60);    // #e74c3c
    colors.warning = QColor(241, 196, 15); // #f1c40f

    // --- Цвета для пути (Специальная палитра) ---
    colors.startNode = QColor(26, 188, 156); // #1abc9c
    colors.endNode = QColor(255, 107, 107);  // #ff6b6b
    colors.pathNode = QColor(243, 156, 18);  // #f39c12
    colors.pathEdge = QColor(243, 156, 18);  // #f39c12

  } else {
    // --- Светлая тема ---
    colors.background = QColor(243, 243, 243);  // #f3f3f3
    colors.surface = QColor(255, 255, 255);     // #ffffff
    colors.primary = QColor(0, 102, 179);       // #0066b3
    colors.primaryDark = QColor(0, 82, 143);    // #00528f
    colors.primaryLight = QColor(51, 153, 255); // #3399ff

    colors.textPrimary = QColor(51, 51, 51);      // #333333
    colors.textSecondary = QColor(102, 102, 102); // #666666
    colors.textDisabled = QColor(170, 170, 170);  // #aaaaaa

    colors.border = QColor(224, 224, 224);  // #e0e0e0
    colors.divider = QColor(238, 238, 238); // #eeeeee

    colors.hover = QColor(240, 240, 240);  // #f0f0f0
    colors.pressed = QColor(0, 82, 143);   // #00528f
    colors.selected = QColor(0, 102, 179); // #0066b3

    // --- Узлы и ребра (Нейтральные) ---
    colors.nodeDefault = QColor(220, 220, 220); // #dcdcdc
    colors.nodeHover = QColor(200, 200, 200);   // #c8c8c8
    colors.edgeDefault = QColor(150, 150, 150); // #969696

    // Статусы
    colors.success = QColor(46, 204, 113); // #2ecc71
    colors.error = QColor(231, 76, 60);    // #e74c3c
    colors.warning = QColor(241, 196, 15); // #f1c40f

    // --- Цвета для пути (Специальная палитра) ---
    colors.startNode = QColor(22, 160, 133); // #16a085
    colors.endNode = QColor(231, 76, 60);    // #e74c3c
    colors.pathNode = QColor(211, 84, 0);    // #d35400
    colors.pathEdge = QColor(211, 84, 0);    // #d35400
  }
  return colors;
}
