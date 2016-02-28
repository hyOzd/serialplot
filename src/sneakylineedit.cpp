#include "sneakylineedit.h"
#include <QFont>

SneakyLineEdit::SneakyLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
    setFrame(false);
    setStyleSheet("QLineEdit{background-color: rgba(0,0,0,0);}");
    setToolTip(trUtf8("Click to edit"));    

    setBold(true);
}

void SneakyLineEdit::focusInEvent(QFocusEvent *event)
{
    setFrame(true);
    setBold(false);
    QLineEdit::focusInEvent(event);
}

void SneakyLineEdit::focusOutEvent(QFocusEvent *event)
{
    setFrame(false);
    setBold(true);
    QLineEdit::focusOutEvent(event);
}

void SneakyLineEdit::setBold(bool bold)
{
    QFont f(font());
    f.setBold(bold);
    setFont(f);
}
