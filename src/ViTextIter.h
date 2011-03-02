#ifndef VI_TEXT_ITER_H
#define VI_TEXT_ITER_H

#include <gtkmm.h>


class ViTextIter : public Gtk::TextIter
{
    public:
        ViTextIter( const Gtk::TextIter &iter );
        ViTextIter( const ViTextIter &iter );

        virtual ~ViTextIter();

        bool iter_next(GtkDirectionType dir);

        bool find_next_non_whitespace(GtkDirectionType dir = GTK_DIR_RIGHT);

        virtual Glib::ustring get_word() const;

        virtual bool forward_next_word_start();
        virtual bool backward_next_word_start();

};

#endif
