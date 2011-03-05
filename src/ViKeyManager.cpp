#include <iostream>
#include "ViKeyManager.h"
#include "ViNormalMode.h"
#include "ViInsertMode.h"
#include "ViCommandMode.h"
#include "utils.h"


#define IS_MODIFIER_KEY( sym )                              \
       ( sym == GDK_Alt_L || sym == GDK_Alt_R ||            \
         sym == GDK_Control_L || sym == GDK_Control_R ||    \
         sym == GDK_Meta_L || sym == GDK_Meta_R ||          \
         sym == GDK_Super_L || sym == GDK_Super_R ||        \
         sym == GDK_Hyper_L || sym == GDK_Hyper_R ||        \
         sym == GDK_Shift_L || sym == GDK_Shift_R )

ViKeyManager::ViKeyManager(Gtk::Window *w, ViUserMessageArea *msg_area) :
    m_current_register(0x00),
    m_mode(vi_normal),
    m_msg_area(msg_area),
    m_registers(),
    m_window(w)
{
    m_handlers[vi_normal] = new ViNormalMode(this);
    m_handlers[vi_insert] = new ViInsertMode();
    m_handlers[vi_command] = new ViCommandMode(this);
}

ViKeyManager::~ViKeyManager()
{
    delete m_msg_area;
}

bool ViKeyManager::map_key(ViMode mode, const char *key, ExecutableAction *action )
{
    m_handlers[mode]->map_key( key, action );
    
    return true;
}

bool ViKeyManager::execute( Glib::ustring &cmds )
{
    return execute( cmds, m_mode );    
}

bool ViKeyManager::execute( Glib::ustring &cmds, ViMode mode )
{

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

} 

void ViKeyManager::show_message( const char *format, ... )
{
    va_list args;
    va_start( args, format );

    // Calculate the final length of the formatted string
    int len = vsnprintf( 0, 0, format, args );

    // Allocate a buffer (including room for null termination)
    char* target_string = new char[++len];

    // Generate the formatted string
    vsnprintf( target_string, len, format, args );

    m_msg_area->show_message( target_string );

    // Clean up
    delete [] target_string;
    va_end( args ); 
}

void ViKeyManager::show_error( const char *format, ...)
{
    va_list args;
    va_start( args, format );

    // Calculate the final length of the formatted string
    int len = vsnprintf( 0, 0, format, args );

    // Allocate a buffer (including room for null termination)
    char* target_string = new char[++len];

    // Generate the formatted string
    vsnprintf( target_string, len, format, args );

    m_msg_area->show_error( target_string );

    // Clean up
    delete [] target_string;
    va_end( args ); 
}

bool ViKeyManager::on_key_press( GdkEventKey *event )
{
    if (IS_MODIFIER_KEY( event->keyval ))
    {
        return true;
    }

    m_last_key = key_to_str( event );

    //
    //  Escape key cannot be overridden
    //
    if (event->keyval == GDK_Escape)
    {
        set_mode(vi_normal);
        return true;
    }

    m_handlers[m_mode]->handle_key_press( m_last_key );


    return true;
}

bool ViKeyManager::on_key_release( GdkEventKey *event)
{
    return true;
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

Glib::ustring ViKeyManager::get_last_key() const
{
    return m_last_key;
}

void ViKeyManager::set_mode( ViMode m )
{
    ViModeHandler *old_handler = m_handlers[m_mode];
    ViMode old = m_mode;
    m_mode = m;
    ViModeHandler *new_handler = m_handlers[m_mode];

    old_handler->exit_mode( m_mode );
    new_handler->enter_mode( old );

/*
    if (m_mode == vi_command)
    {
        m_cmd.assign(m_key);
        m_msg_area->show_message( m_cmd );
    }
*/
}


