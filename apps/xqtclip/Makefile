X_INCLUDE	= -I/opt/Embedix/tools/arm-linux/include
X_LIB		= -L/opt/Embedix/tools/arm-linux/lib -lX11

QTDIR		= /opt/Qtopia/sharp

CC	=	/opt/Embedix/tools/bin/arm-linux-gcc
CXX	=	/opt/Embedix/tools/bin/arm-linux-g++
CFLAGS	=	-pipe -Wall -W -O2 -DNO_DEBUG
CXXFLAGS=	-pipe -DQT_QWS_EBX -DQT_QWS_CUSTOM -DQWS -fno-exceptions -fno-rtti -Wall -W -O2 -DNO_DEBUG
INCPATH	=	-I$(QTDIR)/include $(X_INCLUDE)
LINK	=	$(CXX)
LFLAGS	=	
LIBS	=	$(SUBLIBS) -L$(QTDIR)/lib -lqte $(X_LIB)
MOC	=	$(QTDIR)/bin/moc
UIC	=	$(QTDIR)/bin/uic

TAR	=	tar -cf
GZIP	=	gzip -9f

####### Files

HEADERS =	
CSRCS   =	xevents.c xinit.c
CXXSRCS =	xqtclip.cc
OBJECTS =	$(CSRCS:.c=.o) $(CXXSRCS:.cc=.o) 

SRCMOC	=	moc_xqtclip.cc
OBJMOC	=	$(SRCMOC:.cc=.o)

DIST	=	
TARGET	=	xqtclip
INTERFACE_DECL_PATH = .

####### Implicit rules

.SUFFIXES: .cc .c

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

####### Build rules


all: $(TARGET)

$(TARGET): $(UICDECLS) $(OBJECTS) $(OBJMOC) 
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJMOC) $(LIBS)

moc: $(SRCMOC)

clean:
	-rm -f $(OBJECTS) $(OBJMOC) $(SRCMOC) $(UICIMPLS) $(UICDECLS) $(TARGET)
	-rm -f *~ core

#######
moc_xqtclip.cc: xqtclip.h
	$(MOC) xqtclip.h -o moc_xqtclip.cc


