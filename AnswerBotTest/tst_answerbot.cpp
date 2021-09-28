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
    void level1Tests();
    void level2Tests();
    void level3Tests();

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

void AnswerBot::level1Tests()
{
    QVERIFY(answerer.answer("123*123") == "15129");
    QVERIFY(answerer.answer("Stolicata na Holandiq") == "amsterdam");
}

void AnswerBot::level2Tests()
{

}

void AnswerBot::level3Tests()
{

}

QTEST_APPLESS_MAIN(AnswerBot)

#include "tst_answerbot.moc"
