#ifndef SOURCERER_EDITOR_H
#define SOURCERER_EDITOR_H

#include <gtkmm.h>
#include <gtksourceviewmm/sourceview.h>

class Editor 
{
    public:
        virtual bool is_dirty() const = 0;


    protected:
//        SearchSupport m_search;
};


class SourceEditor : public Editor, public Gtk::VBox
{
    public:
        SourceEditor();

        bool is_dirty() const;

        void set_buffer( Glib::RefPtr< gtksourceview::SourceBuffer > buffer );

    protected:
        gtksourceview::SourceView m_sourceView; 
        Glib::RefPtr< gtksourceview::SourceBuffer > m_buffer;
        Gtk::ScrolledWindow m_scrollView;
};

#endif
