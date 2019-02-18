#include "stdafx.h"

#define SFTP_MAX_FULL_PATH 500
static char _fullPath[SFTP_MAX_FULL_PATH + 1];

#define SFTP_RW_BUFFER 1024
static char _rwBuffer[ SFTP_RW_BUFFER+1];

static long int _sftpErrorCode = 0;

static long int _timeOut = -1L;
	
static ssh_session _ssh_session = NULL;
static sftp_session _sftp_session = NULL;

static int createFullPath(char *fileName, char *directory)
{
	if( strlen(fileName) + strlen(directory) + 2 >= SFTP_MAX_FULL_PATH ) {
		return -1;
	}

	strcpy(_fullPath, directory);
	strcat(_fullPath, "/");
	strcat(_fullPath, fileName);
	return 0;
}


// Test is a file exists at a server
int sftpTest(char *fileName, char *directory,	unsigned long int *pSize, int *pError)
{
	int returnValue = -1;
	sftp_attributes attrs;

	if (createFullPath(fileName, directory) == -1) {
		_sftpErrorCode = -1;
		goto lab_return;
	}

	attrs = sftp_stat(_sftp_session, _fullPath);
	if( attrs == NULL ) {
		int error = sftp_get_error(_sftp_session);
		if( error == SSH_FX_NO_SUCH_FILE ) {
			returnValue = 0;
			if (pSize != NULL) {
				*pSize = -1;
			}
			if( pError != NULL ) {
				*pError = SSH_FX_OK;
			}
		} else {
			_sftpErrorCode = -1;
			if (pError != NULL) {
				*pError = error;
			}
		}
	} else {
		returnValue = 1;
		if( pSize != NULL ) {
			*pSize = attrs->size;
		}
		if (pError != NULL) {
			*pError = SSH_FX_OK;
		}
	}

	lab_return:
	return returnValue;
}


int sftpUpload(char *srcFileName, char *dstFileName, char *dstDirectory, int overwriteIfExists)
{
	_sftpErrorCode = 0;
	
	int access_type = O_RDWR | O_CREAT | O_BINARY;
	if (overwriteIfExists == 1) {
		access_type |= O_TRUNC;
	}
	else {
		access_type |= O_EXCL;
	}

	sftp_file dstFile=NULL;
	int srcFile = -1;
	int status, bytesRead, bytesWritten;

	if (createFullPath(dstFileName, dstDirectory) == -1) {
		_sftpErrorCode = -1;
		goto lab_return;
	}

	dstFile = sftp_open(_sftp_session, _fullPath, access_type, 0666 );
	if (dstFile == NULL) {
		_sftpErrorCode = SFTP_ERROR_WRITING_REMOTE_FILE;
		goto lab_return;
	}

	srcFile = open(srcFileName, O_RDONLY | O_BINARY);
	if( srcFile < 0 ) {
		_sftpErrorCode = SFTP_ERROR_READING_LOCAL_FILE;
		goto lab_return;
	}

	for (;;) {
		bytesRead = read(srcFile, _rwBuffer, SFTP_RW_BUFFER);
		if (bytesRead == 0) {
			break;
		}
		if (bytesRead == -1) {
			_sftpErrorCode = SFTP_ERROR_READING_LOCAL_FILE;
			goto lab_return;
		}
		bytesWritten = sftp_write(dstFile, _rwBuffer, bytesRead);
		if (bytesWritten != bytesRead) {
			_sftpErrorCode = SFTP_ERROR_WRITING_REMOTE_FILE;
			goto lab_return;
		}
	}

lab_return:
	if (!(srcFile < 0)) {
		close(srcFile);
	}
	if (dstFile != NULL) {
		status = sftp_close(dstFile);
		if (status != SSH_OK) {
			_sftpErrorCode = -1;
		}
	}
	return _sftpErrorCode;
}


int sftpDownload(char *dstFileName, char *srcFileName, char *srcDirectory)
{
	_sftpErrorCode = 0;
	int access_type= O_RDONLY;
	sftp_file srcFile = NULL;
	int dstFile = -1;
	int status, bytesRead, bytesWritten;

	if (createFullPath(srcFileName, srcDirectory) == -1) {
		_sftpErrorCode = -1;
		goto lab_return;
	}

	srcFile = sftp_open(_sftp_session, _fullPath, access_type, 0);
	if( srcFile == NULL ) {
		_sftpErrorCode = -1;
		goto lab_return;
	}

	dstFile = open(dstFileName, O_CREAT | O_RDWR | O_TRUNC, 0666);
	if( dstFile < 0 ) {
		_sftpErrorCode = SFTP_ERROR_WRITING_LOCAL_FILE;
		goto lab_return;
	}
	for (;;) {
		bytesRead = sftp_read( srcFile, _rwBuffer, sizeof(_rwBuffer) );
		if( bytesRead == 0 ) {
			break; // EOF
		}
		if( bytesRead < 0 ) {
			_sftpErrorCode = SFTP_ERROR_READING_REMOTE_FILE;
			goto lab_return;
		}
		bytesWritten = write(dstFile, _rwBuffer, bytesRead);
		if ( bytesWritten != bytesRead ) {
			_sftpErrorCode = SFTP_ERROR_WRITING_LOCAL_FILE;
			goto lab_return;
		}
	}

lab_return:
	if( !(dstFile < 0 ) ) {
		close(dstFile);
	}
	if(srcFile != NULL ) {
		status = sftp_close(srcFile);
		if(status != SSH_OK ) {
			_sftpErrorCode = -1;
		}
	}
  return _sftpErrorCode;
}


void sftpSetTimeOut(unsigned long int timeOut) {
	_sftpErrorCode = 0;
	_timeOut = timeOut;
}


int sftpInit( char *server, char *user, char *password ) {
	_sftpErrorCode = 0;
	int status;

	if( _ssh_session != NULL ) {
		return 0;
	}

	_ssh_session = ssh_new();
	if( _ssh_session == NULL ) {
		_sftpErrorCode = SFTP_ERROR;
		goto lab_return;
	}

	ssh_options_set(_ssh_session, SSH_OPTIONS_HOST, server );
	status = ssh_connect(_ssh_session);
	if (status != SSH_OK)
	{
		_sftpErrorCode = SFTP_ERROR;
		goto lab_return;
	}

	// Authenticate ourselves
	status = ssh_userauth_password(_ssh_session, user, password);
	if (status != SSH_AUTH_SUCCESS) {
		_sftpErrorCode = SFTP_ERROR;
		goto lab_return;
	}

 	_sftp_session = sftp_new(_ssh_session);  // Creating a new sftp session
  	if( _sftp_session == NULL ) {
		_sftpErrorCode = SFTP_ERROR;
		goto lab_return;
  	}
  
		status = sftp_init( _sftp_session ); // Initializing a new sftp session
	if (status != SSH_OK) {
		_sftpErrorCode = SFTP_ERROR;
		goto lab_return;
	}

lab_return:
	if( _sftpErrorCode != 0 ) {
		sftpClose();
	}
	return _sftpErrorCode;
}


int sftpClose( void ) {
	_sftpErrorCode = 0;

	if( _sftp_session != NULL ) {
		sftp_free( _sftp_session );
	}

	if( _ssh_session != NULL ) {
		ssh_disconnect(_ssh_session);
		ssh_free(_ssh_session);
	}

	return _sftpErrorCode;
}



int sftpGetLastError(int *sftpErrorCode, int *libsshErrorCode, char *libsshError) {
	if (sftpErrorCode != NULL) {
		*sftpErrorCode = _sftpErrorCode;
	}
	if( libsshErrorCode != NULL ) {
		*libsshErrorCode = sftp_get_error(_sftp_session);
	}
	if (libsshError != NULL) {
		libsshError = ssh_get_error(_ssh_session);
	}
	return 0;
}
