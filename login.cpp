#include "login.h"
#include "search_customer.h"

bool login::isUsernameEntered = false;
bool login::isPasswordEntered = false;

login::login(QWidget* parent) : QDialog(parent), m_userEmail(""), m_userName("") {

    username = new QLabel(tr("Username"));
    password = new QLabel(tr("Password"));
    enter_username = new QLineEdit();
    enter_password = new QLineEdit();
    enter_password->setEchoMode(QLineEdit::Password);
    username->setBuddy(enter_username);
    password->setBuddy(enter_password);

    submit = new QPushButton(tr("Login with Google"));
    submit->setDefault(true);
    // Enable the button by default for OAuth login
    submit->setEnabled(true);

    exit = new QPushButton(tr("Close"));

    /* Signal Slot */
    connect(enter_username, &QLineEdit::textChanged,
            this, &login::updateLoginButton);

    connect(enter_password, &QLineEdit::textChanged,
            this, &login::updateLoginButton);

    connect(exit, &QPushButton::clicked,
            this, &QDialog::reject);  // Use reject instead of close

    connect(submit, &QPushButton::clicked,
            this, &login::submit_request);

    QHBoxLayout* leftLayout = new QHBoxLayout();
    leftLayout->addWidget(username);
    leftLayout->addWidget(enter_username);

    QHBoxLayout* leftDown = new QHBoxLayout();
    leftDown->addWidget(password);
    leftDown->addWidget(enter_password);

    QVBoxLayout* topleftLayout = new QVBoxLayout;
    topleftLayout->addLayout(leftLayout);
    topleftLayout->addLayout(leftDown);

    // Add info label
    QLabel* infoLabel = new QLabel(tr("Enter any username/password or click 'Login with Google'"));
    infoLabel->setStyleSheet("QLabel { color: #666; font-style: italic; }");
    topleftLayout->addWidget(infoLabel);

    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget(submit, 1, Qt::AlignHCenter);
    layout->addWidget(exit, 1, Qt::AlignRight);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addLayout(topleftLayout);
    mainLayout->addLayout(layout);

    setLayout(mainLayout);

    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    setWindowTitle(tr("CRM Login"));
    setFixedHeight(sizeHint().height());

    // Initialize OAuth handler
    oauth = new OAuthLogin(this);

    // Connect OAuth signals
    connect(oauth, &OAuthLogin::loginSuccess, this, [this](QString email, QString name) {
        // Store user info
        m_userEmail = email;
        m_userName = name;

        qDebug() << "OAuth login successful for:" << email << name;

        // Accept the dialog which will trigger the accepted signal in main
        this->accept();
    });

    connect(oauth, &OAuthLogin::loginFailed, this, [this](QString error) {
        QMessageBox::critical(this, "Login Failed",
                              QString("Authentication failed:\n%1\n\nPlease try again.").arg(error));
        submit->setEnabled(true); // Re-enable button after failure
    });
}

void login::submit_request() {
    // Check if both username and password are entered
    if (!enter_username->text().isEmpty() && !enter_password->text().isEmpty()) {
        // Direct login with entered credentials
        m_userEmail = enter_username->text() + "@gmail.com";
        m_userName = enter_username->text();
        this->accept();
    } else {
        // OAuth login (when fields are empty or incomplete)
        submit->setEnabled(false); // Disable button during OAuth process
        oauth->startLogin();
    }
}

void login::updateLoginButton() {
    // Check if both fields have text
    bool hasUsername = !enter_username->text().isEmpty();
    bool hasPassword = !enter_password->text().isEmpty();

    if (hasUsername && hasPassword) {
        // Both fields filled - show "Login" for direct authentication
        submit->setText(tr("Login"));
    } else {
        // Fields empty or incomplete - show "Login with Google" for OAuth
        submit->setText(tr("Login with Google"));
    }

    // Always keep the button enabled
    submit->setEnabled(true);
}

void login::enabledLoginButton() {
    // Keep for backward compatibility, but redirect to updateLoginButton
    updateLoginButton();
}

login::~login() {
    // Destructor
}

void login::showCustomerSearch() {
    search_customer* customerWindow = new search_customer();
    customerWindow->show();
}
