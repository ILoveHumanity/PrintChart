#include "RawDataProcessor.h"
#include <QtSql>
#include <QFile>

DataTable SQLiteRawDataProcessor::getData(QString filePath){
    DataTable dataTable;

    // Ограничиваем время жизни объекта соединения (sqlitdb) отдельной областью видимости.
    // Это гарантирует, что sqlitdb уничтожится ДО вызова removeDatabase.
    {
        // Добавляем драйвер SQLite
        QSqlDatabase sqlitdb = QSqlDatabase::addDatabase("QSQLITE", "myConnection");
        sqlitdb.setDatabaseName(filePath);

        // Пытаемся открыть базу данных
        if (!sqlitdb.open()) {
            qDebug() << "Что-то пошло не так!" << sqlitdb.lastError().text();
        } else {
            QStringList tables = sqlitdb.tables();
            if(tables.size() >= 1){
                QSqlQuery query(sqlitdb);
                QString request = "SELECT Time, Value FROM " + tables[0] + " LIMIT 40;";
                if (query.exec(request)) {
                    while (query.next()) {
                        dataTable.append(qMakePair(query.value(0).toString(), query.value(1).toDouble()));
                    }
                } else {
                    qDebug() << "Ошибка выполнения запроса:" << query.lastError().text();
                }
            }
            // Закрываем базу данных
            sqlitdb.close();
        }
    }
    QSqlDatabase::removeDatabase("myConnection");

    return dataTable;
}

DataTable JsonRawDataProcessor::getData(QString filePath){
    DataTable dataTable;

    QFile file(filePath);

    // Пытаемся открыть файл для чтения
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // Считываем все содержимое файла в байтовый массив
        QByteArray jsonData = file.readAll();
        file.close();

        QJsonParseError errorPtr;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &errorPtr);

        // Проверяем, успешно ли прошел парсинг
        if (errorPtr.error == QJsonParseError::NoError) {
            if (jsonDoc.isArray()) {
                QJsonArray jsonArray = jsonDoc.array();

                const int MAX_RECORDS = 40;

                // Проходим по всем элементам массива с учетом лимита
                for (int i = 0; i < jsonArray.size() && i < MAX_RECORDS; ++i) {
                    QJsonObject jsonObj = jsonArray[i].toObject();

                    QString timeStr = jsonObj["Time"].toString();
                    double value = jsonObj["Value"].toDouble();

                    // Добавляем считанную пару в таблицу данных
                    dataTable.append(qMakePair(timeStr, value));
                }
            }
        } else {
            // Выводим сообщение об ошибке парсинга в консоль для отладки
            qDebug() << "Ошибка парсинга JSON:" << errorPtr.errorString();
        }
    } else {
        // Выводим сообщение об ошибке открытия файла
        qDebug() << "Не удалось открыть файл:" << file.errorString();
    }

    return dataTable;
}
