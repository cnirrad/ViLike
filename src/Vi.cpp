#include "Vi.h"

#include "App.h"

Glib::ustring key_to_str( GdkEventKey *event )
{
    Glib::ustring key_str;
    bool use_shift = false;

    if ( event->keyval == GDK_Shift_L || event->keyval == GDK_Shift_R )
    {
        //
        // Ignore the shift key
        //
        return "";
    }
    if (event->keyval > GDK_space && event->keyval <= GDK_asciitilde)
    {
        key_str = (char)event->keyval;
    }
    else if (event->keyval >= GDK_F1 && event->keyval <= GDK_F35)
    {
        int num = event->keyval - (GDK_F1 - 1);
        key_str = "F" + Glib::ustring::format(num);
        use_shift = true;
    }
    else if (event->keyval == GDK_ISO_Left_Tab)
    {
        key_str = "<S-C-Tab>";
        return key_str;
    }
    else
    {
        for (int i = 0; i < vi_key_map_lgth; ++i)
        {
            if ( vi_key_map[i].keyval == event->keyval )
            {
                key_str = vi_key_map[i].string;
                use_shift = true;
                break;
            }
        }
    }

    if ( key_str == "" ) 
    {
        key_str = "0x" + Glib::ustring::format(std::hex,  event->keyval);
    }

    //
    // Mod1 = Alt
    //
    if ((event->state & GDK_MOD1_MASK) == GDK_MOD1_MASK)
    {
        key_str = "A-" + key_str;
    }
    
    if ((event->state & GDK_CONTROL_MASK) == GDK_CONTROL_MASK)
    {
        key_str = "C-" + key_str;              
    }

    if (use_shift && (event->state & GDK_SHIFT_MASK) == GDK_SHIFT_MASK)
    {
        key_str = "S-" + key_str;
    }

    //
    // Surround with < > if not a single key character.
    //
    if (key_str.length() > 1)
    {
        key_str = "<" + key_str + ">";
    }
    return key_str;
}

GdkEventKey* str_to_key(const Glib::ustring &str)
{
    GdkEventKey* event = (GdkEventKey*)gdk_event_new(GDK_KEY_PRESS);
    event->window = Application::get()->get_main_window()->get_window()->gobj();
    event->send_event = TRUE;
    event->time = GDK_CURRENT_TIME;
    event->state = GDK_KEY_PRESS_MASK;

    if (str.length() == 0)
        return NULL;

    if (str.length() == 1)
    {
        event->keyval = str[0];
        return event;
    }

    if (str[0] == '<' && str[str.length()-1] == '>')
    {
        Glib::ustring keyname = str.substr( 1, str.length() - 2 );
        for (int idx = 0; idx < vi_key_map_lgth; idx++)
        {
            if (keyname == vi_key_map[idx].string)
            {
                event->keyval = vi_key_map[idx].keyval;
                break;
            }
        }
    }

    if (event->keyval == 0)
    {
        g_print("ERROR: str_to_key mapping not found for %s\n", str.data());
        return NULL;
    }

    GdkKeymapKey* keys;
    gint n_keys;
    gdk_keymap_get_entries_for_keyval(NULL,
                                      event->keyval,
                                      &keys,
                                      &n_keys);
    event->hardware_keycode = keys[0].keycode;
    event->group = keys[0].group;

    return event;
}

ExecutableAction* lookup(WidgetToKeyActionMap &map, 
                         const Glib::ustring &widget_type, 
                         const Glib::ustring &key)
{
    KeyActionMap *act_map = map[widget_type];

    if (act_map)
    {
        ExecutableAction *a = (*act_map)[key];
        if (a)
            return a;
    }

    act_map = map["*"];
    if (!act_map)
    {
        g_print("No map found for %s or *\n", widget_type.data());
        return NULL;
    }

    ExecutableAction *a = (*act_map)[key];
    if (!a)
    {
        g_print("Could not find action for %s", key.data());
    } 

    return a;
}


void ExecutableAction::execute()
{
    m_action->activate();
}

