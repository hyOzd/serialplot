
#include <stddef.h>

#include "snapshot.h"
#include "snapshotview.h"

#include <QtDebug>

SnapShot::SnapShot(QMainWindow* parent, QString name) :
    QObject(parent),
    _menuAction(name, this)
{
    view = NULL;
    mainWindow = parent;
    connect(&_menuAction, &QAction::triggered, this, &SnapShot::show);
}

QAction* SnapShot::menuAction()
{
    return &_menuAction;
}

void SnapShot::show()
{
    if (view == NULL)
    {
        qDebug() << "view == NULL";
        view = new SnapShotView(mainWindow, this);
    }
    view->show();
}

void SnapShot::hide()
{

}
