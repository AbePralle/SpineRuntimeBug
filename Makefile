SPINE_C_FOLDER = ../spine-runtimes/spine-c

SPINE_INCLUDE  = $(SPINE_C_FOLDER)/spine-c/include
SPINE_SRC      = $(SPINE_C_FOLDER)/spine-c/src/spine

CC=gcc
CFLAGS = -c -I$(SPINE_INCLUDE)

SPINE_H_FILES = $(shell find $(SPINE_INCLUDE)/spine/*.h)
SPINE_C_FILES = $(shell find $(SPINE_SRC)/*.c)

O_FILES = Build/BugDemo.o $(SPINE_C_FILES:$(SPINE_SRC)/%.c=Build/%.o)

all: configure build run

configure:
	mkdir -p Build
	@if [ ! -e "$(SPINE_C_FOLDER)/spine-c/include/spine/spine.h" ]; then \
	  echo "================================================================================" ; \
	  echo "spine-c runtime not found; please adjust SPINE_C_FOLDER path in Makefile" ; \
	  echo "================================================================================" ; \
		false; \
	fi

build: bugdemo

bugdemo: $(O_FILES)
	gcc $(O_FILES) -o bugdemo

Build/BugDemo.o: BugDemo.c $(SPINE_H_FILES)
	$(CC) $< $(CFLAGS) -o $@

Build/%.o: $(SPINE_SRC)/%.c $(SPINE_H_FILES)
	$(CC) $< $(CFLAGS) -o $@

run:
	./bugdemo

clean:
	rm -rf Build
