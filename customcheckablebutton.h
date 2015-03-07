#ifndef CUSTOMCHECKABLEBUTTON_H
#define CUSTOMCHECKABLEBUTTON_H

#include <QPushButton>

class CustomCheckableButton : public QPushButton
{
    Q_OBJECT
public:
    explicit CustomCheckableButton(QWidget *parent = 0);

signals:

public slots:

private:
    void nextCheckState();

};

#endif // CUSTOMCHECKABLEBUTTON_H
