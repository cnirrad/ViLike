#include <iostream>
#include <gtksourceviewmm/sourcelanguagemanager.h>

#include "MainWindow.h"
#include "ViKeyManager.h"
#include "utils.h"

using namespace gtksourceview;

MainWindow::MainWindow() 
{
    set_title("Sourcerer");
    set_border_width(4);
    set_default_size(500, 500);

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

    vi = new ViKeyManager(this);

    add(m_vbox);

    m_vbox.pack_start(m_scrollView, true, true);
    m_vbox.pack_end(m_statusBar, false, false);
    
    show_all_children();
}

MainWindow::~MainWindow() 
{
    delete vi;
}

ViKeyManager* MainWindow::get_key_manager() const
{
    return vi;
}

bool MainWindow::on_key_press_event(GdkEventKey *event)
{
    Gtk::Widget *focused = get_focus();

    std::cout << G_OBJECT_TYPE_NAME(focused->gobj())
              << " MainWindow::on_key_press " 
              << event->hardware_keycode 
              << ", " << event->keyval
              << ", " << vi->key_to_str( event )
              << std::endl;

    return vi->on_key_press( event );
}

