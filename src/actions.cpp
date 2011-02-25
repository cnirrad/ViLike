
#include <gtkmm.h>
#include <gtksourceviewmm/sourceview.h>

#include "actions.h"
#include "utils.h"


MovementAction::MovementAction( ViKeyManager *vi, GtkMovementStep step, gint count ) :
    MotionAction(vi),
    m_count(count),
    m_step(step)
{
}


void
MovementAction::perform_motion(Gtk::Widget *w, int count_modifier, Glib::ustring &params)
{

    gboolean ret_val;
    gtk_signal_emit_by_name( (GtkObject*)w->gobj(), 
                             "move-cursor",
                             m_step,
                             m_count * count_modifier,
                             m_ext_sel,
                             &ret_val );

    return;
}

ModeAction::ModeAction( ViKeyManager *vi, ViMode mode ) :
    ExecutableAction(vi),
    m_mode(mode),
    m_move_act(NULL)
{

}

ModeAction::ModeAction( MovementAction *act, ViKeyManager *vi, ViMode mode ) :
    ExecutableAction(vi),
    m_mode(mode),
    m_move_act(act)
{
}

bool
ModeAction::execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params)
{
    if (m_move_act)
    {
        m_move_act->execute(w, count_modifier, params);
    }
    m_vi->set_mode(m_mode, w);
    m_vi->clear_key_buffer();

    return true;
}

bool
ReplaceAction::execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params)
{
    if (is_text_widget(w))
    {
        Gtk::TextView *view = dynamic_cast<Gtk::TextView*>(w);
        view->set_overwrite(true);
        m_vi->set_mode(vi_insert);        
    }
    return true;
}

bool
ChooseRegistryAction::execute( Gtk::Widget *w, int count_modifier, Glib::ustring &params )
{
    m_vi->set_current_register( params[0] );
    g_print("Set register = %s\n", params.data());
    return true;
}

//
//  Helper function for yank and delete
//
Glib::ustring yank(ViKeyManager *vi, Gtk::Widget *w, bool del = false)
{
    Glib::ustring text;
    if (is_text_widget(w))
    {
        Gtk::TextView *view = dynamic_cast<Gtk::TextView*>(w);
        Glib::RefPtr<Gtk::TextBuffer> buffer = view->get_buffer();

        Gtk::TextIter start;
        Gtk::TextIter end;

        buffer->get_selection_bounds(start, end);
        text = buffer->get_text(start, end);

        char r = vi->get_current_register();

        vi->set_register(r, text);

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

bool
YankAction::execute( Gtk::Widget *w, int count_modifier, Glib::ustring &params )
{
    yank(m_vi, w);
    return true;
}

bool
PutAction::execute( Gtk::Widget *w, int count_modifier, Glib::ustring &params )
{
    char r = m_vi->get_current_register();

    Glib::ustring text = m_vi->get_register(r);


    if (is_text_widget(w))
    {
        Gtk::TextView *view = dynamic_cast<Gtk::TextView*>(w);
        Glib::RefPtr<Gtk::TextBuffer> buffer = view->get_buffer();
        buffer->insert_at_cursor(text);
    }
    return true;
}

bool
DeleteAction::execute( Gtk::Widget *w, int count_modifier, Glib::ustring &params )
{
    yank(m_vi, w, true);
    return true;
}

bool
DeleteOneAction::execute( Gtk::Widget *w, int count_modifier, Glib::ustring &params )
{
    if (is_text_widget(w))
    {
        Gtk::TextView *view = dynamic_cast<Gtk::TextView*>(w); 
        Glib::RefPtr<Gtk::TextBuffer> buffer = view->get_buffer();

        Gtk::TextBuffer::iterator end = get_cursor_iter( buffer ); 

        if (m_before)
            end.backward_cursor_positions(count_modifier);
        else
            end.forward_cursor_positions(count_modifier);

        set_cursor( end, true );
        yank(m_vi, w, true);
    }
   
    return true;
}

bool
ChangeAction::execute( Gtk::Widget *w, int count_modifier, Glib::ustring &params )
{
    yank(m_vi, w, true); 
    m_vi->set_mode(vi_insert);

    return true;
}

bool
CreateMarkAction::execute( Gtk::Widget *w, int count_modifier, Glib::ustring &params ) 
{
    if (is_text_widget(w))
    {
        Gtk::TextView *view = dynamic_cast<Gtk::TextView*>(w); 
        Glib::RefPtr<Gtk::TextBuffer> buffer = view->get_buffer();

        Gtk::TextBuffer::iterator it = get_cursor_iter( buffer ); 

        if ( params == "" )
        {
            return false;
        }

        buffer->create_mark( params, it );
    }
}

void
GotoMarkAction::perform_motion( Gtk::Widget *w, int count_modifier, Glib::ustring &params ) 
{
    if ( params == "" )
    {
        return;
    }

    if (is_text_widget(w))
    {
        Gtk::TextView *view = dynamic_cast<Gtk::TextView*>(w); 
        Glib::RefPtr<Gtk::TextBuffer> buffer = view->get_buffer();

        Glib::RefPtr< Gtk::TextBuffer::Mark > mark = 
                                        buffer->get_mark( params );

        if ( mark == NULL )
        {
            g_print( "Mark %s not present.\n", params.data() );
            return;
        }

        Gtk::TextBuffer::iterator iter = 
                        buffer->get_iter_at_mark( mark );

        set_cursor( iter, m_ext_sel );
   }

}

void
GotoLineAction::perform_motion( Gtk::Widget *w, int line, Glib::ustring &params ) 
{
    if (m_line != -1)
        line = m_line;

    if (is_text_widget(w))
    {
        Gtk::TextView *view = dynamic_cast<Gtk::TextView*>(w); 
        Glib::RefPtr<Gtk::TextBuffer> buffer = view->get_buffer();

        //
        //  The past in line will be 1 if no number was given, but Vim 
        //  will goto the end of the buffer when no number is given.
        //  The count from the Vi Key manager will still be zero if
        //  not count was given.
        //
        if (m_line == -1 && m_vi->get_count() == 0) 
        {
            line = buffer->get_line_count();
        }
        set_cursor_at_line( buffer, line, m_ext_sel );
    }

}

CompoundAction::CompoundAction( ViKeyManager *vi, ... ) :
    ExecutableAction( vi )
{
    va_list vl;
    va_start(vl, vi);

    ExecutableAction *action = va_arg(vl, ExecutableAction*);
    m_actions.push_back(action);

    va_end(vl);
}

bool 
CompoundAction::execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params)
{
    std::list<ExecutableAction*>::iterator it; 

    for ( it = m_actions.begin(); it != m_actions.end(); it++ )
    {
        ((ExecutableAction*)*it)->execute(w, count_modifier, params);
    }
}

void 
FindAction::perform_motion(Gtk::Widget *w, int count_modifier, Glib::ustring &params)
{
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
        Gtk::TextView *view = dynamic_cast<Gtk::TextView*>(w); 
        Glib::RefPtr<Gtk::TextBuffer> buffer = view->get_buffer();
        
        Gtk::TextBuffer::iterator cursor = get_cursor_iter( buffer ); 
        cursor.forward_cursor_position();

        Gtk::TextBuffer::iterator end(cursor); 
        end.forward_to_line_end();

        Glib::ustring txt = cursor.get_slice( end );

        int idx = txt.find( params[0] );

        if (idx > 0)
        {
            g_print("Found %c at %i\n", params[0], idx);

            //
            //  If extending the selection, include the search
            //  character as part of the selection.
            //
            if (m_ext_sel)
                idx++;

            cursor.forward_cursor_positions(idx);

            set_cursor( cursor, m_ext_sel );
        }
    }
}


bool 
UndoAction::execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params)
{
    if ( is_text_widget( w ) )
    { 
        gtksourceview::SourceView *view = 
            dynamic_cast<gtksourceview::SourceView*>(w); 

        Glib::RefPtr< gtksourceview::SourceBuffer > buffer = 
            view->get_source_buffer();

        if ( buffer->can_undo() )
            buffer->undo();
    }
    return true;
}

bool 
RedoAction::execute(Gtk::Widget *w, int count_modifier, Glib::ustring &params)
{
    if ( is_source_view( w ) )
    { 
        gtksourceview::SourceView *view = 
            dynamic_cast<gtksourceview::SourceView*>(w); 

        Glib::RefPtr< gtksourceview::SourceBuffer > buffer = 
            view->get_source_buffer();

        if ( buffer->can_undo() )
            buffer->redo();
    }
    return true;
}

void
MatchBracketAction::perform_motion(Gtk::Widget *w, int count_modifier, Glib::ustring &params) 
{
    if ( !is_text_widget( w ) )
    { 
        return;
    }

    gtksourceview::SourceView *view = 
        dynamic_cast<gtksourceview::SourceView*>(w); 
    Glib::RefPtr< gtksourceview::SourceBuffer > buffer = 
        view->get_source_buffer();
    Gtk::TextBuffer::iterator iter = get_cursor_iter( buffer ); 
    
    gchar ch = iter.get_char();

    struct match_chars
    {
        gchar begin;
        gchar end;
    };
    const match_chars map[] = { { '{', '}' },
                                { '(', ')' },
                                { '[', ']' },
                                { 0, 0 } };
    const int map_lgth = 3;

    gchar find = '\0';
    match_chars chars = {0, 0};
    bool forward;
    Gtk::TextBuffer::iterator end_iter;
    

    for (int i = 0; i < map_lgth; ++i)
    {
        match_chars mc = map[i];
        if ( ch == mc.begin )
        {
            forward = true;
            chars = mc;
            end_iter = buffer->end();
        }
        else if ( ch == mc.end )
        {
            forward = false;
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

    while (iter != end_iter)
    {
        if (forward)
            iter.forward_char();
        else
            iter.backward_char();

        ch = iter.get_char();

        if (ch == chars.end)
        {
            g_print( "%c == %c / level = %i.  ", ch, chars.end, level);
            if (level == 0)
            {
                set_cursor(iter, m_ext_sel);
                return;
            }
            else
            {
                g_print("level--\n");
                level--;
            }
        }
        else if (ch == chars.begin)
        {
            g_print("level++\n");
            level++;
        }
    }
    g_print("Match not found.\n");
}

