
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

SnapShot::~SnapShot()
{
    if (view != NULL)
    {
        delete view;
    }
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
        connect(view, &SnapShotView::closed, this, &SnapShot::viewClosed);
    }
    view->show();
    view->activateWindow();
    view->raise();
}

void SnapShot::hide()
{

}

void SnapShot::viewClosed()
{
    delete view;
    view = NULL;
}
