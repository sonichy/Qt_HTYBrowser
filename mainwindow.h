#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkReply>
#include <QPushButton>
#include <QLineEdit>
#include <QWebInspector>
#include <QTableWidget>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent*);

private:
    Ui::MainWindow *ui;
    QAction *action_newtab,*action_open,*action_bookmark,*action_source,*action_history,*action_about,*action_find,*action_loadJS,*action_devtool;
    QString filename,js;
    QDialog *find_dialog;
    QLineEdit *lineEdit_find;
    QPushButton *pushButton_findlast,*pushButton_findnext,*pushButton_findclose;
    QStringList SL_bookmark_title,SL_bookmark_url,SL_history_time,SL_history_title,SL_history_url;
    void saveBookmarks();
    void fillBookmarkMenu();
    bool bookmarked;
    struct History {
        QString time;
        QString title;
        QString url;
    };
    void appendHistory(QString stime, QString title, QString url);
    QWebInspector* WI;
    QNetworkAccessManager *NAM;
    QTableWidget *tableSearch;
    void loadHistory();

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
    void loadBookmarks();
    void gotoBookmarkURL(bool);
    void find();
    void hidefind();
    void findlast();
    void findnext();
    void on_pushButton_addBookmark_clicked();
    void loadJS();
    void zoomin();
    void zoomout();
    void zoom1();
    void inspector();
    void tabBarDoubleClick(int);
    void search(QString);
    void cancel();
    void cellClick(int,int);
};

#endif // MAINWINDOW_H
