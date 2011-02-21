
#include <gtkmm.h>
#include "actions.h"
#include "utils.h"


MovementAction::MovementAction( ViKeyManager *vi, GtkMovementStep step, gint count ) :
    MotionAction(vi),
    m_count(count),
    m_step(step)
{
}


bool
MovementAction::execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params)
{
    //
    //  If waiting for a motion, then extend selection will be
    //  true. The selection is how the command waiting for the 
    //  motion will know what to act upon.
    //
    if (m_vi->get_sub_mode() == vi_wait_motion) 
    {
        m_ext_sel = true;
    }
    else
    {
        m_ext_sel = false;
    }

    gboolean ret_val;
    gtk_signal_emit_by_name( (GtkObject*)w->gobj(), 
                             "move-cursor",
                             m_step,
                             m_count * count_modifier,
                             m_ext_sel,
                             &ret_val );

    if (m_vi->get_sub_mode() == vi_wait_motion) 
    {
        KeyActionBase *action = m_vi->get_saved_action();
        if (action)
        {
            action->execute(w, count_modifier, params); 
        }
    }

    //
    // If not in visual mode, then the extend selection was to
    // allow other commands to know the range to act on. Remove the 
    // selection now.
    //
    if (m_ext_sel, m_vi->get_mode() != vi_visual && is_text_widget(w))
    {
        Gtk::TextView *view = dynamic_cast<Gtk::TextView*>(w); 
        Glib::RefPtr<Gtk::TextBuffer> buffer = view->get_buffer();

        Gtk::TextBuffer::iterator it = get_cursor_iter( buffer ); 
        buffer->place_cursor(it);
    } 
    return ret_val;
}

ModeAction::ModeAction( ViKeyManager *vi, ViMode mode ) :
    KeyActionBase(vi),
    m_mode(mode),
    m_move_act(NULL)
{

}

ModeAction::ModeAction( MovementAction *act, ViKeyManager *vi, ViMode mode ) :
    KeyActionBase(vi),
    m_mode(mode),
    m_move_act(act)
{
}

bool
ModeAction::execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params)
{
    if (m_move_act)
    {
        m_move_act->execute(w, count_modifier, params);
    }
    m_vi->set_mode(m_mode);
    m_vi->clear_key_buffer();
    return true;
}

bool
ReplaceAction::execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params)
{
    if (is_text_widget(w))
    {
        Gtk::TextView *view = dynamic_cast<Gtk::TextView*>(w);
        view->set_overwrite(true);
        m_vi->set_mode(vi_insert);        
    }
    return true;
}

bool
ChooseRegistryAction::execute( Gtk::Widget *w, int count_modifier, Glib::ustring &params )
{
    m_vi->set_current_register( params[0] );
    g_print("Set register = %s\n", params.data());
    return true;
}


bool
YankAction::execute( Gtk::Widget *w, int count_modifier, Glib::ustring &params )
{
    Glib::ustring text;
    if (is_text_widget(w))
    {
        Gtk::TextView *view = dynamic_cast<Gtk::TextView*>(w);
        Glib::RefPtr<const Gtk::TextBuffer> buffer = view->get_buffer();

        Gtk::TextIter start;
        Gtk::TextIter end;

        buffer->get_selection_bounds(start, end);
        text = buffer->get_text(start, end);
    }

    char r = m_vi->get_current_register();

    m_vi->set_register(r, text);
    g_print("Set %c with %s\n", r, text.data());
    return true;
}

bool
PutAction::execute( Gtk::Widget *w, int count_modifier, Glib::ustring &params )
{
    char r = m_vi->get_current_register();

    Glib::ustring text = m_vi->get_register(r);


    if (is_text_widget(w))
    {
        Gtk::TextView *view = dynamic_cast<Gtk::TextView*>(w);
        Glib::RefPtr<Gtk::TextBuffer> buffer = view->get_buffer();
        buffer->insert_at_cursor(text);
    }
    return true;
}

bool
DeleteAction::execute( Gtk::Widget *w, int count_modifier, Glib::ustring &params )
{
    Glib::ustring text;
    if (is_text_widget(w))
    {
        Gtk::TextView *view = dynamic_cast<Gtk::TextView*>(w);
        Glib::RefPtr<Gtk::TextBuffer> buffer = view->get_buffer();

        Gtk::TextIter start;
        Gtk::TextIter end;

        buffer->get_selection_bounds(start, end);
        text = buffer->get_text(start, end);

        char r = m_vi->get_current_register();
        m_vi->set_register(r, text);

        g_print("Set %c with %s. Now deleting.\n", r, text.data());

        buffer->erase(start, end);
    }

    return true;
}

