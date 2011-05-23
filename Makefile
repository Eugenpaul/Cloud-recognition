main: 
	#g++ -Wall -Iinc/ src/readhdf.cpp src/pic.cpp src/detection.cpp cloud-recognition.cpp -lpng -lmfhdf -ldf -o cr
	g++ -Wall -Iinc src/* cloud-recognition.cpp -lpng -lmfhdf -ldf -o cr
all:
	make main
clean:
	rm cr
