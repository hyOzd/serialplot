#ifndef HIDABLETABWIDGET_H
#define HIDABLETABWIDGET_H

#include <QTabWidget>
#include <QAction>

class HidableTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit HidableTabWidget(QWidget *parent = 0);
    QAction hideAction;

private slots:
    void onHideAction(bool checked);
    void onTabBarClicked();
};

#endif // HIDABLETABWIDGET_H
