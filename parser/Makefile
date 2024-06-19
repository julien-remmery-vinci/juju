parser: parser.o lexer.o utils.o
	gcc -g -o parser parser.o lexer.o utils.o

parser.o: parser.c parser.h
	gcc -g -c parser.c parser.h

lexer.o: ../lexer/lexer.c ../lexer/lexer.h
	gcc -g -c ../lexer/lexer.c ../lexer/lexer.h

utils.o: ../utils/utils.c ../utils/utils.h
	gcc -g -c ../utils/utils.c ../utils/utils.h

test:
	./parser test.ju

clean:
	rm -f *.o
	rm parser
