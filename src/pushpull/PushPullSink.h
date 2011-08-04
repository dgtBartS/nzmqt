// Copyright 2011 Johann Duscher. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
//    1. Redistributions of source code must retain the above copyright notice, this list of
//       conditions and the following disclaimer.
//
//    2. Redistributions in binary form must reproduce the above copyright notice, this list
//       of conditions and the following disclaimer in the documentation and/or other materials
//       provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY JOHANN DUSCHER ''AS IS'' AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
// FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
// ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// The views and conclusions contained in the software and documentation are those of the
// authors and should not be interpreted as representing official policies, either expressed
// or implied, of Johann Duscher.

#ifndef PUSHPULLSINK_H
#define PUSHPULLSINK_H

#include <QObject>
#include <QDebug>
#include <QList>
#include <QByteArray>

#include "nzmqt/nzmqt.hpp"


class PushPullSink : public QObject
{
    Q_OBJECT

    typedef QObject super;

public:
    explicit PushPullSink(const QString& sinkAddress, QObject *parent)
        : super(parent), sinkAddress_(sinkAddress), batchStarted_(false), task_nbr_(0)
    {
        nzmqt::ZMQContext* context = new nzmqt::ZMQContext(4, this);

        sink_ = context->createSocket(ZMQ_PULL);
        connect(sink_, SIGNAL(readyRead()), SLOT(batchEvent()));
    }

    void run()
    {
        sink_->bindTo(sinkAddress_);
    }

protected slots:
    void batchEvent()
    {
        if (!batchStarted_)
        {
            sink_->receiveMessage();
            batchStarted_ = true;
            qDebug() << "Batch started";
            return;
        }

        if (task_nbr_ < 100)
        {
            sink_->receiveMessage();
            if (task_nbr_++ % 10 == 0)
                qDebug() << ":";
            else
                qDebug() << ".";
        }
        else
        {
            // Calculate and report duration of batch
            // TODO
            qDebug() << "FINISHED";
        }
    }

private:
    nzmqt::ZMQSocket* sink_;
    QString sinkAddress_;
    bool batchStarted_;
    int task_nbr_;
};

#endif // PUSHPULLSINK_H