#ifndef VI_COMMAND_MODE_H
#define VI_COMMAND_MODE_H

#include "Vi.h"

class ViCommandMode : public ViModeHandler
{
    public:
        ViCommandMode( ViKeyManager *vi );
        virtual ~ViCommandMode(); 

        void enter_mode( ViMode from_mode ); 
        void exit_mode( ViMode to_mode );
        bool handle_key_press( const Glib::ustring &key_str );

        void map_key( const Glib::ustring &widet_type, 
                      const Glib::ustring &key,
                      ExecutableAction *a );

    protected:
        ViKeyManager *m_vi;

        Glib::ustring m_cmd;

        KeyActionMap m_keyMap;
};

#endif
