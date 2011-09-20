
main: 
	#g++ -Wall -Iinc/ src/readhdf.cpp src/pic.cpp src/detection.cpp cloud-recognition.cpp -lpng -lmfhdf -ldf -o cr
	g++ -Wall -Iinc src/*.cpp cloud-recognition.cpp -lpng -lqgis_core -lpng -g -O0 -o cr
all:
	make main
clean:
	rm cr
