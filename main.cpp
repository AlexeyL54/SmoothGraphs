#include "src/front/MainWindow.hpp"
#include "src/front/ThemeManager.hpp"
#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  app.setAttribute(Qt::AA_UseStyleSheetPropagationInWidgetStyles);

  ThemeManager themeMng;

  MainWindow mainWindow(themeMng);
  mainWindow.show();

  return app.exec();
}
