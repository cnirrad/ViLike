#include <iostream>
#include "ViKeyManager.h"
#include "utils.h"


#define IS_MODIFIER_KEY( sym )                              \
       ( sym == GDK_Alt_L || sym == GDK_Alt_R ||            \
         sym == GDK_Control_L || sym == GDK_Control_R ||    \
         sym == GDK_Meta_L || sym == GDK_Meta_R ||          \
         sym == GDK_Super_L || sym == GDK_Super_R ||        \
         sym == GDK_Hyper_L || sym == GDK_Hyper_R ||        \
         sym == GDK_Shift_L || sym == GDK_Shift_R )
bool
wait_to_execute_action( unsigned char flags )
{
    return (flags & ( KeyActionBase::await_motion | KeyActionBase::await_param )) > 0;
}


ViKeyManager::ViKeyManager(Gtk::Window *w) :
    m_count(0),
    m_current_register(0x00),
    m_insertMap(),
    m_key(""),
    m_mode(vi_normal),
    m_normalMap(),
    m_registers(),
    m_window(w)
{

}

ViKeyManager::~ViKeyManager()
{
}

bool ViKeyManager::map_key(ViMode mode, const char *key, KeyActionBase *action )
{
    switch ( mode )
    {
        case vi_normal:
            m_normalMap[key] = action;
            break;
        case vi_insert:
            m_insertMap[key] = action;
            break;
        default:
            g_print("Warning: map_key ignored for mode.");
            break;
    }

    return true;
}


bool ViKeyManager::on_key_press( GdkEventKey *event )
{
    //
    //  Escape key cannot be overridden
    //
    if (event->keyval == GDK_Escape)
    {
        m_mode = vi_normal;
        clear_key_buffer();
        return true;
    }

    if (IS_MODIFIER_KEY( event->keyval ))
    {
        return true;
    }

    Gtk::Widget *w = m_window->get_focus();

    Glib::ustring str = key_to_str( event );

    m_key = m_key + str;

    if (m_mode == vi_insert)
    {
        KeyActionBase *action = m_insertMap[m_key];
        if (action)
        {
            action->execute(w, 1, m_params);
            clear_key_buffer(action->m_flags);
            return true;
        }

        //
        //  Pass the key press on
        //
        gboolean ret_val;
        gtk_signal_emit_by_name( (GtkObject*)w->gobj(), 
                                 "key-press-event", 
                                 event,
                                 &ret_val );
        clear_key_buffer();
        return false;
    }

    //
    //  Normal Mode. 
    //
    if (BIT_ON(m_submode, vi_wait_param) && m_action != NULL)
    {
        m_params = m_params + str;
        m_action->execute( w, m_count, m_params );
        clear_key_buffer();
        return true;
    }

    //
    //  Handle a count modifier. 
    //
    if (m_key == str && event->keyval >= GDK_0 && event->keyval <= GDK_9)
    {
        //
        //  Count modifier not allowed to start with '0'
        //
        if (!(event->keyval == GDK_0 && m_count == 0))
        {
            int num = event->keyval - GDK_0;
            m_count = (m_count * 10) + num;
            m_key = "";
            return true;
        }
    }

    KeyActionBase *action = m_normalMap[m_key];
    if (action)
    {
        if (BIT_ON(action->m_flags, KeyActionBase::await_param))
        {
            m_submode = m_submode | vi_wait_param; 
            m_action = action; 
            return true;
        }
        else if (BIT_ON(action->m_flags, KeyActionBase::await_motion))
        {
            m_action = action; 
            m_submode = vi_wait_motion;
            m_key = "";
            m_count = 0;
            return true;
        }

        if (m_count == 0)
            m_count = 1;

        action->execute(w, m_count, m_params);
        clear_key_buffer(action->m_flags);
        return true;
    }
    g_print("No action for key %s\n", m_key.data());

    return true;
}

bool ViKeyManager::on_key_release( GdkEventKey *event)
{
    return true;
}

Glib::ustring ViKeyManager::key_to_str( GdkEventKey *event )
{
    Glib::ustring key_str;

    if ( event->keyval == GDK_Shift_L || event->keyval == GDK_Shift_R )
    {
        //
        // Ignore the shift key
        //
        return "";
    }
    if (event->keyval > GDK_space && event->keyval <= GDK_asciitilde)
    {
        key_str = (char)event->keyval;
    }
    else if (event->keyval == GDK_BackSpace)
    {
        key_str = "BS";
    }
    else if (event->keyval == GDK_Tab)
    {
        key_str = "Tab";
    }
    else if (event->keyval == GDK_Linefeed)
    {
        key_str = "LF";
    }
    else if (event->keyval == GDK_Return)
    {
        key_str = "CR";
    }
    else if (event->keyval == GDK_space)
    {
        key_str = "Space";
    }
    else if (event->keyval == GDK_Page_Up)
    {
        key_str = "PgUp";
    }
    else if (event->keyval == GDK_Page_Down)
    {
        key_str = "PgDn";
    }
    else if (event->keyval == GDK_Home)
    {
        key_str = "Home";
    }
    else if (event->keyval == GDK_End)
    {
        key_str = "End";
    }
    else if (event->keyval >= GDK_F1 && event->keyval <= GDK_F35)
    {
        int num = event->keyval - (GDK_F1 - 1);
        key_str = "F" + Glib::ustring::format(num);
    }
    else
    {
        key_str = "0x" + Glib::ustring::format(std::hex,  event->keyval);
    }

    //
    // Mod1 = Alt
    //
    if ((event->state & GDK_MOD1_MASK) == GDK_MOD1_MASK)
    {
        key_str = "A-" + key_str;
    }
    
    if ((event->state & GDK_CONTROL_MASK) == GDK_CONTROL_MASK)
    {
        key_str = "C-" + key_str;              
    }

    //
    // Surround with < > if not a single key character.
    //
    if (key_str.length() > 1)
    {
        key_str = "<" + key_str + ">";
    }
    return key_str;
}

void ViKeyManager::clear_key_buffer( unsigned char flags )
{
    m_key = "";
    m_params = "";

    if (!wait_to_execute_action( flags ))
    {
        m_submode = vi_sub_none;
        m_action = NULL;
        m_count = 0;
    }

    if ( !BIT_ON(flags, KeyActionBase::no_reset_cur_reg))
        m_current_register = 0x00;
}

char ViKeyManager::get_current_register()
{
    return m_current_register;
}

void ViKeyManager::set_current_register( char reg )
{
    m_current_register = reg;
}

Glib::ustring ViKeyManager::get_register( char reg )
{
    return m_registers[reg]; 
}

void ViKeyManager::set_register( char reg, Glib::ustring text )
{
    m_registers[reg] = text;
}

ViMode ViKeyManager::get_mode() const
{
    return m_mode;
}

void ViKeyManager::set_mode( ViMode m )
{
    m_mode = m;
}
                           

unsigned char ViKeyManager::get_sub_mode() const
{
    return m_submode;
}

KeyActionBase* ViKeyManager::get_saved_action() const
{
    return m_action;
}


bool
MotionAction::execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params)
{

    //
    //  If waiting for a motion, then extend selection will be
    //  true. The selection is how the command waiting for the 
    //  motion will know what to act upon.
    //
    if (BIT_ON(m_vi->get_sub_mode(), vi_wait_motion)) 
    {
        m_ext_sel = true;
    }
    else
    {
        m_ext_sel = false;
    }

    perform_motion(w, count_modifier, params);

    if (BIT_ON(m_vi->get_sub_mode(), vi_wait_motion))
    {
        KeyActionBase *action = m_vi->get_saved_action();
        if (action)
        {
            action->execute(w, count_modifier, params); 
        }
    }

    //
    // If not in visual mode, then the extend selection was to
    // allow other commands to know the range to act on. Remove the 
    // selection now.
    //
    if (m_ext_sel, m_vi->get_mode() != vi_visual && is_text_widget(w))
    {
        Gtk::TextView *view = dynamic_cast<Gtk::TextView*>(w); 
        Glib::RefPtr<Gtk::TextBuffer> buffer = view->get_buffer();

        Gtk::TextBuffer::iterator it = get_cursor_iter( buffer ); 
        buffer->place_cursor(it);
    } 
    return true;
}

