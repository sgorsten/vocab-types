all: test

test: *.cpp *.h ../include/*
	$(CXX) test.cpp test-string_view.cpp -I../include -std=c++14 -o $@

clean:
	rm -f test