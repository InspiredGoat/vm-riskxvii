TARGET = vm_riskxvii

CC = gcc

CFLAGS     = -c -Os #-Wall -Wvla -Werror
ASAN_FLAGS = #-fsanitize=address
SRC        = main.c
OBJ        = $(SRC:.c=.o)

all:$(TARGET)

$(TARGET):$(OBJ)
	$(CC) $(ASAN_FLAGS) -o $@ $(OBJ)
	strip vm_riskxvii

.SUFFIXES: .c .o

.c.o:
	 $(CC) $(CFLAGS) $(ASAN_FLAGS) $<

try: all
	./$(TARGET)

again: clean all
	echo done

tests: all
	$(CC) $(CFLAGS) $(ASAN_FLAGS) gentests.c
	./a.out

run_tests: tests
	./runtests.sh

clean:
	rm -f *.o *.obj $(TARGET)
