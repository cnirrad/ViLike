#ifndef VI_NORMAL_MODE_H
#define VI_NORMAL_MODE_H

#include "Vi.h"

//
// Forward declarations
//
class MotionAction;
class ViActionContext;


class ViNormalMode : public ViModeHandler
{
    public:
        ViNormalMode( ViKeyManager *vi );
        virtual ~ViNormalMode(); 

        void enter_mode( ViMode from_mode ); 
        void exit_mode( ViMode to_mode );
        bool handle_key_press( const Glib::ustring &key_str );

        void map_key( const Glib::ustring &widet_type, 
                      const Glib::ustring &key,
                      ExecutableAction *a );
        /**
         * Resets the key sequence buffer (m_key) to blank and the 
         * count modifier (m_count) to zero.
         */
        void clear_key_buffer( unsigned char flags = 0x00 );

    protected:
        ViKeyManager *m_vi;
        ViActionContext *m_context;

        Glib::ustring m_key;
        int m_count;

        WidgetToKeyActionMap m_keyMap;

        Gtk::Widget *focused;
};

#endif
