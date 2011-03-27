#include "ViMotionAction.h"
#include "ViKeyManager.h"
#include "utils.h"

//
//  MotionAction
//
void
MotionAction::execute()
{
    if (get_vi()->get_mode() == vi_visual)
        get_vi()->set_extend_selection( true );
    else
        get_vi()->set_extend_selection( false );

    ExecutableAction::execute();

    Gtk::Widget *w = get_focused_widget();
    
    if (is_text_widget(w))
    {
        Gtk::TextView *view = static_cast<Gtk::TextView*>(w);
        Glib::RefPtr<Gtk::TextBuffer> buffer = view->get_buffer();
        view->scroll_to( buffer->get_insert() );
    }
}

void
MotionAction::execute_as_subcommand( ExecutableAction *a )
{
    //
    //  If waiting for a motion, then extend selection will be
    //  true. The selection is how the command waiting for the 
    //  motion will know what to act upon.
    //
    get_vi()->set_extend_selection( true );

    //
    //  Execute the motion, and then the main command.
    //
    //ExecutableAction::execute();
    m_action->activate();

    if (a)
    {
        a->execute();
    }

    //
    // If not in visual mode, then the extend selection was to
    // allow other commands to know the range to act on. Remove the 
    // selection now.
    //
    Gtk::Widget *w = get_focused_widget();
    if (m_ext_sel && get_vi()->get_mode() != vi_visual && is_text_widget(w))
    {
        Gtk::TextView *view = static_cast<Gtk::TextView*>(w); 
        Glib::RefPtr<Gtk::TextBuffer> buffer = view->get_buffer();

        Gtk::TextBuffer::iterator it = get_cursor_iter( buffer ); 
        buffer->place_cursor(it);
    } 

    get_vi()->set_extend_selection( false );
    return;
}
