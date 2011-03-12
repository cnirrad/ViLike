#ifndef SOURCERER_EDITOR_AREA_H
#define SOURCERER_EDITOR_AREA_H

#include <gtkmm.h>

#include "Editor.h"

class EditorArea : public Gtk::Notebook
{
    public:
        EditorArea();
        virtual ~EditorArea();

        void add_editor(SourceEditor *editor);

    protected:

        std::map<Glib::ustring, SourceEditor*> m_editors;
};

#endif
