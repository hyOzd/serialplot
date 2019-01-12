#include "datatextview.h"
#include "ui_datatextview.h"

#include "utils.h"

class DataTextViewSink : public Sink
{
public:
    DataTextViewSink(DataTextView* textView)
    {
        _textView = textView;
    }

protected:
    virtual void feedIn(const SamplePack& data) override
    {
        _textView->addData(data);
    };

private:
    DataTextView* _textView;
};

DataTextView::DataTextView(Stream* stream, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataTextView)
{
    _stream = stream;
    ui->setupUi(this);
    sink = new DataTextViewSink(this);

    connect(ui->cbEnable, &QCheckBox::toggled, [this](bool checked)
            {
                if (checked)
                {
                    _stream->connectFollower(sink);
                }
                else
                {
                    _stream->disconnectFollower(sink);
                }
            });

    ui->textView->setMaximumBlockCount(ui->spNumLines->value());
    connect(ui->spNumLines, SELECT<int>::OVERLOAD_OF(&QSpinBox::valueChanged),
            [this](int value)
            {
                ui->textView->setMaximumBlockCount(value);
            });

    connect(ui->pbClear, &QPushButton::clicked, ui->textView, &QPlainTextEdit::clear);
}

DataTextView::~DataTextView()
{
    delete sink;
    delete ui;
}

void DataTextView::addData(const SamplePack& data)
{
    for (unsigned int i = 0; i < data.numSamples(); i++)
    {
        QString str;
        for (unsigned ci = 0; ci < data.numChannels(); ci++)
        {
            str += QString::number(data.data(ci)[i], 'f', ui->spDecimals->value());
            if (ci != data.numChannels()-1) str += " ";
        }
        ui->textView->appendPlainText(str);
    }
}
