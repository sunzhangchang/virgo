OUT_DIR=build
SRC_DIR=src
PROJECT=$(notdir $(SRC_DIR))
include $(SRC_DIR)/Makefile
OBJS=$(addprefix $(OUT_DIR)/,$(SRCS:.cpp=.o))

CFLAGS=-std=c++14 -pedantic -Wall -O3 -nostdlib -fno-builtin -fno-asynchronous-unwind-tables -fno-ident -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-pie
LIBS=-lgdi32 -luser32 -lshell32 -lkernel32
LDFLAGS=-static -nostdlib -fno-builtin -s -Wl,-e,__main,-subsystem,windows $(LIBS)
ARCH=32
ifeq ($(ARCH), 64)
	WINDRES_ARCH=pe-x86-64
else
	WINDRES_ARCH=pe-i386
endif
NAME=virgo
EXE=$(OUT_DIR)/$(NAME).exe
RES=$(OUT_DIR)/$(NAME).res

.PHONY: all clean

all:$(OUT_DIR) $(EXE)

$(OUT_DIR):
	mkdir $@

$(EXE): $(OBJS) $(RES)
	$(CXX) -o $(EXE) $(OBJS) $(RES) -m$(ARCH) $(LDFLAGS)

$(RES):
	windres -O coff -F $(WINDRES_ARCH) $(SRC_DIR)/$(NAME).rc $(RES) 

$(OBJS): $(addprefix $(SRC_DIR)/,$(SRCS))
	$(CXX) -o $@ $(CFLAGS) -m$(ARCH) -c $^

clean:
	rm -rf $(OUT_DIR)

# .PHONY: all clean
# all: $(EXE)
# $(EXE): $(OBJS) $(NAME).res
# 	$(CXX) -o $(EXE) $(OBJS) $(NAME).res -m$(ARCH) $(LDFLAGS)

# $(NAME).res: $(NAME).rc
# 	windres -O coff -F $(WINDRES_ARCH) $(NAME).rc $(NAME).res 

# .cpp.o:
# 	$(CXX) -o $@ $(CFLAGS) -m$(ARCH) -c $<

# clean:
# 	rm -f $(OBJS) $(EXE) $(NAME).res

# format:
# 	astyle --mode=cpp --style=kr --indent=tab --pad-header --delete-empty-lines --align-pointer=name --max-code-length=80 virgo.cpp
