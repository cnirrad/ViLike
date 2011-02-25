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
    MovementAction *left_act = new MovementAction(vi, GTK_MOVEMENT_LOGICAL_POSITIONS, -1);
    MovementAction *right_act = new MovementAction(vi, GTK_MOVEMENT_LOGICAL_POSITIONS, 1);
    MovementAction *up_act = new MovementAction(vi, GTK_MOVEMENT_DISPLAY_LINES, -1);
    MovementAction *down_act = new MovementAction(vi, GTK_MOVEMENT_DISPLAY_LINES, 1);
    MovementAction *page_down_act = new MovementAction(vi, GTK_MOVEMENT_PAGES, 1);
    MovementAction *page_up_act = new MovementAction(vi, GTK_MOVEMENT_PAGES, -1);

    vi->map_key( vi_normal, "h", left_act );
    vi->map_key( vi_normal, "<Left>", left_act );
    vi->map_key( vi_normal, "j", down_act );
    vi->map_key( vi_normal, "<Down>", down_act );
    vi->map_key( vi_normal, "k", up_act );
    vi->map_key( vi_normal, "<Up>", up_act );
    vi->map_key( vi_normal, "l", right_act );
    vi->map_key( vi_normal, "<Right>", right_act );
    vi->map_key( vi_normal, "w", new MovementAction(vi, GTK_MOVEMENT_WORDS, 1));
    vi->map_key( vi_normal, "b", new MovementAction(vi, GTK_MOVEMENT_WORDS, -1));
    vi->map_key( vi_normal, "^", new MovementAction(vi, GTK_MOVEMENT_PARAGRAPHS, -1));
    vi->map_key( vi_normal, "<C-f>", page_down_act );
    vi->map_key( vi_normal, "<PgDn>", page_down_act );
    vi->map_key( vi_normal, "<C-b>", page_up_act );
    vi->map_key( vi_normal, "<PgUp>", page_up_act );
    vi->map_key( vi_normal, "<BS>", left_act );

    MovementAction *bol_act = new MovementAction(vi, GTK_MOVEMENT_DISPLAY_LINE_ENDS, -1);
    MovementAction *eol_act = new MovementAction(vi, GTK_MOVEMENT_DISPLAY_LINE_ENDS, 1);
    vi->map_key( vi_normal, "<Home>", bol_act );
    vi->map_key( vi_normal, "<End>", eol_act );
    vi->map_key( vi_normal, "$", eol_act );
    vi->map_key( vi_normal, "0", bol_act );

    vi->map_key( vi_normal, "f", new FindAction( vi ));
    vi->map_key( vi_normal, "G", new GotoLineAction( vi ));
    vi->map_key( vi_normal, "gg", new GotoLineAction( vi, 1 ));
    vi->map_key( vi_normal, "%", new MatchBracketAction( vi ));

    //
    //  Insert/append
    //
    vi->map_key( vi_normal, "A", new ModeAction( eol_act, vi, vi_insert ));
    vi->map_key( vi_normal, "a", new ModeAction( left_act, vi, vi_insert ));
    vi->map_key( vi_normal, "i", new ModeAction(vi, vi_insert));
    vi->map_key( vi_normal, "R", new ReplaceAction(vi));
    vi->map_key( vi_normal, "O", new ModeAction( up_act, vi, vi_insert ));
    vi->map_key( vi_normal, "o", new ModeAction( down_act, vi, vi_insert ));
    vi->map_key( vi_normal, "V", new ModeAction( vi, vi_visual ));
    vi->map_key( vi_normal, "c", new ChangeAction( vi ));

    //
    //  Registers/yank/put/delete
    //
    vi->map_key( vi_normal, "\"", new ChooseRegistryAction( vi ));
    vi->map_key( vi_normal, "y", new YankAction( vi ));
    vi->map_key( vi_normal, "p", new PutAction( vi ));
    vi->map_key( vi_normal, "d", new DeleteAction( vi ));
    vi->map_key( vi_normal, "x", new DeleteOneAction( vi ));
    vi->map_key( vi_normal, "X", new DeleteOneAction( vi, true ));

    //
    //  Marks
    //
    vi->map_key( vi_normal, "m", new CreateMarkAction( vi ));
    vi->map_key( vi_normal, "'", new GotoMarkAction( vi ));

    //
    //  Undo/Redo
    //
    vi->map_key( vi_normal, "u", new UndoAction( vi ));
    vi->map_key( vi_normal, "<C-r>", new RedoAction( vi ));
}

