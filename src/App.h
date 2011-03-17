#ifndef SOURCERER_APP_H
#define SOURCERER_APP_H

#include "MainWindow.h"
#include "s7.h"

typedef s7_scheme Scheme;

class Application
{
    public:
        void run(int argc, char **argv);

        static Application* get();

        MainWindow* get_main_window();

        Editor* get_current_editor();

        Scheme* get_scheme();

        void quit()
        {
            Gtk::Main::quit();
        }

    protected:
        Application();

        static Application *self;
        MainWindow *m_main_window;

        s7_scheme *m_scm;
};

#endif
