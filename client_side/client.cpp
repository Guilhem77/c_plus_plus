#include "client.h"

Client::Client()
{
    setupUi(this);

    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(receivedData()));
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));

    messageLength = 0;
}

// We try to connect to the server
void Client::on_connectionButton_clicked()
{
    // Getting connected
    messagesList->append(tr("<em>Tentative de connexion en cours...</em>"));
    connectionButton->setEnabled(false);

    socket->abort(); // If there is any previous connection, we desactivate the old connections
    socket->connectToHost(serverIP->text(), serverPort->value()); // We connect to the requested server
}

// Send message to server
void Client::on_sendButton_clicked()
{
    QByteArray package;
    QDataStream out(&package, QIODevice::WriteOnly);

    // We prepare the package which will be sent
    QString sendMessage = tr("<strong>") + pseudo->text() +tr("</strong> : ") + message->text();

    out << (quint16) 0;
    out << sendMessage;
    out.device()->seek(0);
    out << (quint16) (package.size() - sizeof(quint16));

    socket->write(package); // We send the package

    message->clear(); // We empty the message writting zone
    message->setFocus(); // We put back the cursor inside
}

// Push the ENTER has the same effect of Send Button
void Client::on_message_returnPressed()
{
    on_sendButton_clicked();
}

// We received a package
void Client::receivedData()
{
    /* Same principle of when the server receives a package
     * We try to get the message length
     * Once we have it, we wait to receive the whole message */
    QDataStream in(socket);

    if (messageLength == 0)
    {
        if (socket->bytesAvailable() < (int)sizeof(quint16))
             return;

        in >> messageLength;
    }

    if (socket->bytesAvailable() < messageLength)
        return;


    // If we get here, we can received the whole message
    QString messageReceived;
    in >> messageReceived;

    // We print the message on the chat
    messagesList->append(messageReceived);

    // We put back the message length to 0 to receive new messages
    messageLength = 0;
}

// When connection to server is succesfull
void Client::connected()
{
    messagesList->append(tr("<em>Connexion réussie !</em>"));
    connectionButton->setEnabled(true);
}

// We we are disconnected from server
void Client::disconnected()
{
    messagesList->append(tr("<em>Déconnecté du serveur</em>"));
}

// When there is an error
void Client::socketError(QAbstractSocket::SocketError error)
{
    switch(error)
    {
        case QAbstractSocket::HostNotFoundError:
            messagesList->append(tr("<em>ERREUR : le serveur n'a pas pu être trouvé. Vérifiez l'IP et le port.</em>"));
            break;
        case QAbstractSocket::ConnectionRefusedError:
            messagesList->append(tr("<em>ERREUR : le serveur a refusé la connexion. Vérifiez si le programme \"serveur\" a bien été lancé. Vérifiez aussi l'IP et le port.</em>"));
            break;
        case QAbstractSocket::RemoteHostClosedError:
            messagesList->append(tr("<em>ERREUR : le serveur a coupé la connexion.</em>"));
            break;
        default:
            messagesList->append(tr("<em>ERREUR : ") + socket->errorString() + tr("</em>"));
    }

    connectionButton->setEnabled(true);
}
