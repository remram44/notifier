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

#include "MonitoredServer.h"

MonitoredServer::MonitoredServer(QString p_Name, bool p_Sound, bool p_Color,
    bool p_Popup, Server *p_Server)
  : m_name(p_Name), m_play_sound(p_Sound), m_change_color(p_Color),
    m_display_popup(p_Popup), server(p_Server)
{
    connect(server,
        SIGNAL(infosChanged(unsigned int, unsigned int, QString, QString,
            bool)),
        this,
        SIGNAL(infosChanged(unsigned int, unsigned int, QString, QString,
            bool)));
    connect(server, SIGNAL(errorEncountered(QString)),
        this, SIGNAL(errorEncountered(QString)));
    connect(this, SIGNAL(refresh()), server, SLOT(refresh()));
}
