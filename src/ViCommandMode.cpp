#include "ViCommandMode.h"

#include "App.h"
#include "Editor.h"
#include "ViKeyManager.h"
#include "utils.h"

ViCommandMode::ViCommandMode(ViKeyManager *vi) :
    m_vi(vi),
    m_cmd(""),
    m_history_it(),
    m_history()
{
}

ViCommandMode::~ViCommandMode()
{
    m_keyMap.clear();
    m_history.clear();
}


void ViCommandMode::enter_mode( ViMode from_mode )
{
    m_cmd = m_vi->get_last_key();
    m_vi->show_message(m_cmd.data());
    m_history_it = m_history.begin();
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
        execute( m_cmd );

        m_vi->set_mode(vi_normal);
        return true;
    }
    else if (key_str == "<BS>")
    {
        m_cmd = m_cmd.substr(0, m_cmd.length() - 1);

        if (m_cmd.length() == 0)
        {
            m_vi->set_mode(vi_normal);
            return true;
        }
    }
    else if (key_str == "<Up>")
    {
        m_cmd = next_history(Backward, m_cmd[0]);
    }
    else if (key_str == "<Down>")
    {
        m_cmd = next_history(Forward, m_cmd[0]);
    }
    else
    {
        m_cmd += key_str;
    }

    m_vi->show_message("%s", m_cmd.data());
    return true;
}

void ViCommandMode::map_key( const Glib::ustring &widget_type, 
                             const Glib::ustring &key,
                             ExecutableAction *a )
{
    if (key[0] == ':')
        m_commandMap[key.substr(1)] = a;
    else
        m_keyMap[key] = a;
}

void ViCommandMode::execute(const Glib::ustring &cmd)
{
    add_history( cmd );

    char ch = cmd[0];
    Glib::ustring rest = cmd.substr(1);

    if (ch == '/')
    {
        //
        //  Forward search
        //
        execute_search(rest, ch);
    }
    else if (ch == '?')
    {
        //
        //  Backward search
        //
        execute_search(rest, ch);
    }
    else if (ch == ':')
    {
        execute_command(rest);
    }
}


void ViCommandMode::execute_search(const Glib::ustring &cmd, char begin )
{
    //
    //     /{pattern}/{offset}<CR>
    // search forward for the [count]'th occurance of
    // {pattern} and go {offset} lines up or down.
    //
    int count = 1;              
    int offset = 0;
    Glib::ustring pattern;

    int idx = cmd.find( begin ); 
    if (idx > 0)
    {
        Glib::ustring rest = cmd.substr(idx+1);
        offset = convert<int>(rest);

        pattern = cmd.substr(0, idx);
    }
    else
    {
        pattern = cmd;
    }

    Direction dir = Forward;
    if (begin == '?')  
        dir = Backward;

    g_print("Searching for %s with offset %i\n", pattern.data(), offset);

    Editor *ed = Application::get()->get_current_editor();
    ed->search(pattern, dir);
    
}

void ViCommandMode::execute_command(const Glib::ustring &cmd_line)
{
    // TODO: more intelligent parsing
    Glib::ustring cmd;
    Glib::ustring params;
    int idx = cmd_line.find_first_of(' ');
    if (idx > 0)
    {
        params = cmd_line.substr(idx+1);
        cmd = cmd_line.substr(0, idx);
    }
    else
    {
        cmd = cmd_line;
    }

    g_print("Command %s with params '%s'\n", cmd.data(), params.data());

    ExecutableAction *act = m_commandMap[cmd];
    if (act)
    {
        Gtk::Widget *w = get_focused_widget();
        act->execute(w, 1, params);
    }
}

void ViCommandMode::add_history(const Glib::ustring &cmd)
{
    if (!m_history.empty())
        m_history.pop_front();
    m_history.push_front( cmd.raw() );
    m_history.push_front( "BOGUS" );    // Hack - the front entry is never shown

    if (m_history.size() >= 50)         // TODO: add config option
    {
        m_history.pop_back();
    }

}

Glib::ustring 
ViCommandMode::next_history(Direction d, char begin)
{
    if (d == Backward)
    {
        while ( m_history_it != m_history.end() )
        {
            m_history_it++;
            if ( m_history_it != m_history.end() && 
                    begin == (*m_history_it)[0])
                break;
        }
    }
    else 
    {
        if (m_history_it == m_history.begin())
        {
            // already at the beginning
            return m_cmd;
        }

        while ( m_history_it != m_history.begin() )
        {
            m_history_it--;
            if ( begin == (*m_history_it)[0] )
                break;
        }
    }

    if ( m_history_it == m_history.end() )
    {
        return m_cmd;
    }
    
    if ( m_history_it == m_history.begin() )
    {
        return Glib::ustring(1, begin);
    }

    return *m_history_it;
}


