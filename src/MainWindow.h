#ifndef EDITOR_MAIN_WINDOW_H
#define EDITOR_MAIN_WINDOW_H

#include <gtkmm.h>
#include <gtksourceviewmm/sourceview.h>

class ViKeyManager;

class MainWindow : public Gtk::Window
{
    public:
        MainWindow();
        virtual ~MainWindow();

        ViKeyManager* get_key_manager() const;

    protected:

        bool on_key_press_event(GdkEventKey *event);

        gtksourceview::SourceView m_sourceView; 
        Gtk::ScrolledWindow m_scrollView;
        Gtk::Statusbar m_statusBar;
        Gtk::VBox m_vbox;
    
        ViKeyManager *vi;

};

#endif
