#ifndef VI_SOURCERER_H
#define VI_SOURCERER_H

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
    vi_characterwise,
    vi_linewise
};

enum ActionFlags {
    await_motion = 0x01,
    await_param = 0x02,
    no_reset_cur_reg = 0x04,
    is_motion = 0x08
};

enum Direction 
{
    Forward,
    Backward
};

class ExecutableAction;

//
//  Key sequence -> Action mapping
//
typedef std::map<Glib::ustring, ExecutableAction*> KeyActionMap;
typedef std::pair<Glib::ustring, ExecutableAction*> KeyActionPair;

//
//  Widget name -> KeyActionMap mapping
//
typedef std::map<Glib::ustring, KeyActionMap*> WidgetToKeyActionMap;

ExecutableAction* lookup(WidgetToKeyActionMap &map, 
                         const Glib::ustring &widget_type, 
                         const Glib::ustring &key);

struct KeyToStrMapping
{
    guint keyval;
    const char * string;
};

const KeyToStrMapping vi_key_map[] = {
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

GdkEventKey* str_to_key(const Glib::ustring &str);
Glib::ustring key_to_str( GdkEventKey *event );

struct ViRegisterValue
{
    ViOperatorScope scope;
    Glib::ustring text;
};

/**
 *  Interface class for displaying messages to the user.
 */
class ViUserMessageArea
{
    public:
        virtual void show_message( const Glib::ustring &msg ) = 0;
        virtual void show_error( const Glib::ustring &err ) = 0;
};

class ViKeyManager;

/**
 * Abstract base class of all actions.
 */
class ExecutableAction {
    public:
        ExecutableAction(Glib::RefPtr< Gtk::Action > action) : 
            m_flags(0),
            m_action(action)
        {
        }
        ExecutableAction(Glib::RefPtr< Gtk::Action > action, unsigned char flags) : 
            m_flags(flags),
            m_action(action)
        {
        }

        virtual void execute();

        Glib::RefPtr< Gtk::Action > m_action;
        unsigned char m_flags;
};

/**
 *  Interface class for handling a mode.
 */
class ViModeHandler
{
    public:
        virtual void enter_mode( ViMode from_mode ) = 0;
        virtual void exit_mode( ViMode to_mode ) = 0;

        virtual bool handle_key_press( const Glib::ustring &key_str ) = 0;

        virtual void map_key( const Glib::ustring &key, ExecutableAction *a )
        {
            Glib::ustring widget_type("*");
            map_key( widget_type, key, a );
        }

        virtual void map_key( const Glib::ustring &widet_type, 
                              const Glib::ustring &key,
                              ExecutableAction *a ) = 0;

        virtual int get_cmd_count() = 0;
        virtual Glib::ustring get_cmd_params() = 0;
};

#endif
