#include <iostream>

#include "MainWindow.h"
#include "ViKeyManager.h"


MainWindow::MainWindow() 
{
    set_title("Editor");
    set_border_width(4);
    set_default_size(500, 500);

    m_sourceView.set_name("Test");
    m_scrollView.add(m_sourceView);

//    m_sourceView.signal_key_press_event().connect(
//            sigc::mem_fun(*this, &MainWindow::on_key_press));

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

