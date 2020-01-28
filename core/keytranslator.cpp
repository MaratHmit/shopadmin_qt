#include "keytranslator.h"

KeyTranslator::KeyTranslator(QObject *pobj) :
    QObject(pobj)

{

}

KeyTranslator::~KeyTranslator()
{

}

bool KeyTranslator::eventFilter(QObject *pobj, QEvent *pe)
{
    if (pe->type() == QEvent::KeyPress) {

                // for copy

                if (((QKeyEvent*)pe)->modifiers() == Qt::CTRL && ((QKeyEvent*)pe)->key() == 1057) {
                    QKeyEvent* pe = new QKeyEvent(QEvent::KeyPress,
                                                  Qt::Key_C,
                                                  Qt::ControlModifier,
                                                  ""
                                                  );
                    QApplication::sendEvent(pobj, pe);
                    return true;
                }

                // for cut

                if (((QKeyEvent*)pe)->modifiers() == Qt::CTRL && ((QKeyEvent*)pe)->key() == 1063) {
                    QKeyEvent* pe = new QKeyEvent(QEvent::KeyPress,
                                                  Qt::Key_X,
                                                  Qt::ControlModifier,
                                                  ""
                                                  );
                    QApplication::sendEvent(pobj, pe);
                    return true;
                }

                // for paste

                if (((QKeyEvent*)pe)->modifiers() == Qt::CTRL && ((QKeyEvent*)pe)->key() == 1052) {
                    QKeyEvent* pe = new QKeyEvent(QEvent::KeyPress,
                                                  Qt::Key_V,
                                                  Qt::ControlModifier,
                                                  ""
                                                  );
                    QApplication::sendEvent(pobj, pe);
                    return true;
                }



                // for redo

                if (((QKeyEvent*)pe)->modifiers() == Qt::CTRL + Qt::SHIFT && ((QKeyEvent*)pe)->key() == 1071) {
                    QKeyEvent* pe = new QKeyEvent(QEvent::KeyPress,
                                                  Qt::Key_Z,
                                                  Qt::ControlModifier | Qt::ShiftModifier,
                                                  ""
                                                  );
                    QApplication::sendEvent(pobj, pe);
                    return true;
                }

                // for undo

                if (((QKeyEvent*)pe)->modifiers() == Qt::CTRL && ((QKeyEvent*)pe)->key() == 1071) {
                    QKeyEvent* pe = new QKeyEvent(QEvent::KeyPress,
                                                  Qt::Key_Z,
                                                  Qt::ControlModifier,
                                                  ""
                                                  );
                    QApplication::sendEvent(pobj, pe);
                    return true;
                }

                // for select all

                if (((QKeyEvent*)pe)->modifiers() == Qt::CTRL && ((QKeyEvent*)pe)->key() == 1060) {
                    QKeyEvent* pe = new QKeyEvent(QEvent::KeyPress,
                                                  Qt::Key_A,
                                                  Qt::ControlModifier,
                                                  ""
                                                  );
                    QApplication::sendEvent(pobj, pe);
                    return true;
                }

            }

        return false;
        }
}

