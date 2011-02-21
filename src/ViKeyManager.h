#ifndef VI_KEY_MANAGER_H
#define VI_KEY_MANAGER_H

#include <gtkmm.h>


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

class ViKeyManager;

/**
 * Abstract base class of all actions.
 */
class KeyActionBase {
    public:
        KeyActionBase(ViKeyManager *vi) : 
            m_flags(0),
            m_vi(vi)
        {
        }
        KeyActionBase(ViKeyManager *vi, unsigned char flags) : 
            m_flags(flags),
            m_vi(vi)
        {
        }

        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params) = 0;

        unsigned char m_flags;
        ViKeyManager *m_vi;


    enum Flags {
        await_motion = 0x01,
        await_param = 0x02,
        no_reset_cur_reg = 0x04,
        is_motion = 0x08
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

        char get_current_register();
        void set_current_register( char reg );

        Glib::ustring get_register( char reg );
        void set_register( char reg, Glib::ustring text );

        KeyActionBase* get_saved_action() const;

        /**
         * Gets the current ViMode
         */
        ViMode get_mode() const;

        /** 
         * Sets the current ViMode
         */
        void set_mode( ViMode mode );

        ViSubMode get_sub_mode() const;

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


class MotionAction : public KeyActionBase {
    public:
        MotionAction(ViKeyManager *vi) : 
            KeyActionBase(vi, is_motion),
            m_ext_sel(false)
            
        {
        }

        MotionAction(ViKeyManager *vi, unsigned char flags) :
            KeyActionBase(vi, is_motion | flags)
        {
        }

        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params);

    protected:
        gboolean m_ext_sel;
};


#endif
