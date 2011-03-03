#include <stdlib.h>
#include <gtkmm.h>

#include "App.h"

int main(int argc, char *argv[])
{
    Application *app = Application::get();
    app->run(argc, argv);
    return 0;
}


