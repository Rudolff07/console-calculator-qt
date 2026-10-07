#include <QCoreApplication>
#include <QTextStream>
#include <QStringList>
#include <QFile>
#include "calculator.h"

// Печатаем список доступных команд
void printHelp(QTextStream &out)
{
    out << "Доступные команды:\n";
    out << " add <a> <b> - сложение\n";
    out << " sub <a> <b> - вычитание\n";
    out << " mul <a> <b> - умножение\n";
    out << " div <a> <b> - деление\n";
    out << " undo        - отменить последнюю операцию\n";
    out << " reset       - сброс\n";
    out << " help        - эта справка\n";
    out << " quit        - выход\n";
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QTextStream in(stdin);
    QTextStream out(stdout);

    Calculator calc;

    // Первое соединение: успешный результат -> в консоль
    QObject::connect(&calc, &Calculator::resultReady,
                     [&out](double result) {
                         out << "Результат: " << result << "\n";
                         out.flush();
                     });

    // Шаг 7: второй обработчик того же сигнала - пишем историю в файл.
    // Это демонстрирует принцип "один сигнал -> несколько слотов"
    QFile historyFile("history.txt");
    if (!historyFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        out << "Предупреждение: не удалось открыть history.txt для записи\n";
        out.flush();
    }
    QTextStream historyStream(&historyFile);

    QObject::connect(&calc, &Calculator::resultReady,
                     [&historyStream](double result) {
                         historyStream << "Result: " << result << "\n";
                         historyStream.flush();
                     });

    // Третье соединение: ошибки -> в консоль
    QObject::connect(&calc, &Calculator::errorOccurred,
                     [&out](const QString &msg) {
                         out << "Ошибка: " << msg << "\n";
                         out.flush();
                     });

    out << "=== Консольный калькулятор на Qt ===\n";
    printHelp(out);
    out << "\n> ";
    out.flush();

    // Основной цикл: читаем строки и обрабатываем команды
    QString line;
    while (in.readLineInto(&line)) {
        line = line.trimmed();

        if (line.isEmpty()) {
            out << "> ";
            out.flush();
            continue;
        }

        QStringList parts = line.split(' ', Qt::SkipEmptyParts);
        QString command = parts.value(0).toLower();

        // Выход
        if (command == "quit" || command == "exit") {
            out << "До свидания!\n";
            break;
        }

        // Справка
        if (command == "help") {
            printHelp(out);
            out << "> ";
            out.flush();
            continue;
        }

        // Сброс
        if (command == "reset") {
            calc.reset();
            out << "> ";
            out.flush();
            continue;
        }

        // Индивидуальное задание: undo - отмена последней операции.
        // Без аргументов, поэтому стоит до проверки на 3 токена
        if (command == "undo") {
            calc.undo();
            out << "> ";
            out.flush();
            continue;
        }

        // Арифметические команды требуют 3 токена: команда + 2 числа
        if (parts.size() != 3) {
            out << "Ошибка: неверный формат. Используйте: <команда> <a> <b>\n";
            out << "> ";
            out.flush();
            continue;
        }

        bool ok1, ok2;
        double a = parts[1].toDouble(&ok1);
        double b = parts[2].toDouble(&ok2);
        if (!ok1 || !ok2) {
            out << "Ошибка: не удалось преобразовать операнды в числа\n";
            out << "> ";
            out.flush();
            continue;
        }

        bool handled = true;

        if      (command == "add") { calc.add(a, b); }
        else if (command == "sub") { calc.subtract(a, b); }
        else if (command == "mul") { calc.multiply(a, b); }
        else if (command == "div") { calc.divide(a, b); }
        else { handled = false; }

        if (!handled) {
            out << "Неизвестная команда: " << command << "\n";
        }

        out << "> ";
        out.flush();
    }

    return 0;
}