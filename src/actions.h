#ifndef ACTIONS_H
#define ACTIONS_H

#include "ViKeyManager.h"
#include "ViActions.h"


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

class KeySequenceAction : public ExecutableAction {
    public:
        KeySequenceAction( ViKeyManager *vi, const Glib::ustring &keys ) :
            ExecutableAction( vi ),
            m_keys( keys )
        {
        }

        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params);

    protected:
        Glib::ustring m_keys;
};

class InsertLineAction : public ExecutableAction {
    public:
        InsertLineAction( ViKeyManager *vi ) :
            ExecutableAction( vi )
        {
        }

        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params);
};

class FindAction : public MotionAction {
    public:
        FindAction( ViKeyManager *vi, bool forward ) :
            MotionAction( vi, await_param )
        {
            if (forward)
                m_dir = GTK_DIR_RIGHT;
            else
                m_dir = GTK_DIR_LEFT;
        }

        virtual void perform_motion(Gtk::Widget *w, int count_modifier, Glib::ustring &params);

    protected:
        GtkDirectionType m_dir;
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

class MatchBracketAction : public MotionAction {
    public:
        MatchBracketAction( ViKeyManager *vi ) :
            MotionAction( vi )
        {
        }

        virtual void perform_motion(Gtk::Widget *w, int count_modifier, Glib::ustring &params);
};

class SearchWordUnderCursorAction : public MotionAction {
    public:
        SearchWordUnderCursorAction( ViKeyManager *vi, bool forward = true ) :
            MotionAction( vi ), m_forward(forward)
        {
        }

        virtual void perform_motion(Gtk::Widget *w, int count_modifier, Glib::ustring &params);

    protected:
        bool m_forward;
};

class SwapCaseAction : public ExecutableAction {
    public:
        SwapCaseAction( ViKeyManager *vi ) :
            ExecutableAction( vi )
        {
        }

        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params);
};

class WordMotionAction : public MotionAction {
    public:
        WordMotionAction( ViKeyManager *vi, bool forward ) :
            MotionAction( vi ),
            m_forward( forward )
        {
        }

        virtual void perform_motion(Gtk::Widget *w, int count_modifier, Glib::ustring &params);

    protected:
        bool m_forward;
};

class MaxToggleAction : public ExecutableAction {
    public:
        MaxToggleAction( ViKeyManager *vi ) :
            ExecutableAction( vi )
        {
        }
        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params);
};

class FullscreenAction : public ExecutableAction {
    public:
        FullscreenAction( ViKeyManager *vi ) :
            ExecutableAction( vi )
        {
        }
        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params);
};
#endif
