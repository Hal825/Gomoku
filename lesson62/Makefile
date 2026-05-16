.PHONY:all
all:client_ssh server_ssh

server_ssh:EchoTcpServerMain.cc
	g++ -o $@ $^ -std=c++17 -lpthread
client_ssh:EchoTcpClient.cc
	g++ -o $@ $^ -std=c++17 -static

.PHONY:clean
clean:
	rm -f client_ssh server_ssh