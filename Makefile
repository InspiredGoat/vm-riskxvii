TARGET = vm_riskxvii

CC = gcc

CFLAGS     = -DDEBUG_PRINT_INSTRUCTIONS -c -Wall -Wvla -Werror -O0 -g -std=c11
ASAN_FLAGS = -fsanitize=address
SRC        = main.c util.c instruction.c memory.c
OBJ        = $(SRC:.c=.o)

all:$(TARGET)

$(TARGET):$(OBJ)
	$(CC) $(ASAN_FLAGS) -o $@ $(OBJ)

.SUFFIXES: .c .o

.c.o:
	 $(CC) $(CFLAGS) $(ASAN_FLAGS) $<

try: all
	./$(TARGET)

again: clean all
	./$(TARGET)

test:
	gcc 

clean:
	rm -f *.o *.obj $(TARGET)
