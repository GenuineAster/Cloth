LDFLAGS=$(LDGLAGS) -lsfml-graphics -lsfml-window -lsfml-system
CXXFLAGS=-std=c++11 -march=native -msse4a -mfpmath=sse -O3

cloth: cloth.o
	g++ cloth.o $(LDFLAGS) -o cloth

clean:
	rm *.o
