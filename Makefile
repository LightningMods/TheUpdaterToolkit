WINE = wine
WINE_PATH_TOOL = winepath

CC = $(WINE) $(SCE_ORBIS_SDK_DIR)/bin/orbis-clang.exe
CXX = $(WINE) $(SCE_ORBIS_SDK_DIR)/bin/orbis-clang++.exe
LD = $(WINE) $(SCE_ORBIS_SDK_DIR)/bin/orbis-ld.exe
OBJCOPY = $(WINE) $(SCE_ORBIS_SDK_DIR)/bin/orbis-objcopy.exe
PUBCMD = $(WINE) $(SCE_ORBIS_SDK_DIR)/bin/orbis-pub-cmd.exe
MAKE_FSELF = $(SCE_ORBIS_SDK_DIR)/make_fself.py

OBJDIR = obj
BLDDIR = build
MODDIR = sce_module

TARGET = theupdatertoolkit
LIBS = -lkernel_tau_stub_weak -lSceSysmodule_tau_stub_weak -lSceSystemService_stub_weak -lSceSystemService_tau_stub_weak -lSceShellCoreUtil_tau_stub_weak -lScePigletv2VSH_tau_stub_weak -lkernel_util
SDK_MODULES =
EXTRA_MODULES =
AUTH_INFO = dc07c2ac03000036000000007000001000ff000000000002000000000000000000000000000000000000004000400040000000000000004003000000000080000040ffff000000f000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000

SDIR	:=	source

ASM_SRCS = $(wildcard $(SDIR)/*.s)
C_SRCS = $(wildcard $(SDIR)/*.c)

COMMON_FLAGS = -w
COMMON_FLAGS += -fdiagnostics-color=always
COMMON_FLAGS += -I include -I$(TAUON_SDK_DIR)/include -I$(SCE_ORBIS_SDK_DIR)/target/include
COMMON_FLAGS += -DNDEBUG
COMMON_FLAGS += -g

CFLAGS = $(COMMON_FLAGS)
CFLAGS += -std=c11
CFLAGS += -Wno-unused-function -Wno-unused-label -Werror=implicit-function-declaration
CFLAGS += -fno-strict-aliasing
CFLAGS += -fPIC
CFLAGS += -O3

ASFLAGS = $(COMMON_FLAGS)

LDFLAGS = -s -Wl,--addressing=non-aslr,--strip-unused-data
LDFLAGS += -L $(TAUON_SDK_DIR)/lib -L $(SCE_ORBIS_SDK_DIR)/target/lib

OBJS = $(addprefix $(OBJDIR)/,$(ASM_SRCS:.s=.s.o) $(C_SRCS:.c=.c.o))

.PHONY: all clean

all: post-build

pre-build:
	@mkdir -p $(MODDIR) $(OBJDIR) $(BLDDIR)
	@for filename in $(SDK_MODULES); do \
		if [ ! -f "$(MODDIR)/$$filename" ]; then \
			echo Copying $$filename...; \
			cp "`$(WINE_PATH_TOOL) -u \"$(SCE_ORBIS_SDK_DIR)/target/sce_module/$$filename\"`" $(MODDIR)/; \
		fi; \
	done;
	@for filename in $(EXTRA_MODULES); do \
		if [ ! -f "$(MODDIR)/$$filename" ]; then \
			echo Copying $$filename...; \
			cp "extra/$$filename" $(MODDIR)/; \
		fi; \
	done;

post-build: main-build

main-build: pre-build
	@$(MAKE) --no-print-directory pkg

eboot: pre-build $(OBJS)
	$(CC) $(LDFLAGS) -o $(BLDDIR)/$(TARGET).elf $(OBJS) $(LIBS)

$(OBJDIR)/%.s.o: %.s
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) -c $< -o $@

$(OBJDIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

sfo:
	$(PUBCMD) sfo_create sce_sys/param.sfx $(BLDDIR)/param.sfo

pkg: sfo eboot
	$(MAKE_FSELF) --auth-info $(AUTH_INFO) $(BLDDIR)/$(TARGET).elf $(BLDDIR)/$(TARGET).self
	$(PUBCMD) img_create $(TARGET).gp4 $(BLDDIR)/$(TARGET).pkg

clean:
	@rm -rf $(OBJDIR) $(BLDDIR)
