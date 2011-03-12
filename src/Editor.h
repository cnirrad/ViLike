#ifndef SOURCERER_EDITOR_H
#define SOURCERER_EDITOR_H

#include <gtkmm.h>
#include <gtksourceviewmm/sourceview.h>

#include "Search.h"
#include "Vi.h"

class Editor 
{
    public:
        virtual bool is_dirty() const = 0;

        virtual bool search( const Glib::ustring &pattern,
                             Direction direction,
                             bool ext_sel = false ) = 0;

    protected:
};


class SourceEditor : public Editor, public Gtk::VBox
{
    public:
        SourceEditor();

        Glib::ustring get_filepath() const;

        bool is_dirty() const;

        bool open(const Glib::ustring &path);

        bool save();

        void set_buffer( Glib::RefPtr< gtksourceview::SourceBuffer > buffer );

        bool search( const Glib::ustring &pattern,
                     Direction direction,
                     bool ext_sel = false );

    protected:

        gtksourceview::SourceView m_sourceView; 
        Glib::RefPtr< gtksourceview::SourceBuffer > m_buffer;
        Gtk::ScrolledWindow m_scrollView;

        Glib::ustring m_filepath;

        SearchSupport m_search;
};

#endif
