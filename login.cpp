#include "login.h"
#include "search_customer.h"

bool login::isUsernameEntered = false;
bool login::isPasswordEntered = false;

login::login(QWidget* parent) : QDialog(parent){

    username = new QLabel(tr("Username"));
    password = new QLabel(tr("password"));
    enter_username = new QLineEdit();
    enter_password = new QLineEdit();
    username->setBuddy(enter_username);
    password->setBuddy(enter_password);

    submit = new QPushButton(tr("Submit"));
    submit->setDefault(true);
    submit->setEnabled(false);

    exit = new QPushButton(tr("Close"));

    /* Signal Slot */

    connect(enter_username, &QLineEdit::textChanged,
        this, &login::enabledLoginButton);

    connect(enter_password, &QLineEdit::textChanged,
        this, &login::enabledLoginButton);

    connect(exit, &QPushButton::clicked,
        this, &QDialog::close);

    connect(submit, SIGNAL(clicked()),
            this, SLOT(submit_request()));


    QHBoxLayout* leftLayout = new QHBoxLayout();
    leftLayout->addWidget(username);
    leftLayout->addWidget(enter_username);

    QHBoxLayout* leftDown = new QHBoxLayout();
    leftDown->addWidget(password);
    leftDown->addWidget(enter_password);

    QVBoxLayout* topleftLayout = new QVBoxLayout;
    topleftLayout->addLayout(leftLayout);
    topleftLayout->addLayout(leftDown);

    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget(submit, 1, Qt::AlignHCenter);
    layout->addWidget(exit, 1, Qt::AlignRight);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addLayout(topleftLayout);
    mainLayout->addLayout(layout);

    setLayout(mainLayout);

    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    setWindowTitle(tr("CRM Login Window"));
    setFixedHeight(sizeHint().height());

    oauth = new OAuthLogin(this); // Initialize the object

    // In login.cpp - modify the oauth connection
    connect(oauth, &OAuthLogin::loginSuccess, this, [this](QString email, QString name) {
        // Store user info if needed
        m_userEmail = email;
        m_userName = name;
        // Accept the dialog which will trigger the accepted signal in main
        this->accept();
    });

    connect(oauth, &OAuthLogin::loginFailed, this, [this](QString error) {
        QMessageBox::critical(this, "Login Failed", error);
    });
}

void login::submit_request(){
      oauth->startLogin();
}

void login::enabledLoginButton(){
    isUsernameEntered = !enter_username->text().isEmpty();
    isPasswordEntered = !enter_password->text().isEmpty();
    submit->setEnabled(isUsernameEntered && isPasswordEntered);


}

login::~login() {

}

void login::showCustomerSearch() {
    search_customer* customerWindow = new search_customer();
    customerWindow->show();
}
