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
    return (flags & ( await_motion | await_param )) > 0;
}


ViKeyManager::ViKeyManager(Gtk::Window *w) :
    m_context(),
    m_count(0),
    m_current_register(0x00),
    m_insertMap(),
    m_key(""),
    m_mode(vi_normal),
    m_normalMap(),
    m_registers(),
    m_window(w)
{
    m_context = new ViActionContext();
}

ViKeyManager::~ViKeyManager()
{
    delete m_context;
}

bool ViKeyManager::map_key(ViMode mode, const char *key, ExecutableAction *action )
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

bool ViKeyManager::execute( Glib::ustring &cmds )
{
    return execute( cmds, m_mode );    
}

bool ViKeyManager::execute( Glib::ustring &cmds, ViMode mode )
{
    clear_key_buffer();

    GdkEventKey* event;

    for (int idx = 0; idx < cmds.length(); ++idx)
    {
        gchar ch = cmds[idx];

        if ( ch == '<' )
        {
            //
            //  Is this just the '<' key, or is it part of a special key?
            //
            int end = cmds.find( '>', idx + 1 );
            if ( end > idx )
            {
                Glib::ustring substr = cmds.substr( idx,  end - idx + 1);
                g_print("substr for %s: %i -> %i = %s\n", cmds.data(), idx, end - idx + 1, substr.data());
                event = str_to_key( substr );
                idx = end;
            }
            else
            {
                event = str_to_key( cmds.substr( idx, 1 ) );
            }
        }
        else 
        {
            event = str_to_key( cmds.substr( idx, 1 ) );
        }

        if (event != NULL)
            on_key_press( event );
    }

    clear_key_buffer();
}

bool ViKeyManager::on_key_press( GdkEventKey *event )
{
    if (IS_MODIFIER_KEY( event->keyval ))
    {
        return true;
    }

    Gtk::Widget *w = m_window->get_focus();

    //
    //  Escape key cannot be overridden
    //
    if (event->keyval == GDK_Escape)
    {
        set_mode(vi_normal, w);
        clear_key_buffer();
        return true;
    }

    Glib::ustring str = key_to_str( event );

    m_key = m_key + str;

    if (m_mode == vi_insert)
    {
        ExecutableAction *action = m_insertMap[m_key];
        if (action)
        {
            Glib::ustring params = "";
            action->execute(w, 1, params);
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
    if (BIT_ON(m_context->get_flags(), await_param) )
    {
        m_context->set_param( str );
        m_context->execute( w );
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
            m_context->set_count(m_count);
            m_key = "";
            return true;
        }
    }

    ExecutableAction *action = m_normalMap[m_key];
    if (action)
    {
        if (m_context->get_action() == NULL)
        {
            m_context->set_action( action );
        }
        else if (BIT_ON(action->m_flags, is_motion))
        {
            MotionAction *motion = dynamic_cast<MotionAction*>(action);
            m_context->set_motion( motion );
        }

        m_context->execute(w);
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
    else if (event->keyval >= GDK_F1 && event->keyval <= GDK_F35)
    {
        int num = event->keyval - (GDK_F1 - 1);
        key_str = "F" + Glib::ustring::format(num);
    }
    else
    {
        for (int i = 0; i < vi_key_map_lgth; ++i)
        {
            if ( vi_key_map[i].keyval == event->keyval )
            {
                key_str = vi_key_map[i].string;
                break;
            }
        }
    }

    if ( key_str == "" ) 
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

GdkEventKey* ViKeyManager::str_to_key(const Glib::ustring &str)
{
    GdkEventKey* event = (GdkEventKey*)gdk_event_new(GDK_KEY_PRESS);
    event->window = m_window->get_window()->gobj();
    event->send_event = TRUE;
    event->time = GDK_CURRENT_TIME;
    event->state = GDK_KEY_PRESS_MASK;

    if (str.length() == 0)
        return NULL;

    if (str.length() == 1)
    {
        event->keyval = str[0];
        return event;
    }

    if (str[0] == '<' && str[str.length()-1] == '>')
    {
        Glib::ustring keyname = str.substr( 1, str.length() - 2 );
        for (int idx = 0; idx < vi_key_map_lgth; idx++)
        {
            if (keyname == vi_key_map[idx].string)
            {
                event->keyval = vi_key_map[idx].keyval;
                break;
            }
        }
    }

    if (event->keyval == 0)
    {
        g_print("ERROR: str_to_key mapping not found for %s\n", str.data());
        return NULL;
    }

    GdkKeymapKey* keys;
    gint n_keys;
    gdk_keymap_get_entries_for_keyval(NULL,
                                      event->keyval,
                                      &keys,
                                      &n_keys);
    event->hardware_keycode = keys[0].keycode;
    event->group = keys[0].group;

    return event;
}

void ViKeyManager::clear_key_buffer( unsigned char flags )
{
    m_key = "";

    if (!wait_to_execute_action( flags ))
    {
        m_context->reset();
        m_count = 0;
    }

    if ( !BIT_ON(flags, no_reset_cur_reg))
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

void ViKeyManager::set_mode( ViMode m, Gtk::Widget *w )
{
    m_mode = m;

    if (w && is_text_widget(w))
    {
        Gtk::TextView *view = dynamic_cast<Gtk::TextView*>(w);
        if (m_mode == vi_insert )
        {
            view->set_overwrite(false); 
        }
        else 
        {
            view->set_overwrite(true);
        }
    }
}

//
//  VI Action Context
//
ViActionContext* ViKeyManager::get_context() const
{
    return m_context;
}

void
ViActionContext::execute(Gtk::Widget *w)
{
    if ( (BIT_ON(m_flags, await_motion) && m_motion == NULL) ||
         (BIT_ON(m_flags, await_param) && m_param == "")) 
    {
        return;
    }
    
    if (m_motion != NULL)
    {
        m_motion->execute_as_subcommand(w, this);
    }
    else if (m_action != NULL)
    {
        m_action->execute(w, m_count, m_param);
    }
}

void
ViActionContext::reset()
{
    m_action = NULL;
    m_motion = NULL;
    m_count = -1;
    m_param = "";
    m_flags = 0x00;
}

//
//  MotionAction
//
bool
MotionAction::execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params)
{
    if (m_vi->get_mode() == vi_visual)
        m_ext_sel = true;

    perform_motion(w, count_modifier, params);
    
    if (is_text_widget(w))
    {
        Gtk::TextView *view = dynamic_cast<Gtk::TextView*>(w);
        Glib::RefPtr<Gtk::TextBuffer> buffer = view->get_buffer();
        view->scroll_to( buffer->get_insert() );
    }
}

bool
MotionAction::execute_as_subcommand(Gtk::Widget *w, ViActionContext *context)
{
    //
    //  If waiting for a motion, then extend selection will be
    //  true. The selection is how the command waiting for the 
    //  motion will know what to act upon.
    //
    m_ext_sel = true;

    perform_motion(w, context->get_count(), context->get_param());

    if (context)
    {
        ExecutableAction *a = context->get_action();
        a->execute(w, context->get_count(), context->get_param()); 
    }

    //
    // If not in visual mode, then the extend selection was to
    // allow other commands to know the range to act on. Remove the 
    // selection now.
    //
    if (m_ext_sel && m_vi->get_mode() != vi_visual && is_text_widget(w))
    {
        Gtk::TextView *view = dynamic_cast<Gtk::TextView*>(w); 
        Glib::RefPtr<Gtk::TextBuffer> buffer = view->get_buffer();

        Gtk::TextBuffer::iterator it = get_cursor_iter( buffer ); 
        buffer->place_cursor(it);
    } 
    return true;
}

