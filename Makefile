TARGET = vm_riskxvii

CC = gcc

CFLAGS     = -c -Wvla -std=c11 -Werror -Wall
COMPRESSION_FLAGS = -flto -s -fno-exceptions -fdata-sections -O1
DEBUG_FLAGS = -fsanitize=address -g
SRC        = vm_riskxvii.c vm_instructions.c vm_core.c heapbank.c utilities.c
OBJ        = $(SRC:.c=.o)
TESTS = funny_fish double no_file

all:$(TARGET)

$(TARGET):$(OBJ)
	$(CC) -o $@ $(OBJ) $(COMPRESSION_FLAGS)

.SUFFIXES: .c .o

.c.o:
	 $(CC) $(CFLAGS) $(COMPRESSION_FLAGS) $<

run:
	./$(TARGET)

tests:

run_tests:$(TESTS)

$(TESTS):
	./vm_riskxvii tests/$@.mi < tests/$@.in | diff tests/$@.out -

clean:
	rm -f *.o *.obj *.gcno $(TARGET)