/*            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *                    Version 2, December 2004
 *
 * Copyright (C) 2004 Sam Hocevar
 *  14 rue de Plaisance, 75014 Paris, France
 * Everyone is permitted to copy and distribute verbatim or modified
 * copies of this license document, and changing it is allowed as long
 * as the name is changed.
 *
 *            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
 *
 *  0. You just DO WHAT THE FUCK YOU WANT TO.
 */

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
