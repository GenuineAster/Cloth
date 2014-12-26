LDFLAGS=$(LDGLAGS) -lsfml-graphics -lsfml-window -lsfml-system -lGL

DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CXXFLAGS=-std=c++11 -g -DDEBUG
	LDFLAGS=$(LDGLAGS) -g -lsfml-graphics -lsfml-window -lsfml-system
else
	CXXFLAGS=-std=c++11 -march=native -msse4a -mfpmath=sse -O3 -DNDEBUG
endif
	

cloth: cloth.o
	$(CXX) $< $(LDFLAGS) -o $@

cloth_multithreaded: cloth_multithreaded.o
	$(LD) $< $(LDFLAGS) -o $@

clean:
	rm *.o
