#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkReply>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QAction *action_newtab,*action_open,*action_source,*action_history,*action_about;
    QString filename;

private slots:
    void gotoURL();
    void onLinkClicked(const QUrl&);
    void goBack();
    void goForward();
    void about();
    void loadStart();
    void loadFinish(bool);
    void openFile();
    void downloadRequest(const QNetworkRequest &request);
    void downloadFinish();
    void downloadProgress(qint64, qint64);    
    void linkHover(const QString&, const QString&, const QString&);
    void newTab();
    void closeTab(int);
    void currentChange(int);
    void titleChange(QString);
    void iconChange();
    void switchTab();
    void fillURL();
    void closeCurrentTab();
    void stop();
    void viewSource();
    void loadProgress(int i);
    void history();
    void refresh();
};

#endif // MAINWINDOW_H
