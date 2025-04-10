# 사용할 컴파일러 설정 (g++는 C++ 컴파일러야)
CXX = g++

# 컴파일할 때 줄 옵션들 (-Wall: 경고 보여줘, -O2: 최적화, -std=c++11: C++11 문법 사용)
CXXFLAGS = -Wall -Wextra -O2 -std=c++11

# 만들 실행 파일 이름 (sum-nbo라는 프로그램이 만들어질 거야)
TARGET = sum-nbo

# 소스코드 파일 이름 (.cpp 파일)
SRCS = sum-nbo.cpp

# 현재 폴더에 있는 모든 .bin 파일들 목록 가져오기
BINS = $(wildcard *.bin)

# make만 쳤을 때 실행됨 → 프로그램을 컴파일함
all: $(TARGET)

# 프로그램 컴파일하기 (g++로 sum-nbo.cpp를 컴파일해서 실행 파일 만들기)
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 숫자를 입력하면 (예: make 123) → 해당 숫자를 4바이트 바이너리 파일로 저장
# 숫자를 4바이트 네트워크 바이트 순서(빅엔디안)로 저장
# 숫자가 아닌 값을 넣으면 에러 메시지 출력
# 숫자인지 확인
# 맨 앞 바이트 -> 두 번째 바이트 -> 세 번째 바이트 -> 마지막 바이트 → .bin 파일로 저장
%:
	@if echo $@ | grep -Eq '^[0-9]+$$'; then \
		printf "Creating binary file with value $@\n"; \
		printf "%08x\n" $@; \
		python3 -c "import sys; sys.stdout.buffer.write(int('$@').to_bytes(4, 'big'))" > $@.bin; \
		echo "Created $@.bin with value $@"; \
	else \
		echo "Invalid target '$@'. Only integers are supported."; \
		false; \
	fi



# 테스트 실행: sum-nbo 프로그램을 실행하면서 .bin 파일들을 인자로 줌
# .bin 파일이 없으면 No .bin print
test: $(TARGET)
	@if [ -z "$(BINS)" ]; then \
		echo "No .bin files found. Create them with 'make <number>'"; \
		exit 1; \
	else \
		echo "Running test with all .bin files..."; \
		./$(TARGET) $(BINS); \
	fi


# 정리 명령어: 실행 파일과 .bin 파일들 모두 삭제
clean:
	@echo "Cleaning up..."
	@rm -f $(TARGET) *.bin
	@echo "Removed $(TARGET) and all .bin files"

# all, clean, test는 항상 실행되도록 지정 (파일 이름이랑 헷갈리지 않게)
# .PHONY는 make가 "이건 진짜 파일이 아니라 명령어야!" 하고 헷갈리지 않게 도와주는 표시
.PHONY: all clean test
