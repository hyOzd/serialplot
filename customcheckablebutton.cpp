#include "customcheckablebutton.h"

CustomCheckableButton::CustomCheckableButton(QWidget *parent) :
    QPushButton(parent)
{
    this->setCheckable(true);
}

void CustomCheckableButton::nextCheckState()
{
    /* Do nothing! Check state will be altered by parent. */
}
