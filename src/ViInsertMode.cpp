
#include "ViInsertMode.h"

#include "App.h"
#include "utils.h"

ViInsertMode::ViInsertMode()
{
}

ViInsertMode::~ViInsertMode()
{
    m_keyMap.clear();   
}

void ViInsertMode::enter_mode( ViMode from_mode )
{

}


void ViInsertMode::exit_mode( ViMode to_mode )
{
}

bool 
ViInsertMode::handle_key_press( const Glib::ustring &str ) 
{
    Gtk::Widget *w = get_focused_widget();

    const char *type = G_OBJECT_TYPE_NAME(w->gobj());
    Glib::ustring type_str(type);
    ExecutableAction *action = lookup(m_keyMap, type_str, str);

    if (action)
    {
        Glib::ustring params = "";
        action->execute(w, 1, params);
        return true;
    }

    //
    //  Pass the key press on
    //
    GdkEventKey *event = str_to_key( str );

    gboolean ret_val;
    gtk_signal_emit_by_name( (GtkObject*)w->gobj(), 
                             "key-press-event", 
                             event,
                             &ret_val );
    return false;

}

void ViInsertMode::map_key( const Glib::ustring &widget_type, 
                            const Glib::ustring &key,
                            ExecutableAction *a )
{
    KeyActionMap *map = m_keyMap[widget_type];
    if ( !map )
    {
        map = new KeyActionMap();
    }

    (*map)[key] = a;
}
