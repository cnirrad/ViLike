#include "utils.h"

bool is_text_widget( Gtk::Widget *w ) 
{
   const gchar *type = G_OBJECT_TYPE_NAME(w->gobj());

   if (strcmp(type, "gtkmm__GtkTextView") == 0 ||
       strcmp(type, "gtkmm__GtkSourceView") == 0) 
   {
        return true;
   }
   return false;
}



Gtk::TextBuffer::iterator 
get_cursor_iter( Glib::RefPtr<Gtk::TextBuffer> buffer )
{
    Glib::RefPtr< Gtk::TextBuffer::Mark > cursor;

    cursor = buffer->get_insert();
    return buffer->get_iter_at_mark( cursor );
}

void set_cursor( Gtk::TextBuffer::iterator location, bool ext_sel )
{
        Glib::RefPtr<Gtk::TextBuffer> buffer = location.get_buffer();
        if ( ext_sel )
        {
            Glib::RefPtr< Gtk::TextBuffer::Mark > sel_bound =
                                buffer->get_selection_bound();
            buffer->move_mark( sel_bound, location );
        }
        else 
        {
            buffer->place_cursor( location );
        }
}
