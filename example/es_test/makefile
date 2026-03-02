.PHONY : ALL
ALL : test

test : test.cc
	g++ -o $@ $^ -std=c++17 -lcpr -lelasticlient
.PHONY : clean
clean : 
	rm -f test