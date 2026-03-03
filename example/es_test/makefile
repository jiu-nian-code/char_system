.PHONY : ALL
ALL : test

test : test.cc
	g++ -o $@ $^ -std=c++17 -lcpr -lelasticlient -lspdlog -lfmt -lgflags  -l:libjsoncpp.so.19
.PHONY : clean
clean : 
	rm -f test