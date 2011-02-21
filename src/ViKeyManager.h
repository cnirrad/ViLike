#ifndef VI_KEY_MANAGER_H
#define VI_KEY_MANAGER_H

#include <gtkmm.h>

#include "KeyAction.h"

enum ViMode
{
    vi_normal,
    vi_insert,
    vi_command,
    vi_visual
};

enum ViSubMode
{
    vi_sub_none,
    vi_wait_motion,
    vi_wait_param
};

/**
 * Abstract base class of all actions.
 */
class KeyActionBase {
    public:
        KeyActionBase() : m_flags(0) {}
        KeyActionBase(unsigned char flags) : m_flags(flags) {}
        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params) = 0;

        unsigned char m_flags;

    enum Flags {
        await_motion = 0x01,
        await_param = 0x02,
        no_reset_cur_reg = 0x04
    };
};

typedef std::map<Glib::ustring, KeyActionBase*> KeyMap;
typedef std::pair<Glib::ustring, KeyActionBase*> KeyMapPair;

class ViKeyManager
{
    public:
        ViKeyManager(Gtk::Window *w);
        virtual ~ViKeyManager();

        virtual bool on_key_press( GdkEventKey *event );
        virtual bool on_key_release( GdkEventKey *event);

        /**
         * Adds a key mapping.
         */
        virtual bool map_key(ViMode mode, const char *key, KeyActionBase *cb);

        /**
         * Translates a keypress.
         */
        virtual Glib::ustring key_to_str(GdkEventKey *event);

        /**
         * Resets the key sequence buffer (m_key) to blank and the 
         * count modifier (m_count) to zero.
         */
        void clear_key_buffer( unsigned char flags = 0x00 );

        void set_current_register( char reg );

        /** 
         * Sets the current ViMode
         */
        void set_mode( ViMode mode );


    private:
        ViMode m_mode;
        ViSubMode m_submode;
        Gtk::Window *m_window;

        Glib::ustring m_key;
        Glib::ustring m_params;
        int m_count;
        char m_current_register;       // the register to use for the next operation. (0x00 means no register) 
        KeyActionBase *m_action;       // the current action if in a sub mode

        KeyMap m_normalMap;
        KeyMap m_insertMap;

        std::map<char, Glib::ustring> m_registers;
};



#endif
