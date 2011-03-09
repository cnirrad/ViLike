
#include "Search.h"
#include "ViTextIter.h"

SearchSupport::SearchSupport()
{

}

SearchSupport::~SearchSupport()
{
    m_matches.clear();
}

void SearchSupport::set_buffer( Glib::RefPtr<Gtk::TextBuffer> buffer )
{
    m_buffer = buffer;
}

bool SearchSupport::search( const Glib::ustring &pattern,
                            ViTextIter &start,
                            Direction direction )
{
    if (m_buffer == NULL)
    {
        g_print("ERROR: Buffer not set.\n");
        return false;
    }

    if (!find_all(pattern))
    {
        g_print("No matches found.\n");
        return false;
    }

    std::list< MatchInfo* >::iterator end;

    if (direction == Forward)
    {
        std::list< MatchInfo* >::iterator it;
        for ( it = m_matches.begin(); it != m_matches.end(); it++)
        {
            if ((*it)->start_pos > start.get_offset())
            {
                g_print("Matched! current position = %i\n", start.get_offset());
                start.set_offset((*it)->start_pos);
                return true;
            }
        }
    }
    else
    {
        std::list< MatchInfo* >::reverse_iterator it;
        for ( it = m_matches.rbegin(); it != m_matches.rend(); it++)
        {
            if ((*it)->start_pos < start.get_offset())
            {
                g_print("Matched! current position = %i\n", start.get_offset());
                start.set_offset((*it)->start_pos);
                return true;
            }
        }
    }

    g_print("Match not found. current cursor position = %i\n", start.get_offset());
    return false;
}

//
// Protected
//
bool
SearchSupport::find_all( const Glib::ustring &pattern )
{
    m_matches.clear();

    Glib::ustring text = m_buffer->get_text();

    GRegex *regex;
    GMatchInfo *match_info;
    GRegexCompileFlags compile_flags;
    GRegexMatchFlags match_options;
    GError *error = NULL;

    regex = g_regex_new( pattern.data(), 
                         compile_flags, 
                         match_options, &error );

    if ( error )
    {
        g_print("Error while creating regex: %s\n", error->message);
        return false;
    }

    
    if (!g_regex_match( regex, text.data(), 
                        match_options, &match_info ))
    {
        return false;
    }

    gint start, end;

    int idx=0;
    while (g_match_info_matches(match_info))
    {
        if (!g_match_info_fetch_pos( match_info, 0, &start, &end ))
        {
            m_matches.clear();
            return false;
        }

        MatchInfo *info = new MatchInfo();
        info->start_pos = start;
        info->end_pos = end;
        m_matches.push_back( info );

        g_match_info_next( match_info, NULL ); 
    }

    g_match_info_free( match_info );
    g_regex_unref( regex );

    return true;
}
