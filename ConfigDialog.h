#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include "ui_ConfigDialog.h"

/**
 * Servers configuration dialog.
 */
class ConfigDialog : public QDialog {

    Q_OBJECT

private:
    Ui::ConfigDialog ui;

public:
    ConfigDialog(QWidget *parent);

};

#endif
