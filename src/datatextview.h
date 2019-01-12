#ifndef DATATEXTVIEW_H
#define DATATEXTVIEW_H

#include <QWidget>

#include "stream.h"

namespace Ui {
class DataTextView;
}

class DataTextViewSink;

class DataTextView : public QWidget
{
    Q_OBJECT

public:
    explicit DataTextView(Stream* stream, QWidget *parent = 0);
    ~DataTextView();

protected:
    void addData(const SamplePack& data);

    friend DataTextViewSink;

private:
    Ui::DataTextView *ui;
    DataTextViewSink* sink;
    Stream* _stream;
};

#endif // DATATEXTVIEW_H
