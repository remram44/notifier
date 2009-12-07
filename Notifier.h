#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <QtGui>
#include <QHttp>
#include <QSound>

#define HOST "yoshi.rez-gif.supelec.fr"
#define PORT 80
#define URI "/tw/"

#ifndef PREFIX
#define PREFIX "/usr/local"
#endif

class Notifier : public QWidget {

    Q_OBJECT

private:
    QSystemTrayIcon *m_pTrayIcon;
    QSound *m_pBeep;
    QHttp *m_pHTTP;

public:
    Notifier(QWidget *pParent = NULL);

public slots:
    void refresh();
    void requestFinished(int id, bool error);

};

#endif
