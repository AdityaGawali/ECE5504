CC = g++

CFLAGS = -Wall -Werror -w -std=c11 -march=native -O3 -D_POSIX_C_SOURCE=199309L

BIN = final_project

SRC = src/main.cpp              \
      lib/block.cpp            \
      lib/page.cpp              \
      lib/dump.cpp

OBJS = src/main.o             \
       lib/block.o           \
       lib/page.o             \
       lib/dump.o


.SUFFIXES: .o .cpp

$(BIN): $(OBJS)
	$(CC) $(OBJS) -o $(BIN)

.c.o:
	$(CC) -Iinclude -c $< -o $@


run: $(BIN)
	@$(MAKE) && ./$(BIN)

clean:
	rm -f $(OBJS) $(BIN)
