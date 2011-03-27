
#include <gtkmm.h>
#include <gtksourceviewmm.h>

#include "App.h"
#include "ViKeyManager.h"
#include "ViMotionAction.h"
#include "actions.h"


Application *Application::self = 0;

void setup_vi_keybindings(MainWindow *win, Glib::RefPtr<Gtk::ActionGroup> ui_mgr);

void
Application::run(int argc, char **argv)
{
    Gtk::Main kit(argc, argv);

    gtksourceview::init();

    m_scm = s7_init();

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

    m_action_group = Gtk::ActionGroup::create();

    m_main_window = new MainWindow();
    setup_vi_keybindings(m_main_window, m_action_group);

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

Scheme* Application::get_scheme()
{
    return m_scm;
}

void setup_vi_keybindings(MainWindow *win, 
                          Glib::RefPtr<Gtk::ActionGroup> act_grp) 
{
    ViKeyManager *vi = win->get_key_manager();
/*
    //
    //  Movement keys
    //

    vi->map_key( vi_normal, "<Left>", left_act );
    vi->map_key( vi_normal, "<Down>", down_act );
    vi->map_key( vi_normal, "<Up>", up_act );
    vi->map_key( vi_normal, "<Right>", right_act );
    vi->map_key( vi_normal, "W", new MovementAction(vi, GTK_MOVEMENT_WORDS, 1));
    vi->map_key( vi_normal, "<PgDn>", page_down_act );
    vi->map_key( vi_normal, "<PgUp>", page_up_act );
    vi->map_key( vi_normal, "<BS>", left_act );

    vi->map_key( vi_normal, "<Home>", bol_act );
    vi->map_key( vi_normal, "<End>", eol_act );


    //
    //  Insert/append
    //
    vi->map_key( vi_normal, "V", new ModeAction( vi, vi_visual ));

    
*/

#define MK_ACTION( name, label, mode, key_binding, flags, fun_ptr )  do {                       \
    Glib::RefPtr<Gtk::Action> _action = Gtk::Action::create( name, label );               \
    act_grp->add( _action, fun_ptr );                                                     \
    vi->map_key( mode, key_binding, new ExecutableAction(  _action, flags ) );       \
} while (0)

    MK_ACTION( "set-insert-mode", "Switches to insert mode", 
               vi_normal, "i", 0, 
               sigc::bind( 
                   sigc::mem_fun(vi, &ViKeyManager::set_mode), vi_insert )); 

    MK_ACTION( "set-command-mode", "Switches to insert mode", 
               vi_normal, ":", 0, 
               sigc::bind( 
                   sigc::mem_fun(vi, &ViKeyManager::set_mode), vi_command )); 

    MK_ACTION( "set-replace-mode", "Switches to replace (overwrite) mode", 
               vi_normal, "R", 0, sigc::ptr_fun( set_replace_mode ) );

    MK_ACTION( "yank", "Yanks (copies) the selected text", 
               vi_normal, "y", await_motion, 
               sigc::ptr_fun( yank_text ) );

    MK_ACTION( "yank-line", "Yanks (copies) the current line", 
               vi_normal, "Y", 0, 
               sigc::bind( sigc::ptr_fun( execute_vi_key_sequence ), "0y$" ));

    MK_ACTION( "put", "Puts (pastes) text", 
               vi_normal, "p", 0, 
               sigc::bind( sigc::ptr_fun( put_text ), Forward ) ); 

    MK_ACTION( "put-before", "Puts (pastes) text before the cursor", 
               vi_normal, "P", 0, 
               sigc::bind( sigc::ptr_fun( put_text ), Backward ) ); 

    MK_ACTION( "set-register", "Sets the register for use", 
               vi_normal, "\"", await_param, 
               sigc::ptr_fun( choose_register ) );

    MK_ACTION( "toggle-maximized", "Toggles between maximized/unmaximized", 
               vi_normal, "<C-A-m>", 0, 
               sigc::bind( sigc::ptr_fun(toggle_window_state), Gdk::WINDOW_STATE_MAXIMIZED) );

    MK_ACTION( "toggle-fullscreen", "Enters/Exit fullscreen mode", 
               vi_normal, "<C-A-f>", 0, 
               sigc::bind( sigc::ptr_fun(toggle_window_state), Gdk::WINDOW_STATE_FULLSCREEN) );

    MK_ACTION( "minimize-window", "Minimizes the main window", 
               vi_command, ":minimize", 0, 
               sigc::bind( sigc::ptr_fun(toggle_window_state), Gdk::WINDOW_STATE_ICONIFIED) );

    MK_ACTION( "toggle-sticky-window", "Toggles sticky window", 
               vi_normal, "<C-A-s>", 0, 
               sigc::bind( sigc::ptr_fun(toggle_window_state), Gdk::WINDOW_STATE_STICKY) );

    MK_ACTION( "next-tab", "Switches to the next tab", 
               vi_normal, "<C-Tab>", 0,
               sigc::bind( sigc::ptr_fun(next_tab), Forward) );

    MK_ACTION( "prev-tab", "Switches to the previous tab", 
               vi_normal, "<S-C-Tab>", 0,
               sigc::bind( sigc::ptr_fun(next_tab), Forward) );

    MK_ACTION( "quit", Gtk::Stock::QUIT,
               vi_command, ":q", 0, sigc::ptr_fun(application_quit) );

    MK_ACTION( "open-file", "Opens the given file", 
               vi_command, ":e", 0, sigc::ptr_fun(open_file) );

    MK_ACTION( "close", "Closes the current file", 
               vi_command, ":close", 0, sigc::ptr_fun(close_current_file) );

    MK_ACTION( "yank-line", "Yank line", 
               vi_normal, "yy", 0, sigc::bind(sigc::ptr_fun(yank_line), false) );

    MK_ACTION( "delete-text", "Deletes text",
               vi_normal, "d", await_motion,
               sigc::ptr_fun(delete_text) );

    MK_ACTION( "delete-char", "Delete character under cursor", 
               vi_normal, "x", 0,
               sigc::bind(sigc::ptr_fun(delete_char), Forward) );

    MK_ACTION( "delete-prev-char", "Delete character before cursor", 
               vi_normal, "X", 0,
               sigc::bind(sigc::ptr_fun(delete_char), Backward) );

    MK_ACTION( "delete-line", "Delete line", 
               vi_normal, "dd", 0,
               sigc::bind(sigc::ptr_fun(yank_line), true) );

    MK_ACTION( "focus-down", "Move Focus Down", 
               vi_normal, "<C-j>", 0,
               sigc::bind(sigc::ptr_fun(change_focus), Gtk::DIR_DOWN) );

    MK_ACTION( "focus-up", "Move Focus Up", 
               vi_normal, "<C-k>", 0,
               sigc::bind(sigc::ptr_fun(change_focus), Gtk::DIR_UP) );

    MK_ACTION( "focus-left", "Move Focus Left", 
               vi_normal, "<C-h>", 0,
               sigc::bind(sigc::ptr_fun(change_focus), Gtk::DIR_LEFT) );

    MK_ACTION( "focus-right", "Move Focus Right", 
               vi_normal, "<C-l>", 0,
               sigc::bind(sigc::ptr_fun(change_focus), Gtk::DIR_RIGHT) );

    MK_ACTION( "swap-case", "Swaps the case of the char under the cursor", 
               vi_normal, "~", 0, sigc::ptr_fun(swap_case) );

    MK_ACTION( "search-word-under-cursor", "Searches for the word under the cursor", 
               vi_normal, "*", 0, 
               sigc::bind( sigc::ptr_fun(search_word_under_cursor), Forward ));

    MK_ACTION( "reverse-search-word-under-cursor", "Searches backwards for the word under the cursor", 
               vi_normal, "#", 0, 
               sigc::bind( sigc::ptr_fun(search_word_under_cursor), Backward ));

    MK_ACTION( "undo", Gtk::Stock::UNDO, vi_normal, "u", 0, sigc::ptr_fun(undo) );

    MK_ACTION( "redo", Gtk::Stock::REDO, vi_normal, "<C-r>", 0, sigc::ptr_fun(redo) );

    MK_ACTION( "create-mark", "Creates a new mark at the cursor location",
               vi_normal, "m", await_param, sigc::ptr_fun(create_mark) );

    MK_ACTION( "change-text", "Selects and replaces text",
               vi_normal, "c", await_motion, sigc::ptr_fun(change_text) );

    MK_ACTION( "change-to-end-of-line", "Deletes to end of line and changes to insert mode",
               vi_normal, "C", 0, 
               sigc::bind( sigc::ptr_fun(execute_vi_key_sequence), "c$" ) );

    MK_ACTION( "change-line", "Deletes entire line and changes to insert mode",
               vi_normal, "S", 0, 
               sigc::bind( sigc::ptr_fun(execute_vi_key_sequence), "0c$" ) );

    MK_ACTION( "change-char", "Deletes character under cursor and changes to insert mode",
               vi_normal, "s", 0, 
               sigc::bind( sigc::ptr_fun(execute_vi_key_sequence), "cl" ) );

    MK_ACTION( "move-start-of-line-first-char-and-insert", "Moves to first column with text and goes to insert mode",
               vi_normal, "I", 0, 
               sigc::bind( sigc::ptr_fun( execute_vi_key_sequence ), "^i" ));

    MK_ACTION( "insert-line-before", "Inserts a line previous to the current cursor line",
               vi_normal, "O", 0, 
               sigc::bind( sigc::ptr_fun( execute_vi_key_sequence ), "0i<CR><Esc>ki" ));

    MK_ACTION( "insert-line-after", "Inserts a line after the current cursor line",
               vi_normal, "o", 0, 
               sigc::bind( sigc::ptr_fun( execute_vi_key_sequence ), "$i<CR><Esc>i" ));

    MK_ACTION( "append", "Moves cursor right and enters insert mode",
               vi_normal, "a", 0, 
               sigc::bind( sigc::ptr_fun( execute_vi_key_sequence ), "li" ));

    MK_ACTION( "append-at-end-of-line", "Moves cursor to the end of the line and enters insert mode",
               vi_normal, "A", 0, 
               sigc::bind( sigc::ptr_fun( execute_vi_key_sequence ), "$i" ));

#define MK_MOTION( name, label, mode, key_binding, flags, fun_ptr )  do {                       \
    Glib::RefPtr<Gtk::Action> _action = Gtk::Action::create( name, label );               \
    act_grp->add( _action, fun_ptr );                                                     \
    vi->map_key( mode, key_binding, new MotionAction( _action, flags ) );            \
} while (0)


    MK_MOTION( "move-char-left", "Move to the left one character", vi_normal, "h", 0, 
               sigc::bind(sigc::ptr_fun(move_cursor), GTK_MOVEMENT_LOGICAL_POSITIONS, -1));

    MK_MOTION( "move-char-right", "Move to the right one character", vi_normal, "l", 0, 
               sigc::bind(sigc::ptr_fun(move_cursor), GTK_MOVEMENT_LOGICAL_POSITIONS, 1));

    MK_MOTION( "move-line-up", "Move cursor up by lines", vi_normal, "k", 0, 
               sigc::bind(sigc::ptr_fun(move_cursor), GTK_MOVEMENT_DISPLAY_LINES, -1));

    MK_MOTION( "move-line-down", "Move cursor down by lines", vi_normal, "j", 0, 
               sigc::bind(sigc::ptr_fun(move_cursor), GTK_MOVEMENT_DISPLAY_LINES, 1));

    MK_MOTION( "page-down", "Page down", vi_normal, "<C-f>", 0, 
               sigc::bind(sigc::ptr_fun(move_cursor), GTK_MOVEMENT_PAGES, 1));

    MK_MOTION( "page-up", "Page up", vi_normal, "<C-b>", 0, 
               sigc::bind(sigc::ptr_fun(move_cursor), GTK_MOVEMENT_PAGES, -1));

    MK_MOTION( "move-next-word", "Move to the start of the next word", vi_normal, "w", 0, 
               sigc::bind(sigc::ptr_fun(move_by_word), Forward));

    MK_MOTION( "move-prev-word", "Move to the start of the next word", vi_normal, "b", 0, 
               sigc::bind(sigc::ptr_fun(move_by_word), Backward));

    MK_MOTION( "move-start-of-line", "Move to the start of the current line", vi_normal, "0", 0, 
               sigc::bind(sigc::ptr_fun(move_cursor), GTK_MOVEMENT_DISPLAY_LINE_ENDS, -1));

    MK_MOTION( "move-start-of-line-first-char", "Moves to the first character of the line",
               vi_normal, "^", 0, 
               sigc::bind( sigc::ptr_fun(execute_vi_key_sequence), "0w") );

    MK_MOTION( "move-end-of-line", "Move to the end of the current line", vi_normal, "$", 0, 
               sigc::bind(sigc::ptr_fun(move_cursor), GTK_MOVEMENT_DISPLAY_LINE_ENDS, 1));

    MK_MOTION( "match-brace", "Finds the matching brace", 
               vi_normal, "%", 0, sigc::ptr_fun(match_brace) );

    MK_MOTION( "find-char", "Finds the next matching character", 
               vi_normal, "f", await_param, 
               sigc::bind( sigc::ptr_fun(find_char), GTK_DIR_RIGHT ));

    MK_MOTION( "reverse-find-char", "Finds the previous matching character", 
               vi_normal, "F", await_param, 
               sigc::bind( sigc::ptr_fun(find_char), GTK_DIR_LEFT ));

    MK_MOTION( "goto-line", "Goes to a line number in the file", 
               vi_normal, "G", 0, sigc::ptr_fun(goto_line));

    MK_MOTION( "goto-buffer-end", "Goes to a the end of the buffer",
               vi_command, ":$", 0, 
               sigc::bind(sigc::ptr_fun(goto_specific_line), -1));

    MK_MOTION( "goto-buffer-begin", "Goes to a the start of the buffer",
               vi_normal, "gg", 0, 
               sigc::bind(sigc::ptr_fun(goto_specific_line), 1));

    MK_MOTION( "goto-mark", "Goes to a the chosen mark",
               vi_normal, "'", await_param, sigc::ptr_fun(goto_mark));
/*    
 

    vi->map_key( vi_command, ":tabNext", nextTab );
    vi->map_key( vi_command, ":tabPrev", prevTab );
    vi->map_key( vi_command, ":fullscreen", fullAct );
    vi->map_key( vi_command, ":maximize", maxAct );

    //
    //  Command Mode
    //
    ModeAction *cmdMode = new ModeAction( vi, vi_command );
    vi->map_key( vi_normal, "/", cmdMode );
    vi->map_key( vi_normal, "?", cmdMode );
*/
}
