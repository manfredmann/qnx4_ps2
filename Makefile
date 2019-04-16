TARGET = ps2
SRCDIR = src/
SRCS = main.cpp
BINDIR = 386r/
OBJDIR = obj/
OBJS = $(SRCS:.cpp=.o)
CC = cc
INCLUDEDIRS += ./inc
LIBS = plib

CFLAGS = -WC,-xs -5 -ms \
					$(addprefix -I, $(INCLUDEDIRS))
LDFLAGS = -T1 -ms -M \
		  		$(addprefix -l, $(LIBS))

USAGEFILE = $(TARGET).use

.PHONY:     			all clean

all:					$(BINDIR)$(TARGET)

clean:
						-rm -f $(OBJDIR)*.o $(BINDIR)$(TARGET) $(BINDIR)*.map *.err 

$(OBJDIR)%.o : $(SRCDIR)%.cpp
						$(CC) $(CFLAGS) -c -o $@ $<

$(BINDIR)$(TARGET): 	$(addprefix $(OBJDIR), $(OBJS))
						$(CC) $(LDFLAGS) -o $@ $^
						-usemsg $@ $(USAGEFILE)
