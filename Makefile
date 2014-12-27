LDFLAGS:= -lsfml-graphics -lsfml-window -lsfml-system -lGL

DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CXXFLAGS:=-std=c++11 -g -DDEBUG
	LDFLAGS:= -g -lsfml-graphics -lsfml-window -lsfml-system -lGL
else
	CXXFLAGS:=-std=c++11 -march=native -msse4a -mfpmath=sse -O3 -DNDEBUG
endif

cloth: cloth.o
	$(CXX) $^ $(LDFLAGS) -o $@

gui: CXXFLAGS := $(CXXFLAGS) -DUSE_IMGUI
gui: cloth.o imgui/imgui.o
	$(CXX) $^ $(LDFLAGS) -o cloth_$@

clean:
	rm -f *.o
	rm -f imgui/*.o
