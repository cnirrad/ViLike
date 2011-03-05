#ifndef VI_INSERT_MODE_H
#define VI_INSERT_MODE_H

#include "Vi.h"

class ViInsertMode : public ViModeHandler
{
    public:
        ViInsertMode();
        virtual ~ViInsertMode();
        
        void enter_mode( ViMode from_mode ); 
        void exit_mode( ViMode to_mode );
        bool handle_key_press( const Glib::ustring &key_str );

        void map_key( const Glib::ustring &widet_type, 
                      const Glib::ustring &key,
                      ExecutableAction *a );

    protected:
        WidgetToKeyActionMap m_keyMap;
};

#endif
