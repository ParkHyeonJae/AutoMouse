#include <iostream>
#include <Windows.h>
#include <shellapi.h>
#include <string>

int main(void)
{
	system("mode con: cols=50 lines=10");

	int duration = 0.0f;
	std::wstring buffer;
	std::cout << "==== ���丶�콺 ���α׷� ====" << "\n";
	std::cout << "����� : ���α׷� Ȱ��ȭ ���·� space Ű �Է����� ���丶�콺 Ȱ��ȭ/��Ȱ��ȭ ����" << "\n";
	std::cout << "Ŭ�� �ֱ⸦ �Է��� �ּ���(���� : millisecond) : ";
	std::wcin >> buffer;
	
	ShellExecute(NULL, L"open", L"AutoMouse.exe", buffer.c_str(), NULL, SW_SHOW);


	return 0;
}