TARGET = vm_riskxvii

CC = gcc

CFLAGS     = -c -Wall -Wvla -Werror -O2 -g -std=c11
ASAN_FLAGS = #-fsanitize=address
SRC        = main.c
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
