#include "log.h"


// static変数の初期化
std::string Logger::fileName = "";
FILE* Logger::file = NULL;
FILE* Logger::file2 = NULL;

void Logger::Initialize(const std::string& fileName){
	// ファイルの名前を保存
	Logger::fileName = fileName;

	// ファイルを開き、ポインタを保存
	file = fopen( fileName.data(), "w" );

	// ファイルが開けなかった場合、
	if ( file == NULL ){
		// エラーメッセージを出し、プログラムを終了します。
//		MessageBox( NULL, "Failed to open log file for writing.\nApplication will be closed now.", "Error Info", MB_ICONWARNING );
		exit( 0 );
	}
}


void Logger::resultInitialize(const std::string& fileName){
	// ファイルの名前を保存
	Logger::fileName = fileName;

	// ファイルを開き、ポインタを保存
	file2 = fopen( fileName.data(), "w" );

	// ファイルが開けなかった場合、
	if ( file2 == NULL ){
		// エラーメッセージを出し、プログラムを終了します。
//		MessageBox( NULL, "Failed to open log file for writing.\nApplication will be closed now.", "Error Info", MB_ICONWARNING );
		exit( 0 );
	}
}


void Logger::Fileopen(const std::string& fileName){

	// ファイルを開き、ポインタを保存
	file = fopen( fileName.data(), "a" );

	// ファイルが開けなかった場合、
	if ( file == NULL ){
		// エラーメッセージを出し、プログラムを終了します。
//		MessageBox( NULL, "Failed to open log file for writing.\nApplication will be closed now.", "Error Info", MB_ICONWARNING );
		exit( 0 );
	}
}


void Logger::ResultFileopen(const std::string& fileName){

	// ファイルを開き、ポインタを保存
	file2 = fopen( fileName.data(), "a" );

	// ファイルが開けなかった場合、
	if ( file == NULL ){
		// エラーメッセージを出し、プログラムを終了します。
//		MessageBox( NULL, "Failed to open log file for writing.\nApplication will be closed now.", "Error Info", MB_ICONWARNING );
		exit( 0 );
	}
}


void Logger::Fileclose(){
	// ファイルを閉じる
	fclose(file);
}


void Logger::ResultFileclose(){
	// ファイルを閉じる
	fclose(file2);
}


void Logger::Writetime(){
	tm* newTime;
	__time64_t longTime;

	//タスク実行時に開始するタイマーの時刻を保存すればよい

	// 現在の時間を獲得し、ローカル時間（日本の時間）に変換する
	_time64( &longTime );
	newTime = _localtime64( &longTime );
	// 書き込むログの時刻を保存する
	fprintf( file, "[ %02d月%02d日 - %02d:%02d:%02d ]: \t",
	newTime->tm_mon + 1, newTime->tm_mday,
	newTime->tm_hour, newTime->tm_min, newTime->tm_sec);
	// ファイルにを書き込む
	fflush( file );
}

//文字列のログ
void Logger::WriteString(const std::string& log){

	// 書き込むログの内容を保存する
	fprintf( file, "%s\t",log.c_str() );
	// ファイルにを書き込む
	fflush( file );
}

//変数のログ
void Logger::Write(float a){

	// 書き込むログの内容を保存する
	fprintf( file, "%f\t",a );
	// ファイルにを書き込む
	fflush( file );
}

//int型変数のログ
void Logger::Writeint(int a){

	// 書き込むログの内容を保存する
	fprintf( file, "%d\t",a );
	// ファイルにを書き込む
	fflush( file );
}

//改行用
void Logger::Writeln(){

	// 書き込むログの内容を保存する
	fprintf( file, "\n");
	// ファイルにを書き込む
	fflush( file );
}
