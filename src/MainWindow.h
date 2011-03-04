#ifndef EDITOR_MAIN_WINDOW_H
#define EDITOR_MAIN_WINDOW_H

#include <gtkmm.h>

#include "Editor.h"
#include "EditorArea.h"

class ViKeyManager;

class MainWindow : public Gtk::Window
{
    public:
        MainWindow();
        virtual ~MainWindow();

        ViKeyManager* get_key_manager() const;

        EditorArea* get_editor_area(); 
        Gtk::Statusbar* get_status_bar(); 

        bool is_fullscreen() const;
        bool is_maximized() const;

    protected:

        bool on_key_press_event(GdkEventKey *event);

        SourceEditor m_sourceEditor;
        EditorArea m_editor_area;

        Gtk::Statusbar m_statusBar;
        Gtk::VBox m_vbox;
    
        ViKeyManager *vi;

};

#endif
