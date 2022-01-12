CIONOM_ROOT_DIR = ../..

CIONOM_LIB_SOURCES = $(wildcard $(CIONOM_ROOT_DIR)/implementation/cionom/*.c)
CIONOM_LIB_OBJECTS = $(CIONOM_LIB_SOURCES:.c=.o)
CIONOM_LIB = lib/$(LIB_PREFIX)cionom$(DYNAMIC_LIB_SUFFIX)

CIONOM_EXEC_SOURCES = $(wildcard $(CIONOM_ROOT_DIR)/implementation/cli/*.c)
CIONOM_EXEC_OBJECTS = $(CIONOM_EXEC_SOURCES:.c=.o)
CIONOM_EXEC = $(CIONOM_ROOT_DIR)/cionom-cli$(EXECUTABLE_SUFFIX)

CIONOM_LIB_CFLAGS = $(GEN_CORE_CFLAGS) -I$(CIONOM_ROOT_DIR)/implementation/cionom/include
CIONOM_LIB_LFLAGS = $(GEN_CORE_LFLAGS) -lcionom

build_message_cionom:
	@$(ECHO) "$(SECTION_PREFIX) Cíonom"
	@$(ECHO) "$(INFO_PREFIX) Cíonom Reference Implementation"

cionom: build_message_cionom $(CIONOM_EXEC) ### @Cíonom Builds the Cíonom reference implementation

$(CIONOM_LIB): CFLAGS = $(GEN_CORE_CFLAGS)
$(CIONOM_LIB): LFLAGS = -Llib $(GEN_CORE_LFLAGS)
$(CIONOM_LIB): $(CIONOM_LIB_OBJECTS)
$(CIONOM_LIB_OBJECTS): $(GEN_CORE_LIB)

$(CIONOM_EXEC): CFLAGS = $(CIONOM_LIB_CFLAGS)
$(CIONOM_EXEC): LFLAGS = -Llib $(CIONOM_LIB_LFLAGS)
$(CIONOM_EXEC): $(CIONOM_EXEC_OBJECTS)
$(CIONOM_EXEC_OBJECTS): $(CIONOM_LIB)

clean_cionom:
	@$(ECHO) "$(ACTION_PREFIX)"
	-rm $(CIONOM_OBJECTS)
	-rm $(CIONOM_EXEC)
	@$(ECHO) "$(ACTION_SUFFIX)"
