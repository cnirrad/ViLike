#include <iostream>
#include "ViKeyManager.h"

#define MODIFIER_MASK = GDK_CONTROL_MASK &&     \
                        GDK_MOD1_MASK    &&     \
                        GDK_MOD2_MASK    &&     \
                        GDK_MOD3_MASK    &&     \
                        GDK_MOD4_MASK    &&     \
                        GDK_MOD5_MASK

#define IS_MODIFIER_KEY( sym )                              \
       ( sym == GDK_Alt_L || sym == GDK_Alt_R ||            \
         sym == GDK_Control_L || sym == GDK_Control_R ||    \
         sym == GDK_Meta_L || sym == GDK_Meta_R ||          \
         sym == GDK_Super_L || sym == GDK_Super_R ||        \
         sym == GDK_Hyper_L || sym == GDK_Hyper_R )

ViKeyManager::ViKeyManager(Gtk::Window *w) :
    m_mode(vi_normal),
    m_window(w)
{
    
}

ViKeyManager::~ViKeyManager()
{
}

bool ViKeyManager::map_key(ViMode mode, const char *key, ViCallback cb)
{
    return true;
}


bool ViKeyManager::on_key_press( GdkEventKey *event )
{

    if ((event->state & GDK_MOD1_MASK) == GDK_MOD1_MASK)
    {
        g_print("Alt!");
    }

    Gtk::Widget *w = m_window->get_focus();

    if (m_mode == vi_insert)
    {
        if (event->keyval == GDK_Escape)
        {
            m_mode = vi_normal;
            return false;
        }
        if (IS_MODIFIER_KEY( event->keyval ))
        {
            g_print("Ignoring Modifier\n");
            return true;
        }

        gboolean ret_val;
        gtk_signal_emit_by_name( (GtkObject*)w->gobj(), 
                                 "key-press-event", 
                                 event,
                                 &ret_val );
        return false;
    }

    // TODO: Don't hardcode this
    if (event->length > 0 && event->string[0] == 'i')
    {
        m_mode = vi_insert;
    } 

    if (is_text_widget( w ))
    {

    }
    return true;
}

bool ViKeyManager::on_key_release( GdkEventKey *event)
{
    return true;
}

Glib::ustring ViKeyManager::key_to_str( GdkEventKey *event )
{
    Glib::ustring key_str;

    if (event->keyval > GDK_space && event->keyval <= GDK_asciitilde)
    {
        key_str = (char)event->keyval;
    }
    else if (event->keyval == GDK_BackSpace)
    {
        key_str = "BS";
    }
    else if (event->keyval == GDK_Tab)
    {
        key_str = "Tab";
    }
    else if (event->keyval == GDK_Linefeed)
    {
        key_str = "LF";
    }
    else if (event->keyval == GDK_Return)
    {
        key_str = "CR";
    }
    else if (event->keyval == GDK_Escape)
    {
        key_str = "Esc";
    }
    else if (event->keyval == GDK_space)
    {
        key_str = "Space";
    }
    else if (event->keyval >= GDK_F1 && event->keyval <= GDK_F35)
    {
        int num = event->keyval - (GDK_F1 - 1);
        key_str = "F" + Glib::ustring::format(num);
    }
    else
    {
        key_str = "0x" + Glib::ustring::format(std::hex,  event->keyval);
    }

    //
    //
    //
    if ((event->state & GDK_MOD1_MASK) == GDK_MOD1_MASK)
    {
        key_str = "A-" + key_str;
    }
    
    if ((event->state & GDK_CONTROL_MASK) == GDK_CONTROL_MASK)
    {
        key_str = "C-" + key_str;              
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
                           
//
//  Protected
//
bool ViKeyManager::is_text_widget( Gtk::Widget *w ) const
{
   const gchar *type = G_OBJECT_TYPE_NAME(w->gobj());

   if (strcmp(type, "gtkmm__GtkTextView") == 0) 
   {
        return true;
   }
   return false;
}


