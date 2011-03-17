#ifndef SOURCERER_REPL_H
#define SOURCERER_REPL_H

#include <gtkmm.h>

class ReplWindow : public Gtk::VBox
{
    public:
        ReplWindow();
        virtual ~ReplWindow();


    protected:
        void append_text(Glib::ustring txt);
        void evaluate(Glib::ustring &expression);

        virtual bool on_key(GdkEventKey *e);

        Glib::ustring get_current_expression();
        void set_prompt();

        Gtk::TextView m_repl;
        Glib::RefPtr<Gtk::TextBuffer> m_buffer;
        Glib::RefPtr<Gtk::TextTag> m_prompt_tag;

        Gtk::ScrolledWindow m_scrollView;

};

#endif
