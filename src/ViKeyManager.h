#ifndef VI_KEY_MANAGER_H
#define VI_KEY_MANAGER_H

#include <gtkmm.h>

#include "Vi.h"

class ExecutableAction;
class MotionAction;


class ViKeyManager;
class ViActionContext;


class ViKeyManager
{
    public:
        ViKeyManager(Gtk::Window *w, ViUserMessageArea *msg_area);
        virtual ~ViKeyManager();

        virtual bool on_key_press( GdkEventKey *event );
        virtual bool on_key_release( GdkEventKey *event);

        /**
         * Adds a key mapping.
         */
        virtual bool map_key(ViMode mode, const char *key, ExecutableAction *cb);

        virtual bool execute( Glib::ustring &cmds );
        virtual bool execute( Glib::ustring &cmds, ViMode mode );

        virtual void show_message( const char *format, ... );
        virtual void show_error( const char *format, ... );

        char get_current_register();
        void set_current_register( char reg );

        Glib::ustring get_register( char reg );
        void set_register( char reg, Glib::ustring text );

        /**
         * Gets the current ViMode
         */
        ViMode get_mode() const;

        Glib::ustring get_last_key() const;

        /** 
         * Sets the current ViMode
         */
        void set_mode( ViMode mode );


    private:
        ViMode m_mode;
        Gtk::Window *m_window;

        char m_current_register;       // the register to use for the next operation. (0x00 means no register) 
        std::map<char, Glib::ustring> m_registers;

        ViModeHandler *m_handlers[4];

        ViUserMessageArea *m_msg_area;

        Glib::ustring m_last_key;
};


#endif
