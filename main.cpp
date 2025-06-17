#include "login.h"
#include <Windows.h>
#include "search_customer.h"
#include "CRM_Dashboard.h"

int main(int argc, char* argv[]) {
    SetProcessDPIAware();
    QApplication app(argc, argv);

    login loginWindow;
    CRM_Dashboard* customerWindow = nullptr;

    QObject::connect(&loginWindow, &QDialog::accepted, [&]() {
        if (!customerWindow) {
            customerWindow = new CRM_Dashboard();
            customerWindow->show();
        }
    });

    loginWindow.show();
    int result = app.exec();

    if (customerWindow) {
        delete customerWindow;
    }

    return result;
}
