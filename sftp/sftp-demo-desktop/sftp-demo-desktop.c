#include "sftp.h"
#include <windows.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>


static char *_server = "u38989.ssh.masterhost.ru";
static char *_user = "u38989";
static char *_password = "amitin9sti";
static char *_dstDirectory = "/home/u38989";

static char *testError = "Failed to test\n";
static char *downloadError = "Failed to download\n";
static char *uploadError = "Failed to upload\n";

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	int status;
	int errorLog = -1;
	
	errorLog = open("error.log", O_CREAT | O_TRUNC | O_RDWR, 0777);
	if( errorLog < 0 ) {
		exit(1);
	}
	
	status = sftpInit( _server, _user, _password );
	if( status != 0 ) {
		write( errorLog, "Can't init...\n", 12 );
		return 1;
	}

	int size, error;
	char *fileToTest = "download.dat";
	status = sftpTest(fileToTest, _dstDirectory, &size, &error);
	if (status == -1) { // An error!
		write(errorLog, testError, strlen(testError));
	} else {
		char buffer[100];
		sprintf(buffer, "file tested: %s, status=%d, size=%d, error=%d\n", fileToTest , status, size, error);
		write(errorLog, buffer, strlen(buffer) + 1);
	}
	
	char *fileToDownload = "download.dat";
	status = sftpDownload(fileToDownload, fileToDownload, _dstDirectory);
	if (status != 0) {
		write(errorLog, downloadError, strlen(downloadError));
	} 

	char *fileToUpload = "upload.dat";
	status = sftpUpload(fileToUpload, fileToUpload, _dstDirectory, 1);
	if (status != 0) {
		write(errorLog, uploadError, strlen(uploadError));
	}

	sftpClose();

	close(errorLog);

	return 0;
}
