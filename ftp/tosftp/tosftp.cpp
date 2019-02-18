// A demo for sftp file transfer
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "sftp.h"

//static char *_server = "u38989.ssh.masterhost.ru";
//static char *_user = "u38989";
//static char *_password = "amitin9sti";
//static char *_dstDirectory = "/home/u38989";

#define CONNECTION_NAMES_BUFFER 1000
wchar_t _lpszConnectionNames[CONNECTION_NAMES_BUFFER + 1];

#define MAX_CONNECTIONS_NUMBER 100
wchar_t *_connections[MAX_CONNECTIONS_NUMBER];
int _connectionsNumber = 0;
int readConnections(wchar_t *buffer);

#define PROFILE_STRING_BUFFER 1000

wchar_t _server[PROFILE_STRING_BUFFER + 1];
wchar_t _directory[PROFILE_STRING_BUFFER + 1];
wchar_t _user[PROFILE_STRING_BUFFER + 1];
wchar_t _password[PROFILE_STRING_BUFFER + 1];
wchar_t _mode[PROFILE_STRING_BUFFER + 1];

int readConnection(wchar_t *fileName, wchar_t *connectionName);

#define MAX_FILES_NUMBER 100
wchar_t *_fileNames[MAX_FILES_NUMBER];
int _filesNumber = 0;
int readFileNames(wchar_t *fileNamesBuffer);


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* cmdLine, int nCmdShow)
{
	int exitStatus = -1;
	long long size;
	DWORD status;

	int argCount;
	LPWSTR *szArgList = CommandLineToArgvW(GetCommandLine(), &argCount);
	if (szArgList == nullptr) {
		goto lab_exit;
		// MessageBox(NULL, L"Unable to parse command line", L"Error", MB_OK);
	}

	if (argCount < 3) {
		goto lab_exit;
	}

	if (readConnections(szArgList[1]) == 0) {
		goto lab_exit;
	}

	for (int iconn = 0; iconn < _connectionsNumber; iconn++) {
		wchar_t action[PROFILE_STRING_BUFFER + 1];
		status = GetPrivateProfileString(_connections[iconn], L"Action", NULL, action, PROFILE_STRING_BUFFER, szArgList[1]);
		if (status <= 0 || status >= PROFILE_STRING_BUFFER - 2) {
			continue;
		}

		if (readConnection(szArgList[2], _connections[iconn]) == -1) {
			break;
		}

		wchar_t filesDir[PROFILE_STRING_BUFFER + 1];
		status = GetPrivateProfileString(_connections[iconn], L"FilesDir", NULL, filesDir, PROFILE_STRING_BUFFER, szArgList[1]);
		if (status <= 0 || status >= PROFILE_STRING_BUFFER - 2) {
			continue;
		}

		wchar_t fileNamesBuffer[PROFILE_STRING_BUFFER + 1];
		status = GetPrivateProfileString(_connections[iconn], L"FileNames", NULL, fileNamesBuffer, PROFILE_STRING_BUFFER, szArgList[1]);
		if (status <= 0 || status >= PROFILE_STRING_BUFFER - 2) {
			continue;
		}

		bool actionPut;
		if( wcscmp(action, "PUT") == 0 ) {
			actionPut = true;
		} else {
			actionPut = false;
		}

		if (sftpSetCredentials(_server, _user, _password) == -1) {
			fprintf(outp, "Error setting credentials!\nExiting...\n");
			goto lab_close;
		}


		if (readFileNames(fileNamesBuffer) == -1) {
			for( int if = 0 ; if < _filesNumber ; if++ ) {
				if( actionPut ) {
					
				}
			}
			continue;
		}
	}

lab_exit:
	if (szArgList != nullptr) {
		LocalFree(szArgList);
	}

	exit(exitStatus);
}



	int readConnections(wchar_t *buffer)
	{
		DWORD charsRead = GetPrivateProfileSectionNames(_lpszConnectionNames, CONNECTION_NAMES_BUFFER, buffer);
		if (charsRead <= 0 || charsRead == CONNECTION_NAMES_BUFFER - 2) {
			return 0;
		}

		_connections[0] = &_lpszConnectionNames[0];
		_connectionsNumber = 1;
		for (unsigned int i = 0; i < charsRead - 1; i++) {
			if (_lpszConnectionNames[i] == L'\x0' && _lpszConnectionNames[i + 1] != L'\x0') {
				_connections[_connectionsNumber] = &_lpszConnectionNames[i + 1];
				_connectionsNumber++;
				i++;
			}
		}
		return _connectionsNumber;
	}


	int readConnection(wchar_t *fileName, wchar_t *connectionName)
	{
		DWORD status;

		status = GetPrivateProfileString(connectionName, L"Host", NULL, _server, PROFILE_STRING_BUFFER, fileName);
		if (status <= 0 || status >= PROFILE_STRING_BUFFER - 2) {
			return -1;
		}

		int serverNameLength = wcslen(_server);
		int directoryFoundAt = -1;
		for (int i = 0; i < serverNameLength; i++) {
			if (_server[i] == L'/') { // A host name contains as well a directory...
				directoryFoundAt = i;
				break;
			}
		}
		if (directoryFoundAt > 0) {
			_server[directoryFoundAt] = L'\x0';
			_directory[0] = L'/';
			int directoryNameLength = 1;
			for (int i = directoryFoundAt + 1; i < serverNameLength; i++) {
				_directory[directoryNameLength] = _server[i];
				directoryNameLength++;
			}
			if (_directory[directoryNameLength - 1] != L'/') {
				_directory[directoryNameLength] = L'/';
				directoryNameLength++;
			}
			_directory[directoryNameLength] = L'\x0';
		}
		else {
			_directory[0] = L'\x0';
		}

		status = GetPrivateProfileString(connectionName, L"User", NULL, _user, PROFILE_STRING_BUFFER, fileName);
		if (status <= 0 || status >= PROFILE_STRING_BUFFER - 2) {
			return -1;
		}
		status = GetPrivateProfileString(connectionName, L"Password", NULL, _password, PROFILE_STRING_BUFFER, fileName);
		if (status <= 0 || status >= PROFILE_STRING_BUFFER - 2) {
			return -1;
		}

		status = GetPrivateProfileString(connectionName, L"Mode", NULL, _mode, PROFILE_STRING_BUFFER, fileName);
		if (status <= 0 || status >= PROFILE_STRING_BUFFER - 2) {
			return -1;
		}

		return 0;
	}

int readFileNames( wchar_t *fileNamesBuffer ) {
	int fileNamesBufferLength = wcslen(fileNamesBuffer);

	_fileNames[0] = fileNamesBuffer;
	_filesNumber = 1;

	for ( int ibuff = 0 ; ibuff < fileNamesBufferLength ; ibuff++) {
		if (fileNamesBuffer[ibuff] == L',') { // A separation comma found
			fileNamesBuffer[ibuff] = L'\x0';
			ibuff++;
			_fileNames[_filesNumber] = &fileNamesBuffer[ibuff];
			_filesNumber += 1;
		}
	}

	for( int ifile = 0 ; ifile < _filesNumber ; ifile++ ) {
		int buflen = wcslen(_fileNames[ifile]);
		for( ; wcslen(_fileNames[ifile]) > 0 ; ) {
			if (_fileNames[ifile][0] != L' ') {
				break;
			}
			_fileNames[ifile] = &_fileNames[ifile][1];
		}
		buflen = wcslen(_fileNames[ifile]);
		for (int ibuff = buflen-1 ; ibuff >= 0 ; ibuff--) {
			if (_fileNames[ifile][ibuff] != L' ') {
				break;
			}
			_fileNames[ifile][ibuff] = L'\x0';
		}
	}
	return 0;
}
