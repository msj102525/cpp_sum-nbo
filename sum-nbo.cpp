#include <iostream>     // 화면에 출력하거나 입력받을 때 사용
#include <fstream>      // 파일을 읽고 쓸 때 사용
#include <vector>       // 여러 개의 숫자를 저장할 수 있는 동적 배열
#include <cstdint>      // uint32_t 같은 정확한 크기의 정수 자료형 사용
#include <arpa/inet.h>  // ntohl 함수 사용 (네트워크 바이트 순서를 호스트 순서로 바꿔줌)
#include <iomanip>      // 숫자 출력 형식 조절 (16진수, 자릿수 맞추기 등)

// 1. .bin 파일을 여러 개 받아서,
// 2. 각 파일에서 4바이트(32비트) 정수를 하나씩 읽고,
// 3. 그 정수들을 더한 값을 출력.
// 4. 출력할 때는 10진수 + 16진수로!

int main(int argc, char *argv[]) {
    // 파일 이름을 인자로 입력했는지 확인 (최소 1개 이상 받아야 함)
    if (argc < 2) {
        std::cout << "syntax : sum-nbo <file1> [<file2>...]\n";
        std::cout << "sample : sum-nbo a.bin b.bin\n";
        return 1;  // 프로그램 종료 (에러)
    }

    uint32_t sum = 0;                  // 합계를 저장할 변수
    std::vector<uint32_t> numbers;     // 읽어온 숫자들을 저장할 배열

    // 입력받은 각 파일을 하나씩 처리
    for (int i = 1; i < argc; i++) {
        std::ifstream file(argv[i], std::ios::binary);  // 이진 모드로 파일 열기
        if (!file) {
            std::cerr << "Error: Cannot open file " << argv[i] << std::endl;
            continue;  // 다음 파일로 넘어감
        }

        // 파일 크기 확인 (최소 4바이트여야 함)
        file.seekg(0, std::ios::end);        // 파일 끝으로 이동
        std::streamsize file_size = file.tellg();  // 현재 위치 = 파일 크기
        file.seekg(0, std::ios::beg);        // 다시 파일 처음으로 이동

        if (file_size < 4) {
            std::cerr << "Error: File " << argv[i] << " is too small (less than 4 bytes)" << std::endl;
            continue;  // 다음 파일로 넘어감
        }

        // 4바이트 정수 읽기
        uint32_t value;
        if (!file.read(reinterpret_cast<char*>(&value), sizeof(uint32_t))) {
            std::cerr << "Error: Failed to read 4 bytes from file " << argv[i] << std::endl;
            continue;
        }

        // 네트워크 바이트 순서 (big-endian) → 호스트 바이트 순서로 변환
        value = ntohl(value);

        // 숫자 저장하고 합계에 더함
        numbers.push_back(value);
        sum += value;
    }

    // 처리한 파일이 하나도 없을 때
    if (numbers.empty()) {
        std::cerr << "Error: No valid files to process" << std::endl;
        return 1;
    }

    // 결과 출력
    for (size_t i = 0; i < numbers.size(); i++) {
        // 숫자 출력 (10진수 + 16진수 형식)
        std::cout << numbers[i] << "(0x" 
                  << std::hex << std::setfill('0') << std::setw(8) 
                  << numbers[i] << std::dec << ")";
        if (i < numbers.size() - 1) {
            std::cout << " + ";
        }
    }

    // 합계 출력
    std::cout << " = " << sum << "(0x" 
              << std::hex << std::setfill('0') << std::setw(8) 
              << sum << ")" << std::endl;

    return 0;  // 정상 종료
}

