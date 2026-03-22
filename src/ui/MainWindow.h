#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QContextMenuEvent>
#include <QMessageBox>
#include <QLabel>
#include <QString>
#include <QBoxLayout>
#include <QWidget>


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();

protected:
#ifndef QT_NO_CONTEXTMENU
    void contextMenuEvent(QContextMenuEvent *event) override;
#endif // QT_NO_CONTEXTMENU

private slots:
    void open();
    void about();

private:
    void createActions();
    void createMenus();

    QMenu* fileMenu;
    QAction *openAct;
    QAction *aboutAct;
    QLabel *infoLabel;

};

#endif // MAINWINDOW_H
