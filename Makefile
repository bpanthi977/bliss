# Declaration of variables
CC = gcc
CC_FLAGS = -w 
LINKER_FLAGS =

# File names
EXEC = bin/bliss
SOURCES = main.c env.c eval.c glist.c parser.c stdlibrary.c print.c dtypes.c debug.c
OBJECTS = main.o env.o eval.o glist.o parser.o stdlibrary.o print.o dtypes.o debug.o 
HEADERS = env.h eval.h glist.h parser.h stdlibrary.h print.h dtypes.h debug.h
CODEFILES = $(SOURCES) $(HEADERS)

#ALL
all: bindir $(EXEC)

#Create necessary directories	
bindir:
	mkdir -p bin/


# Main target
$(EXEC): $(OBJECTS)
	$(CC) $(CC_FLAGS) $(addprefix bin/, $(OBJECTS)) $(LINKER_FLAGS) -o $(EXEC)

run :
	$(EXEC)

# To compile 
%.o: %.c
	$(CC) -c $(CC_FLAGS) $(@:%.o=%.c) -o bin/$@

# To link object file to standalone exec files
link-%:
	$(CC) $(CC_FLAGS) $(addprefix bin/, $(OBJECTS)) $(LINKER_FLAGS) -o bin/$(@:link-%=%)

# To remove generated files
clean:
#rm -f $(EXEC) $(addprefix bin/, $(OBJECTS))
	rm -f -r bin/	
	rm -f $(CBPFILE:%.cbp=%.zip)

