OUT_DIR:=build
SRC_DIR:=src

SRC_FILE := $(wildcard *.cpp)
SRC_FILE += $(wildcard $(SRC_DIR)/*.cpp)

OBJS := $(subst $(SRC_DIR), $(OUT_DIR), $(patsubst %.cpp, %.o, $(SRC_FILE)))

PROJECT=$(notdir $(SRC_DIR))

CFLAGS=-std=c++20 -pedantic -Wall -O3 -nostdlib -fno-builtin -fno-asynchronous-unwind-tables -fno-ident -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-pie
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
	
$(OUT_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) -o $@ $(CFLAGS) -m$(ARCH) -c $^
	
$(RES):
	windres -O coff -F $(WINDRES_ARCH) $(SRC_DIR)/$(NAME).rc $(RES) 

$(EXE): $(OBJS) $(RES)
	$(CXX) -o $(EXE) $(OBJS) $(RES) -m$(ARCH) $(LDFLAGS)

clean:
	rm -rf $(OUT_DIR)

rebuild: clean all

target1:
	@echo $(OBJS)

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
