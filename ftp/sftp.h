// File transfer via SSH
#ifndef __SFTP_H
#define __SFTP_H

#define SFTP_ERROR_Ok 0
#define SFTP_ERROR -1
#define SFTP_ERROR_FAILED_TO_READ_LOCAL -2
#define SFTP_ERROR_FAILED_TO_READ_REMOTE -3
#define SFTP_ERROR_FAILED_TO_WRITE_LOCAL -4
#define SFTP_ERROR_FAILED_TO_WRITE_REMOTE -5

// Uploads a file to a server. Returns negative value if failed, 0 if ok.
// The connection credentials must be set earlier...
int sftpUpload(char *srcFileName,					// A file to transfer to a server
	char *dstFileName, 								// A name for the file when it is stored at the server
	char *dstDirectory); 							// A directory to transfer the file into. For Linux servers starts with '/'

																		// Downloads a file from a server. Returns negative value if failed, 0 if ok.
																		// The connection credentials must be set earlier...
int sftpDownload(char *dstFileName, 				// A file name to save the downloaded file under 
	char *srcFileName, 								// A file to download
	char *srcDirectory); 							// The directory to find the file at the server

																		// Test is a file exists at a server. "1" - yes, "0" - no, "-1" - error.
																		// The connection credentials must be set earlier...
int sftpTest(char *fileName, 					// A file name to test
	char *directory, 							// A server directory to test in
	long long int *size);					// If not NULL receives the size of file in bytes

																		// Uploads a file to a server. Returns negative value if failed, 0 if ok.
int sftpUploadP(char *srcFileName,					// A file to transfer to a server
	char *dstFileName, 								// A name for the file when it is stored at the server
	char *dstDirectory, 							// A directory to transfer the file into. For Linux servers starts with '/'
	char *server, char *user, char *password);		// Connection connection: server, login, password

																								// Downloads a file from a server. Returns negative value if failed, 0 if ok.
int sftpDownloadP(char *dstFileName, 				// A file name to save the downloaded file under 
	char *srcFileName, 								// A file to download
	char *srcDirectory, 							// The directory to find the file at the server
	char *server, char *user, char *password); 	// Connection connection: server, login, password

																							// Test is a file exists at a server. "1" - yes, "0" - no, "-1" - error.
int sftpTestP(char *fileName, 					// A file name to test
	char *directory, 							// A server directory to test in
	char *server, char *user, char *password,	// Connection connection: server, login, password
	long long *size);					// If not NULL, will be assigned with a size of the file in bytes

																		// 
int sftpSetCredentials(char *server, char *user, char *password);

int sftpInit(void); // Must be called before doing anything else...

void sftpClose(void); // Must be called when all transfers are finished...

int sftpGetLastError(int *sftpErrorCode, 		// 
	int *curlErrorCode, 						//
	char *curlErrorText);						//

#endif