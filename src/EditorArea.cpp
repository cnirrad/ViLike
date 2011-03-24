#include "EditorArea.h"

EditorArea::EditorArea() : Gtk::Notebook()
{
    set_show_tabs(true);
    set_scrollable(true);
}

EditorArea::~EditorArea()
{
}

void EditorArea::add_editor(SourceEditor *editor)
{
    m_editors[editor->get_file()->get_path()] = editor;    
    int idx = append_page(*editor, editor->get_file()->get_basename());
    editor->show_all();
    set_current_page(idx);
}

SourceEditor* 
EditorArea::get_editor_for_file(Glib::ustring &path)
{
    Gtk::Notebook_Helpers::PageList::iterator it;
    for (it = pages().begin(); it != pages().end(); it++)
    {
        Gtk::Widget *w = it->get_child();

        SourceEditor *ed = reinterpret_cast<SourceEditor*>(it->get_child());    
        if (ed->get_file()->get_path() == path)
        {
            return NULL;
        }
    }
    g_print("Editor not found for file %s\n", path.data());
    return NULL;
}

void EditorArea::close_editor(SourceEditor *editor)
{
    remove_page(*editor);
}

