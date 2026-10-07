#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QObject>
#include <QString>
#include <QVector>

class Calculator : public QObject
{
    Q_OBJECT

public:
    explicit Calculator(QObject *parent = nullptr);

    double  result() const       { return m_result; }
    bool    hasError() const     { return m_hasError; }
    QString errorMessage() const { return m_errorMessage; }

    // Тип операции - нужен для истории (чтобы знать, что откатывать)
    enum class Op { Add, Sub, Mul, Div };

public slots:
    void add(double a, double b);
    void subtract(double a, double b);
    void multiply(double a, double b);
    void divide(double a, double b);
    void reset();

    // Слот отмены последней операции (индивидуальное задание)
    void undo();

signals:
    void resultReady(double result);
    void errorOccurred(const QString &message);

private:
    double  m_result       = 0.0;
    bool    m_hasError     = false;
    QString m_errorMessage;

    // Одна запись в истории операций.
    // previousResult - это результат ДО операции, именно его вернёт undo
    struct HistoryEntry {
        Op     op;
        double operandA;
        double operandB;
        double previousResult;
    };

    QVector<HistoryEntry> m_history;

    void setResult(double value);
    void pushHistory(Op op, double a, double b);
};

#endif // CALCULATOR_H