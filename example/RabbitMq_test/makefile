.PHONY : ALL
ALL : publish consume

publish : publish.cc
	g++ -o  $@ $^ -std=c++17 -L/usr/lib -lamqpcpp -lev
consume : consume.cc
	g++ -o  $@ $^ -std=c++17 -L/usr/lib -lamqpcpp -lev
.PHONY : clean
clean :
	rm -f main