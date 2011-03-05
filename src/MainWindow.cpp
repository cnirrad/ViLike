#include <iostream>

#include "MainWindow.h"
#include "Vi.h"
#include "ViKeyManager.h"
#include "utils.h"

using namespace gtksourceview;


class MessageArea : public ViUserMessageArea
{
    public:
        MessageArea( Gtk::Statusbar *status ) : m_status(status)
        {
            m_context = m_status->get_context_id( "message_area" );
        }

        void show_message( const Glib::ustring &msg )
        {
            m_status->push( msg, m_context );
        }

        void show_error( const Glib::ustring &err )
        {
            m_status->push( err, m_context );
        }

    protected:
        Gtk::Statusbar *m_status;
        int m_context;

};


MainWindow::MainWindow() 
{
    set_title("Sourcerer");
    set_border_width(4);
    set_default_size(500, 500);


    MessageArea *area = new MessageArea(&m_statusBar);
    vi = new ViKeyManager(this, area);

    add(m_vbox);

    m_editor_area.append_page(m_sourceEditor1);
    m_editor_area.append_page(m_sourceEditor2);
    m_editor_area.append_page(m_sourceEditor3);

    m_vbox.pack_start(m_editor_area, true, true);
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

EditorArea* MainWindow::get_editor_area() 
{
    return &m_editor_area;
}

Gtk::Statusbar*
MainWindow::get_status_bar() 
{
    return &m_statusBar;
}

bool MainWindow::is_fullscreen() const
{
    Gdk::WindowState state = get_window()->get_state();
    
    return ( (state & Gdk::WINDOW_STATE_FULLSCREEN) ==
                Gdk::WINDOW_STATE_FULLSCREEN );
}

bool MainWindow::is_maximized() const
{
    Gdk::WindowState state = get_window()->get_state();
    
    return ( (state & Gdk::WINDOW_STATE_MAXIMIZED) ==
                Gdk::WINDOW_STATE_MAXIMIZED );
}

bool MainWindow::on_key_press_event(GdkEventKey *event)
{
    Gtk::Widget *focused = get_focus();

    if (!focused)
    {
        std::cout << "ERROR: focused = NULL" << std::endl;
        return false;
    }
       

    std::cout << G_OBJECT_TYPE_NAME(focused->gobj())
              << " MainWindow::on_key_press " 
              << event->hardware_keycode 
              << ", " << event->keyval
              << ", " << key_to_str( event )
              << std::endl;

    return vi->on_key_press( event );
}

