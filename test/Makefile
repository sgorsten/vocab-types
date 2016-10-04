all: test

test: *.cpp *.h ../include/*
	$(CXX) *.cpp -std=c++14 -o $@

clean:
	rm -f test