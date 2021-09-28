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
    QTextStream out(&file);
    out << cfg;
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
        auto hit = capitals.find(m_country.captured(1).toUtf8());
        if (hit != capitals.end())
            return hit.value();
    }
    auto m_capital = re_capital.match(question);
    if (m_capital.hasMatch()) {
        auto hit = countries.find(m_capital.captured(2).toUtf8());
        if (hit != countries.end())
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

    capitals =
    {
        {"abu dabi", "obedineni arabski emirstva"},
        {"abuja", "nigeriq"},
        {"adis abeba", "etiopiq"},
        {"akra", "gana"},
        {"aljir", "aljir"},
        {"alofi", "niue"},
        {"aman", "yordaniq"},
        {"amsterdam", "holandiq"},
        {"amsterdam ", "niderlandiq"},
        {"andora la vela", "andora"},
        {"ankara", "turciq"},
        {"antananarivo", "madagaskar"},
        {"asmara", "eritreq"},
        {"astana", "kazahstan"},
        {"asunsion", "paragvai"},
        {"atina", "gurciq"},
        {"ashhabad", "turkmenistan"},
        {"bagdad", "irak"},
        {"baku", "azerbaijan"},
        {"bamako", "mali"},
        {"bandar seri begavan", "brunei"},
        {"bangi", "centralnoafrikanskata republika"},
        {"banjul", "gambiq"},
        {"bankok", "tailand"},
        {"baster", "seint kic i nevis"},
        {"beirut", "livan"},
        {"belgrad", "sarbiq"},
        {"belmopan", "beliz"},
        {"berlin", "germaniq"},
        {"bern", "shveicariq"},
        {"bishkek", "kirgizstan"},
        {"bisau", "gvineq-bisau"},
        {"blumfontein", "republika iujna afrika"},
        {"bogota", "kolumbiq"},
        {"brazavil", "kongo"},
        {"bratislava", "slovakiq"},
        {"brijtaun", "barbados"},
        {"briuksel", "belgiq"},
        {"budapeshta", "ungariq"},
        {"buenos aires", "arjentina"},
        {"bujumbura", "burundi"},
        {"bukuresht", "rumaniq"},
        {"vaduc", "lihtenshtain"},
        {"valeta", "malta"},
        {"varshava", "polsha"},
        {"vatikana", "vatikana"},
        {"vashington", "sasht"},
        {"viena", "avstriq"},
        {"vientqn", "laos"},
        {"vilnius", "litva"},
        {"vindhuk", "namibiq"},
        {"gaborone", "bocvana"},
        {"gvatemala", "gvatemala"},
        {"daka", "bangladesh"},
        {"dakar", "senegal"},
        {"damask", "siriq"},
        {"dar es salaam", "tanzaniq"},
        {"jakarta", "indoneziq"},
        {"jibuti", "jibuti"},
        {"jorjtaun", "gviana"},
        {"dili", "iztochen timor"},
        {"dodoma", "tanzaniq"},
        {"doha", "katar"},
        {"dushanbe", "tajikistan"},
        {"dablin", "irlandiq"},
        {"el aiun", "zapadna sahara"},
        {"erevan", "armeniq"},
        {"zagreb", "harvatiq"},
        {"islamabad", "pakistan"},
        {"kabul", "afganistan"},
        {"kairo", "egipet"},
        {"kampala", "uganda"},
        {"kanbera", "avstraliq"},
        {"karakas", "venecuela"},
        {"kastriis", "seint lusiq"},
        {"katmandu", "nepal"},
        {"keiptaun", "republika iujna afrika"},
        {"kigali", "ruanda"},
        {"kiev", "ukraina"},
        {"kingstaun", "seint vinsent i grenadini"},
        {"kingstan", "qmaika"},
        {"kinshasa", "demokratichna republika kongo"},
        {"kito", "ekvador"},
        {"kishinev", "moldova"},
        {"konakri", "gvineq"},
        {"kopenhagen", "daniq"},
        {"kuala lumpur", "malaiziq"},
        {"kuveit", "kuveit"},
        {"la pas", "boliviq"},
        {"libarvil", "gabon"},
        {"lilongve", "malavi"},
        {"lima", "peru"},
        {"lisabon", "portugaliq"},
        {"lobamba", "esvatini"},
        {"lome", "togo"},
        {"london", "velikobritaniq"},
        {"london ", "angliq"},
        {"luanda", "angola"},
        {"lusaka", "zambiq"},
        {"liublqna", "sloveniq"},
        {"liuksemburg", "liuksemburg"},
        {"majuro", "marshalovi ostrovi"},
        {"madrid", "ispaniq"},
        {"malabo", "ekvatorialna gvineq"},
        {"male", "maldivi"},
        {"managua", "nikaragua"},
        {"manama", "bahrein"},
        {"manila", "filipini"},
        {"maputo", "mozambik"},
        {"maseru", "lesoto"},
        {"maskat", "oman"},
        {"mbabane", "esvatini"},
        {"meksiko", "meksiko"},
        {"ngerulmud", "palau"},
        {"minsk", "belarus"},
        {"mogadishu", "somaliq"},
        {"monako", "monako"},
        {"monroviq", "liberiq"},
        {"montevideo", "urugvai"},
        {"moroni", "komorski ostrovi"},
        {"moskva", "rusiq"},
        {"naipidal", "mianmar"},
        {"nairobi", "keniq"},
        {"nasau", "bahamski ostrovi"},
        {"njamena", "chad"},
        {"niamei", "niger"},
        {"nikoziq", "kipar"},
        {"nuakshot", "mavritaniq"},
        {"nuku'alofa", "tonga"},
        {"nuuk", "grenlandiq"},
        {"niu delhi", "indiq"},
        {"oslo", "norvegiq"},
        {"otava", "kanada"},
        {"palikir", "mikroneziq"},
        {"panama", "panama"},
        {"paramaribo", "surinam"},
        {"parij", "franciq"},
        {"pekin", "kitai"},
        {"pnom pen", "kamboja"},
        {"podgorica", "cherna gora"},
        {"port vila", "vanuatu"},
        {"port lui", "mavricii"},
        {"port morsbi", "papua nova gvineq"},
        {"port o prens", "haiti"},
        {"port af spein", "trinidad i tobago"},
        {"porto novo", "benin"},
        {"praga", "chehiq"},
        {"praq", "kabo verde"},
        {"pretoriq", "republika iujna afrika"},
        {"phenqn", "severna koreq"},
        {"rabat", "maroko"},
        {"reikqvik", "islandiq"},
        {"riga", "latviq"},
        {"rim", "italiq"},
        {"riqd", "saudicka arabiq"},
        {"rozo", "dominika"},
        {"san marino", "san marino"},
        {"san salvador", "salvador"},
        {"san hose", "kosta rika"},
        {"san huan", "puerto riko"},
        {"sana", "yemen"},
        {"edinburg", "shotlandiq"},
        {"santo domingo", "dominikanska republika"},
        {"santqgo de chile", "chili"},
        {"sao tome", "sao tome i prinsipi"},
        {"saraevo", "bosna i hercegovina"},
        {"seint jorjes", "grenada"},
        {"seint jons", "antigua i barbuda"},
        {"seul", "iujna koreq"},
        {"singapur", "singapur"},
        {"skopie", "severna makedoniq"},
        {"sofia", "bulgaria"},
        {"sofiq", "bulgariq"},
        {"stokholm", "shveciq"},
        {"sukre", "boliviq"},
        {"suva", "fiji"},
        {"taipe", "taivan"},
        {"talin", "estoniq"},
        {"tashkent", "uzbekistan"},
        {"tbilisi", "gruziq"},
        {"tegusigalpa", "honduras"},
        {"teheran", "iran"},
        {"tirana", "albaniq"},
        {"tokio", "qponiq"},
        {"tripoli", "libiq"},
        {"tunis", "tunis"},
        {"thimphu", "butan"},
        {"uagadugu", "burkina faso"},
        {"wellington", "nova zelandiq"},
        {"ulan bator", "mongoliq"},
        {"friitaun", "siera leone"},
        {"funafuti", "tuvalu"},
        {"havana", "kuba"},
        {"hanoi", "vietnam"},
        {"harare", "zimbabve"},
        {"hartum", "sudan"},
        {"helzinki", "finlandiq"},
        {"honiara", "solomonovi ostrovi"},
        {"shri jaqvardanapura kote", "shri lanka"},
        {"iujna tarava", "kiribati"},
        {"qmusukro", "kot d'ivoar"},
        {"qren", "nauru"},
        {"qunde", "kamerun"},
    };

    for (auto it = capitals.constKeyValueBegin();
         it != capitals.constKeyValueEnd();
         it++)
    {
        countries.insert(it->second, it->first);
    }
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

void Answerer::networkAnswer(QByteArray answer)
{
    qDebug() << "> " << answer;
    if (answer != "Google miss")
        makeWrite(answer);
    emit interruptLoop();
}
