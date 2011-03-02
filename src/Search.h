#ifndef SOURCERER_SEARCH_H
#define SOURCERER_SEARCH_H

#include <gtkmm.h>
#include "ViTextIter.h"

class SearchSupport
{
    public:
        SearchSupport(Glib::RefPtr<Gtk::TextBuffer> buffer);
        virtual ~SearchSupport();

        /**
         * Direction:
         * @Forward: search forward
         * @Backward: search backward
         *
         * Enum used to define the direction of the search.
         */
        enum Direction 
        {
            Forward,
            Backward
        };

        /**
         * search:
         * @pattern: a regular expression
         * @start: a text iterator that contains the position to
         * start search from. When a match is found, this will be
         * updated to the start of the first match.
         * @direction: the direction to search.
         *
         * Performs a search for the given pattern. @start is updated
         * to the first match found from the position of the passed in
         * @start iterator.
         *
         * Returns: a bool that will be true if a match is found,
         * false otherwise.
         */
        bool search( const Glib::ustring &pattern,
                     ViTextIter &start,
                     Direction direction);

    protected:
        /**
         *  Finds all matches for the given pattern. The matches
         *  are stored in the m_matches member.
         */
        bool find_all( const Glib::ustring &pattern );


        /**
         * A reference to the TextBuffer
         */
        Glib::RefPtr<Gtk::TextBuffer> m_buffer;

        /**
         *  A struct that holds information about each match.
         */
        struct MatchInfo
        {
            int start_pos;
            int end_pos;
        };

        /**
         *  A list of matches from the last search.
         */
        std::list< MatchInfo* > m_matches;
};


#endif
