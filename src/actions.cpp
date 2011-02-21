
#include <gtkmm.h>
#include "actions.h"
#include "utils.h"


MovementAction::MovementAction( GtkMovementStep step, gint count, gboolean ext_sel ) :
    m_count(count),
    m_ext_sel(ext_sel),
    m_step(step)
{
        
}

bool
MovementAction::execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params)
{
    g_print("move-cursor count_modifier = %i", count_modifier);
    gboolean ret_val;
    gtk_signal_emit_by_name( (GtkObject*)w->gobj(), 
                             "move-cursor",
                             m_step,
                             m_count * count_modifier,
                             m_ext_sel,
                             &ret_val );
    return ret_val;
}

ModeAction::ModeAction( ViKeyManager *vi, ViMode mode ) :
    m_mode(mode),
    m_vi(vi)
{

}

ModeAction::ModeAction( MovementAction *act, ViKeyManager *vi, ViMode mode ) :
    m_mode(mode),
    m_move_act(act),
    m_vi(vi)
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

ChooseRegistryAction::ChooseRegistryAction( ViKeyManager *vi ) : 
    KeyActionBase( await_param | no_reset_cur_reg ),
    m_vi( vi )
{

}

bool
ChooseRegistryAction::execute( Gtk::Widget *w, int count_modifier, Glib::ustring &params )
{
    m_vi->set_current_register( params[0] );
    return true;
}

