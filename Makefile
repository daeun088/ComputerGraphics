# 실행 파일 이름
TARGET = kk_project

# 소스 파일 목록
SRC = main.cpp

# 컴파일러 및 옵션
CXX = g++
CXXFLAGS = -Wall -std=c++11

# 라이브러리 (OpenGL 관련)
LIBS = -lGL -lGLU -lglut

# 빌드 명령
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LIBS)

clean:
	rm -f $(TARGET)
