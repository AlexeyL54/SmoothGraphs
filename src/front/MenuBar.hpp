#pragma once

#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QMainWindow>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QVBoxLayout>

#include "MainWindow.hpp"

/**
 * @brief Кастомный виджет - меню
 *
 * Структура компонента
 *
 *QVBoxLayout(Главный контейнер)
├── QFrame(Панель меню / Navbar)
│   └── QHBoxLayout
│       ├── QPushButton("Кнопка 1")
│       ├── QPushButton("Кнопка 2")
│       ├── QPushButton(Play Icon)
│       └── QPushButton(Stop Icon)
└── QPushButton(Кнопка - триггер для сворачивания)
 */
class MenuBar : public MainWindow {};
