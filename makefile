ifeq ($(OS), Windows_NT)
	INCLUDES := include\\
	BUILD := build\\
	BIN := bin\\
	EXECUTABLE := $(BIN)pwdManager.exe
	SOURCE := source\\
	DELETE := del
else
	INCLUDES := include/
	BUILD := build/
	BIN := bin/
	EXECUTABLE := $(BIN)pwdManager.exe
	SOURCE := source/
	DELETE := rm
endif

CPP := g++
GCC := gcc
CPPFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Werror -Isqlite3 -g

all: $(EXECUTABLE)

$(EXECUTABLE): $(BUILD)main.o $(BUILD)PasswordManager.o $(BUILD)sqlite3.o $(BUILD)Ceasar.o
	$(CPP) $(CPPFLAGS) $(BUILD)main.o $(BUILD)PasswordManager.o $(BUILD)sqlite3.o $(BUILD)Ceasar.o -o $(EXECUTABLE)

$(BUILD)main.o: $(SOURCE)main.cpp
	$(CPP) $(CPPFLAGS) -c $(SOURCE)main.cpp -I$(INCLUDES) -o $(BUILD)main.o

$(BUILD)sqlite3.o: $(SOURCE)sqlite3.c
	$(GCC) -c $(SOURCE)sqlite3.c -I$(INCLUDES) -o $(BUILD)sqlite3.o

$(BUILD)PasswordManager.o: $(SOURCE)PasswordManager.cpp
	$(CPP) $(CPPFLAGS) -c $(SOURCE)PasswordManager.cpp -I$(INCLUDES) -o $(BUILD)PasswordManager.o

$(BUILD)Ceasar.o: $(SOURCE)Ceasar.cpp
	$(CPP) $(CPPFLAGS) -c $(SOURCE)Ceasar.cpp -I$(INCLUDES) -o $(BUILD)Ceasar.o


# clean:

# 	-$(DELETE) $(BUILD)main.o
# 	-$(DELETE) $(BUILD)PasswordManager.o
# 	-$(DELETE) $(BUILD)sqlite3.o
# 	-$(DELETE) $(BUILD)Ceasar.o