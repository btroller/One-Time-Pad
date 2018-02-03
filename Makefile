TARGET  = otp
CC      = gcc
CCFLAGS = -std=c89 -pedantic -Wall -Werror
LDFLAGS = -lm
SOURCES = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGET) $(LDFLAGS) $(OBJECTS)

$(OBJECTS): $(SOURCES) $(HEADERS)
	$(CC) -c $(CCFLAGS) $(SOURCES)

clean:
	rm $(TARGET) $(OBJECTS)
