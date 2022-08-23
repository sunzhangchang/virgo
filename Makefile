SRCS=virgo.cpp
OBJS=$(SRCS:.cpp=.o)
CFLAGS=-std=c++14 -pedantic -Wall -O3 -fno-builtin -fno-asynchronous-unwind-tables -fno-ident -ffunction-sections -fdata-sections
LIBS=-lgdi32 -luser32 -lshell32 -lkernel32
LDFLAGS=-static -fno-builtin -s -Wl,-subsystem,windows $(LIBS)
ARCH=32
ifeq ($(ARCH), 64)
	WINDRES_ARCH=pe-x86-64
else
	WINDRES_ARCH=pe-i386
endif
NAME=virgo
EXE=$(NAME).exe

.PHONY: all clean
all: $(EXE)
$(EXE): $(OBJS) $(NAME).res
	$(CXX) -o $(EXE) $(OBJS) $(NAME).res -m$(ARCH) $(LDFLAGS)

$(NAME).res: $(NAME).rc
	windres -O coff -F $(WINDRES_ARCH) $(NAME).rc $(NAME).res 

.cpp.o:
	$(CXX) -o $@ $(CFLAGS) -m$(ARCH) -c $<

clean:
	rm -f $(OBJS) $(EXE) $(NAME).res

format:
	astyle --mode=cpp --style=kr --indent=tab --pad-header --delete-empty-lines --align-pointer=name --max-code-length=80 virgo.cpp
