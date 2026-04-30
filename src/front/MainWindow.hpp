// MainWindow.hpp
#pragma once

#include <QApplication>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "../back/Graph.hpp"
#include "GraphView.hpp"
#include "MenuBar.hpp"
#include "ThemeManager.hpp"

/**
 * @class MainWindow
 * @brief Главное окно приложения для редактирования графов.
 *
 * Класс MainWindow объединяет все компоненты приложения:
 * - Менюбар для управления файлами и настройками
 * - Графическое представление графа (GraphView)
 * - Управление темами оформления
 * - Координацию между визуальным представлением и логической структурой графа
 */
class MainWindow : public QWidget {
  Q_OBJECT

public:
  /**
   * @brief Конструктор главного окна.
   * @param themeMng Менеджер тем для управления оформлением.
   * @param parent Родительский виджет (по умолчанию nullptr).
   */
  explicit MainWindow(ThemeManager &themeMng, QWidget *parent = nullptr);

  /**
   * @brief Обновляет стиль всего окна и всех компонентов.
   *
   * Применяет текущую тему ко всем виджетам:
   * - Глобальный стиль приложения
   * - Стиль менюбара
   * - Стиль графового представления
   * - Цвета узлов и рёбер
   */
  void updateStyle();

  /**
   * @brief Деструктор главного окна.
   */
  ~MainWindow();

  /**
   * @brief Возвращает указатель на менюбар.
   * @return Указатель на MenuBar.
   *
   * Используется для доступа к кнопкам менюбара из других частей приложения.
   */
  MenuBar *getMenuBar() const { return menuBar_; }

private:
  QGraphicsScene *scene_;          ///< Сцена для отрисовки графа
  GraphView *graphView_ = nullptr; ///< Представление графа
  MenuBar *menuBar_ = nullptr;     ///< Панель меню
  ThemeManager *themeMng_;         ///< Менеджер тем
  Graph *graph_;                   ///< Логическая структура графа

  /**
   * @brief Генерирует глобальную таблицу стилей для всего приложения.
   * @return QString с CSS стилями.
   *
   * Включает стили для:
   * - Базовых виджетов (QWidget, QPushButton)
   * - Меню (QMenu)
   * - Диалогов (QDialog, QMessageBox)
   * - Полей ввода (QLineEdit, QTextEdit)
   * - Списков (QListWidget, QTreeWidget, QTableWidget)
   * - Скроллбаров (QScrollBar)
   * - Статусбара и тултипов
   */
  QString generateGlobalStyleSheet() const;

  /**
   * @brief Генерирует таблицу стилей для менюбара.
   * @return QString с CSS стилями.
   *
   * Включает стили для:
   * - Контейнера менюбара
   * - Кнопок (QPushButton, QToolButton)
   * - Меню и их пунктов
   * - Специальной кнопки свёртки (wrapBtn)
   */
  QString generateMenuBarStyleSheet() const;

  /**
   * @brief Генерирует таблицу стилей для графового представления.
   * @return QString с CSS стилями.
   *
   * Устанавливает цвет фона для QGraphicsView.
   */
  QString generateGraphViewStyleSheet() const;

  /**
   * @brief Обновляет цвета всех узлов и рёбер на сцене.
   *
   * Применяет текущие цвета темы ко всем элементам графа:
   * - Цвет узлов по умолчанию и при наведении
   * - Цвет рёбер
   * - Цвет границ
   */
  void updateGraphColors();

  /**
   * @brief Отображает уведомление пользователю.
   * @param message Текст уведомления.
   * @param isError Флаг ошибки (true) или информации (false).
   */
  void showNotification(const QString &message, bool isError = false);

  /**
   * @brief Сохраняет граф в файл.
   * @param filepath Путь к файлу для сохранения.
   * @return true если сохранение успешно, false в противном случае.
   */
  bool saveGraphToFile(const QString &filepath);

  /**
   * @brief Загружает граф из файла.
   * @param filepath Путь к файлу для загрузки.
   * @return true если загрузка успешна, false в противном случае.
   */
  bool loadGraphFromFile(const QString &filepath);

protected:
  /**
   * @brief Обработчик события изменения размера окна.
   * @param event Событие изменения размера.
   *
   * Адаптирует размеры GraphView и MenuBar при изменении размера окна.
   */
  void resizeEvent(QResizeEvent *event) override;

private slots:
  /**
   * @brief Слот для обработки изменения темы.
   *
   * Вызывает updateStyle() для применения новой темы.
   */
  void onThemeChanged();

  /**
   * @brief Слот для сохранения графа.
   *
   * Реализует умное сохранение:
   * - Если граф загружен из файла и не модифицирован - сохраняет в тот же файл
   * - Если граф новый или нужно сохранить как - вызывает диалог выбора файла
   * - После сохранения выводит уведомление с путём к файлу
   */
  void onSaveGraph();

  /**
   * @brief Слот для открытия графа из файла.
   *
   * - Проверяет наличие несохранённых изменений
   * - Открывает диалог выбора файла (.gphz)
   * - Загружает и отображает граф
   */
  void onOpenGraph();
};
