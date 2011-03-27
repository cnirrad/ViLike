#include "ViNormalMode.h"

#include "App.h"
#include "utils.h"
#include "ViKeyManager.h"
#include "ViMotionAction.h"

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

ViNormalMode::ViNormalMode(ViKeyManager *vi) :
    m_vi(vi)
{
    m_context = new ViActionContext();
}

ViNormalMode::~ViNormalMode()
{
    delete m_context;
    m_keyMap.clear();
}

void ViNormalMode::enter_mode( ViMode from_mode )
{
    clear_key_buffer();

    Gtk::Widget *w = get_focused_widget();
    if (w && is_text_widget(w))
    {
        Gtk::TextView *view = dynamic_cast<Gtk::TextView*>(w);
        view->set_overwrite(true);
    }
}


void ViNormalMode::exit_mode( ViMode to_mode )
{
    Gtk::Widget *w = get_focused_widget();
    if (w && is_text_widget(w))
    {
        Gtk::TextView *view = dynamic_cast<Gtk::TextView*>(w);
        view->set_overwrite(false);
    }
}

bool 
ViNormalMode::handle_key_press( const Glib::ustring &str )
{
    Gtk::Widget *w = get_focused_widget();

    m_key += str;

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
    if (m_key == str && str[0] >= '0' && str[0] <= '9') 
    {
        //
        //  Count modifier not allowed to start with '0'
        //
        if (!(str == "0" && m_count == 0))
        {
            int num = str[0] - '0';
            m_count = (m_count * 10) + num;
            m_context->set_count(m_count);
            m_key = "";
            return true;
        }
    }

    const char *type = G_OBJECT_TYPE_NAME(w->gobj());
    Glib::ustring type_str(type);
    ExecutableAction *action = lookup(m_keyMap, type_str, m_key);
    if (action)
    {
        if (m_context->get_action() == NULL)
        {
            m_context->set_action( action );
        }
        else if (m_context->get_action() == action)
        {
            //
            //  Double key action, such as 'dd' or 'yy'.
            //
            m_key += m_key;
            action = lookup(m_keyMap, type_str, m_key);
            if (action != NULL)
            {
                m_context->set_action( action );
            }
        }
        else if (BIT_ON(action->m_flags, is_motion))
        {
            MotionAction *motion = static_cast<MotionAction*>(action);
            m_context->set_motion( motion );
        }

        m_context->execute(w);
        clear_key_buffer(action->m_flags);
        return true;
    }
    g_print("No action for key %s\n", m_key.data());
}

void ViNormalMode::map_key( const Glib::ustring &widget_type, 
                            const Glib::ustring &key,
                            ExecutableAction *a )
{
    KeyActionMap *map = m_keyMap[widget_type];
    if ( !map )
    {
        map = new KeyActionMap();
        m_keyMap[widget_type] = map;
    }

    (*map)[key] = a;

    g_print("Mapped normal mode command %s for widget %s\n", key.data(), widget_type.data());
}

bool
wait_to_execute_action( unsigned char flags )
{
    return (flags & ( await_motion | await_param )) > 0;
}

void ViNormalMode::clear_key_buffer( unsigned char flags )
{
    m_key = "";

    if (!wait_to_execute_action( flags ))
    {
        m_context->reset();
        m_count = 0;
    }

    if ( !BIT_ON(flags, no_reset_cur_reg))
        m_vi->set_current_register(0x00);
}

int ViNormalMode::get_cmd_count()
{
    return m_context->get_count();
}

Glib::ustring ViNormalMode::get_cmd_params()
{
    return m_context->get_param();
}

//
//  VI Action Context
//
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
        m_motion->execute_as_subcommand(m_action);
    }
    else if (m_action != NULL)
    {
        m_action->execute();
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

void ViActionContext::set_motion(MotionAction *motion) 
{ 
    m_motion = motion; 
    m_flags = m_flags ^ await_motion;
    m_flags = m_flags | motion->m_flags;
}

void ViActionContext::set_action(ExecutableAction *action) 
{ 
    m_action = action; 
    m_flags = action->m_flags;
}

void ViActionContext::set_param( Glib::ustring p ) 
{ 
    m_param = p; 
    m_flags = m_flags ^ await_param;
}

