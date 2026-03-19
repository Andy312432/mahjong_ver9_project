CXX = g++
CLEAN = del /s /q
# $(CXX) -o ClientSocketMain.exe ClientSocketMain.cpp  -O2 -lws2_32 -std=c++17
all:
	$(CXX) -o ./ClientSocketMain.exe ClientSocketMain.cpp -Wall -O3 -lws2_32 -std=c++17 -static -pthread -DIO_MODE=0
	$(CXX) -o ./ClientStdin.exe ClientStdinMain.cpp -Wall -O3 -lws2_32 -std=c++17 -static -pthread -DIO_MODE=1
	$(CXX) -o ./ClientSocket_ntpu.exe ClientSocketMain.cpp -Wall -O3 -lws2_32 -std=c++17 -static -pthread -DIO_MODE=1
	$(CLEAN) "./MJ_ClientTest/1/*.csv"
	$(CLEAN) "./MJ_ClientTest/2/*.csv"
	$(CLEAN) "./MJ_ClientTest/3/*.csv"
	$(CLEAN) "./MJ_ClientTest/4/*.csv"
clean:
	$(CLEAN) *.o
