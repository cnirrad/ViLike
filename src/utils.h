#ifndef SOURCERER_UTILS_H
#define SOURCERER_UTILS_H

#include <gtkmm.h>

#define BIT_ON( bit_field, bit )                                \
    (bit_field & bit) == bit

/**
 *  Returns true if widget w is a text view or source view.
 */
bool is_text_widget( Gtk::Widget *w );


/**
 *  Gets an iterator for the current cursor position.
 */
Gtk::TextBuffer::iterator 
get_cursor_iter( Glib::RefPtr<Gtk::TextBuffer> buffer );


#endif
