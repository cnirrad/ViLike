#ifndef SOURCERER_UTILS_H
#define SOURCERER_UTILS_H

#include <gtkmm.h>

#include "ViTextIter.h"

#define BIT_ON( bit_field, bit )                                \
    (bit_field & bit) == bit

/**
 *  Returns true if widget w is a text view or source view.
 */
bool is_text_widget( Gtk::Widget *w );

bool is_source_view( Gtk::Widget *w );

/** 
 *  Gets an iterator for the current cursor position.
 */
ViTextIter
get_cursor_iter( Glib::RefPtr<Gtk::TextBuffer> buffer );

Glib::ustring
get_selected_text( Glib::RefPtr<Gtk::TextBuffer> buffer );

bool iter_next( Gtk::TextBuffer::iterator &iter, GtkDirectionType dir );

/**
 * Sets the cursor position to location. If ext_sel is true, 
 * a selection will be made from the current cursor location
 * to the given location.
 */
void set_cursor( Gtk::TextBuffer::iterator location, bool ext_sel );

void set_cursor_at_line( Glib::RefPtr<Gtk::TextBuffer> buffer, int line, bool ext_sel );

#endif
