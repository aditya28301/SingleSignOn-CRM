#ifndef SEARCH_CUSTOMER_H
#define SEARCH_CUSTOMER_H

#include <QApplication>
#include <QObject>
#include <QMainWindow>
#include <QMenu>
#include <QActionGroup>
#include <QAction>
#include <QLabel>
#include <QVBoxLayout>
#include <QStatusBar>
#include <QMenuBar>
#include <QEventLoop>
#include <QTimer>
#include <QMessageBox>
#include <QString>
class search_customer : public QMainWindow
{
    Q_OBJECT
public:
    search_customer();
private slots:
    void newFile();
    void open();
    void save();
    void print();
    void exit();
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void bold();
    void italic();
    void leftAlign();
    void rightAlign();
    void justify();
    void center();
    void setLineSpacing();
    void setParagraph();
    void about();
    void aboutQt();
    void close();

private:
    void createActions();
    void createMenu();

    QMenu* fileMenu;
    QMenu* editMenu;
    QMenu* formatMenu;
    QMenu* helpMenu;
    QActionGroup* alignmentGroup;
    QAction* newAct;
    QAction* openAct;
    QAction* saveAct;
    QAction* printAct;
    QAction* exitAct;
    QAction* undoAct;
    QAction* redoAct;
    QAction* cutAct;
    QAction* copyAct;
    QAction* pasteAct;
    QAction* boldAct;
    QAction* italicAct;
    QAction* leftAlignAct;
    QAction* rightAlignAct;
    QAction* justifyAct;
    QAction* centerAct;
    QAction* setLineSpacingAct;
    QAction* setParagraphSpacingAct;
    QAction* aboutAct;
    QAction* aboutQtAct;
    QLabel* infoLabel;
};

#endif // SEARCH_CUSTOMER_H
