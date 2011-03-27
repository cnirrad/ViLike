#ifndef ACTIONS_H
#define ACTIONS_H

#include "ViKeyManager.h"
#include "ViMotionAction.h"

/**
 *  Cursor movement commands.
 */
void move_cursor( GtkMovementStep step, gint count );


void change_mode( ViMode mode );

/**
 *  Sets the text view to replacement mode.
 */
void set_replace_mode();

/**
 *  Sets the register to use.
 */
void choose_register();

/**
 *  Deletes the highlighted text.
 */
void delete_text();

/**
 *  Deletes character under the cursor.  Accepts a count to delete
 *  that many characters from the cursor on.
 */
void delete_char(Direction dir);

/**
 *  Change the highlighted text.
 */
void change_text();

/**
 *  Create a mark.
 */
void create_mark();

/**
 *  Moves the cursor to the chosen mark.
 */
void goto_mark();

/**
 *  Moves the cursor to a specific line in the file. A -1 will move 
 *  the cursor to the end of the buffer.
 */
void goto_specific_line( int line );

void goto_line();

/**
 *  Executes a sequence of key presses.
 */
void execute_vi_key_sequence( Glib::ustring keys );

/**
 *  Executes a sequence of key presses. This version executes
 *  the keys from the command params.
 */
void execute_key_sequence_from_params();

/**
 *  Finds the next character (which is given as a action parameter) if it is on
 *  the same line as the cursor.
 */
void find_char(GtkDirectionType dir);

void undo();

void redo();

/**
 *  When the cursor is on a bracket/brace/paren it will find
 *  its match.
 */
void match_brace();

/**
 *  Searches for the word under the cursor.
 */
void search_word_under_cursor(Direction dir);

/**
 *  Swaps the case for the character under the cursor (or the
 *  highlighted region).
 */
void swap_case();

/**
 *  Moves the cursor to the start of the next (or previous if 
 *  dir == Backward) word.
 */
void move_by_word( Direction dir );

/**
 *  Toggles the given window state (i.e., fullscreen, maximized, iconified)
 */
void toggle_window_state( Gdk::WindowState state );

/**
 *  Goes to the next tab in a Gtk::Notebook
 */
void next_tab( Direction dir );

/**
 *  Quits the application.
 */
void application_quit();

/**
 *  Closes the file that currently has focus.
 */
void close_current_file();

/**
 *  Yanks the selected text into the chosen register (or the
 *  default register if none chosen.
 */
void yank_text();

/**
 *  Select an entire line and yank it. The line will be deleted
 *  if del is true.
 */
void yank_line( bool del );

/**
 *  Put (paste) text from chosen register (or the default register
 *  if none chosen. The direction given will indicate if the text
 *  will be placed before or after the cursor (or on the previous 
 *  or next line for linewise data).
 */
void put_text(Direction dir);

/**
 *  Move the focus to the widget in the given direction.
 */
void change_focus( Gtk::DirectionType dir );

void open_file();

void close_file();

#endif
