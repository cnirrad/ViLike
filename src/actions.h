#ifndef ACTIONS_H
#define ACTIONS_H

#include "ViKeyManager.h"


class MovementAction : public MotionAction {
    public:
        MovementAction( ViKeyManager *vi, GtkMovementStep step, gint count );

        virtual void perform_motion(Gtk::Widget *w, int count_modifier, Glib::ustring &param);

    protected:
        GtkMovementStep m_step;
        gint m_count;
};

class ModeAction : public KeyActionBase {
    public:
        ModeAction( ViKeyManager *vi, ViMode mode );
        ModeAction( MovementAction *act, ViKeyManager *vi, ViMode mode );

        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params);

    protected:
        ViMode m_mode;
        MovementAction *m_move_act;
};

class ReplaceAction : public KeyActionBase {
    public:
        ReplaceAction( ViKeyManager *vi ) : 
            KeyActionBase(vi)
        {
        }

        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params);

};

class ChooseRegistryAction : public KeyActionBase {
    public:
        ChooseRegistryAction( ViKeyManager *vi ) :
            KeyActionBase( vi, await_param )
        {
        }

        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params);
};

class YankAction : public KeyActionBase {
    public:
        YankAction( ViKeyManager *vi ) : 
            KeyActionBase( vi, await_motion )
        {
        }

        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params);

};

class PutAction : public KeyActionBase {
    public:
        PutAction( ViKeyManager *vi ) :
            KeyActionBase( vi )
        {
        }

        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params);

};

class DeleteAction : public KeyActionBase {

    public:
        DeleteAction( ViKeyManager *vi ) : 
            KeyActionBase( vi, await_motion )
        {
        }

        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params);

};

class ChangeAction : public KeyActionBase {
    public:
        ChangeAction( ViKeyManager *vi ) : 
            KeyActionBase( vi, await_motion )
        {
        }

        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params);

};

class CreateMarkAction : public KeyActionBase {
    public:
        CreateMarkAction( ViKeyManager *vi ) :
            KeyActionBase( vi, await_param )
        {
        }

        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params);
};

class GotoMarkAction : public MotionAction {
    public:
        GotoMarkAction( ViKeyManager *vi ) :
            MotionAction( vi, await_param )
        {
        }

        virtual void perform_motion(Gtk::Widget *w, int count_modifier, Glib::ustring &params);
};

class CompoundAction : public KeyActionBase {
    public:
        CompoundAction( ViKeyManager *vi, ... );

        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params);

    protected:
        std::list<KeyActionBase *> m_actions;
};

class FindAction : public KeyActionBase {
    public:
        FindAction( ViKeyManager *vi ) :
            KeyActionBase( vi, await_param )
        {
        }

        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params);
};

#endif
