#include "search_customer.h"
search_customer::search_customer() {

    QWidget* widget = new QWidget();
    setCentralWidget(widget);

    QWidget* topFiller = new QWidget();
    topFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    infoLabel = new QLabel(tr("window"));
    infoLabel->setFrameStyle(QFrame::Box | QFrame::Sunken);
    infoLabel->setAlignment(Qt::AlignCenter);

    QWidget* bottomFiller = new QWidget();
    bottomFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(10,10, 10, 10);
    layout->addWidget(topFiller);
    layout->addWidget(infoLabel);
    layout->addWidget(bottomFiller);
    widget->setLayout(layout);

   createActions();
    createMenu();

    QString message = tr("This is the Middle text");
    this->statusBar()->showMessage(message);
    setWindowTitle(tr("Search customer"));
    setMinimumSize(500,500);
    resize(500,500);
}
void search_customer::newFile(){
    infoLabel->setText(tr("Invoked <b> File->New</b>"));
}

void search_customer::open(){
    infoLabel->setText(tr("Invoked <b> File->Open</b>"));
}

void search_customer::save(){
    infoLabel->setText(tr("Inovked <b> File->Save</b>"));
}

void search_customer::print(){
    infoLabel->setText(tr("Invoked File->Print"));
}
void search_customer::exit(){
    infoLabel->setText(tr("Invoked <b>File->Exit"));
    QEventLoop loop;
    QTimer::singleShot(0, this, SLOT(close()));
}

void search_customer::undo(){
    infoLabel->setText(tr("Invoked Edit->Undo"));
}

void search_customer::redo(){
 infoLabel->setText(tr("Invoked Edit->Redo"));
}

void search_customer::cut(){
 infoLabel->setText(tr("Invoked Edit->Cut"));
}
void search_customer::copy(){
 infoLabel->setText(tr("Invoked Edit->Copy"));
}
void search_customer::paste(){
 infoLabel->setText(tr("Invoked Edit->Paste"));
}
void search_customer::bold(){
 infoLabel->setText(tr("Invoked Edit-><b>Bold</b>"));
}
void search_customer::italic(){
 infoLabel->setText(tr("Invoked Edit->Italic"));
}
void search_customer::leftAlign(){
 infoLabel->setText(tr("Invoked Edit->leftAlign"));
}
void search_customer::rightAlign(){
     infoLabel->setText(tr("Invoked Edit->righAlign"));
}
void search_customer::justify(){
 infoLabel->setText(tr("Invoked Edit->justify"));
}
void search_customer::center(){
 infoLabel->setText(tr("Invoked Edit->Center"));
}
void search_customer::setLineSpacing(){
 infoLabel->setText(tr("Invoked Edit->SetLineSpacing"));
}
void search_customer::setParagraph(){
 infoLabel->setText(tr("Invoked Edit->setParagraph"));
}
void search_customer::about(){
 infoLabel->setText(tr("Invoked Edit->About"));
}
void search_customer::aboutQt(){
 infoLabel->setText(tr("Invoked Edit->AboutQt"));
}

void search_customer::close(){
    QString message = QString(tr("The Window will now close"));
    QMessageBox::information(this, "Exiting", message);
    QMainWindow::close();
}
void search_customer::createActions(){
    newAct = new QAction(tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    QObject::connect(newAct, SIGNAL(triggered()),
                     this, SLOT(newFile()));

    openAct = new QAction(tr("&Open"), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open a Existing file"));
    QObject::connect(openAct, SIGNAL(triggered()),
                     this, SLOT(open()));

    saveAct = new QAction(tr("&Save"));
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save File"));
    QObject::connect(saveAct, SIGNAL(triggered()),
                     this, SLOT(save()));

    printAct = new QAction(tr("&Print"), this);
    printAct->setShortcuts(QKeySequence::Print);
    printAct->setStatusTip(tr("Print File"));
    QObject::connect(printAct, SIGNAL(triggered()),
                     this, SLOT(print()));

    exitAct = new QAction(tr("Exit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit Application"));
    QObject::connect(exitAct, SIGNAL(triggered()),
                     this, SLOT(exit()));

    undoAct = new QAction(tr("Undo"), this);
    undoAct->setShortcuts(QKeySequence::Undo);
    undoAct->setStatusTip(tr("Undo Action"));
    QObject::connect(undoAct, SIGNAL(triggered()),
                     this, SLOT(undo()));

    redoAct = new QAction(tr("Redo"), this);
    redoAct->setShortcuts(QKeySequence::Redo);
    redoAct->setStatusTip(tr("Redo Action"));
    QObject::connect(redoAct, SIGNAL(triggered()),
            this, SLOT(redo()));

    cutAct = new QAction(tr("Cut"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut Action"));
    QObject::connect(cutAct, SIGNAL(triggered()),
                     this, SLOT(cut()));

    copyAct = new QAction(tr("Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy Action"));
    QObject::connect(copyAct, SIGNAL(triggered()),
                     this, SLOT(copy()));

    pasteAct = new QAction(tr("Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste Action"));
    QObject::connect(pasteAct, SIGNAL(triggered()),
                     this, SLOT(paste()));

    boldAct = new QAction(tr("Bold"), this);
    boldAct->setShortcuts(QKeySequence::Bold);
    boldAct->setStatusTip(tr("Bold Action"));
    QObject::connect(boldAct, SIGNAL(triggered()),
                     this, SLOT(bold()));

    QFont boldFont = boldAct->font();
    boldFont.setBold(true);
    boldAct->setFont(boldFont);

    italicAct = new QAction(tr("Italic"), this);
    italicAct->setShortcuts(QKeySequence::Italic);
    italicAct->setStatusTip(tr("Italic Action"));
    QObject::connect(italicAct, SIGNAL(triggered()),
                     this, SLOT(italic()));

    QFont italicFont = italicAct->font();
    italicFont.setItalic(true);
    italicAct->setFont(italicFont);

    setLineSpacingAct = new QAction(tr("Set &Line Spacing..."), this);
    setLineSpacingAct->setStatusTip(tr("Change the gap between the lines of a "
                                       "paragraph"));
    QObject::connect(setLineSpacingAct, SIGNAL(triggered()),
            this, SLOT(setLineSpacing()));

    setParagraphSpacingAct = new QAction(tr("Set &Paragraph Spacing..."), this);
    setParagraphSpacingAct->setStatusTip(tr("Change the gap between paragraphs"));
    QObject::connect(setParagraphSpacingAct, SIGNAL(triggered()),
            this, SLOT(setParagraph()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    QObject::connect(aboutAct, SIGNAL(triggered()),
                     this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    QObject::connect(aboutQtAct, SIGNAL(triggered()),
                     qApp, SLOT(aboutQt()));
    QObject::connect(aboutQtAct, SIGNAL(triggered()),
                     this, SLOT(aboutQt()));


    leftAlignAct = new QAction(tr("&Left Align"), this);
    leftAlignAct->setCheckable(true);
    leftAlignAct->setShortcut(tr("Ctrl+L"));
    leftAlignAct->setStatusTip(tr("Left align the selected text"));
    QObject::connect(leftAlignAct, SIGNAL(triggered()),
                     this, SLOT(leftAlign()));

    rightAlignAct = new QAction(tr("&Right Align"), this);
    rightAlignAct->setCheckable(true);
    rightAlignAct->setShortcut(tr("Ctrl+R"));
    rightAlignAct->setStatusTip(tr("Right align the selected text"));
    QObject::connect(rightAlignAct, SIGNAL(triggered()),
                     this, SLOT(rightAlign()));

    justifyAct = new QAction(tr("&Justify"), this);
    justifyAct->setCheckable(true);
    justifyAct->setShortcut(tr("Ctrl+J"));
    justifyAct->setStatusTip(tr("Justify the selected text"));
    QObject::connect(justifyAct, SIGNAL(triggered()),
                     this, SLOT(justify()));

    centerAct = new QAction(tr("&Center"), this);
    centerAct->setCheckable(true);
    centerAct->setShortcut(tr("Ctrl+E"));
    centerAct->setStatusTip(tr("Center the selected text"));
    QObject::connect(centerAct, SIGNAL(triggered()),
                     this, SLOT(center()));
}

void search_customer::createMenu(){
    fileMenu = this->menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(printAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
    editMenu->addSeparator();
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);
    editMenu->addSeparator();

    helpMenu = menuBar()->addMenu(tr("Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);

    formatMenu = editMenu->addMenu(tr("&Format"));
    formatMenu->addAction(boldAct);
    formatMenu->addAction(italicAct);
    formatMenu->addSeparator();
    formatMenu->addAction(leftAlignAct);
    formatMenu->addAction(rightAlignAct);
    formatMenu->addAction(justifyAct);
    formatMenu->addAction(centerAct);
    formatMenu->addSeparator();
    formatMenu->addAction(setLineSpacingAct);
    formatMenu->addAction(setParagraphSpacingAct);

    alignmentGroup = new QActionGroup(this);
    alignmentGroup->addAction(leftAlignAct);
    alignmentGroup->addAction(rightAlignAct);
    alignmentGroup->addAction(justifyAct);
    alignmentGroup->addAction(centerAct);
    leftAlignAct->setChecked(true);
}
