#include "ViActions.h"
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
        Gtk::TextView *view = dynamic_cast<Gtk::TextView*>(w);
        Glib::RefPtr<Gtk::TextBuffer> buffer = view->get_buffer();
        view->scroll_to( buffer->get_insert() );
    }
}

bool
MotionAction::execute_as_subcommand(Gtk::Widget *w, ViActionContext *context)
{
    //
    //  If waiting for a motion, then extend selection will be
    //  true. The selection is how the command waiting for the 
    //  motion will know what to act upon.
    //
    m_ext_sel = true;

    perform_motion(w, context->get_count(), context->get_param());

    if (context)
    {
        ExecutableAction *a = context->get_action();
        a->execute(w, context->get_count(), context->get_param()); 
    }

    //
    // If not in visual mode, then the extend selection was to
    // allow other commands to know the range to act on. Remove the 
    // selection now.
    //
    if (m_ext_sel && m_vi->get_mode() != vi_visual && is_text_widget(w))
    {
        Gtk::TextView *view = dynamic_cast<Gtk::TextView*>(w); 
        Glib::RefPtr<Gtk::TextBuffer> buffer = view->get_buffer();

        Gtk::TextBuffer::iterator it = get_cursor_iter( buffer ); 
        buffer->place_cursor(it);
    } 
    return true;
}
