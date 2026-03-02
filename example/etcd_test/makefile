.PHONY : ALL
ALL : discovery registry

discovery : discovery.cc ../brpc_test/main.pb.cc
	g++ -o $@ $^ -std=c++17 -letcd-cpp-api -lcpprest -pthread -lspdlog -lfmt -lgflags -lbrpc -lssl -lcrypto -lprotobuf -lleveldb
registry : registry.cc ../brpc_test/main.pb.cc
	g++ -o $@ $^ -std=c++17 -letcd-cpp-api -lcpprest -pthread -lspdlog -lfmt -lgflags -lbrpc -lssl -lcrypto -lprotobuf -lleveldb
clean:
	rm -rf registry discovery