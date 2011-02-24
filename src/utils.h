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

/**
 * Sets the cursor position to location. If ext_sel is true, 
 * a selection will be made from the current cursor location
 * to the given location.
 */
void set_cursor( Gtk::TextBuffer::iterator location, bool ext_sel );

void set_cursor_at_line( Glib::RefPtr<Gtk::TextBuffer> buffer, int line, bool ext_sel );

#endif
