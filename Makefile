test: jupiler
	./jupiler test.ju

jupiler: compiler.o parser.o lexer.o utils.o codegen.o
	gcc -g -o jupiler compiler.o parser.o lexer.o utils.o codegen.o

compiler.o: compiler/compiler.c
	gcc -g -c compiler/compiler.c

codegen.o: codegen/codegen.c codegen/codegen.h
	gcc -g -c codegen/codegen.c

parser.o: parser/parser.c parser/parser.h
	gcc -g -c parser/parser.c

lexer.o: lexer/lexer.c lexer/lexer.h
	gcc -g -c lexer/lexer.c

utils.o: utils/utils.c utils/utils.h
	gcc -g -c utils/utils.c

run: jupiler
	./jupiler test.ju
	gcc -g -o out out.s

clean:
	rm -f *.o *.s jupiler out

