GIT ?= git

GENSTONE_DIR = implementation/Genstone
CIONOM_DIR = .

include $(GENSTONE_DIR)/build/common.mk

MODULES = $(GENSTONE_DIR)/genstone/gentests.mk $(GENSTONE_DIR)/genstone/gencore.mk
MODULE_NAMES = $(subst $(GENSTONE_DIR)/genstone/,,$(subst .mk,,$(MODULES)))
CLEAN_TARGETS = $(addprefix clean_,$(MODULE_NAMES)) clean_common
TEST_TARGETS = $(addprefix test_,$(MODULE_NAMES))

include $(MODULES) $(CIONOM_DIR)/implementation/cionom.mk

.PHONY: all
.DEFAULT_GOAL := all
all: $(MODULE_NAMES) cionom

.PHONY: clean
clean: $(CLEAN_TARGETS) clean_cionom

.PHONY: test
test: all $(TEST_TARGETS) test_cionom
