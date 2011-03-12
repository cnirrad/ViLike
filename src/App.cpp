
#include <gtkmm.h>
#include <gtksourceviewmm.h>

#include "App.h"
#include "ViKeyManager.h"
#include "ViMotionAction.h"
#include "actions.h"

Application *Application::self = 0;

void setup_vi_keybindings(MainWindow *win);

void
Application::run(int argc, char **argv)
{
    Gtk::Main kit(argc, argv);

    gtksourceview::init();

    Glib::RefPtr< gtksourceview::SourceLanguageManager > lm = 
        gtksourceview::SourceLanguageManager::create();

    g_print("Search paths:\n");
    std::list<Glib::ustring> paths = lm->get_search_path();
    std::list<Glib::ustring>::iterator it;
    for (it = paths.begin(); it != paths.end(); it++)
    {
        g_print("%s:", (*it).data());
    }

    g_print("\nLanguage IDs:\n");
    std::list<Glib::ustring> ids = lm->get_language_ids();
    for (it = ids.begin(); it != ids.end(); it++)
    {
        g_print("%s,", (*it).data());
    }

    m_main_window = new MainWindow();
    setup_vi_keybindings(m_main_window);

    Gtk::Main::run(*m_main_window);
}

Application* Application::get()
{
    if (self == NULL)
        self = new Application();
    return self;
}

MainWindow* Application::get_main_window()
{
    return m_main_window;
}

Application::Application()
{
}

Editor* Application::get_current_editor()
{
    EditorArea* editor_area = m_main_window->get_editor_area(); 

    int current = editor_area->get_current_page();

    Gtk::Widget *w = editor_area->get_nth_page(current);
    Editor *ed = dynamic_cast<Editor*>(w);

    return ed;
}

void setup_vi_keybindings(MainWindow *win) 
{
    ViKeyManager *vi = win->get_key_manager();

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
    vi->map_key( vi_normal, "w", new WordMotionAction(vi, true));
    vi->map_key( vi_normal, "W", new MovementAction(vi, GTK_MOVEMENT_WORDS, 1));
    vi->map_key( vi_normal, "b", new WordMotionAction(vi, false));
    vi->map_key( vi_normal, "^", new KeySequenceAction( vi, "0w" ));
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

    vi->map_key( vi_normal, "f", new FindAction( vi, true ));
    vi->map_key( vi_normal, "F", new FindAction( vi, false ));
    vi->map_key( vi_normal, "G", new GotoLineAction( vi ));
    vi->map_key( vi_normal, "gg", new GotoLineAction( vi, 1 ));
    vi->map_key( vi_normal, "%", new MatchBracketAction( vi ));
    vi->map_key( vi_normal, "*", new SearchWordUnderCursorAction( vi ));
    vi->map_key( vi_normal, "#", new SearchWordUnderCursorAction( vi, false ));

    //
    //  Insert/append
    //
    vi->map_key( vi_normal, "A", new ModeAction( eol_act, vi, vi_insert ));
    vi->map_key( vi_normal, "a", new ModeAction( left_act, vi, vi_insert ));
    vi->map_key( vi_normal, "i", new ModeAction(vi, vi_insert));
    vi->map_key( vi_normal, "I", new KeySequenceAction( vi, "^i" ));
    vi->map_key( vi_normal, "R", new ReplaceAction(vi));
    vi->map_key( vi_normal, "O", new KeySequenceAction( vi, "0i<CR><Esc>ki" ));
    vi->map_key( vi_normal, "o", new KeySequenceAction( vi, "$i<CR><Esc>i" ));
    vi->map_key( vi_normal, "V", new ModeAction( vi, vi_visual ));
    vi->map_key( vi_normal, "c", new ChangeAction( vi ));
    vi->map_key( vi_normal, "C", new KeySequenceAction( vi, "c$" ));
    vi->map_key( vi_normal, "S", new KeySequenceAction( vi, "0c$" ));
    vi->map_key( vi_normal, "s", new KeySequenceAction( vi, "cl" ));

    //
    //  Modify
    //
    vi->map_key( vi_normal, "~", new SwapCaseAction( vi ));

    //
    //  Registers/yank/put/delete
    //
    vi->map_key( vi_normal, "\"", new ChooseRegistryAction( vi ));
    vi->map_key( vi_normal, "y", new YankAction( vi ));
    vi->map_key( vi_normal, "Y", new KeySequenceAction( vi, "0y$" ));
    vi->map_key( vi_normal, "p", new PutAction( vi, true ));
    vi->map_key( vi_normal, "P", new PutAction( vi, false ));
    vi->map_key( vi_normal, "d", new DeleteAction( vi ));
    vi->map_key( vi_normal, "D", new KeySequenceAction( vi, "d$" ));
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

    //
    //  Window management
    //
    WindowToggleAction *maxAct = new WindowToggleAction( vi, Gdk::WINDOW_STATE_MAXIMIZED );
    WindowToggleAction *iconifyAct = new WindowToggleAction( vi, Gdk::WINDOW_STATE_ICONIFIED );
    WindowToggleAction *fullAct = new WindowToggleAction( vi, Gdk::WINDOW_STATE_FULLSCREEN );
    WindowToggleAction *stickAct = new WindowToggleAction( vi, Gdk::WINDOW_STATE_STICKY );
    NextTabAction *nextTab = new NextTabAction( vi, true );
    NextTabAction *prevTab = new NextTabAction( vi, false );

    vi->map_key( vi_normal, "<C-A-m>", maxAct );
    vi->map_key( vi_normal, "<C-A-M>", iconifyAct );
    vi->map_key( vi_normal, "<C-A-f>", fullAct );
    vi->map_key( vi_normal, "<C-A-s>", stickAct );
    vi->map_key( vi_normal, "<C-Tab>", nextTab );
    vi->map_key( vi_normal, "<S-C-Tab>", prevTab );

    vi->map_key( vi_command, ":q", new QuitAction( vi ));
    vi->map_key( vi_command, ":tabNext", nextTab );
    vi->map_key( vi_command, ":tabPrev", prevTab );
    vi->map_key( vi_command, ":fullscreen", fullAct );
    vi->map_key( vi_command, ":minimize", iconifyAct );
    vi->map_key( vi_command, ":maximize", maxAct );


    vi->map_key( vi_command, ":e", new OpenFileAction( vi ));

    //
    //  Command Mode
    //
    ModeAction *cmdMode = new ModeAction( vi, vi_command );
    vi->map_key( vi_normal, ":", cmdMode );
    vi->map_key( vi_normal, "/", cmdMode );
    vi->map_key( vi_normal, "?", cmdMode );

}
