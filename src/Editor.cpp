#include <gtksourceviewmm/sourceview.h>

#include "Editor.h"
#include "utils.h"

SourceEditor::SourceEditor()
{

    Glib::RefPtr< gtksourceview::SourceBuffer > buffer = 
            gtksourceview::SourceBuffer::create(Glib::RefPtr<Gtk::TextTagTable>());

    m_sourceView.set_source_buffer( buffer );
    m_sourceView.set_show_line_numbers( true );
    m_sourceView.set_overwrite( true );

    buffer->set_highlight_matching_brackets(true);
    buffer->set_max_undo_levels(100);

    Glib::RefPtr< Gio::File > file = Gio::File::create_for_path( 
            "/home/darrin/dev/projects/sourcerer/src/actions.cpp");

    char *contents;
    gsize length;
    if (!file->load_contents( contents, length ))
    {
        g_print("Error: Couldn't load file.\n");
    }

    buffer->begin_not_undoable_action();
    buffer->set_text(contents);
    set_cursor_at_line( buffer, 1, false );
    buffer->end_not_undoable_action();

    m_scrollView.add(m_sourceView);
    m_scrollView.set_policy(Gtk::POLICY_AUTOMATIC, 
                            Gtk::POLICY_AUTOMATIC);


    pack_start(m_scrollView, true, true);
}

bool SourceEditor::is_dirty() const
{
    return false;        
}

void SourceEditor::set_buffer( Glib::RefPtr< gtksourceview::SourceBuffer > buffer )
{
    m_buffer = buffer;
}

