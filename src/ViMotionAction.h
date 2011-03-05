#ifndef SOURCERER_VI_ACTIONS_H
#define SOURCERER_VI_ACTIONS_H

#include <gtkmm.h>

#include "Vi.h"

//
//  Forward declarations
//
class ViKeyManager;

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
        bool execute_as_subcommand(Gtk::Widget *w, 
                                   ExecutableAction *act,
                                   int count,
                                   Glib::ustring &params);

        virtual void perform_motion(Gtk::Widget *w, int count_modifier, Glib::ustring &param) = 0;

    protected:
        bool m_ext_sel;
};
#endif
