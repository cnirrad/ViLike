#include <stdlib.h>
#include <gtkmm.h>

#include "MainWindow.h"
#include "ViKeyManager.h"
#include "actions.h"

void setup_vi_keybindings(MainWindow &win);

int main(int argc, char *argv[])
{
    Gtk::Main kit(argc, argv);
    MainWindow window;

    setup_vi_keybindings(window);

    Gtk::Main::run(window);

    return 0;
}

void setup_vi_keybindings(MainWindow &win) 
{
    ViKeyManager *vi = win.get_key_manager();

    //
    //  Movement keys
    //
    MovementAction *left_act = new MovementAction(GTK_MOVEMENT_LOGICAL_POSITIONS, -1);
    MovementAction *right_act = new MovementAction(GTK_MOVEMENT_LOGICAL_POSITIONS, 1);
    MovementAction *up_act = new MovementAction(GTK_MOVEMENT_DISPLAY_LINES, -1);
    MovementAction *down_act = new MovementAction(GTK_MOVEMENT_DISPLAY_LINES, 1);

    vi->map_key( vi_normal, "h", left_act );
    vi->map_key( vi_normal, "j", down_act );
    vi->map_key( vi_normal, "k", up_act );
    vi->map_key( vi_normal, "l", right_act );
    vi->map_key( vi_normal, "w", new MovementAction(GTK_MOVEMENT_WORDS, 1));
    vi->map_key( vi_normal, "b", new MovementAction(GTK_MOVEMENT_WORDS, -1));

    MovementAction *eol_act = new MovementAction(GTK_MOVEMENT_DISPLAY_LINE_ENDS, 1);
    vi->map_key( vi_normal, "$", eol_act );
    vi->map_key( vi_normal, "0", new MovementAction(GTK_MOVEMENT_DISPLAY_LINE_ENDS, -1));

    //
    //  Insert/append
    //
    vi->map_key( vi_normal, "A", new ModeAction( eol_act, vi, vi_insert ));
    vi->map_key( vi_normal, "a", new ModeAction( left_act, vi, vi_insert ));
    vi->map_key( vi_normal, "i", new ModeAction(vi, vi_insert));
    vi->map_key( vi_normal, "R", new ReplaceAction(vi));

    //
    //  Registers
    //
    vi->map_key( vi_normal, "\"", new ChooseRegistryAction( vi ));
}

