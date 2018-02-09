#ifndef HEADER_CLIENT
#define HEADER_CLIENT

#include <QtWidgets>
#include <QtNetwork>
#include "ui_Client.h"


class Client : public QWidget, private Ui::Client
{
    Q_OBJECT

    public:
        Client();

    private slots:
        void on_connectionButton_clicked();
        void on_sendButton_clicked();
        void on_message_returnPressed();
        void receivedData();
        void connected();
        void disconnected();
        void socketError(QAbstractSocket::SocketError error);

    private:
        QTcpSocket *socket; // represent the server
        quint16 messageLength;
};

#endif
