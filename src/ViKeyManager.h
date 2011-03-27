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

        ViRegisterValue get_register( char reg );
        void set_register( char reg, Glib::ustring text );
        void set_register( char reg, Glib::ustring text, ViOperatorScope scope );

        /**
         * Gets the current ViMode
         */
        ViMode get_mode() const;

        Glib::ustring get_last_key() const;

        /**
         *  Retrieves the count modifier. This can be used by a 
         *  function that implements a given action.
         */
        int get_cmd_count();
        
        /**
         *  Retreives the paramaters for the current command, if any.
         */
        Glib::ustring get_cmd_params();
        
        /**
         *  Returns a boolean to indicate if motion commands should
         *  simply move the cursor (false) or extend the selection (true).
         */
        bool get_extend_selection();
        void set_extend_selection(bool ext_sel);

        /** 
         * Sets the current ViMode
         */
        void set_mode( ViMode mode );

        void perfom_last_search();
        void set_last_search( const Glib::ustring &search, Direction d );

    private:
        ViMode m_mode;
        Gtk::Window *m_window;

        char m_current_register;       // the register to use for the next operation. (0x00 means no register) 
        std::map<char, ViRegisterValue> m_registers;

        ViModeHandler *m_handlers[4];

        ViUserMessageArea *m_msg_area;

        Glib::ustring m_last_key;
        Glib::ustring m_last_search;
        Direction m_last_search_direction;

        bool m_ext_selection;
};


#endif
