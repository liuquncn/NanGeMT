CC = g++
LFLAGS = -lm -lboost_regex
CPPFLAGS = -ggdb -Wall -I/usr/local/include/boost/

through: through.o segment.o postag.o parse.o unity.o prsrbase.o chongdie.o transform.o agree.o generate.o enagree.o
	$(CC) -o through through.o segment.o postag.o parse.o unity.o prsrbase.o chongdie.o transform.o agree.o generate.o enagree.o $(LFLAGS)

clean:
	rm through; rm *.o; rm *.pyc; rm *~;

through.o: through.cc
	$(CC) -c $(CPPFLAGS) -o through.o through.cc

segment.o: segment.cc
	$(CC) -c $(CPPFLAGS) -o segment.o segment.cc

postag.o: postag.cc
	$(CC) -c $(CPPFLAGS) -o postag.o postag.cc

parse.o: parse.cc
	$(CC) -c $(CPPFLAGS) -o parse.o parse.cc

unity.o: unity.cc
	$(CC) -c $(CPPFLAGS) -o unity.o unity.cc

prsrbase.o: prsrbase.cc
	$(CC) -c $(CPPFLAGS) -o prsrbase.o prsrbase.cc

chongdie.o: chongdie.cc
	$(CC) -c $(CPPFLAGS) -o chongdie.o chongdie.cc

transform.o: transform.cc
	$(CC) -c $(CPPFLAGS) -o transform.o transform.cc

agree.o: agree.cc
	$(CC) -c $(CPPFLAGS) -o agree.o agree.cc

generate.o: generate.cc
	$(CC) -c $(CPPFLAGS) -o generate.o generate.cc

enagree.o: enagree.cc
	$(CC) -c $(CPPFLAGS) -o enagree.o enagree.cc
