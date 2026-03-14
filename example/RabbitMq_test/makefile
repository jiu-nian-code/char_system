.PHONY : ALL
ALL : publish consume

publish : publish.cc
	g++ -o  $@ $^ -std=c++17 -L/usr/lib -lamqpcpp -lev -lspdlog -lfmt -lgflags
consume : consume.cc
	g++ -o  $@ $^ -std=c++17 -L/usr/lib -lamqpcpp -lev -lspdlog -lfmt -lgflags
.PHONY : clean
clean :
	rm -f publish consume