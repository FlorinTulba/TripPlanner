################################################################################
#              Makefile for compiling TripPlanner with g++
#
# TripPlanner explores various issues common to navigation and booking systems.
#
# Copyrights from the libraries used by the program:
# - (c) 2017 Boost (www.boost.org)
#		License: <http://www.boost.org/LICENSE_1_0.txt>
#
# (c) 2017 Florin Tulba <florintulba@yahoo.com>
#		License: <http://www.gnu.org/licenses/agpl-3.0.txt>
################################################################################

# This Makefile uses the technique described in the following article:
# http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/

SOURCES = \
	connection.cpp \
	constraints.cpp \
	credentialsProvider.cpp \
	customDateTimeProcessor.cpp \
	dbSource.cpp \
	jsonSource.cpp \
	main.cpp \
	place.cpp \
	placeBase.cpp \
	planner.cpp \
	pricing.cpp \
	results.cpp \
	routeAlternative.cpp \
	routeSharedInfo.cpp \
	transpModes.cpp \
	util.cpp \
	variant.cpp \
	variants.cpp

COMPILER = g++
LINKER = g++

DEPDIR := .d
$(shell mkdir -p $(DEPDIR) >/dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

CXX_FLAGS = -std=c++14 -m64 -Ofast -Wall
COMPILE_FLAGS = -c $(CXX_FLAGS) $(DEPFLAGS) \
	-Wno-missing-declarations \
	-Wno-unknown-pragmas \
	-Wno-reorder
LINK_FLAGS = $(CXX_FLAGS)

SRC_DIR = ./src/
OUT_DIR = ./x64/$(COMPILER)/
TARGET = TripPlanner.exe

BOOST_DIR = /cygdrive/e/Work/VS/C_Cpp/FromOthers/boost/
INCLUDES = -I"$(SRC_DIR)" -I"$(BOOST_DIR)"
LIBS_DIRS = -L"$(BOOST_DIR)/cygwin/stage/lib/" -L"$(OUT_DIR)"
LIB_DEPS = -lboost_date_time -lboost_serialization \
	-lboost_system -lboost_filesystem

# The target depends on the `o` files generated in OUT_DIR
# from the corresponding `cpp` among SOURCES
$(OUT_DIR)$(TARGET) : $(patsubst %.cpp, $(OUT_DIR)%.o, $(SOURCES))
	@echo
	@echo ==== Linking the object files ====
	$(LINKER) $(LINK_FLAGS) -o $@ $(OUT_DIR)*.o $(LIBS_DIRS) $(LIB_DEPS)

# Command for compiling each unit. It displays the name of that unit.
define compileSrc =
	@echo
	@echo ---- Compiling \'$(notdir $(basename $<))\' ----
	$(COMPILER) $(COMPILE_FLAGS) $(INCLUDES) -o $@ $<
	@mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@
endef

$(OUT_DIR)%.o : $(SRC_DIR)%.cpp
$(OUT_DIR)%.o : $(SRC_DIR)%.cpp $(DEPDIR)/%.d
	$(compileSrc)

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(SOURCES))))

.PHONY : clean
clean :
	rm -f $(OUT_DIR)*.exe $(OUT_DIR)*.o
