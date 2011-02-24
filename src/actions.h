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

class ModeAction : public ExecutableAction {
    public:
        ModeAction( ViKeyManager *vi, ViMode mode );
        ModeAction( MovementAction *act, ViKeyManager *vi, ViMode mode );

        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params);

    protected:
        ViMode m_mode;
        MovementAction *m_move_act;
};

class ReplaceAction : public ExecutableAction {
    public:
        ReplaceAction( ViKeyManager *vi ) : 
            ExecutableAction(vi)
        {
        }

        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params);

};

class ChooseRegistryAction : public ExecutableAction {
    public:
        ChooseRegistryAction( ViKeyManager *vi ) :
            ExecutableAction( vi, await_param )
        {
        }

        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params);
};

class YankAction : public ExecutableAction {
    public:
        YankAction( ViKeyManager *vi ) : 
            ExecutableAction( vi, await_motion )
        {
        }

        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params);

};

class PutAction : public ExecutableAction {
    public:
        PutAction( ViKeyManager *vi ) :
            ExecutableAction( vi )
        {
        }

        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params);

};

class DeleteAction : public ExecutableAction {

    public:
        DeleteAction( ViKeyManager *vi ) : 
            ExecutableAction( vi, await_motion )
        {
        }

        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params);

};

class DeleteOneAction : public ExecutableAction {

    public:
        DeleteOneAction( ViKeyManager *vi, bool before = false ) : 
            ExecutableAction( vi ),
            m_before( before )
        {
        }

        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params);

    protected:
        bool m_before;
};

class ChangeAction : public ExecutableAction {
    public:
        ChangeAction( ViKeyManager *vi ) : 
            ExecutableAction( vi, await_motion )
        {
        }

        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params);

};

class CreateMarkAction : public ExecutableAction {
    public:
        CreateMarkAction( ViKeyManager *vi ) :
            ExecutableAction( vi, await_param )
        {
        }

        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params);
};

class GotoLineAction : public MotionAction {
    public:
        GotoLineAction( ViKeyManager *vi, int hardcoded_line_number = -1 ) :
            MotionAction( vi ),
            m_line(hardcoded_line_number)
        {
        }

        virtual void perform_motion(Gtk::Widget *w, int count_modifier, Glib::ustring &params);
    protected:
        int m_line;
};

class GotoMarkAction : public MotionAction {
    public:
        GotoMarkAction( ViKeyManager *vi ) :
            MotionAction( vi, await_param )
        {
        }

        virtual void perform_motion(Gtk::Widget *w, int count_modifier, Glib::ustring &params);
};

class CompoundAction : public ExecutableAction {
    public:
        CompoundAction( ViKeyManager *vi, ... );

        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params);

    protected:
        std::list<ExecutableAction *> m_actions;
};

class FindAction : public MotionAction {
    public:
        FindAction( ViKeyManager *vi ) :
            MotionAction( vi, await_param )
        {
        }

        virtual void perform_motion(Gtk::Widget *w, int count_modifier, Glib::ustring &params);
};

class UndoAction : public ExecutableAction {
    public:
        UndoAction( ViKeyManager *vi ) :
            ExecutableAction( vi )
        {
        }

        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params);
};

class RedoAction : public ExecutableAction {
    public:
        RedoAction( ViKeyManager *vi ) :
            ExecutableAction( vi )
        {
        }

        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params);
};

#endif
