#include <iostream>
#include <time.h>
#include <Windows.h>


class Logger
{
public:
	static void Initialize(const std::string& fileName);
	static void resultInitialize(const std::string& fileName);
	static void Writetime();
	static void WriteString(const std::string& log);
	static void Write(float a);
	static void Writeint(int a);
	static void Writeln();
	static void Fileopen(const std::string& fileName);
	static void ResultFileopen(const std::string& fileName);
	static void Fileclose();
	static void ResultFileclose();

protected:
	static std::string fileName;
	static FILE* file;
	static FILE* file2;
};
