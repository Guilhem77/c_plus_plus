#include "server.h"

Server::Server()
{
    // Creation and disposition of windows widgets
    serverState = new QLabel;
    leaveButton = new QPushButton(tr("Quitter"));
    connect(leaveButton, SIGNAL(clicked()), qApp, SLOT(quit()));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(serverState);
    layout->addWidget(leaveButton);
    setLayout(layout);

    setWindowTitle(tr("Server"));

    // Server management
    server = new QTcpServer(this);
    if (!server->listen(QHostAddress::Any, 50885)) // We start the server on all free IPs and port 50585
    {
        // If the server didn't start correctly
        serverState->setText(tr("Le serveur n'a pas pu être démarré. Raison :<br />") + server->errorString());
    }
    else
    {
        // If the server start correctly
        serverState->setText(tr("Le serveur a été démarré sur le port <strong>") + QString::number(server->serverPort()) + tr("</strong>.<br />Des clients peuvent maintenant se connecter."));
        connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    }

    messageLength = 0;
}

void Server::newConnection()
{
    sendToAll(tr("<em>Un nouveau client vient de se connecter</em>"));

    QTcpSocket *newClient = server->nextPendingConnection();
    clients << newClient;

    connect(newClient, SIGNAL(readyRead()), this, SLOT(receivedData()));
    connect(newClient, SIGNAL(disconnected()), this, SLOT(logOutClient()));
}

void Server::receivedData()
{
    // 1 : We receive a package from one client

    // We determine which client send the message
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (socket == 0) // If we didn't find the client we stop the function
        return;

    // If all is ok we get the message
    QDataStream in(socket);

    if (messageLength == 0) // If we don't know the message length we try to get it
    {
        if (socket->bytesAvailable() < (int)sizeof(quint16)) // We didn't receive the whole length message
             return;

        in >> messageLength; // If we receive the message length we get it
    }

    if (socket->bytesAvailable() < messageLength)
        return;


    // We can get the message
    QString message;
    in >> message;


    // 2 : We send the message to all clients
    sendToAll(message);

    messageLength = 0;
}

void Server::logOutClient()
{
    sendToAll(tr("<em>Un client vient de se déconnecter</em>"));

    // We determine which client has disconnected
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (socket == 0) // If we didn't find the client we stop the function
        return;

    clients.removeOne(socket);

    socket->deleteLater();
}

void Server::sendToAll(const QString &message)
{
    // Package preparation
    QByteArray package;
    QDataStream out(&package, QIODevice::WriteOnly);

    out << (quint16) 0; // we wirte 0 to reserve the place of message length
    out << message; // We add the message
    out.device()->seek(0); // We rewind to the beggining of the package
    out << (quint16) (package.size() - sizeof(quint16)); // We write the length and remove the 0


    // We send the package to all connected clients
    for (int i = 0; i < clients.size(); i++)
    {
        clients[i]->write(package);
    }

}
