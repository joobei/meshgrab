#include "log.h"


// static�ϐ��̏�����
std::string Logger::fileName = "";
FILE* Logger::file = NULL;
FILE* Logger::file2 = NULL;

void Logger::Initialize(const std::string& fileName){
	// �t�@�C���̖��O��ۑ�
	Logger::fileName = fileName;

	// �t�@�C�����J���A�|�C���^��ۑ�
	file = fopen( fileName.data(), "w" );

	// �t�@�C�����J���Ȃ������ꍇ�A
	if ( file == NULL ){
		// �G���[���b�Z�[�W���o���A�v���O�������I�����܂��B
//		MessageBox( NULL, "Failed to open log file for writing.\nApplication will be closed now.", "Error Info", MB_ICONWARNING );
		exit( 0 );
	}
}


void Logger::resultInitialize(const std::string& fileName){
	// �t�@�C���̖��O��ۑ�
	Logger::fileName = fileName;

	// �t�@�C�����J���A�|�C���^��ۑ�
	file2 = fopen( fileName.data(), "w" );

	// �t�@�C�����J���Ȃ������ꍇ�A
	if ( file2 == NULL ){
		// �G���[���b�Z�[�W���o���A�v���O�������I�����܂��B
//		MessageBox( NULL, "Failed to open log file for writing.\nApplication will be closed now.", "Error Info", MB_ICONWARNING );
		exit( 0 );
	}
}


void Logger::Fileopen(const std::string& fileName){

	// �t�@�C�����J���A�|�C���^��ۑ�
	file = fopen( fileName.data(), "a" );

	// �t�@�C�����J���Ȃ������ꍇ�A
	if ( file == NULL ){
		// �G���[���b�Z�[�W���o���A�v���O�������I�����܂��B
//		MessageBox( NULL, "Failed to open log file for writing.\nApplication will be closed now.", "Error Info", MB_ICONWARNING );
		exit( 0 );
	}
}


void Logger::ResultFileopen(const std::string& fileName){

	// �t�@�C�����J���A�|�C���^��ۑ�
	file2 = fopen( fileName.data(), "a" );

	// �t�@�C�����J���Ȃ������ꍇ�A
	if ( file == NULL ){
		// �G���[���b�Z�[�W���o���A�v���O�������I�����܂��B
//		MessageBox( NULL, "Failed to open log file for writing.\nApplication will be closed now.", "Error Info", MB_ICONWARNING );
		exit( 0 );
	}
}


void Logger::Fileclose(){
	// �t�@�C�������
	fclose(file);
}


void Logger::ResultFileclose(){
	// �t�@�C�������
	fclose(file2);
}


void Logger::Writetime(){
	tm* newTime;
	__time64_t longTime;

	//�^�X�N���s���ɊJ�n����^�C�}�[�̎�����ۑ�����΂悢

	// ���݂̎��Ԃ��l�����A���[�J�����ԁi���{�̎��ԁj�ɕϊ�����
	_time64( &longTime );
	newTime = _localtime64( &longTime );
	// �������ރ��O�̎�����ۑ�����
	fprintf( file, "[ %02d��%02d�� - %02d:%02d:%02d ]: \t",
	newTime->tm_mon + 1, newTime->tm_mday,
	newTime->tm_hour, newTime->tm_min, newTime->tm_sec);
	// �t�@�C���ɂ���������
	fflush( file );
}

//������̃��O
void Logger::WriteString(const std::string& log){

	// �������ރ��O�̓��e��ۑ�����
	fprintf( file, "%s\t",log.c_str() );
	// �t�@�C���ɂ���������
	fflush( file );
}

//�ϐ��̃��O
void Logger::Write(float a){

	// �������ރ��O�̓��e��ۑ�����
	fprintf( file, "%f\t",a );
	// �t�@�C���ɂ���������
	fflush( file );
}

//int�^�ϐ��̃��O
void Logger::Writeint(int a){

	// �������ރ��O�̓��e��ۑ�����
	fprintf( file, "%d\t",a );
	// �t�@�C���ɂ���������
	fflush( file );
}

//���s�p
void Logger::Writeln(){

	// �������ރ��O�̓��e��ۑ�����
	fprintf( file, "\n");
	// �t�@�C���ɂ���������
	fflush( file );
}
