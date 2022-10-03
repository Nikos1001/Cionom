CIONOM_LIB_SOURCES = $(wildcard $(CIONOM_DIR)/implementation/cionom/*.c)
CIONOM_LIB_OBJECTS = $(CIONOM_LIB_SOURCES:.c=$(OBJECT_SUFFIX))
CIONOM_LIB = $(CIONOM_DIR)/lib/$(LIB_PREFIX)cionom$(DYNAMIC_LIB_SUFFIX)

CIONOM_EXEC_SOURCES = $(wildcard $(CIONOM_DIR)/implementation/cli/*.c)
CIONOM_EXEC_OBJECTS = $(CIONOM_EXEC_SOURCES:.c=$(OBJECT_SUFFIX))
CIONOM_EXEC = $(CIONOM_DIR)/cionom-cli$(EXECUTABLE_SUFFIX)

CIONOM_EXTERNAL_SOURCES = $(wildcard $(CIONOM_DIR)/implementation/external/*.c)
CIONOM_EXTERNAL_OBJECTS = $(CIONOM_EXTERNAL_SOURCES:.c=$(OBJECT_SUFFIX))
CIONOM_EXTERNAL = $(CIONOM_DIR)/lib/$(LIB_PREFIX)cionom-external$(DYNAMIC_LIB_SUFFIX)

ifeq ($(SANITIZERS),ENABLED)
# TODO: Report cfisan issue and re-enable here
	CIONOM_COMMON_CFLAGS = -fsanitize=undefined,address
	CIONOM_COMMON_LFLAGS = -fsanitize=undefined,address
endif

CIONOM_LIB_CFLAGS = $(GEN_CORE_CFLAGS) -I$(CIONOM_DIR)/implementation/cionom/include
CIONOM_LIB_LFLAGS = $(GEN_CORE_LFLAGS) -lcionom
CIONOM_LIB_LIBDIRS = $(GEN_CORE_LIBDIRS) $(CIONOM_DIR)/lib

CIONOM_DIAGNOSTIC_CFLAGS = $(GEN_CORE_DIAGNOSTIC_CFLAGS) -Wno-gnu-binary-literal

$(CIONOM_DIR)/lib:
	@$(ECHO) "$(ACTION_PREFIX)$(MKDIR) $@$(ACTION_SUFFIX)"
	-@$(MKDIR) $@

$(CIONOM_LIB): CFLAGS = $(GEN_CORE_CFLAGS) $(CIONOM_DIAGNOSTIC_CFLAGS) $(CIONOM_COMMON_CFLAGS)
$(CIONOM_LIB): LFLAGS = $(GEN_CORE_LFLAGS) $(CIONOM_COMMON_LFLAGS)
$(CIONOM_LIB): LIBDIRS = $(CIONOM_LIB_LIBDIRS)
$(CIONOM_LIB): $(CIONOM_LIB_OBJECTS) $(GEN_CORE_LIB) | $(CIONOM_DIR)/lib

$(CIONOM_EXEC): CFLAGS = $(CIONOM_LIB_CFLAGS) $(CIONOM_DIAGNOSTIC_CFLAGS) $(CIONOM_COMMON_CFLAGS) -DCIO_CLI_VERSION="\"@$(shell $(GIT) rev-parse --short HEAD)\""
$(CIONOM_EXEC): LFLAGS = $(CIONOM_LIB_LFLAGS) $(CIONOM_COMMON_LFLAGS)
$(CIONOM_EXEC): LIBDIRS = $(CIONOM_LIB_LIBDIRS)
$(CIONOM_EXEC): $(CIONOM_EXEC_OBJECTS) $(CIONOM_LIB) $(CIONOM_EXTERNAL)

$(CIONOM_EXTERNAL): CFLAGS = $(CIONOM_LIB_CFLAGS) $(CIONOM_DIAGNOSTIC_CFLAGS) $(CIONOM_COMMON_CFLAGS)
$(CIONOM_EXTERNAL): LFLAGS = $(CIONOM_LIB_LFLAGS) $(CIONOM_COMMON_LFLAGS)
$(CIONOM_EXTERNAL): LIBDIRS = $(CIONOM_LIB_LIBDIRS)
$(CIONOM_EXTERNAL): $(CIONOM_EXTERNAL_OBJECTS) $(CIONOM_LIB)

.PHONY: cionom
cionom: $(CIONOM_LIB) $(CIONOM_EXEC) $(CIONOM_EXTERNAL)

.PHONY: test_cionom
test_cionom: cionom

.PHONY: clean_cionom
clean_cionom:
	@$(ECHO) "$(ACTION_PREFIX)"
	-$(RM) $(CIONOM_LIB_OBJECTS)
	-$(RM) $(CIONOM_LIB)
	-$(RM) $(CIONOM_EXEC_OBJECTS)
	-$(RM) $(CIONOM_EXEC)
	-$(RM) $(CIONOM_EXTERNAL_OBJECTS)
	-$(RM) $(CIONOM_EXTERNAL)
	@$(ECHO) "$(ACTION_SUFFIX)"
