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
    m_editors[editor->get_filepath()] = editor;    
    int idx = append_page(*editor);
    editor->show_all();
    set_current_page(idx);
}
