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

        void execute_command(const Glib::ustring &cmd);

    protected:
        void execute_search(const Glib::ustring &pattern, char begin);

        void add_history(const Glib::ustring &cmd);
        Glib::ustring next_history(Direction d, char begin);

        ViKeyManager *m_vi;
        Glib::ustring m_cmd;
        KeyActionMap m_keyMap;

        std::list<std::string> m_history;
        std::list<std::string>::iterator m_history_it;
};

#endif
