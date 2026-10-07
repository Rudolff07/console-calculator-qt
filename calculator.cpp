#include "calculator.h"
#include <QDebug>
#include <QtMath>

Calculator::Calculator(QObject *parent)
    : QObject(parent)
    , m_result(0.0)
    , m_hasError(false)
{
    qDebug() << "Calculator created";
}

void Calculator::setResult(double value)
{
    m_result = value;
    m_hasError = false;
    m_errorMessage.clear();
    emit resultReady(m_result);
}

// Записываем операцию в историю ПЕРЕД её выполнением.
// previousResult - то, что было ДО операции, поэтому и сохраняем заранее
void Calculator::pushHistory(Op op, double a, double b)
{
    HistoryEntry entry;
    entry.op             = op;
    entry.operandA       = a;
    entry.operandB       = b;
    entry.previousResult = m_result;
    m_history.append(entry);
}

void Calculator::add(double a, double b)
{
    qDebug() << "add(" << a << ", " << b << ")";
    pushHistory(Op::Add, a, b);
    setResult(a + b);
}

void Calculator::subtract(double a, double b)
{
    qDebug() << "subtract(" << a << ", " << b << ")";
    pushHistory(Op::Sub, a, b);
    setResult(a - b);
}

void Calculator::multiply(double a, double b)
{
    qDebug() << "multiply(" << a << ", " << b << ")";
    pushHistory(Op::Mul, a, b);
    setResult(a * b);
}

void Calculator::divide(double a, double b)
{
    qDebug() << "divide(" << a << ", " << b << ")";

    // При делении на ноль операция не состоялась - в историю не пишем
    if (qFuzzyIsNull(b)) {
        m_hasError = true;
        m_errorMessage = "Деление на ноль невозможно!";
        emit errorOccurred(m_errorMessage);
        return;
    }
    pushHistory(Op::Div, a, b);
    setResult(a / b);
}

void Calculator::reset()
{
    qDebug() << "reset()";
    m_result = 0.0;
    m_hasError = false;
    m_errorMessage.clear();
    m_history.clear();      // сбрасываем и историю, иначе undo вернёт старые данные
    emit resultReady(m_result);
}

// ===== Индивидуальное задание (средний уровень, вариант 7) =====
// Отмена последней операции: берём последнюю запись из истории
// и возвращаем результат, который был до неё
void Calculator::undo()
{
    qDebug() << "undo()";

    // Откатывать нечего - сообщаем через сигнал ошибки
    if (m_history.isEmpty()) {
        m_hasError = true;
        m_errorMessage = "Нечего отменять: история операций пуста";
        emit errorOccurred(m_errorMessage);
        return;
    }

    // takeLast() забирает последний элемент и удаляет его из стека
    HistoryEntry last = m_history.takeLast();

    // Возвращаем результат, который был до этой операции
    m_result = last.previousResult;
    m_hasError = false;
    m_errorMessage.clear();

    // Оповещаем подписчиков (консоль + history.txt)
    emit resultReady(m_result);
}
// ===== Конец индивидуального задания =====