#ifndef EDITOR_MAIN_WINDOW_H
#define EDITOR_MAIN_WINDOW_H

#include <gtkmm.h>

#include "Editor.h"
#include "EditorArea.h"
#include "ReplWindow.h"

class ViKeyManager;

class MainWindow : public Gtk::Window
{
    public:
        MainWindow();
        virtual ~MainWindow();

        ViKeyManager* get_key_manager() const;

        enum InfoArea
        {
            Side = 0,
            Bottom
        };

        Gtk::Notebook* get_info_area(InfoArea which);

        EditorArea* get_editor_area(); 
        Gtk::Statusbar* get_status_bar(); 

        bool is_fullscreen() const;
        bool is_maximized() const;

    protected:

        bool on_key_press_event(GdkEventKey *event);

        SourceEditor m_sourceEditor;
        EditorArea m_editor_area;
        Gtk::Notebook m_info_area_bottom;
        Gtk::Notebook m_info_area_side;
        Gtk::Statusbar m_statusBar;

        ReplWindow m_repl;

        Gtk::VBox m_vbox;
        Gtk::VPaned m_vpane;
    
        ViKeyManager *vi;

};

#endif
