#ifndef EDITOR_MAIN_WINDOW_H
#define EDITOR_MAIN_WINDOW_H

#include <gtkmm.h>
//#include <gtksourceviewmm/sourceview.h>

//class ViTextView : public Gtk::TextView
//{
//    public:
//        ViTextView();
//
//    protected:
//        virtual bool on_key_press(GdkEventKey *event);
//};
class ViKeyManager;

class MainWindow : public Gtk::Window
{
    public:
        MainWindow();
        virtual ~MainWindow();


    protected:

        bool on_key_press_event(GdkEventKey *event);

 //       gtksourceview::SourceView m_sourceView; 
        Gtk::ScrolledWindow m_scrollView;
//        ViTextView m_sourceView;
        Gtk::TextView m_sourceView;
    
        ViKeyManager *vi;

};

#endif
