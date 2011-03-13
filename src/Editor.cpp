#include <gtksourceviewmm.h>
#include <gtksourceview/gtksourcelanguagemanager.h>

#include "App.h"
#include "Editor.h"
#include "utils.h"

bool
Editor::save_as()
{
    Gtk::FileChooserDialog dialog("Save As...", Gtk::FILE_CHOOSER_ACTION_SAVE );
    dialog.set_transient_for(*Application::get()->get_main_window());

    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button("Select", Gtk::RESPONSE_OK);

    int result = dialog.run();

    switch( result )
    {
        case (Gtk::RESPONSE_OK):
            m_file = dialog.get_file();
            save();
            break;
        case (Gtk::RESPONSE_CANCEL):
            break;
        default:
            break;
    }
}

SourceEditor::SourceEditor()
{
    m_scrollView.add(m_sourceView);
    m_scrollView.set_policy(Gtk::POLICY_AUTOMATIC, 
                            Gtk::POLICY_AUTOMATIC);

    m_sourceView.set_show_line_numbers( true );
    Pango::FontDescription font("monospace");
    m_sourceView.modify_font( font );
    //m_sourceView.set_overwrite( true );


    pack_start(m_scrollView, true, true);
}

bool SourceEditor::is_dirty() const
{
    return false;        
}

bool SourceEditor::open(const Glib::ustring &path)
{
    m_file = Gio::File::create_for_path(path);

    Glib::RefPtr< gtksourceview::SourceLanguageManager > lm = 
        gtksourceview::SourceLanguageManager::get_default();

    Glib::RefPtr<gtksourceview::SourceLanguage> lang = 
        Glib::wrap(gtk_source_language_manager_guess_language(
                    lm->gobj(), path.data(), NULL));

    Glib::RefPtr< gtksourceview::SourceBuffer > buffer;
    if (lang != NULL)
    {
        g_print("Source language: %s\n", lang->get_name().data());

        buffer = gtksourceview::SourceBuffer::create( lang );
    }
    else
    {
        buffer = gtksourceview::SourceBuffer::create( Gtk::TextTagTable::create() );
    }

    if (m_file->query_exists())
    {
        char *contents;
        gsize length;
        if (!m_file->load_contents( contents, length ))
        {
            g_print("Error: Couldn't load file.\n");
            return false;
        }

        buffer->begin_not_undoable_action();
        buffer->set_text(contents);
        set_cursor_at_line( buffer, 1, false );
        buffer->end_not_undoable_action();
    }

    set_buffer( buffer );

    return true;
}

bool SourceEditor::save()
{
    return false;
}

void SourceEditor::set_buffer( Glib::RefPtr< gtksourceview::SourceBuffer > buffer )
{
    m_buffer = buffer;
    m_sourceView.set_source_buffer( buffer );
    m_search.set_buffer( buffer );

    if (buffer->get_language() != NULL)
    {
        buffer->set_highlight_syntax( true );
        buffer->set_highlight_matching_brackets(true);
    }

    buffer->set_max_undo_levels(100);
}

bool SourceEditor::search( const Glib::ustring &pattern,
                           Direction direction,
                           bool ext_sel )
{
    ViTextIter cursor = get_cursor_iter( m_buffer );
    if (m_search.search( pattern, cursor, direction ))
    {
        set_cursor( cursor, ext_sel );
        return true;
    }

    return false;
}

