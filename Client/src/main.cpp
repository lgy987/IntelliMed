#include "loginform.h"
#include "networkmanager.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NetworkManager::instance().connectToServer("127.0.0.1", 12345);
    LoginForm w;
    w.show();
    return a.exec();

}
