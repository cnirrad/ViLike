#ifndef VI_KEY_MANAGER_H
#define VI_KEY_MANAGER_H

#include <gtkmm.h>

enum ViMode
{
    vi_normal,
    vi_insert,
    vi_command,
    vi_visual
};

typedef sigc::slot3< void, Glib::RefPtr<Gtk::Widget>, int, char > ViCallback; 

class ViKeyManager
{
    public:
        ViKeyManager(Gtk::Window *w);
        virtual ~ViKeyManager();

        virtual bool on_key_press( GdkEventKey *event );
        virtual bool on_key_release( GdkEventKey *event);

        virtual bool map_key(ViMode mode, const char *key, ViCallback cb);

        virtual Glib::ustring key_to_str(GdkEventKey *event);

    protected:
        virtual bool is_text_widget( Gtk::Widget *w ) const;

    private:
        ViMode m_mode;
        Gtk::Window *m_window;

};


#endif
