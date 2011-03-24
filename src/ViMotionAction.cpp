#include "ViMotionAction.h"
#include "ViKeyManager.h"
#include "utils.h"

//
//  MotionAction
//
bool
MotionAction::execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params)
{
    if (m_vi->get_mode() == vi_visual)
        m_ext_sel = true;
    else
        m_ext_sel = false;

    perform_motion(w, count_modifier, params);
    
    if (is_text_widget(w))
    {
        Gtk::TextView *view = static_cast<Gtk::TextView*>(w);
        Glib::RefPtr<Gtk::TextBuffer> buffer = view->get_buffer();
        view->scroll_to( buffer->get_insert() );
    }
}

bool
MotionAction::execute_as_subcommand(Gtk::Widget *w, 
                                    ExecutableAction *a,
                                    int count,
                                    Glib::ustring &params)
{
    //
    //  If waiting for a motion, then extend selection will be
    //  true. The selection is how the command waiting for the 
    //  motion will know what to act upon.
    //
    m_ext_sel = true;

    perform_motion(w, count, params);

    if (a)
    {
        a->execute(w, count, params);
    }

    //
    // If not in visual mode, then the extend selection was to
    // allow other commands to know the range to act on. Remove the 
    // selection now.
    //
    if (m_ext_sel && m_vi->get_mode() != vi_visual && is_text_widget(w))
    {
        Gtk::TextView *view = static_cast<Gtk::TextView*>(w); 
        Glib::RefPtr<Gtk::TextBuffer> buffer = view->get_buffer();

        Gtk::TextBuffer::iterator it = get_cursor_iter( buffer ); 
        buffer->place_cursor(it);
    } 
    return true;
}
