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

bool is_source_view( Gtk::Widget *w )
{
   const gchar *type = G_OBJECT_TYPE_NAME(w->gobj());

   if ( strcmp(type, "gtkmm__GtkSourceView") == 0 ) 
   {
        return true;
   }
   return false;
}

ViTextIter
get_cursor_iter( Glib::RefPtr<Gtk::TextBuffer> buffer )
{
    Glib::RefPtr< Gtk::TextBuffer::Mark > cursor;

    cursor = buffer->get_insert();
    return ViTextIter(buffer->get_iter_at_mark( cursor ));
}

Glib::ustring
get_selected_text( Glib::RefPtr<Gtk::TextBuffer> buffer )
{
    Gtk::TextIter start, end;
    if (buffer->get_selection_bounds( start, end ))
    {
        return buffer->get_text(start, end); 
    }
    else
    {
        gunichar ch = get_cursor_iter(buffer).get_char();
        return Glib::ustring(1, ch);
    }
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

void set_cursor_at_line( Glib::RefPtr<Gtk::TextBuffer> buffer, int line, bool ext_sel )
{
    Gtk::TextBuffer::iterator iter = 
                 buffer->get_iter_at_line( line - 1 );

    set_cursor( iter, ext_sel );
}
