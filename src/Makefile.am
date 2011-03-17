
AM_CFLAGS = -Wall -g -O0

bin_PROGRAMS = sourcerer

sourcerer_SOURCES =  main.cc \
					 App.cpp \
					 MainWindow.cpp \
					 actions.cpp \
					 utils.cpp \
					 Vi.cpp \
					 ViKeyManager.cpp \
					 ViMotionAction.cpp \
					 ViTextIter.cpp \
					 ViCommandMode.cpp \
					 ViNormalMode.cpp \
					 ViInsertMode.cpp \
					 Editor.cpp \
					 EditorArea.cpp \
					 Search.cpp \
					 s7.c \
					 ReplWindow.cpp


sourcerer_LDFLAGS = 

AM_CPPFLAGS = $(GTKMM_CFLAGS)
sourcerer_LDADD = $(GTKMM_LIBS) 
