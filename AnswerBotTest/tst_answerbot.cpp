#include <QtTest>
#include <QSettings>
#include "answerer.h"

class AnswerBot : public QObject
{
    Q_OBJECT
    Answerer answerer;
    QSettings settings;
public:
    AnswerBot();
    ~AnswerBot();

private slots:
    void test_case1();

};

AnswerBot::AnswerBot()
    : answerer(settings, this),
      settings(QSettings::IniFormat, QSettings::UserScope,
                           "CatCompany", "AnswerBot")
{

}

AnswerBot::~AnswerBot()
{

}

void AnswerBot::test_case1()
{

}

QTEST_APPLESS_MAIN(AnswerBot)

#include "tst_answerbot.moc"
