### Project name (also used for output file name)
NAME = release
### Target device
DEVICE  = atmega168

### Source files and search directory
CSRC    := src/hal/display.c src/hal/iic/iic.c src/hal/avr/mcu.c
CSRC 	+= src/hal/avr/adc.c src/hal/avr/pwmi.c src/hal/avr/spi.c
CPPSRC  :=
ASRC    =

ifeq ($(MAKECMDGOALS), test_display)
CSRC += src/tests/01-display.c
NAME =test_display
else
ifeq ($(MAKECMDGOALS), test_pwm)
CSRC += src/tests/02-PWM.c
NAME =test_pwm
else
CSRC 	+= src/alarm.c src/hal/rtc/chip_m41t56.c src/hal/rtc/service.c
CSRC 	+= src/tms/tms.c src/user.c
CPPSRC 	+= src/main.cpp src/show/alarm.cpp src/show/show.cpp src/show/date.cpp
CPPSRC 	+= src/show/error.cpp src/show/intro.cpp src/show/set.cpp
CPPSRC 	+= src/show/set_alarm.cpp src/show/set_time.cpp src/show/set_date.cpp
CPPSRC 	+= src/show/set_caliber.cpp src/show/time.cpp
ifeq ($(MAKECMDGOALS), debug)
NAME =debug
endif
endif
endif

### Optimization level (0, 1, 2, 3, 4 or s)
OPTIMIZE = s

### C Standard level (c89, gnu89, c99 or gnu99)
CSTD = gnu99

### C++ Standard level (-std=c++98, -std=gnu++98 or -ansi)
CPPSTD = -std=gnu++98

### Include dirs, library dirs and definitions
LIBS	=
LIBDIRS	=
INCDIRS	=
DEFS	= F_CPU=16000000UL __RTC_M41T56__
ADEFS	= $(DEFS)

### Warning contorls
WARNINGS = extra #all

### Output directory
OUTDIR = out
OBJDIR = $(OUTDIR)/$(NAME)
BINDIR = $(OUTDIR)/$(NAME)

### Output file format (ihex, bin or both) and debugger type
OUTPUT	= ihex
DEBUG	= dwarf-2


### Programs to build porject
CC      = avr-gcc
CCPP    = avr-g++
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE    = avr-size
NM      = avr-nm


# Define all object files
COBJ      := $(CSRC:.c=.o)
CPPOBJ    := $(CPPSRC:.cpp=.o)
AOBJ      := $(ASRC:.S=.o)
COBJ      := $(addprefix $(OBJDIR)/,$(COBJ))
CPPOBJ    := $(addprefix $(OBJDIR)/,$(CPPOBJ))
AOBJ      := $(addprefix $(OBJDIR)/,$(AOBJ))
BUILD     := $(OUTDIR)/$(NAME)_$(DEVICE)


# Flags for C & C++ files
CCFLAGS += -g$(DEBUG)
CCFLAGS += -mmcu=$(DEVICE)
CCFLAGS += -O$(OPTIMIZE)
CCFLAGS += $(addprefix -W,$(WARNINGS))
CCFLAGS += $(addprefix -I,$(INCDIRS))
CCFLAGS += $(addprefix -D,$(DEFS))
CCFLAGS += -Wp,-MD,-MP,-MT,$@,-MF,$@.d
CCFLAGS += -fpack-struct -fshort-enums -ffunction-sections -fdata-sections
CCFLAGS += -fstack-usage -Wstack-usage=4 -Wframe-larger-than=4
CCFLAGS += -Wmissing-declarations
ifeq ($(NAME), release)
CCFLAGS += -mcall-prologues -flto
endif


# Flags for C files
CFLAGS += $(CCFLAGS)
CFLAGS += -std=$(CSTD)
CFLAGS += -Wstrict-prototypes


# Flags for C++ files
CPPFLAGS += $(CCFLAGS)
CPPFLAGS += $(CPPSTD)
CPPFLAGS += -fno-exceptions


# Assembler flags
ASFLAGS += $(addprefix -D,$(ADEFS)) -Wa,-gstabs,-g$(DEBUG)
ALL_ASFLAGS = -mmcu=$(DEVICE) -I. -x assembler-with-cpp $(ASFLAGS)


# Linker flags
LDFLAGS += -Wl,-Map,$(BUILD).map -mmcu=$(DEVICE)
ifeq ($(NAME), release)
LDFLAGS += -Wl,--gc-sections -mrelax
endif

# Default target.
all: mkdir version build size

debug: mkdir version build size
test_display: mktestdir version build size
test_pwm: mktestdir version build size

ifeq ($(OUTPUT),ihex)
build: elf hex lst sym
hex: $(BUILD).hex
else
ifeq ($(OUTPUT),binary)
build: elf bin lst sym
bin: $(BUILD).bin
else
ifeq ($(OUTPUT),both)
build: elf hex bin lst sym
hex: $(BUILD).hex
bin: $(BUILD).bin
else
$(error "Invalid format: $(OUTPUT)")
endif
endif
endif

mkdir: mkcommondir
	$(shell mkdir -p $(OBJDIR)/src/hal/rtc)
	$(shell mkdir -p $(OBJDIR)/src/show)
	$(shell mkdir -p $(OBJDIR)/src/tms)

mkcommondir:
	$(shell mkdir -p $(OBJDIR)/src/hal/avr)
	$(shell mkdir -p $(OBJDIR)/src/hal/iic)
	$(shell mkdir -p $(BINDIR))

mktestdir: mkcommondir
	$(shell mkdir -p $(OBJDIR)/src/tests)

elf: $(BUILD).elf
lst: $(BUILD).lst
sym: $(BUILD).sym


# Display compiler version information.
version :
	@$(CC) --version

# Create final output file (.hex or .bin) from ELF output file.
%.hex: %.elf
	@echo
	$(OBJCOPY) -O ihex $< $@

%.bin: %.elf
	@echo
	$(OBJCOPY) -O binary $< $@

# Create extended listing file from ELF output file.
%.lst: %.elf
	@echo
	$(OBJDUMP) -h -S -C $< > $@

# Create a symbol table from ELF output file.
%.sym: %.elf
	@echo
	$(NM) -n $< > $@

# Display size of file.
size:
	@echo
	$(SIZE) -C --mcu=$(DEVICE) $(BUILD).elf


# Link: create ELF output file from object files.
%.elf:  $(AOBJ) $(COBJ) $(CPPOBJ)
	@echo
	@echo Linking...
	$(CCPP) $(LDFLAGS) -o"$@" $(AOBJ) $(COBJ) $(CPPOBJ)

# Compile: create object files from C source files. ARM or Thumb(-2)
$(COBJ) : $(OBJDIR)/%.o : %.c
	@echo
	@echo $< :
	$(CC) -c $(CFLAGS) $< -o $@

# Compile: create object files from C++ source files. ARM or Thumb(-2)
$(CPPOBJ) : $(OBJDIR)/%.o : %.cpp
	@echo
	@echo $< :
	$(CCPP) -c $(CPPFLAGS) $< -o $@

# Assemble: create object files from assembler source files. ARM or Thumb(-2)
$(AOBJ) : $(OBJDIR)/%.o : %.S
	@echo
	@echo $< :
	$(CC) -c $(ALL_ASFLAGS) $< -o $@


# Target: clean project.
clean:
	@echo
	rm -f -r $(OUTDIR)

# Include the dependency files.
include $(wildcard $(OBJDIR)/*.d)
