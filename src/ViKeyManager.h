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

enum ViOperatorScope
{
    vi_linewise,
    vi_characterwise
};

class ViKeyManager;
class ViActionContext;

enum Flags {
    await_motion = 0x01,
    await_param = 0x02,
    no_reset_cur_reg = 0x04,
    is_motion = 0x08
};

/**
 * Abstract base class of all actions.
 */
class ExecutableAction {
    public:
        ExecutableAction(ViKeyManager *vi) : 
            m_flags(0),
            m_vi(vi)
        {
        }
        ExecutableAction(ViKeyManager *vi, unsigned char flags) : 
            m_flags(flags),
            m_vi(vi)
        {
        }

        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params) = 0;

        unsigned char m_flags;
        ViKeyManager *m_vi;

};

class MotionAction : public ExecutableAction {
    public:
        MotionAction(ViKeyManager *vi) : 
            ExecutableAction(vi, is_motion),
            m_ext_sel(false)
        {
        }

        MotionAction(ViKeyManager *vi, unsigned char flags) :
            ExecutableAction(vi, is_motion | flags)
        {
        }

        bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params);
        bool execute_as_subcommand(Gtk::Widget *w, ViActionContext *context);

        virtual void perform_motion(Gtk::Widget *w, int count_modifier, Glib::ustring &param) = 0;

    protected:
        gboolean m_ext_sel;
};

class ViUserMessageArea
{
    public:
        virtual void show_message( const Glib::ustring &msg ) = 0;
        virtual void show_error( const Glib::ustring &err ) = 0;
};

class ViActionContext {
    public:
        ViActionContext()
        {
            reset();
        }

        void execute(Gtk::Widget *w);

        void reset();

        ExecutableAction* get_action() { return m_action; }
        void set_action(ExecutableAction *action) 
        { 
            m_action = action; 
            m_flags = action->m_flags;
        }

        MotionAction* get_motion() { return m_motion; }
        void set_motion(MotionAction *motion) 
        { 
            m_motion = motion; 
            m_flags = m_flags ^ await_motion;
            m_flags = m_flags | motion->m_flags;
        }

        int get_count() { return m_count; }
        void set_count( int count ) { m_count = count; }

        Glib::ustring& get_param() { return m_param; }
        void set_param( Glib::ustring p ) 
        { 
            m_param = p; 
            m_flags = m_flags ^ await_param;
        }

        unsigned char get_flags() { return m_flags; }
        void set_submode(unsigned char f) { m_flags = f; }

    protected:
        ExecutableAction *m_action;
        MotionAction *m_motion;
        unsigned int m_count;
        Glib::ustring m_param;
        unsigned char m_flags;
};

typedef std::map<Glib::ustring, ExecutableAction*> KeyMap;
typedef std::pair<Glib::ustring, ExecutableAction*> KeyMapPair;

struct KeyMapping
{
    guint keyval;
    const char * string;
};

const KeyMapping vi_key_map[] = {
    { GDK_Escape,           "Esc" },
    { GDK_BackSpace,        "BS" },
    { GDK_Tab,              "Tab" },
    { GDK_Linefeed,         "LF" },
    { GDK_Return,           "CR" },
    { GDK_space,            "Space" },
    { GDK_Page_Up,          "PgUp" },
    { GDK_Page_Down,        "PgDn" },
    { GDK_Home,             "Home" },
    { GDK_End,              "End" },
    { GDK_Up,               "Up" },
    { GDK_Down,             "Down" },
    { GDK_Left,             "Left" },
    { GDK_Right,            "Right" }
};
const unsigned int vi_key_map_lgth = sizeof( vi_key_map ) / sizeof( vi_key_map[0] );

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

        /**
         * Translates a keypress.
         */
        virtual Glib::ustring key_to_str(GdkEventKey *event);
        virtual GdkEventKey* str_to_key(const Glib::ustring &str);

        /**
         * Resets the key sequence buffer (m_key) to blank and the 
         * count modifier (m_count) to zero.
         */
        void clear_key_buffer( unsigned char flags = 0x00 );

        char get_current_register();
        void set_current_register( char reg );

        Glib::ustring get_register( char reg );
        void set_register( char reg, Glib::ustring text );

        ViActionContext* get_context() const;

        /**
         * Gets the current ViMode
         */
        ViMode get_mode() const;

        /** 
         * Sets the current ViMode
         */
        void set_mode( ViMode mode, Gtk::Widget *w = 0);

        int get_count() const { return m_count; }

    private:
        ViMode m_mode;
        Gtk::Window *m_window;

        Glib::ustring m_key;
        int m_count;
        char m_current_register;       // the register to use for the next operation. (0x00 means no register) 
        ViActionContext *m_context;       

        KeyMap m_normalMap;
        KeyMap m_insertMap;

        std::map<char, Glib::ustring> m_registers;

        Glib::ustring m_last_pattern;

        ViUserMessageArea *m_msg_area;
};


#endif
