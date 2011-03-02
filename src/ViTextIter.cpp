#include "ViTextIter.h"

ViTextIter::ViTextIter( const Gtk::TextIter &iter ) : 
    Gtk::TextIter( iter )
{
}

ViTextIter::ViTextIter( const ViTextIter &iter ) : Gtk::TextIter( iter )
{
    if (iter == *this)
        return;
    *this = iter;

}

ViTextIter::~ViTextIter()
{
}


bool is_word_char(gunichar ch)
{
    const Glib::ustring WORD_SEP("<>(){}:.-*+/\\#");

    if (isspace(ch))
    {
        return false;
    }

    int idx = WORD_SEP.find( ch );
   
    bool retval = (idx == -1);

    //g_print("is_word_char(%c) = %i -> %s\n", ch,idx, (retval) ? "true" : "false");

    return retval;
}

Glib::ustring ViTextIter::get_word() const
{
    if (!is_word_char(get_char()))
    {
        g_print("Not on a word char.\n");
        return "";
    }
            
    ViTextIter start(*this);
    
    while(start.iter_next(GTK_DIR_LEFT))
    {
        if (!is_word_char(start.get_char()))
        {
            start++; 
            break;
        }
    }

    ViTextIter end(*this);

    while(end.iter_next(GTK_DIR_RIGHT))
    {
        if (!is_word_char(end.get_char()))
        {
            break;
        }
    }

    return start.get_text(end);
}

bool ViTextIter::iter_next(GtkDirectionType dir)
{
    bool retval = false;

    if (dir == GTK_DIR_LEFT)
    {
        backward_char();
        retval = is_start();
    }
    else if (dir == GTK_DIR_RIGHT)
    {
        forward_char();
        retval = is_end();
    }
    else if (dir == GTK_DIR_UP)
    {
        backward_line();
        retval = is_start();
    }
    else if (dir == GTK_DIR_DOWN)
    {
        forward_line();
        retval = is_end();
    }

    return !retval;
}

bool ViTextIter::find_next_non_whitespace(GtkDirectionType dir)
{
    while(iter_next(dir))
    {
        gunichar ch = this->get_char();
        if ( !isspace(ch) ) 
        {
            return true;
        }
    }
    return false;
}

bool ViTextIter::forward_next_word_start() 
{
    gunichar ch = get_char();
    bool found;

    if (is_word_char(ch))
    {
        while(iter_next(GTK_DIR_RIGHT))
        {
            if (!is_word_char(this->get_char()))
            {
                if (isspace(this->get_char()))
                    found = find_next_non_whitespace();
                break;
            }
        }
    }
    else
    {
        return find_next_non_whitespace();
    }
    return found;
}

bool ViTextIter::backward_next_word_start()
{
    g_print("backward_next_word_start\n");
    gunichar ch = get_char();

    //
    //  Backup one so that we aren't matching on the current word
    //
    if (!iter_next(GTK_DIR_LEFT))
        return false;

    if (!is_word_char(ch))
    {
        //
        //  Not a word char, so find the end of the word
        //
        if (!find_next_non_whitespace(GTK_DIR_LEFT))
        {
            return false;
        }
    }

    //
    //  Just find the start of the word
    //
    while(iter_next(GTK_DIR_LEFT))
    {
        if (!is_word_char(get_char()))
        {
            forward_char();                
            return true;
        }
    }

}

