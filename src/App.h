#ifndef SOURCERER_APP_H
#define SOURCERER_APP_H

#include "MainWindow.h"

class Application
{
    public:
        void run(int argc, char **argv);

        static Application* get();

        MainWindow* get_main_window();

    protected:
        Application();

        static Application *self;
        MainWindow *m_main_window;
};

#endif
