.PHONY : ALL
ALL : discovery registry

discovery : discovery.cc
	g++ -o $@ $^ -std=c++17 -letcd-cpp-api -lcpprest -pthread -lspdlog -lfmt -lgflags
registry : registry.cc
	g++ -o $@ $^ -std=c++17 -letcd-cpp-api -lcpprest -pthread -lspdlog -lfmt -lgflags
clean:
	rm -rf registry discovery