parser: parser.o lexer.o utils.o
	gcc -g -o parser parser.o lexer.o utils.o

parser.o: parser.c
	gcc -c parser.c

lexer.o: ../lexer/lexer.c ../lexer/lexer.h
	gcc -c ../lexer/lexer.c ../lexer/lexer.h

utils.o: ../utils/utils.c ../utils/utils.h
	gcc -c ../utils/utils.c ../utils/utils.h

test:
	./parser test.ju

clean:
	rm -f *.o
	rm parser