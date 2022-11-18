CC := clang
CFLAGS := -g -Wall -Werror -I/home/curtsinger/.local/include -L/home/curtsinger/.local/lib -fsanitize=address

# Special settings for macOS users. This assumes you installed openssl with the brew package manager
SYSTEM := $(shell uname -s)
ifeq ($(SYSTEM),Darwin)
  CFLAGS += -I$(shell brew --prefix openssl)/include -L$(shell brew --prefix openssl)/lib
endif

all: launcher

clean:
	rm -rf launcher launcher.dSYM

launcher: launcher.c
	$(CC) $(CFLAGS) -o launcher launcher.c -lcrypto -lpthread -lm

zip:
	@echo "Generating launcher.zip file to submit to Gradescope..."
	@zip -q -r launcher.zip . -x .git/\* .vscode/\* .clang-format .gitignore launcher
	@echo "Done. Please upload launcher.zip to Gradescope."

format:
	@echo "Reformatting source code."
	@clang-format -i --style=file $(wildcard *.c) $(wildcard *.h)
	@echo "Done."

.PHONY: all clean zip format