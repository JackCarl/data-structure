.PHONY: all clean install

############################################
ROOT := $(shell /bin/pwd)/
export ROOT
############################################
-include $(ROOT)/mconfig
############################################
RM = rm
CP = cp
RMFLAGS = -fr
#PACK    = packtool 
############################################
RMS  = $(ROOT)/bin
############################################
DIRS := $(addprefix $(ROOT)/, $(SUB_DIRS)) 

############################################

all:
	@set -e; \
	for dir in ${DIRS}; \
	do \
		cd $$dir && $(MAKE) ; \
	done
	@echo ""
	@echo "=======>all Completed<========="
	@echo ""

############################################
clean:
	@set -e; \
	for dir in $(DIRS); \
	do \
		cd $$dir && $(MAKE) clean;\
	done
	$(RM) $(RMFLAGS) $(RMS)
	@echo ""
	@echo "=======>Clean OK<========="
	@echo ""

############################################
install:
	@${INSTALL_SCRIPT}
	@echo  ""
	@echo "=======>Install OK<========="
	@echo ""

