all: test

test: *.cpp *.h ../include/*
	$(CXX) *.cpp -I../include -std=c++14 -o $@

clean:
	rm -f test