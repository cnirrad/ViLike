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

    m_sourceView.set_name("Test");
    m_sourceView.set_source_buffer(
            gtksourceview::SourceBuffer::create(Glib::RefPtr<Gtk::TextTagTable>()) );
    m_sourceView.set_show_line_numbers( true );
    m_sourceView.set_overwrite( true );

    Glib::RefPtr< gtksourceview::SourceBuffer > buffer = 
                            m_sourceView.get_source_buffer();
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

    buffer->set_text(contents);
    set_cursor_at_line( buffer, 1, false );

    m_scrollView.add(m_sourceView);
    m_scrollView.set_policy(Gtk::POLICY_AUTOMATIC, 
                            Gtk::POLICY_AUTOMATIC);

    vi = new ViKeyManager(this);

    add(m_scrollView);
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

