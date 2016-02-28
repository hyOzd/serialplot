#ifndef SNEAKYLINEEDIT_H
#define SNEAKYLINEEDIT_H

#include <QLineEdit>
#include <QFocusEvent>

class SneakyLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit SneakyLineEdit(QWidget *parent = 0);

private:
    void focusInEvent(QFocusEvent * event);
    void focusOutEvent(QFocusEvent * event);
    void setBold(bool bold);
};

#endif // SNEAKYLINEEDIT_H
