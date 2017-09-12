#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QMenu>
#include <QFileDialog>
#include <QShortcut>
#include <QWebView>
#include <QWebFrame>
#include <QTextEdit>
#include <QNetworkProxy>
#include <QDesktopWidget>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lineEditURL->installEventFilter(this);
    WI = new QWebInspector(this);
    ui->verticalLayout->addWidget(WI);
    WI->setVisible(false);
    ui->pushButtonStop->setVisible(false);
    setStyleSheet("QTabBar:tab{width:150px;text-align:left;} QPushButton::menu-indicator{width:0px;}");
    ui->pushButtonBack->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));
    ui->pushButtonForward->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
    ui->pushButtonStop->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
    setWindowState(Qt::WindowMaximized);    
    connect(ui->pushButtonBack, SIGNAL(pressed()), this, SLOT(goBack()));
    connect(ui->pushButtonForward, SIGNAL(pressed()), this, SLOT(goForward()));
    connect(ui->pushButtonGoto, SIGNAL(pressed()), this, SLOT(gotoURL()));
    connect(ui->lineEditURL,SIGNAL(returnPressed()),this,SLOT(gotoURL()));
    connect(ui->lineEditURL,SIGNAL(textEdited(QString)),this,SLOT(search(QString)));
    connect(ui->pushButtonStop, SIGNAL(pressed()), this, SLOT(stop()));

    QMenu *menu=new QMenu;
    action_newtab = new QAction("新标签页",menu);
    action_newtab->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));
    action_open = new QAction("打开本地网页",menu);
    action_open->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));    
    action_bookmark = new QAction("书签",menu);
    //action_bookmark->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_B));
    action_find = new QAction("查找",menu);
    action_find->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));
    action_source = new QAction("查看网页源码",menu);
    action_source->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_U));
    action_history = new QAction("历史记录",menu);
    action_history->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_H));
    action_devtool = new QAction("开发者工具",menu);
    action_devtool->setShortcut(QKeySequence(Qt::Key_F12));
    action_loadJS = new QAction("重载JS",menu);
    action_about = new QAction("关于",menu);
    action_about->setShortcut(QKeySequence(Qt::Key_F1));
    menu->addAction(action_newtab);
    menu->addAction(action_open);
    menu->addAction(action_bookmark);
    menu->addAction(action_find);
    menu->addAction(action_source);
    menu->addAction(action_history);
    menu->addAction(action_devtool);
    menu->addAction(action_loadJS);    
    menu->addAction(action_about);
    ui->pushButtonMenu->setMenu(menu);
    connect(action_newtab,SIGNAL(triggered(bool)),this,SLOT(newTab()));
    connect(action_open,SIGNAL(triggered(bool)),this,SLOT(openFile()));    
    connect(action_find,SIGNAL(triggered(bool)),this,SLOT(find()));
    connect(action_source,SIGNAL(triggered(bool)),this,SLOT(viewSource()));
    connect(action_history,SIGNAL(triggered(bool)),this,SLOT(history()));
    connect(action_devtool,SIGNAL(triggered(bool)),this,SLOT(inspector()));
    connect(action_loadJS,SIGNAL(triggered(bool)),this,SLOT(loadJS()));
    connect(action_about,SIGNAL(triggered(bool)),this,SLOT(about()));
    connect(new QShortcut(QKeySequence(Qt::Key_Up),this), SIGNAL(activated()),this, SLOT(prevURL()));
    connect(new QShortcut(QKeySequence(Qt::Key_Down),this), SIGNAL(activated()),this, SLOT(nextURL()));
    connect(new QShortcut(QKeySequence(Qt::Key_Return),this), SIGNAL(activated()),this, SLOT(gotoURL()));
    connect(new QShortcut(QKeySequence(Qt::Key_Enter),this), SIGNAL(activated()),this, SLOT(gotoURL()));
    connect(new QShortcut(QKeySequence(Qt::Key_Escape),this), SIGNAL(activated()),this, SLOT(cancel()));
    connect(new QShortcut(QKeySequence(Qt::Key_F5),this), SIGNAL(activated()),this, SLOT(refresh()));    
    connect(new QShortcut(QKeySequence(Qt::ALT + Qt::Key_Left),this), SIGNAL(activated()),this, SLOT(goBack()));
    connect(new QShortcut(QKeySequence(Qt::ALT + Qt::Key_Right),this), SIGNAL(activated()),this, SLOT(goForward()));
    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Tab),this), SIGNAL(activated()),this, SLOT(switchTab()));    
    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Return),this), SIGNAL(activated()),this, SLOT(fillURL()));
    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Enter),this), SIGNAL(activated()),this, SLOT(fillURL()));
    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_W),this), SIGNAL(activated()),this, SLOT(closeCurrentTab()));
    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_D),this), SIGNAL(activated()),this, SLOT(on_pushButton_addBookmark_clicked()));
    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Plus),this), SIGNAL(activated()),this, SLOT(zoomin()));
    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Minus),this), SIGNAL(activated()),this, SLOT(zoomout()));
    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_0),this), SIGNAL(activated()),this, SLOT(zoom1()));    
    connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(currentChange(int)));
    connect(ui->tabWidget,SIGNAL(tabBarDoubleClicked(int)),this,SLOT(tabBarDoubleClick(int)));
    newTab();
    ui->lineEditURL->setText("http://www.baidu.com/");
    QWebSettings::setIconDatabasePath(".");

    find_dialog = new QDialog;
    find_dialog->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    QHBoxLayout *hbox = new QHBoxLayout;
    lineEdit_find = new QLineEdit;
    hbox->addWidget(lineEdit_find);
    pushButton_findlast = new QPushButton;
    pushButton_findlast->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));
    pushButton_findlast->setFlat(true);
    pushButton_findlast->setFocusPolicy(Qt::NoFocus);
    hbox->addWidget(pushButton_findlast);
    pushButton_findnext = new QPushButton;
    pushButton_findnext->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
    pushButton_findnext->setFlat(true);
    pushButton_findnext->setFocusPolicy(Qt::NoFocus);
    hbox->addWidget(pushButton_findnext);
    pushButton_findclose = new QPushButton;
    pushButton_findclose->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
    pushButton_findclose->setFlat(true);
    pushButton_findclose->setFocusPolicy(Qt::NoFocus);
    hbox->addWidget(pushButton_findclose);
    find_dialog->setLayout(hbox);
    find_dialog->resize(250,20);
    connect(lineEdit_find,SIGNAL(returnPressed()),this,SLOT(findnext()));
    connect(pushButton_findlast,SIGNAL(pressed()),this,SLOT(findlast()));
    connect(pushButton_findnext,SIGNAL(pressed()),this,SLOT(findnext()));
    connect(pushButton_findclose,SIGNAL(pressed()),this,SLOT(hidefind()));

    loadBookmarks();
    loadHistory();
    loadJS();

    QString FileNameHistory = QDir::currentPath() + "/history";
    QFile *file=new QFile(FileNameHistory);
    if(!QFileInfo(FileNameHistory).isFile()){
        file->open(QIODevice::WriteOnly);
        file->close();
    }

//    QNetworkProxy proxy;
//    proxy = QNetworkProxy::HttpProxy;
//    proxy.setHostName("127.0.0.1");
//    proxy.setPort(8087);
//    NAM = new QNetworkAccessManager;
//    NAM->setProxy(proxy);

    tableSearch = new QTableWidget(this);
    tableSearch->setShowGrid(false);
    tableSearch->setColumnCount(1);
    tableSearch->setSelectionMode(QAbstractItemView::SingleSelection);
    tableSearch->horizontalHeader()->setVisible(false);
    tableSearch->verticalHeader()->setVisible(false);    
    tableSearch->setVisible(false);
    connect(tableSearch,SIGNAL(cellClicked(int,int)),this,SLOT(cellClick(int,int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newTab()
{
    WI->setVisible(false);
    QWebView *webView = new QWebView;
    webView->settings()->setAttribute(QWebSettings::PluginsEnabled,true);
    webView->settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows,true);
    webView->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled,true);
    webView->page()->setLinkDelegationPolicy(QWebPage::DelegateExternalLinks);
    //webView->page()->setNetworkAccessManager(NAM);
    connect(webView, SIGNAL(linkClicked(const QUrl&)), this, SLOT(onLinkClicked(const QUrl&)));
    connect(webView->page(),SIGNAL(linkHovered(QString,QString,QString)),this,SLOT(linkHover(QString,QString,QString)));
    connect(webView,SIGNAL(loadProgress(int)),this,SLOT(loadProgress(int)));
    connect(webView,SIGNAL(loadStarted()),this,SLOT(loadStart()));
    connect(webView,SIGNAL(loadFinished(bool)),this,SLOT(loadFinish(bool)));
    connect(webView->page(),SIGNAL(downloadRequested(QNetworkRequest)), this, SLOT(downloadRequest(QNetworkRequest)));    
    connect(webView,SIGNAL(titleChanged(QString)),this,SLOT(titleChange(QString)));
    connect(webView,SIGNAL(iconChanged()),this,SLOT(iconChange()));
    ui->tabWidget->addTab(webView,QIcon("icon.png"),"新标签页");
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
}

void MainWindow::gotoURL()
{
    tableSearch->setVisible(false);
    ((QWebView*)(ui->tabWidget->currentWidget()))->load(QUrl(ui->lineEditURL->text()));
}

void MainWindow::stop()
{
    ((QWebView*)(ui->tabWidget->currentWidget()))->stop();
}

void MainWindow::onLinkClicked(const QUrl &url)
{
    //newTab();
    ui->lineEditURL->setText(url.toString());
    ((QWebView*)(ui->tabWidget->currentWidget()))->load(url);
}

void MainWindow::goBack()
{
   ((QWebView*)(ui->tabWidget->currentWidget()))->page()->triggerAction(QWebPage::Back);
}

void MainWindow::goForward()
{
    ((QWebView*)(ui->tabWidget->currentWidget()))->page()->triggerAction(QWebPage::Forward);
}

void MainWindow::about()
{
    newTab();
    ((QWebView*)(ui->tabWidget->currentWidget()))->load(QUrl::fromLocalFile(QDir::currentPath()+"/about.htm"));
}

void MainWindow::loadStart()
{
    ui->pushButtonGoto->setVisible(false);
    ui->pushButtonStop->setVisible(true);
    QString title = ((QWebView*)(ui->tabWidget->currentWidget()))->title();
    QString surl = ((QWebView*)(ui->tabWidget->currentWidget()))->url().toString();
    for(int i=0;i<SL_bookmark_title.size();i++){
        if(SL_bookmark_title.at(i)==title && SL_bookmark_url.at(i)==surl){
            ui->pushButton_addBookmark->setIcon(QIcon(":/bookmark_on.png"));
            bookmarked = true;
            break;
        }else{
            ui->pushButton_addBookmark->setIcon(QIcon(":/bookmark_off.png"));
            bookmarked = false;
        }
    }    
}

void MainWindow::loadFinish(bool b)
{
    ui->pushButtonGoto->setVisible(true);
    ui->pushButtonStop->setVisible(false);
    if(b){
        ui->progressBar->setValue(0);
        ui->lineEditURL->setText(((QWebView*)(ui->tabWidget->currentWidget()))->url().toString());
    }else{
        if(((QWebView*)(ui->tabWidget->currentWidget()))->url().toString().contains(".")){
            if(!((QWebView*)(ui->tabWidget->currentWidget()))->url().toString().startsWith("http://")){
                ((QWebView*)(ui->tabWidget->currentWidget()))->load(QUrl( "http://" + ((QWebView*)(ui->tabWidget->currentWidget()))->url().toString()) );
            }
        }else{
            ((QWebView*)(ui->tabWidget->currentWidget()))->load(QUrl( "http://www.baidu.com/s?wd=" + ui->lineEditURL->text()) );
        }
    }    
}

void MainWindow::openFile()
{
    if(filename==""){
        filename = QFileDialog::getOpenFileName(this, "打开网页", ".");
    }else{
        filename = QFileDialog::getOpenFileName(this, "打开网页", filename);
    }
    if(!filename.isEmpty()){
        ((QWebView*)(ui->tabWidget->currentWidget()))->load(QUrl::fromLocalFile(filename));
        ui->lineEditURL->setText("file://"+filename);
    }
}

// 下载：http://www.qtcentre.org/threads/41997-Qwebview-Copy-Text-And-Save-Image
//      http://www.linuxjournal.com/magazine/using-webkit-your-desktop-application
void MainWindow::downloadRequest(const QNetworkRequest &request)
{
    QString defaultFileName = QFileInfo(request.url().toString()).fileName();
    QString fileName = QFileDialog::getSaveFileName(this, tr("保存文件"), defaultFileName);
    if (fileName.isEmpty()) return;
    QNetworkRequest newRequest = request;
    newRequest.setAttribute(QNetworkRequest::User, fileName);
    QNetworkAccessManager *networkManager = ((QWebView*)(ui->tabWidget->currentWidget()))->page()->networkAccessManager();
    QNetworkReply *reply = networkManager->get(newRequest);
    connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(downloadProgress(qint64, qint64)));
    connect(reply, SIGNAL(finished()), this, SLOT(downloadFinish()));
}

void MainWindow::downloadFinish()
{
    QNetworkReply *reply = ((QNetworkReply*)sender());
    QNetworkRequest request = reply->request();
    QVariant v = request.attribute(QNetworkRequest::User);
    QString fileName = v.toString();
    QFile file(fileName);
    if (file.open(QFile::ReadWrite))
        file.write(reply->readAll());
}

QString sbytes(qint64 bytes){
    QString unit="B";
    double dbytes=bytes*1.0;
    if(bytes>999999999){
        dbytes/=(1024*1024*1024);
        unit="GB";
    }else{
        if(bytes>999999){
            dbytes/=(1024*1024);
            unit="MB";
        }else{
            if(bytes>999){
                dbytes/=1024;
                unit="KB";
            }
        }
    }
    return QString("%1%2").arg(QString::number(dbytes,'f',2)).arg(unit);
}

void MainWindow::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    statusBar()->showMessage(QString("%1/%2").arg(sbytes(bytesReceived)).arg(sbytes(bytesTotal)));
}

void MainWindow::titleChange(QString title)
{
    ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), title);
    ui->tabWidget->setTabToolTip(ui->tabWidget->currentIndex(),title);
    //QString title = ((QWebView*)(ui->tabWidget->currentWidget()))->title();
    QString surl = ((QWebView*)(ui->tabWidget->currentWidget()))->url().toString();
    if(title!="" && surl!="" && surl!="htybrowser://history")
        appendHistory(QDateTime::currentDateTime().toString("MM-dd hh:mm"),title,surl);
}

void MainWindow::linkHover(const QString &link, const QString &title, const QString &textContent)
{
    Q_UNUSED(title);
    Q_UNUSED(textContent);
    statusBar()->showMessage(link);
}

void MainWindow::closeTab(int i)
{
    ui->tabWidget->removeTab(i);
    if(ui->tabWidget->count()<1){
        newTab();
    }else{
        if(WI->isVisible()){
            WI->setPage(((QWebView*)(ui->tabWidget->currentWidget()))->page());
            WI->show();
        }
    }
}

void MainWindow::currentChange(int i)
{
    //qDebug() << i;
    if(i!=-1)
        ui->lineEditURL->setText(((QWebView*)(ui->tabWidget->currentWidget()))->url().toString());
}

void MainWindow::iconChange()
{
    ui->tabWidget->setTabIcon(ui->tabWidget->currentIndex(),((QWebView*)(ui->tabWidget->currentWidget()))->icon());
}

void MainWindow::switchTab()
{
    int index = ui->tabWidget->currentIndex();
    if(index == ui->tabWidget->count()-1){
        ui->tabWidget->setCurrentIndex(0);
        return;
    }
    if(index < ui->tabWidget->count()-1){
        ui->tabWidget->setCurrentIndex(index+1);        
    }
    if(WI->isVisible()){
        WI->setPage(((QWebView*)(ui->tabWidget->currentWidget()))->page());
        WI->show();
    }
}

void MainWindow::fillURL()
{
    ui->lineEditURL->setText("http://www."+ui->lineEditURL->text()+".com");
    gotoURL();
}

void MainWindow::closeCurrentTab()
{
    ui->tabWidget->removeTab(ui->tabWidget->currentIndex());
    if(ui->tabWidget->count()<1)newTab();
}

void MainWindow::viewSource()
{
    QString s = ((QWebView*)(ui->tabWidget->currentWidget()))->page()->currentFrame()->toHtml();
    s=s.replace("<","&lt;");
    s=s.replace(">","&gt;");
    s="<title>源码</title><pre>"+s+"</pre>";
    //qDebug() << s;
    QUrl url = ((QWebView*)(ui->tabWidget->currentWidget()))->url();
    newTab();
    ((QWebView*)(ui->tabWidget->currentWidget()))->setHtml(s,url);
}

void MainWindow::loadProgress(int i)
{
    ui->progressBar->setValue(i);
    ((QWebView*)(ui->tabWidget->currentWidget()))->page()->mainFrame()->evaluateJavaScript(js);
}

void MainWindow::refresh()
{
    ((QWebView*)(ui->tabWidget->currentWidget()))->load(((QWebView*)(ui->tabWidget->currentWidget()))->url());
}

void MainWindow::find()
{
    qDebug() << lineEdit_find->height();
    lineEdit_find->setText(((QWebView*)(ui->tabWidget->currentWidget()))->selectedText());
    find_dialog->move(x() + width() - find_dialog->width(), y()+90);
    find_dialog->show();
    find_dialog->raise();
}

void MainWindow::hidefind()
{
    ((QWebView*)(ui->tabWidget->currentWidget()))->findText("");
    find_dialog->hide();
}

void MainWindow::findlast()
{
    ((QWebView*)(ui->tabWidget->currentWidget()))->findText(lineEdit_find->text(),QWebPage::FindBackward);
}

void MainWindow::findnext()
{
    //((QWebView*)(ui->tabWidget->currentWidget()))->findText(lineEdit_find->text(),QWebPage::HighlightAllOccurrences);
    ((QWebView*)(ui->tabWidget->currentWidget()))->findText(lineEdit_find->text());
}

void MainWindow::loadBookmarks()
{
    QString FileNameBookmark = QDir::currentPath() + "/bookmark";
    QFile *file=new QFile(FileNameBookmark);
    if(!QFileInfo(FileNameBookmark).isFile()){
        file->open(QIODevice::WriteOnly);
        file->close();
    }
    if(file->open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream ts(file);
        QString s=ts.readAll();
        file->close();
        QStringList line=s.split("\n");
        for(int i=0;i<line.size();i++){
            if(line.at(i).contains("#")){
                QStringList strlist=line.at(i).split("#");
                SL_bookmark_title.append(strlist.at(0));
                SL_bookmark_url.append(strlist.at(1));
            }
        }
    }
    fillBookmarkMenu();
}

void MainWindow::fillBookmarkMenu()
{
    QMenu *submenu = new QMenu;
    for(int i=0;i<SL_bookmark_title.size();i++){
        QAction *action_bookmark_title = new QAction(submenu);
        action_bookmark_title->setText(SL_bookmark_title.at(i));
        action_bookmark_title->setToolTip(SL_bookmark_url.at(i));
        submenu->addAction(action_bookmark_title);
        connect(action_bookmark_title,SIGNAL(triggered(bool)),this,SLOT(gotoBookmarkURL(bool)));
    }
    action_bookmark->setMenu(submenu);
}

void MainWindow::on_pushButton_addBookmark_clicked()
{
    QString title = ((QWebView*)(ui->tabWidget->currentWidget()))->title();
    QString surl = ((QWebView*)(ui->tabWidget->currentWidget()))->url().toString();
    if(title!="" && surl!=""){
        if(bookmarked){
            ui->pushButton_addBookmark->setIcon(QIcon(":/bookmark_off.png"));
            SL_bookmark_title.removeOne(title);
            SL_bookmark_url.removeOne(surl);
        }else{
            ui->pushButton_addBookmark->setIcon(QIcon(":/bookmark_on.png"));
            SL_bookmark_title.append(title);
            SL_bookmark_url.append(surl);
        }
        bookmarked = !bookmarked;
        fillBookmarkMenu();
        saveBookmarks();
    }
}

void MainWindow::saveBookmarks()
{
    QString FileNameBookmark = QDir::currentPath() + "/bookmark";
    QFile file(FileNameBookmark);
    if(file.open(QFile::WriteOnly))
    {
        QTextStream ts(&file);
        QString s;
        for(int i=0;i<SL_bookmark_title.size();i++){
            s = s+ SL_bookmark_title.at(i) + "#" + SL_bookmark_url.at(i);
            if(i < SL_bookmark_title.size()-1)
                s = s + "\n";
        }
        ts << s;
        file.close();
    }
}

void MainWindow::gotoBookmarkURL(bool)
{
    QAction *action = qobject_cast<QAction*>(sender()); // 获取发出信号的对象
    qDebug() << action->toolTip();
    ui->lineEditURL->setText(action->toolTip());
    ((QWebView*)(ui->tabWidget->currentWidget()))->load(QUrl(ui->lineEditURL->text()));
}

void MainWindow::loadHistory()
{
    //QList<History> historys;
    SL_history_time.clear();
    SL_history_title.clear();
    SL_history_url.clear();
    QString FileNameHistory = QDir::currentPath() + "/history";
    QFile *file=new QFile(FileNameHistory);
    if(file->open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream ts(file);
        QString s=ts.readAll();
        file->close();
        QStringList line=s.split("\n");
        for(int i=0;i<line.size();i++){
            if(line.at(i).contains("#")){
                QStringList strlist=line.at(i).split("#");
//                History history;
//                history.time = strlist.at(0);
//                history.title = strlist.at(1);
//                history.url = strlist.at(2);
//                historys.append(history);
                SL_history_time.append(strlist.at(0));
                SL_history_title.append(strlist.at(1));
                SL_history_url.append(strlist.at(2));
            }
        }
    }
}

void MainWindow::history()
{
    loadHistory();
    QString s = "<html><head><title>历史记录</title><style>a{text-decoration:none;color:black;} table{margin:10 auto;} td{padding:5px;white-space:nowrap;overflow:hidden;text-overflow:ellipsis;} td:first-child{color:gray;} td:nth-child(2){max-width:500px;} td:nth-child(3){color:gray;max-width:500px;}</style></head><body><table>";
    for(int i=SL_history_time.size()-1; i>=0; i--){
        //s += "<tr><td>" + historys.at(i).time + "</td><td><a href=" + historys.at(i).url + ">" + historys.at(i).title + "</td><td>" + historys.at(i).url + "</td></tr>";
        s += "<tr><td>" + SL_history_time.at(i) + "</td><td><a href=" + SL_history_url.at(i) + ">" + SL_history_title.at(i) + "</td><td>" + SL_history_url.at(i) + "</td></tr>";
    }
    s+="</table></body></html>";
    newTab();
    QUrl url("HTYBrowser://history");
    ((QWebView*)(ui->tabWidget->currentWidget()))->setHtml(s,url);
    ui->tabWidget->setTabIcon(ui->tabWidget->currentIndex(),QIcon(":/history.ico"));
}

void MainWindow::appendHistory(QString stime, QString title, QString surl)
{
    QString FileNameHistory = QDir::currentPath() + "/history";
    QFile file(FileNameHistory);
    if(file.open(QFile::WriteOnly | QIODevice::Append)){
        QTextStream ts(&file);
        QString s = stime + "#" + title + "#" + surl + "\n";
        //s = s + s0;
        ts << s;
        file.close();
    }
}

void MainWindow::loadJS()
{
    QString FileNameJS = QDir::currentPath() + "/js.js";
    QFile *file=new QFile(FileNameJS);
    if(!QFileInfo(FileNameJS).isFile()){
        file->open(QIODevice::WriteOnly);
        file->close();
    }else{
        if(file->open(QIODevice::ReadOnly | QIODevice::Text)){
            QTextStream ts(file);
            js=ts.readAll();
            file->close();
        }
    }
}

void MainWindow::zoomin()
{
    ((QWebView*)(ui->tabWidget->currentWidget()))->setZoomFactor(((QWebView*)(ui->tabWidget->currentWidget()))->zoomFactor() + 0.1);
}

void MainWindow::zoomout()
{
    ((QWebView*)(ui->tabWidget->currentWidget()))->setZoomFactor(((QWebView*)(ui->tabWidget->currentWidget()))->zoomFactor() - 0.1);
}

void MainWindow::zoom1()
{
    ((QWebView*)(ui->tabWidget->currentWidget()))->setZoomFactor(1);
}

void MainWindow::inspector()
{
    if(WI->isVisible()){
        WI->setVisible(false);
    }else{        
        WI->setPage(((QWebView*)(ui->tabWidget->currentWidget()))->page());
        WI->setVisible(true);
    }
}

void MainWindow::tabBarDoubleClick(int index)
{
    qDebug() << "tab" << index;
    if(index==-1){
        newTab();
    }
}

void MainWindow::search(QString key)
{
    if(key!=""){
        tableSearch->setRowCount(0);
        tableSearch->setVisible(true);
        //QStringList result = SL_history_url.filter(key);
        QStringList SLURL;
        SLURL.append(SL_history_url);
        SLURL.append(SL_bookmark_url);
        QStringList result = SLURL.filter(key);
        result.removeDuplicates();
        if(result.size()==0){
            tableSearch->setVisible(false);
        }else{
            for(int i=0; i<result.size(); i++){
                tableSearch->insertRow(i);
                tableSearch->setItem(i,0,new QTableWidgetItem(result.at(i)));
            }
        }
    }
}

void MainWindow::cancel()
{   
    tableSearch->hide();
    stop();
}

void MainWindow::cellClick(int r,int c)
{
    //qDebug() << tableSearch->item(r,c)->text();
    ui->lineEditURL->setText(tableSearch->item(r,c)->text());
    gotoURL();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    tableSearch->resize(ui->lineEditURL->width(),220);
    tableSearch->setColumnWidth(0,ui->lineEditURL->width());
    tableSearch->move(ui->lineEditURL->x(), ui->lineEditURL->y()+ui->lineEditURL->height());
    //qDebug() << ui->lineEditURL->size();
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->lineEditURL){
        if(event->type() == QEvent::FocusIn){
            qDebug() << "focus in";
            //ui->lineEditURL->selectAll();
            //ui->lineEditURL->setFocus(Qt::OtherFocusReason);
            QTimer::singleShot(0,ui->lineEditURL,SLOT(selectAll()));
        }
    }
    return QWidget::eventFilter(watched,event);
}

void MainWindow::prevURL()
{
    if(tableSearch->isVisible() && tableSearch->currentRow()>0){
        tableSearch->setCurrentCell(tableSearch->currentRow()-1,0);
        ui->lineEditURL->setText(tableSearch->item(tableSearch->currentRow(),0)->text());
    }else{
       ((QWebView*)(ui->tabWidget->currentWidget()))->page()->mainFrame()->setScrollBarValue(Qt::Vertical, ((QWebView*)(ui->tabWidget->currentWidget()))->page()->mainFrame()->scrollBarValue(Qt::Vertical) - 100 );
    }
}

void MainWindow::nextURL()
{
    if(tableSearch->isVisible() && tableSearch->currentRow()<tableSearch->rowCount()-1){
        tableSearch->setCurrentCell(tableSearch->currentRow()+1,0);
        ui->lineEditURL->setText(tableSearch->item(tableSearch->currentRow(),0)->text());
    }else{
        ((QWebView*)(ui->tabWidget->currentWidget()))->page()->mainFrame()->setScrollBarValue(Qt::Vertical, ((QWebView*)(ui->tabWidget->currentWidget()))->page()->mainFrame()->scrollBarValue(Qt::Vertical) + 100 );
    }
}
