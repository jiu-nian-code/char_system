.PHONY : ALL
ALL : server client test

server : server.cc  main.pb.cc
	g++ -o $@ $^ -std=c++17 -lbrpc -lgflags -lssl -lcrypto -lprotobuf -lleveldb
client : client.cc main.pb.cc
	g++ -o $@ $^ -std=c++17 -lbrpc -lgflags -lssl -lcrypto -lprotobuf -lleveldb
test : test.cc
	g++ -o $@ $^ -std=c++17 -lcpprest -pthread -lspdlog -lfmt -lgflags -lbrpc -lssl -lcrypto -lprotobuf -lleveldb
.PHONY : clean
clean :
	rm -f server client