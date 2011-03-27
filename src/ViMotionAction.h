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
        MotionAction(Glib::RefPtr< Gtk::Action > action) : 
            ExecutableAction(action, is_motion),
            m_ext_sel(false)
        {
        }

        MotionAction(Glib::RefPtr< Gtk::Action > action, unsigned char flags) :
            ExecutableAction(action, is_motion | flags)
        {
        }

        void execute();
        void execute_as_subcommand( ExecutableAction *act );


    protected:
        bool m_ext_sel;
};
#endif
