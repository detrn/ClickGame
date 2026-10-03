CC = gcc 

MAIN = src/main.c 

FLAGS = -Wall -Wextra -Iinclude 

LDLIBS = -Lexternal -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 

TARGET = game 

$(TARGET): $(MAIN) 
	$(CC) $(FLAGS) $(MAIN) -o $(TARGET) $(LDLIBS)

clean: 
	rm -f $(TARGET)
	
