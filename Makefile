main: 
	g++ cloud-recognition.cpp -lpng -lmfhdf -ldf -o cr
all:
	g++ cloud-recognition.cpp -lpng -lmfhdf -ldf -o cr
clean:
	rm cr