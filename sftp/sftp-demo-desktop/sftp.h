// File transfer via SSH
#ifndef __SFTP_H
#define __SFTP_H

#include "stdafx.h"

#define SFTP_UPLOAD_OVERWRITE 0x01
#define SFTP_UPLOAD_RESUME 0x02

#define SFTP_ERROR_Ok 0
#define SFTP_ERROR -1
#define SFTP_ERROR_WRITING_REMOTE_FILE -100
#define SFTP_ERROR_READING_REMOTE_FILE -101
#define SFTP_ERROR_WRITING_LOCAL_FILE -110
#define SFTP_ERROR_READING_LOCAL_FILE -111

// Uploads a file to a server. Returns negative value if failed, 0 if ok.
int sftpUpload(char *srcFileName,					// A file to transfer to a server
	char *dstFileName, 								// A name for the file when it is stored at the server
	char *dstDirectory, 		// Connection requisites: server, login, password
	int overwriteIfExists);

// Downloads a file from a server. Returns negative value if failed, 0 if ok.
int sftpDownload( char *dstFileName, 				// A file name to save the downloaded file under 
	char *srcFileName, 								// A file to download
	char *srcDirectory ); 	// Connection requisites: server, login, password

// Test is a file exists at a server. "1" - yes, "0" - no, "-1" - error.
int sftpTest(char *fileName, 					// A file name to test
	char *directory, 							// A server directory to test in
	unsigned long int *size,					// If not NULL receives the size of file in bytes
	int *error); // If not NULL receives the error code (if occured)

int sftpInit(char *server, char *user, char *password); // Must be called before doing anything else...

int sftpClose( void ); // Must be called when all transfers are finished...

int sftpGetLastError( int *, int *, char * );						//

#endif