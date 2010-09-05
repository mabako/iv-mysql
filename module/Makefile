CC = g++
FLAGS = -m32 -shared -Wall -fPIC -I./ `mysql_config --libs --include`

OUTFILE = iv-mysql.so

SRC = $(wildcard ./*.cpp ./SDK/*.cpp)

all: iv-mysql

iv-mysql:
	$(CC) $(FLAGS) -o $(OUTFILE) $(SRC)
	strip --strip-unneeded $(OUTFILE)

clean:
	rm -f $(OBJ)
