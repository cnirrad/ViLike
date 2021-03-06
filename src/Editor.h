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

        virtual bool open(const Glib::ustring &path) = 0;

        virtual bool save() = 0;
        virtual bool save_as();

        virtual bool search( const Glib::ustring &pattern,
                             Direction direction,
                             bool ext_sel = false ) = 0;

        Glib::RefPtr< Gio::File > get_file() 
        {
            return m_file;
        }

    protected:
        Glib::RefPtr< Gio::File > m_file;
};


class SourceEditor : public Editor, public Gtk::VBox
{
    public:
        SourceEditor();

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

        SearchSupport m_search;
};

#endif
