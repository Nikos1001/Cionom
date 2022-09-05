GENSTONE_DIR = implementation/Genstone
CIONOM_DIR = .

include $(GENSTONE_DIR)/build/common.mk
include $(CIONOM_DIR)/implementation/cionom.mk

.PHONY: all
.DEFAULT_GOAL := all
all: $(MODULE_NAMES) cionom

.PHONY: clean
clean: $(CLEAN_TARGETS) clean_cionom

.PHONY: test
test: all $(TEST_TARGETS) test_cionom

documentation: all # TODO: Custom CSS on Doxygen
	doxygen build/documentation/Doxyfile
