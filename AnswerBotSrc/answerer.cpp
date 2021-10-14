#include "answerer.h"

QByteArray Answerer::answer(QByteArray q)
{
    QByteArray clear;
    makeWrite(clear);
    QRegularExpression re("((den|day) (v|w) )?(jb|jailbreak)( den| day)?");
    QRegularExpression re2("(jivi|jiwi) (teroristi|t)");
    QRegularExpression re3("[ ]*\\?");
    QRegularExpression re4("(kolko|e|sa|ima|ostanali|=| do |do)");
    QByteArray ql = QString(q.toLower())
            .replace(re, jbDay.toUtf8())
            .replace(re2, alivePrisoners.toUtf8())
            .remove(re3)
            .toUtf8();
    QByteArray qlmath = QString(ql).remove(re4).toUtf8();

    QByteArray answer("Not found, trying google");
    int error;
    double res = te_interp(qlmath.constData(), &error);

    if (!error) {
        answer = QByteArray::number(res, 'f', 2);
        if (answer.endsWith(".00"))
            answer.chop(3);
        makeWrite(answer);
    } else if (ql.indexOf("stolica") >= 0) {
        answer = answerCapitals(ql);
        makeWrite(answer);
    } else {
        netManager.fromGoogleTranslated(q.toPercentEncoding());
    }

    return answer;
}

int Answerer::writeConfig(QByteArray cfg)
{
    QFile file(settings.value("path").toString() + "//quest.cfg");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return 1;
    file.write(cfg);
    file.close();
    return 0;
}

int Answerer::makeWrite(QByteArray& expr)
{
    return writeConfig(makeConfig(expr));
}

QByteArray Answerer::answerCapitals(QByteArray &question) const
{
    QRegularExpression re_country("^([a-z ]+) e stolica");
    QRegularExpression re_capital("(^na| na) ([a-z ]+?)( stolica|\\?|$)");

    auto m_country = re_country.match(question);
    if (m_country.hasMatch()) {
        auto hit = data.capitals.find(m_country.captured(1).toUtf8());
        if (hit != data.capitals.end())
            return hit.value();
    }
    auto m_capital = re_capital.match(question);
    if (m_capital.hasMatch()) {
        auto hit = data.countries.find(m_capital.captured(2).toUtf8());
        if (hit != data.countries.end())
            return hit.value();
    }

    return QByteArray("Error");
}

QByteArray Answerer::makeConfig(QByteArray &answer) const
{
    return QByteArray("alias quest \"say " + answer + "; alias quest\"");
}

Answerer::Answerer(QSettings &_settings, QObject *parent)
    : QObject(parent), settings(_settings), netManager(this)
{
    connect(&netManager, &NetworkManager::parsed,
            this, &Answerer::networkAnswer);
}
void Answerer::setAlivePrisoners(const QString &newAlivePrisoners)
{
    alivePrisoners = newAlivePrisoners;
}

void Answerer::setTotalPrisoners(const QString &newTotalPrisoners)
{
    totalPrisoners = newTotalPrisoners;
}

void Answerer::setJbDay(const QString &newJbDay)
{
    jbDay = newJbDay;
}

void Answerer::translate(const QByteArray &q)
{
    netManager.translate(q);
}

void Answerer::printRandomCountryCode()
{
    int number = QRandomGenerator::global()->bounded(0, data.codesCount);
    int i = 0;
    for (auto it = data.codes.constKeyValueBegin();
         it != data.codes.constKeyValueEnd();
         it++)
    {
        if (i == number)
            qDebug() << it->first << " " << it->second;
        i++;
    }
}

void Answerer::networkAnswer(QByteArray answer)
{
    qDebug() << "> " << answer;
    if (answer != "Google miss") {
        makeWrite(answer);
        Bot::execute(netManager.isTranslate() ? RELOAD_LAUNCH : RELOAD);
    }
    emit interruptLoop();
}
