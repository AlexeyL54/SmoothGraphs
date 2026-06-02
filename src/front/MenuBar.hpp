#pragma once

#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QMainWindow>
#include <QMenu>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QStyle>
#include <QToolButton>
#include <QVBoxLayout>

/**
 * @brief Класс панели меню приложения.
 *
 * Предоставляет виджет с кнопками управления:
 * открытие, сохранение (графа/решения), смена темы,
 * запуск/остановка алгоритма и сворачивание панели.
 */
class MenuBar : public QWidget {
  Q_OBJECT

public:
  /**
   * @brief Конструктор.
   * @param parent Родительский виджет.
   */
  explicit MenuBar(QWidget *parent = nullptr);

  /**
   * @brief Деструктор.
   */
  ~MenuBar();

  /**
   * @brief Возвращает максимальную высоту панели.
   * @return Максимальная высота в пикселях.
   */
  int maxHeight();

  // Геттеры для кнопок

  /**
   * @brief Возвращает указатель на кнопку "Открыть".
   * @return QPushButton* Указатель на кнопку открытия.
   */
  QPushButton *getOpenBtn() const { return openBtn_; }

  /**
   * @brief Возвращает указатель на кнопку "Сохранить".
   * @return QToolButton* Указатель на кнопку сохранения.
   */
  QToolButton *getSaveBtn() const { return saveBtn_; }

  /**
   * @brief Возвращает указатель на кнопку выбора темы.
   * @return QToolButton* Указатель на кнопку темы.
   */
  QToolButton *getThemeBtn() const { return themeBtn_; }

  /**
   * @brief Возвращает указатель на кнопку запуска.
   * @return QPushButton* Указатель на кнопку запуска.
   */
  QPushButton *getRunBtn() const { return runBtn_; }

  /**
   * @brief Возвращает указатель на кнопку остановки.
   * @return QPushButton* Указатель на кнопку остановки.
   */
  QPushButton *getStopBtn() const { return stopBtn_; }

signals:
  /**
   * @brief Сигнал запроса сохранения графа.
   */
  void saveGraphRequested();

  /**
   * @brief Сигнал запроса сохранения решения.
   */
  void saveSolutionRequested();

private:
  const int MAX_HEIGHT_ = 60; ///< Максимальная высота панели в пикселях.

  QVBoxLayout *mainLaout_ = nullptr;    ///< Главный вертикальный layout.
  QFrame *menuFrame_ = nullptr;         ///< Рамка, содержащая кнопки.
  QHBoxLayout *buttonLayout_ = nullptr; ///< Горизонтальный layout для кнопок.

  QPushButton *openBtn_ = nullptr;  ///< Кнопка "Открыть".
  QToolButton *saveBtn_ = nullptr;  ///< Кнопка "Сохранить" с выпадающим меню.
  QToolButton *themeBtn_ = nullptr; ///< Кнопка выбора темы.
  QPushButton *runBtn_ = nullptr;   ///< Кнопка запуска.
  QPushButton *stopBtn_ = nullptr;  ///< Кнопка остановки.
  QPushButton *wrapBtn_ = nullptr;  ///< Кнопка сворачивания/разворачивания.

  QPropertyAnimation *wrapAnimation_ = nullptr; ///< Анимация сворачивания.

  bool isExpanded_ = true; ///< Флаг: развёрнута ли панель.

  /**
   * @brief Настраивает все кнопки меню.
   */
  void setButtons();

  /**
   * @brief Добавляет кнопки в layout.
   */
  void addButtons();

  /**
   * @brief Настраивает анимацию сворачивания.
   */
  void setWrapAnimation();

  /**
   * @brief Сворачивает или разворачивает панель с анимацией.
   */
  void wrapMenu();
};
