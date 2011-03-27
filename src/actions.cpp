
#include <gtkmm.h>
#include <gtksourceviewmm/sourceview.h>

#include "App.h"
#include "actions.h"
#include "Editor.h"
#include "utils.h"
#include "ViTextIter.h"


void move_cursor( GtkMovementStep step, gint count )
{
    int count_modifier = get_vi()->get_cmd_count();
    Gtk::Widget *w = get_focused_widget();

    if (count_modifier == -1)
        count_modifier = 1;

    if (is_text_widget(w))
    {
        Gtk::TextView *view = static_cast<Gtk::TextView*>(w); 
        Glib::RefPtr<Gtk::TextBuffer> buffer = view->get_buffer();

        ViTextIter cursor = get_cursor_iter( buffer ); 

        gboolean ret_val;
        gtk_signal_emit_by_name( (GtkObject*)w->gobj(), 
                                 "move-cursor",
                                 step,
                                 count * count_modifier,
                                 get_vi()->get_extend_selection(),
                                 &ret_val );
        
    }

    return;
}

void move_and_change_mode( ViMode mode )
{
    //TODO
}

/*
bool
ModeAction::execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params)
{
    if (m_move_act)
    {
        if (count_modifier == -1)
            count_modifier = 1;
        m_move_act->execute(w, count_modifier, params);
    }
    get_vi()->set_mode(m_mode);

    return true;
}
*/

void set_replace_mode()
{
    Gtk::Widget *w = get_focused_widget();

    if (is_text_widget(w))
    {
        Gtk::TextView *view = static_cast<Gtk::TextView*>(w);
        get_vi()->set_mode(vi_insert);        
        view->set_overwrite(true);
    }
    return;
}

void choose_register()
{
    Glib::ustring params = get_vi()->get_cmd_params();
    get_vi()->set_current_register( params[0] );
    g_print("Set register = %s\n", params.data());
}

//
//  Helper function for yank and delete
//
Glib::ustring do_yank( Gtk::Widget *w, bool del = false, ViOperatorScope scope = vi_characterwise)
{
    ViKeyManager *vi = get_vi();
    Glib::ustring text;
    if (is_text_widget(w))
    {
        Gtk::TextView *view = static_cast<Gtk::TextView*>(w);
        Glib::RefPtr<Gtk::TextBuffer> buffer = view->get_buffer();

        Gtk::TextIter start;
        Gtk::TextIter end;

        buffer->get_selection_bounds(start, end);
        text = buffer->get_text(start, end);

        char r = vi->get_current_register();

        vi->set_register(r, text, scope);

        if (r != 0x00)
            g_print("Set %c with \"%s\"\n", r, text.data());
        else
            g_print("Set default register with \"%s\"\n", text.data());

        if (del)
        {
            buffer->erase(start, end);
        }
    }
    return text;
}

void delete_text()
{
    Gtk::Widget *w = get_focused_widget();
    do_yank( w, true );
    return; 
}


void delete_char(Direction dir)
{
    int count_modifier = get_vi()->get_cmd_count();
    Gtk::Widget *w = get_focused_widget();
    
    if (count_modifier == -1)
        count_modifier = 1;

    if (is_text_widget(w))
    {
        Gtk::TextView *view = static_cast<Gtk::TextView*>(w); 
        Glib::RefPtr<Gtk::TextBuffer> buffer = view->get_buffer();

        ViTextIter end = get_cursor_iter( buffer ); 

        if (dir == Backward)
            end.backward_cursor_positions(count_modifier);
        else
            end.forward_cursor_positions(count_modifier);

        set_cursor( end, true );
        do_yank(w, true);
    }
   
    return;
}


void change_text()
{
    Gtk::Widget *w = get_focused_widget();
    do_yank(w, true); 
    get_vi()->set_mode(vi_insert);

    return; 
}

void create_mark()
{
    Glib::ustring params = get_vi()->get_cmd_params();

    if ( params == "" )
    {
        return;
    }

    Gtk::Widget *w = get_focused_widget();

    if (is_text_widget(w))
    {
        Gtk::TextView *view = static_cast<Gtk::TextView*>(w); 
        Glib::RefPtr<Gtk::TextBuffer> buffer = view->get_buffer();

        ViTextIter it = get_cursor_iter( buffer ); 

        buffer->create_mark( params, it );
    }
}


void goto_mark()
{
    Glib::ustring params = get_vi()->get_cmd_params();

    if ( params == "" )
    {
        return;
    }

    Gtk::Widget *w = get_focused_widget();

    if (is_text_widget(w))
    {
        Gtk::TextView *view = static_cast<Gtk::TextView*>(w); 
        Glib::RefPtr<Gtk::TextBuffer> buffer = view->get_buffer();

        Glib::RefPtr< Gtk::TextBuffer::Mark > mark = 
                                        buffer->get_mark( params );

        if ( mark == NULL )
        {
            get_vi()->show_error( "Mark %s not present.", params.data() );
            return;
        }

        Gtk::TextBuffer::iterator iter = 
                        buffer->get_iter_at_mark( mark );

        set_cursor( iter, get_vi()->get_extend_selection() );
   }

}


void goto_specific_line( int line )
{
    Gtk::Widget *w = get_focused_widget();

    if (is_text_widget(w))
    {
        Gtk::TextView *view = static_cast<Gtk::TextView*>(w); 
        Glib::RefPtr<Gtk::TextBuffer> buffer = view->get_buffer();

        //
        //  The past in line will be 1 if no number was given, but Vim 
        //  will goto the end of the buffer when no number is given.
        //  The count from the Vi Key manager will still be zero if
        //  not count was given.
        //
        if (line == -1 ) // && get_vi()->get_count() == 0) 
        {
            line = buffer->get_line_count();
        }

        set_cursor_at_line( buffer, line, get_vi()->get_extend_selection() );
    }

}

void goto_line()
{
    int line = get_vi()->get_cmd_count();
    goto_specific_line( line );
}


void execute_vi_key_sequence( Glib::ustring keys )
{
    get_vi()->execute( keys );
    return;
}

void execute_key_sequence_from_params()
{
    Glib::ustring params = get_vi()->get_cmd_params();
    execute_vi_key_sequence( params );
}

void find_char(GtkDirectionType dir)
{
    Glib::ustring params = get_vi()->get_cmd_params();
    Gtk::Widget *w = get_focused_widget();

    if (params.length() > 1)
    {
        if (params == "<Space>")
        {
            params = " ";
        }
        else
        {
            g_print("Invalid search character: %s", params.data());
            return;
        }
    }

    if (is_text_widget(w))
    {
        Gtk::TextView *view = static_cast<Gtk::TextView*>(w); 
        Glib::RefPtr<Gtk::TextBuffer> buffer = view->get_buffer();
        
        ViTextIter cursor = get_cursor_iter( buffer ); 

        //
        //  Take one step to make sure we don't match on the
        //  current character.
        //
        if (!cursor.iter_next(dir))
        {
            return;
        }

        while (cursor.iter_next(dir))
        {
            gunichar ch = cursor.get_char();
            if (ch == '\r' || ch == '\n')
                return;

            if (ch == params[0])
            {
                //
                //  If extending selection, include the 
                //  matched character.
                //
                if (get_vi()->get_extend_selection())
                    cursor.iter_next(dir);

                set_cursor( cursor, get_vi()->get_extend_selection() );
                return;
            }
        }
    }
}


void undo()
{
    Gtk::Widget *w = get_focused_widget();
    if ( is_source_view( w ) )
    { 
        gtksourceview::SourceView *view = 
            static_cast<gtksourceview::SourceView*>(w); 

        Glib::RefPtr< gtksourceview::SourceBuffer > buffer = 
            view->get_source_buffer();

        if ( buffer->can_undo() )
            buffer->undo();
    }
    return;
}

void redo()
{
    Gtk::Widget *w = get_focused_widget();

    if ( is_source_view( w ) )
    { 
        gtksourceview::SourceView *view = 
            static_cast<gtksourceview::SourceView*>(w); 

        Glib::RefPtr< gtksourceview::SourceBuffer > buffer = 
            view->get_source_buffer();

        if ( buffer->can_redo() )
            buffer->redo();
    }
    return;
}


void match_brace()
{
    Gtk::Widget *w = get_focused_widget();

    if ( !is_text_widget( w ) )
    { 
        return;
    }

    gtksourceview::SourceView *view = 
        static_cast<gtksourceview::SourceView*>(w); 
    Glib::RefPtr< gtksourceview::SourceBuffer > buffer = 
        view->get_source_buffer();
    ViTextIter iter = get_cursor_iter( buffer ); 
    
    gchar ch = iter.get_char();

    struct match_chars
    {
        gchar begin;
        gchar end;
    };
    const match_chars map[] = { { '{', '}' },
                                { '(', ')' },
                                { '[', ']' },
                                { '<', '>' },
                                { 0, 0 } };
    const int map_lgth = 4;

    gchar find = '\0';
    match_chars chars = {0, 0};
    GtkDirectionType dir;
    Gtk::TextIter end_iter;
    

    for (int i = 0; i < map_lgth; ++i)
    {
        match_chars mc = map[i];
        if ( ch == mc.begin )
        {
            dir = GTK_DIR_RIGHT;
            chars = mc;
            end_iter = buffer->end();
        }
        else if ( ch == mc.end )
        {
            dir = GTK_DIR_LEFT;
            chars.begin = mc.end;
            chars.end = mc.begin;
            end_iter = buffer->begin();
        }
    }

    if (chars.begin == '\0')
    {
        g_print("Not a supported bracket: %c\n", ch);
        return;
    }

    g_print("Matching %c with %c.\n", chars.begin, chars.end);

    int level = 0;

    while (iter.iter_next(dir))
    {
        ch = iter.get_char();

        if (ch == chars.end)
        {
            if (level == 0)
            {
                if (get_vi()->get_extend_selection())
                    iter.iter_next(dir);     // make sure the ending bracket is selected too
                set_cursor(iter, get_vi()->get_extend_selection());
                return;
            }
            else
            {
                level--;
            }
        }
        else if (ch == chars.begin)
        {
            level++;
        }
    }
    g_print("Match not found.\n");
}

void search_word_under_cursor(Direction dir)
{
    Gtk::Widget *w = get_focused_widget();

    if (is_text_widget(w))
    {
        Gtk::TextView *view = static_cast<Gtk::TextView*>(w); 
        Glib::RefPtr<Gtk::TextBuffer> buffer = view->get_buffer();
        
        ViTextIter cursor = get_cursor_iter( buffer ); 
        ViTextIter iter( cursor ); 

        Glib::ustring word = iter.get_word();
        if (word == "")
        {
            get_vi()->show_error("No string under cursor.");
            return;
        }

        //
        //  Make sure the word is properly escaped and then add the
        //  boundry assertion.
        //
        gchar *esc = g_regex_escape_string( word.data(), word.length() );
        word = "\\b";
        word += esc;
        word += "\\b";

        Editor *ed = Application::get()->get_current_editor();
        if ( !ed->search(word, dir, get_vi()->get_extend_selection()) )
            get_vi()->show_error("Match not found.");
    }
}

void swap_case()
{
    Gtk::Widget *w = get_focused_widget();

    if (is_text_widget(w))
    {
        Gtk::TextView *view = static_cast<Gtk::TextView*>(w); 
        Glib::RefPtr<Gtk::TextBuffer> buffer = view->get_buffer();

        char ch;    
        Glib::ustring text = get_selected_text(buffer);
        Glib::ustring replace;

        for (int idx = 0; idx < text.length(); ++idx)
        {
            ch = text[idx]; 
            if (islower(ch))
            {
                ch = toupper(ch);
                replace.push_back(ch);
            }
            else if (toupper(ch))
            {
                ch = tolower(ch);
                replace.push_back(ch);
            }
            else
            {
                replace.push_back(ch);
            }
        }

        ViTextIter start = get_cursor_iter( buffer );

        if (buffer->get_has_selection())
        {
            buffer->erase_selection();
        }
        else
        {
            ViTextIter end(start);
            end.forward_chars(replace.length());
            buffer->erase(start, end);
        }

        start = get_cursor_iter( buffer );
        buffer->insert(start, replace);
    }
    return;
}



void move_by_word( Direction dir )
{
    int count_modifier = get_vi()->get_cmd_count();
    Gtk::Widget *w = get_focused_widget();

    if (count_modifier == -1)
        count_modifier = 1;

    if (is_text_widget(w))
    {
        Gtk::TextView *view = static_cast<Gtk::TextView*>(w); 
        Glib::RefPtr<Gtk::TextBuffer> buffer = view->get_buffer();

        ViTextIter cursor = get_cursor_iter( buffer ); 

        ViTextIter iter(cursor);
        for (int i=0; i<count_modifier; ++i)
        {
            if (dir == Forward)
                iter.forward_next_word_start();
            else
                iter.backward_next_word_start();
        }
        set_cursor(iter, get_vi()->get_extend_selection());
    }
}

void toggle_window_state( Gdk::WindowState state )
{
    MainWindow *main_win = Application::get()->get_main_window();

    Gdk::WindowState s = main_win->get_window()->get_state();

    bool is_on = false;

    if ( ( s & state ) == state )
    {
        is_on = true;
    }

    switch( state )
    {
        case Gdk::WINDOW_STATE_MAXIMIZED:
            if (is_on)
                main_win->unmaximize();
            else
                main_win->maximize();
            break;
        case Gdk::WINDOW_STATE_FULLSCREEN:
            if (is_on)
                main_win->unfullscreen();
            else
                main_win->fullscreen();
            break;
        case Gdk::WINDOW_STATE_STICKY:
            if (is_on)
                main_win->get_window()->unstick();
            else
                main_win->get_window()->stick();
            break;
        case Gdk::WINDOW_STATE_ICONIFIED:
            if (is_on)
                main_win->get_window()->deiconify();
            else
                main_win->get_window()->iconify();
            break;
        default:
            break;
    };

}

void next_tab( Direction dir )
{
    //
    //  This should go up the widget heirarchy to find the first
    //  notebook, but for now we'll just go straight to the EditorArea
    //
    EditorArea *e = Application::get()->get_main_window()->get_editor_area();

    int num_pages = e->get_n_pages();
    int cur_page = e->get_current_page();

    if (cur_page == -1)
        return;

    if (dir == Forward)
    {
        if (cur_page == num_pages - 1)
        {
            //
            //  Wrap
            //
            e->set_current_page(0);
        }
        else
        {
            e->next_page();
        }
    }
    else
    {
        if (cur_page == 0)
        {
            //
            //  Wrap
            //
            e->set_current_page(num_pages - 1);
        }
        else
        {
            e->prev_page();
        }
    }
}

void application_quit()
{
    //
    //  TODO: Ask user to save any modified buffers.
    //
    Application::get()->quit();
}

void open_file()
{
    SourceEditor *e = Gtk::manage(new SourceEditor()); 

    EditorArea *ea = Application::get()->get_main_window()->get_editor_area();

    Glib::ustring params = get_vi()->get_cmd_params();

    if (e->open(params))
    {
        ea->add_editor(e);
        g_print("Added page for file %s\n", params.data());
    }
    else
    {
        g_print("Error could not open file %s\n", params.data());
        get_vi()->show_error("Error: could not open file %s", params.data());
    }
}

void close_current_file()
{
    SourceEditor *ed = 
        static_cast<SourceEditor*>(Application::get()->get_current_editor());
    EditorArea *ea = Application::get()->get_main_window()->get_editor_area();

    if (ed->is_dirty())
    {
        g_print("Closing dirty editor.\n");
    }
    ea->close_editor(ed);
    return;
}

void yank_text() 
{
    do_yank(get_focused_widget());
}

void
yank_line(bool del)
{
    Gtk::Widget *w = get_focused_widget();

    if (is_text_widget(w))
    {
        Gtk::TextView *view = static_cast<Gtk::TextView*>(w); 
        Glib::RefPtr<Gtk::TextBuffer> buffer = view->get_buffer();

        ViTextIter cursor = get_cursor_iter( buffer ); 
        int cur_offset = cursor.get_line_offset();
    
        cursor.set_line_offset(0);
        set_cursor(cursor, false);

        cursor.forward_line();
        set_cursor(cursor, true);

        do_yank( w, del, vi_linewise );

        //
        //  Move back to the original offset
        //
        cursor = get_cursor_iter( buffer );
        if (cur_offset > cursor.get_chars_in_line())
        {
            cur_offset = cursor.get_chars_in_line();
        }
        cursor.set_line_offset( cur_offset );
        set_cursor(cursor, false);
    }
}

void put_text(Direction dir)
{
    char r = get_vi()->get_current_register();

    ViRegisterValue val = get_vi()->get_register(r);

    Gtk::Widget *w = get_focused_widget();
    if (is_text_widget(w))
    {
        Gtk::TextView *view = static_cast<Gtk::TextView*>(w);
        Glib::RefPtr<Gtk::TextBuffer> buffer = view->get_buffer();

        ViTextIter cursor = get_cursor_iter( buffer ); 

        if (val.scope == vi_characterwise)
        {
            if (dir == Forward)
                cursor.forward_char();
        }
        else 
        {
            if (dir == Forward)
                cursor.forward_line();
            else
                cursor.set_line_offset(0);
        }

        buffer->insert(cursor, val.text);
    }

}

void change_focus( Gtk::DirectionType dir )
{
    MainWindow *shell = Application::get()->get_main_window();

    shell->child_focus( dir );
}

