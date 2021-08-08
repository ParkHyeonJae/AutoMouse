#include <iostream>
#include <Windows.h>
#include <shellapi.h>
#include <string>

int main(void)
{
	system("mode con: cols=50 lines=10");

	int duration = 0.0f;
	std::wstring buffer;
	std::cout << "==== 오토마우스 프로그램 ====" << "\n";
	std::cout << "사용방법 : 프로그램 활성화 상태로 space 키 입력으로 오토마우스 활성화/비활성화 가능" << "\n";
	std::cout << "클릭 주기를 입력해 주세요(단위 : millisecond) : ";
	std::wcin >> buffer;
	
	ShellExecute(NULL, L"open", L"AutoMouse.exe", buffer.c_str(), NULL, SW_SHOW);


	return 0;
}