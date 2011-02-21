#ifndef ACTIONS_H
#define ACTIONS_H

#include "ViKeyManager.h"


class MovementAction : public KeyActionBase {
    public:
        MovementAction( GtkMovementStep step, gint count, gboolean ext_sel = false );

        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params);

    protected:
        GtkMovementStep m_step;
        gint m_count;
        gboolean m_ext_sel;
};

class ModeAction : public KeyActionBase {
    public:
        ModeAction( ViKeyManager *vi, ViMode mode );
        ModeAction( MovementAction *act, ViKeyManager *vi, ViMode mode );

        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params);

    protected:
        ViKeyManager *m_vi;
        ViMode m_mode;
        MovementAction *m_move_act;
};

class ReplaceAction : public KeyActionBase {
    public:
        ReplaceAction( ViKeyManager *vi ) : m_vi(vi) 
        {
        }

        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params);

    protected:
        ViKeyManager *m_vi;
};

class ChooseRegistryAction : public KeyActionBase {
    public:
        ChooseRegistryAction( ViKeyManager *vi );

        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params);
    protected:
        ViKeyManager *m_vi;
};

#endif
