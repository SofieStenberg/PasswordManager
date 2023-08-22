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
CPPFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Werror -Isqlite3

all: $(EXECUTABLE)

$(EXECUTABLE): $(BUILD)main.o $(BUILD)PasswordManager.o $(BUILD)sqlite3.o 
	$(CPP) $(CPPFLAGS) $(BUILD)main.o $(BUILD)PasswordManager.o $(BUILD)sqlite3.o -o $(EXECUTABLE)

$(BUILD)main.o: $(SOURCE)main.cpp
	$(CPP) $(CPPFLAGS) -c $(SOURCE)main.cpp -I$(INCLUDES) -o $(BUILD)main.o

$(BUILD)PasswordManager.o: $(SOURCE)PasswordManager.cpp
	$(CPP) $(CPPFLAGS) -c $(SOURCE)PasswordManager.cpp -I$(INCLUDES) -o $(BUILD)PasswordManager.o


# clean:

# 	-$(rm) $(EXECUTABLE)
# 	-$(rm) $(BUILD)main.o
# 	-$(rm) $(BUILD)PasswordManager.o