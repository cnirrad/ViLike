
#include "ReplWindow.h"
#include "App.h"
#include "utils.h"

#include "s7.h"

const Glib::ustring PROMPT_MARK_NAME("Prompt");
const Glib::ustring PROMPT("\nscheme>");

ReplWindow::ReplWindow() : Gtk::VBox()
{
    m_scrollView.add(m_repl);
    m_scrollView.set_policy(Gtk::POLICY_AUTOMATIC, 
                            Gtk::POLICY_AUTOMATIC);
    m_buffer = Gtk::TextBuffer::create();
    m_repl.set_buffer( m_buffer );

    pack_start(m_scrollView, true, true);

    m_repl.signal_key_press_event().connect( 
        sigc::mem_fun(*this, &ReplWindow::on_key ), false );


    m_prompt_tag = Gtk::TextTag::create( "prompt" );
    m_prompt_tag->property_editable() = false;

    m_buffer->get_tag_table()->add(m_prompt_tag);
    m_buffer->create_mark(PROMPT_MARK_NAME,
            m_buffer->begin());

    set_prompt();
}

ReplWindow::~ReplWindow()
{
}

void ReplWindow::append_text(Glib::ustring txt)
{
    m_buffer->insert(m_buffer->end(), txt);
}

void ReplWindow::evaluate(Glib::ustring &expression)
{
    Scheme *scm = Application::get()->get_scheme();
    if (!expression.empty())
    {
        const char *errmsg;
        s7_pointer old_port, result;                
        int gc_loc = -1;

        //
        //  Open a port to catch error info
        //
        old_port = s7_set_current_error_port(scm, 
                s7_open_output_string(scm));
        if (old_port != s7_nil(scm))
        {
            gc_loc = s7_gc_protect(scm, old_port);
        }

        result = s7_eval_c_string(scm, expression.data());

        errmsg = s7_get_output_string(scm, s7_current_error_port(scm));

        append_text( "\n" );

        if ((errmsg) && (*errmsg))
        {
            append_text(errmsg);
        }
        else
        {
            char *result_as_string;
            result_as_string = s7_object_to_c_string(scm, result);
            if (result_as_string)
            {
                append_text(result_as_string);        
                free(result_as_string);
            }
        }

        s7_close_output_port(scm, s7_current_error_port(scm));
        s7_set_current_error_port(scm, old_port);
        if (gc_loc != -1)
            s7_gc_unprotect_at(scm, gc_loc);
    }
}

bool
ReplWindow::on_key(GdkEventKey *e)
{

    g_print("ReplWindow key press\n");

    if (e->keyval == GDK_Return)
    {
        Glib::ustring exp = get_current_expression(); 
        evaluate(exp);

        g_print("expression: %s\n", exp.data());
        set_prompt();
    }

    return false;
}

Glib::ustring
ReplWindow::get_current_expression()
{
    Gtk::TextIter start = m_buffer->get_iter_at_mark(
            m_buffer->get_mark( PROMPT_MARK_NAME ));

    return m_buffer->get_text(start, m_buffer->end());
}

void
ReplWindow::set_prompt()
{
    ViTextIter iter = get_cursor_iter( m_buffer );

    m_buffer->insert_with_tag( iter, PROMPT, "prompt" );
    iter = m_buffer->end();
    m_buffer->insert( iter, " " );
    iter = m_buffer->end();
    iter.backward_char();
    m_buffer->move_mark_by_name( PROMPT_MARK_NAME, iter );
    set_cursor( iter, false );
}

