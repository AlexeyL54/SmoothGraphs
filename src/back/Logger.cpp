#include "Logger.hpp"
#include <QFile>
#include <QTextStream>

Logger::Logger(bool timestamp) : includeTimestamp_(timestamp) {};

/**
 * @brief Преобразует статус в строку.
 * @param status Статус для преобразования
 * @return Строковое представление статуса
 */
QString Logger::statusToString(STATUS status) {
  switch (status) {
  case ERROR:
    return "ERROR";
  case WARNING:
    return "WARNING";
  case INFO:
    return "INFO";
  case SUCCESS:
    return "SUCCESS";
  default:
    return "UNKNOWN";
  }
}

/**
 * @brief Форматирует одну запись лога в строку.
 * @param timestamp Временная метка
 * @param status Статус сообщения
 * @param message Текст сообщения
 * @return Отформатированная строка
 */
QString Logger::formatEntry(const QDateTime &timestamp, STATUS status,
                            const QString &message) {
  QString result;
  if (includeTimestamp_) {
    result += timestamp.toString("yyyy-MM-dd hh:mm:ss.zzz") + " ";
  }
  result += "[" + statusToString(status) + "]: " + message;
  return result;
}

/**
 * @brief Добавляет сообщение в лог.
 * @param status Статус сообщения (ERROR, WARNING, INFO, SUCCESS)
 * @param message Текст сообщения
 */
void Logger::addMessage(STATUS status, const QString &message) {
  Entry entry;
  entry.timestamp = QDateTime::currentDateTime();
  entry.status = status;
  entry.message = message;
  entries_.append(entry);
}

/**
 * @brief Возвращает весь лог в виде строки.
 * @return Строка со всеми сообщениями, разделёнными переносом строки
 */
QString Logger::getLog() {
  QString result;
  for (const Entry &entry : entries_) {
    result += formatEntry(entry.timestamp, entry.status, entry.message) + "\n";
  }
  return result;
}

/**
 * @brief Возвращает лог, отфильтрованный по статусу.
 * @param status Статус для фильтрации
 * @return Строка с сообщениями указанного статуса
 */
QString Logger::getLogByStatus(STATUS status) {
  QString result;
  for (const Entry &entry : entries_) {
    if (entry.status == status) {
      result +=
          formatEntry(entry.timestamp, entry.status, entry.message) + "\n";
    }
  }
  return result;
}

/**
 * @brief Сохраняет лог в файл.
 * @param path Путь к файлу
 * @return Количество сохранённых сообщений, -1 при ошибке открытия файла
 */
int Logger::saveToFile(const QString &path) { return saveToFile(path, INFO); }

/**
 * @brief Сохраняет лог в файл с фильтрацией по минимальному статусу.
 * @param path Путь к файлу
 * @param minStatus Минимальный статус для сохранения
 * @return Количество сохранённых сообщений, -1 при ошибке открытия файла
 */
int Logger::saveToFile(const QString &path, STATUS minStatus) {
  QFile file(path);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    return -1;
  }

  QTextStream out(&file);
  int count = 0;

  for (const Entry &entry : entries_) {
    if (entry.status >= minStatus) {
      out << formatEntry(entry.timestamp, entry.status, entry.message) << "\n";
      count++;
    }
  }

  return count;
}

/**
 * @brief Очищает все сообщения из лога.
 */
void Logger::clear() { entries_.clear(); }

/**
 * @brief Включает/выключает добавление временных меток.
 * @param include true - добавлять, false - не добавлять
 */
void Logger::setIncludeTimestamp(bool include) { includeTimestamp_ = include; }
