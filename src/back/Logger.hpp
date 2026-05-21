#pragma once

#include <QDateTime>
#include <QList>
#include <QString>

/**
 * @brief Статусы сообщений логгера.
 */
enum STATUS {
  ERROR,   ///< Ошибка
  WARNING, ///< Предупреждение
  INFO,    ///< Информация
  SUCCESS  ///< Успех
};

/**
 * @brief Класс для логирования сообщений.
 *
 * Все методы статические. Лог хранится в памяти и может быть сохранён в файл.
 */
class Logger {
public:
  Logger(bool timeStamp);
  /**
   * @brief Добавляет сообщение в лог.
   * @param status Статус сообщения (ERROR, WARNING, INFO, SUCCESS)
   * @param message Текст сообщения
   */
  void addMessage(STATUS status, const QString &message);

  /**
   * @brief Возвращает весь лог в виде строки.
   * @return Строка со всеми сообщениями, разделёнными переносом строки
   */
  QString getLog();

  /**
   * @brief Возвращает лог, отфильтрованный по статусу.
   * @param status Статус для фильтрации
   * @return Строка с сообщениями указанного статуса
   */
  QString getLogByStatus(STATUS status);

  /**
   * @brief Сохраняет лог в файл.
   * @param path Путь к файлу
   * @return Количество сохранённых сообщений, -1 при ошибке открытия файла
   */
  int saveToFile(const QString &path);

  /**
   * @brief Сохраняет лог в файл с фильтрацией по минимальному статусу.
   * @param path Путь к файлу
   * @param minStatus Минимальный статус для сохранения
   * @return Количество сохранённых сообщений, -1 при ошибке открытия файла
   */
  int saveToFile(const QString &path, STATUS minStatus);

  /**
   * @brief Очищает все сообщения из лога.
   */
  void clear();

  /**
   * @brief Включает/выключает добавление временных меток.
   * @param include true - добавлять, false - не добавлять
   */
  void setIncludeTimestamp(bool include);

private:
  /**
   * @brief Структура одной записи лога.
   */
  struct Entry {
    QDateTime timestamp; ///< Время добавления сообщения
    STATUS status;       ///< Статус сообщения
    QString message;     ///< Текст сообщения
  };

  /**
   * @brief Запрещаем создание экземпляров.
   */
  Logger() = delete;

  /**
   * @brief Преобразует статус в строку.
   * @param status Статус для преобразования
   * @return Строковое представление статуса
   */
  static QString statusToString(STATUS status);

  /**
   * @brief Форматирует одну запись лога в строку.
   * @param timestamp Временная метка
   * @param status Статус сообщения
   * @param message Текст сообщения
   * @return Отформатированная строка
   */
  QString formatEntry(const QDateTime &timestamp, STATUS status,
                      const QString &message);

  QList<Entry> entries_;  ///< Список всех записей лога
  bool includeTimestamp_; ///< Флаг добавления временных меток
};
