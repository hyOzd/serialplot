/*
  Copyright © 2017 Hasan Yavuz Özderya

  This file is part of serialplot.

  serialplot is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  serialplot is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with serialplot.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef RECORDPANEL_H
#define RECORDPANEL_H

#include <QWidget>
#include <QString>
#include <QToolBar>
#include <QAction>

namespace Ui {
class RecordPanel;
}

class RecordPanel : public QWidget
{
    Q_OBJECT

public:
    explicit RecordPanel(QWidget *parent = 0);
    ~RecordPanel();

    QToolBar* toolbar();

private:
    Ui::RecordPanel *ui;
    QToolBar recordToolBar;
    QAction recordAction;
    QString selectedFile;
    bool overwriteSelected;

    /**
     * @brief Increments the file name.
     *
     * If file name doesn't have a number at the end of it, a number is appended
     * with underscore starting from 1.
     *
     * @return false if user cancels
     */
    bool incrementFileName(void);

    /**
     * @brief Used to ask user confirmation if auto generated file
     * name exists.
     *
     * If user confirms overwrite, `selectedFile` is set to
     * `fileName`. User is also given option to select file and is
     * shown a file select dialog in this case.
     *
     * @param fileName auto generated file name.
     * @return false if user cancels
     */
    bool confirmOverwrite(QString fileName);

    void startRecording(void);
    void stopRecording(void);

private slots:
    /**
     * @brief Opens up the file select dialog
     *
     * If you cancel the selection operation, currently selected file is not
     * changed.
     *
     * @return true if file selected, false if user cancels
     */
    bool selectFile();

    void onRecord(bool start);

};

#endif // RECORDPANEL_H
