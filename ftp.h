/*
 * Author   : Mark Zammit
 * Contact  : iimarco@me.com
 * Version  : 1.13.11.21
 */

#ifndef _FTP_H_
#define _FTP_H_

/* Checks that Microsoft Visual C++ compiler is in use
 * otherwise this will not compile */
#if !defined(_MSC_VER)
#error ftp.h only supported by MSVC Compiler 2005+
#endif

#if defined(_MSC_VER)

#include <Windows.h>
#include <WinInet.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include "connstream.h"
#pragma comment (lib, "wininet.lib")

/** NOTE: Use GetLastError() to examine the @false result from any method */

class FTP : public connstream
{
    public:
        /** Creates an internet connection and sets up local variables */
        FTP(void);
        /** Disconnects session on scope end if not already disconnected */
        virtual ~FTP(void);

        /// CONNECTION METHODS ///
        /** bool connect(TSTR, TSTR, TSTR)
         *  Connects to FTP server.
         *      @lpszServerName : Fully qualified domain name or IP Address
         *      @lpszUser       : User id
         *      @lpszPassword   : User password
         *  Returns : @true on connect of @false on failure
         */
        /** bool disconect(void)
         *  Disconnects from active FTP server.
         *
         *  Returns : @true on successful disconnect or @false if already disconnected
         */
        bool Connect(TSTR lpszServerName,
                     TSTR lpszUser,
                     TSTR lpszPassword,
                     int port = INTERNET_DEFAULT_FTP_PORT);
        bool Disconnect(void);

        /// GET/PUSH METHODS ///
        /** Uploads a file from client location to server location.
         *      @lpszLocation   : Client relative file location
         *      @lpszRemFile    : File name to be saved on server, leave blank for original name
         *  Returns : @true on successful upload or @false on failed upload, could be
         *              because file already exists on server or because there is no active connection.
         */
         /** Downloads a file from server location to client location.
         *      @lpszLocation    : Server relative file loaction
         *      @lpszRemName     : File name to be saved on client, leave blank for original name
         *  Returns : @true on successful upload or @false on failed upload, could be
         *              because file already exists on client or because there is no active connection.
         */
        bool Upload(TSTR lpszLocation, TSTR lpszRemFile);
        bool Download(TSTR lpszLocation, TSTR lpszRemName);

        /// DIRECTORY METHODS ///
        /** bool ChangeDir(TSTR)
         *  Change Directory to either a fully qualified path or relative path.
         *      @lpszDirectory  : Directory on server to change to
         *  Returns : @true on succcessful change or @false if no directory exists or
         *              if there is no active connection.
         */
        /** bool MakeDir(TSTR)
         *  Creates Directory for either a fully qualified path or relative path.
         *      @lpszDirectory  : Directory on server to create
         *  Returns : @true on succcessful directory creation or @false if create failed or
         *              if there is no active connection.
         */
        /** bool RemoveDir(TSTR)
         *  Removes Directory for either a fully qualified path or relative path.
         *      @lpszDirectory  : Directory on server to remove
         *  Returns : @true on succcessful removal or @false if directory doesn't exist or
         *              if there is no active connection.
         */
        /** TSTR CurrentDir(void)
         *  Retrieves current directory path
         *
         *  Returns : currently pointed to server directory as a TSTR for unicode support
         */
        /** LIST SearchDir(TSTR)
         *  Gets a directory listing based off a system valid search string.
         *      @lpszSearchStr  : Search string thats system valid, e.g. *.txt, file.txt, *.* etc.
         *  Returns a vector list of file names.
         */
        bool    ChangeDir(TSTR lpszDirectory);
        bool    MakeDir(TSTR lpszDirectory);
        bool    RemoveDir(TSTR lpszDirectory);
        TSTR    CurrentDir(void);
        LIST    SearchDir(TSTR lpszSearchStr);

        /// FILE HANDLING METHODS ///
        /** bool Remove(TSTR)
         *  Removes a file from FTP server.
         *      @lpszFileName   : Name of file to be deleted on server.
         *  Returns : @true of file was successfully removed or @false if no file exists or
         *              if there is no active connection.
         */
        /** bool Rename(TSTR, TSTR)
         *  Renames a file on FTP server.
         *      @lpszFileName       : Name of file to be renamed on server.
         *      @lpszNewFileName    : Name for file to be renamed to on server.
         *  Returns : @true of file was successfully renamed or @false if no file exists or
         *              if there is no active connection.
         */
        /** bool Exists(TSTR)
         *  Checks whether a file exists on the host.
         *      @filename   : Name of file to check existence of.
         *  Returns : @true if file is present or @false if no file exists or
         *              if there is no active connection.
         */
        /** LONGLONG GetFileSize(TSTR)
         *  File size of existing server file.
         *      @lpszFileName   : Name of file to retrieve size of on server.
         *  Returns : File size as a long long or @INVALID_FILE if file does not exist or
         *              if there is no active connection.
         */
        /** bool GetFileData(TSTR, WIN32_FIND_DATA&)
         *  File size of existing server file.
         *      @lpszFileName   : Name of file to retrieve size of on server.
         *      @fileData       : Stores the resultant WIN32_FIND_DATA struct
         *  Returns : @true of file was successfully renamed or @false if no file exists or
         *              if there is no active connection.
         */
        bool        Remove(TSTR lpszFileName);
        bool        Rename(TSTR lpszOldFileName, TSTR lpszNewFileName);
        bool        Exists(TSTR lpszFilename);
        LONGLONG    GetFileSize(TSTR lpszFileName);
        bool        GetFileData(TSTR lpszFileName, WIN32_FIND_DATA& data);


        /// MISCELLANEOUS METHODS ///
        /** HINTERNET GetHandle(void)
         *  Gets handle to an open FTP Session for cross-API use
         *
         *  Returns : HINTERNET handle to FTP Sessions or NULL if there is no connection
         *              NB: NULL is returned so that deallocated (bad handles) aren't returned.
         */
        /** bool Command(TSTR)
         *  Executes a user defined FTP Command
         *
         *  Returns : @true if command was successfully exeucted or @false if not or
         *              if there is no active connection.
         */
        HINTERNET   GetHandle(void);
        bool        Command(TSTR lpszCommand);

        /** int GetLastError()
         *  Retrieves the last error (return) value executed by the last method call.
         *  If a method returned false then this method can be called for more detail.
         *
         *  Returns : 0x00 on successful last method call, or 0x01+ for a failed/info result.
         *      NB: Refer to MSDN System Error Codes for information on return codes.
         *      Ref: http://msdn.microsoft.com/en-us/library/windows/desktop/ms681381(v=vs.85).aspx
         */
        int GetLastError(void);

    private:
        HINTERNET   m_hInternet;
        HINTERNET   m_hFtpSession;
};

#endif

#endif
