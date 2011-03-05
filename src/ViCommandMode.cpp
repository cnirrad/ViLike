#include "ViCommandMode.h"

#include "ViKeyManager.h"

ViCommandMode::ViCommandMode(ViKeyManager *vi) :
    m_vi(vi)
{
}

ViCommandMode::~ViCommandMode()
{
    m_keyMap.clear();
}


void ViCommandMode::enter_mode( ViMode from_mode )
{
    m_cmd = m_vi->get_last_key();
    m_vi->show_message(m_cmd.data());
}

void ViCommandMode::exit_mode( ViMode to_mode )
{
    m_cmd = "";
    m_vi->show_message("");
}

bool 
ViCommandMode::handle_key_press( const Glib::ustring &key_str )
{
    if (key_str == "<Space>")
    {
        m_cmd += " ";
    }
    else if (key_str == "<CR>")
    {
        //execute command

        m_vi->set_mode(vi_normal);
    }
    else
    {
        m_cmd += key_str;
        m_vi->show_message("%s", m_cmd.data());
    }
}

void ViCommandMode::map_key( const Glib::ustring &widget_type, 
                            const Glib::ustring &key,
                            ExecutableAction *a )
{
}
