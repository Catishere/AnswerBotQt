TEMPLATE = subdirs

SUBDIRS += \
    AnswerBotTest \
    AnswerBotSrc

DESTDIR = $$PWD/build

# what subproject depends on others
AnswerBotTest.depends = AnswerBotSrc
