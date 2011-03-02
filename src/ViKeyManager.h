#ifndef VI_KEY_MANAGER_H
#define VI_KEY_MANAGER_H

#include <gtkmm.h>

class ExecutableAction;
class MotionAction;

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

        MotionAction* get_motion() { return m_motion; }

        int get_count() { return m_count; }
        void set_count( int count ) { m_count = count; }

        void set_action(ExecutableAction *action);
        void set_motion(MotionAction *motion);

        Glib::ustring& get_param() { return m_param; }
        void set_param( Glib::ustring p );

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
        ViActionContext *m_context;       

        KeyMap m_normalMap;
        KeyMap m_insertMap;

        char m_current_register;       // the register to use for the next operation. (0x00 means no register) 
        std::map<char, Glib::ustring> m_registers;

        ViUserMessageArea *m_msg_area;
};


#endif
