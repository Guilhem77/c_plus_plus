#ifndef HEADER_SERVER
#define HEADER_SERVER

#include <QtWidgets>
#include <QtNetwork>


class Server : public QWidget
{
    Q_OBJECT

    public:
        Server();
        void sendToAll(const QString &message);

    private slots:
        void newConnection();
        void receivedData();
        void logOutClient();

    private:
        QLabel *serverState;
        QPushButton *leaveButton;

        QTcpServer *server;
        QList<QTcpSocket *> clients;
        quint16 messageLength;
};

#endif
