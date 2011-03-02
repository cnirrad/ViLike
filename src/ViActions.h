#ifndef SOURCERER_VI_ACTIONS_H
#define SOURCERER_VI_ACTIONS_H

#include <gtkmm.h>

//
//  Forward declarations
//
class ViKeyManager;
class ViActionContext;


enum ActionFlags {
    await_motion = 0x01,
    await_param = 0x02,
    no_reset_cur_reg = 0x04,
    is_motion = 0x08
};

/**
 * Abstract base class of all actions.
 */
class ExecutableAction {
    public:
        ExecutableAction(ViKeyManager *vi) : 
            m_flags(0),
            m_vi(vi)
        {
        }
        ExecutableAction(ViKeyManager *vi, unsigned char flags) : 
            m_flags(flags),
            m_vi(vi)
        {
        }

        virtual bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params) = 0;

        unsigned char m_flags;
        ViKeyManager *m_vi;

};

class MotionAction : public ExecutableAction {
    public:
        MotionAction(ViKeyManager *vi) : 
            ExecutableAction(vi, is_motion),
            m_ext_sel(false)
        {
        }

        MotionAction(ViKeyManager *vi, unsigned char flags) :
            ExecutableAction(vi, is_motion | flags)
        {
        }

        bool execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params);
        bool execute_as_subcommand(Gtk::Widget *w, ViActionContext *context);

        virtual void perform_motion(Gtk::Widget *w, int count_modifier, Glib::ustring &param) = 0;

    protected:
        bool m_ext_sel;
};
#endif
