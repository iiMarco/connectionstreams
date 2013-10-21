#include <ftp.h>

#if defined(_MSC_VER)

FTP::FTP()
{
    m_hInternet = InternetOpen(NULL,
                               INTERNET_OPEN_TYPE_DIRECT,
                               NULL,
                               NULL,
                               0);
    m_connected = false;
}

FTP::~FTP()
{
    if(m_connected)
    {
        InternetCloseHandle(m_hFtpSession);
        InternetCloseHandle(m_hInternet);
    }
}


bool FTP::Connect(TSTR lpszServerName, TSTR lpszUser, TSTR lpszPassword, int port)
{
    m_hFtpSession = InternetConnect(m_hInternet,
                                    (LPTSTR)lpszServerName.c_str(),
                                    port,
                                    (LPTSTR)lpszUser.c_str(),
                                    (LPTSTR)lpszPassword.c_str(),
                                    INTERNET_SERVICE_FTP,
                                    0,
                                    0);
    if (!m_hFtpSession)
        return false;

    return (m_connected = true);
}

bool FTP::Disconnect()
{
    if(m_connected)
    {
        InternetCloseHandle(m_hFtpSession);
        InternetCloseHandle(m_hInternet);
        m_connected = false;
    }

    return !m_connected;
}

bool FTP::Upload(TSTR lpszLocation, TSTR lpszRemFile)
{
    return !m_connected ? false : FtpPutFile(m_hFtpSession,
                                             (LPTSTR)lpszLocation.c_str(),
                                             lpszRemFile.empty() ?
                                                (LPTSTR)lpszLocation.c_str()
                                                : (LPTSTR)lpszRemFile.c_str(),
                                             FTP_TRANSFER_TYPE_BINARY,
                                             0) == TRUE;
}

bool FTP::Download(TSTR lpszLocation, TSTR lpszRemFile)
{
    return !m_connected ?
        false
        : FtpGetFile(m_hFtpSession,
                     (LPTSTR)lpszLocation.c_str(),
                     lpszRemFile.empty() ?
                        (LPTSTR)lpszLocation.c_str()
                        : (LPTSTR)lpszRemFile.c_str(),
                     0,
                     0,
                     FTP_TRANSFER_TYPE_BINARY,
                     0) == TRUE;
}

bool FTP::MakeDir(TSTR lpszDirectory)
{
    return !m_connected ?
        false
        : FtpCreateDirectory(m_hFtpSession,
                             (LPTSTR)lpszDirectory.c_str()) == TRUE;
}

bool FTP::ChangeDir(TSTR lpszDirectory)
{
    return !m_connected ?
        false
        : FtpSetCurrentDirectory(m_hFtpSession,
                                 (LPTSTR)lpszDirectory.c_str()) == TRUE;
}

bool FTP::Remove(TSTR lpszFileName)
{
    return !m_connected ?
        false
        : FtpDeleteFile(m_hFtpSession,
                        (LPTSTR)lpszFileName.c_str()) == TRUE;
}

bool FTP::Rename(TSTR lpszOldFileName, TSTR lpszNewFileName)
{
    return !m_connected ?
        false
        : FtpRenameFile(m_hFtpSession,
                        (LPTSTR)lpszOldFileName.c_str(),
                        (LPTSTR)lpszNewFileName.c_str()) == TRUE;
}

bool FTP::Exists(TSTR lpszFileName)
{
    if(!m_connected)
        return false;

    WIN32_FIND_DATA data;

    HINTERNET fHandle = FtpFindFirstFile(m_hFtpSession,
                                         (LPTSTR)lpszFileName.c_str(),
                                         &data,
                                         INTERNET_FLAG_HYPERLINK |
                                         INTERNET_FLAG_DONT_CACHE |
                                         INTERNET_FLAG_RELOAD |
                                         INTERNET_FLAG_RESYNCHRONIZE,
                                         NULL);
    if(!fHandle)
        return false;

    InternetCloseHandle(fHandle);

    return true;

}

bool FTP::RemoveDir(TSTR lpszDirectory)
{
    return !m_connected ?
        false
        : FtpRemoveDirectory(m_hFtpSession,
                             (LPTSTR)lpszDirectory.c_str()) == TRUE;
}

HINTERNET FTP::GetHandle()
{
    return m_connected ? m_hFtpSession : NULL;
}

TSTR FTP::CurrentDir()
{
    LPTSTR lpszCurrentDirectory = (LPTSTR)GlobalAlloc(GPTR, MAX_PATH + 1);
    DWORD lpdwCurrentDirectory  = MAX_PATH;
    TSTR strCurrentDirectory;

    if (!FtpGetCurrentDirectory(m_hFtpSession,
                                lpszCurrentDirectory,
                                &lpdwCurrentDirectory))
    {
        strCurrentDirectory = _T("ERROR-1");
        return strCurrentDirectory;
    }

    int len = (int)lpdwCurrentDirectory;
    for (int i = 0; i < len; i++)
    {
        strCurrentDirectory += lpszCurrentDirectory[i];
    }
    GlobalFree((HANDLE)lpszCurrentDirectory);

    return strCurrentDirectory;
}

LONGLONG FTP::GetFileSize(TSTR lpszFileName)
{
    if(!m_connected)
        return INVALID_FILE;

    HINTERNET fHandle = FtpOpenFile(m_hFtpSession,
                                    (LPTSTR)lpszFileName.c_str(),
                                    GENERIC_READ,
                                    INTERNET_FLAG_TRANSFER_BINARY,
                                    NULL);
    LARGE_INTEGER llFileSize;
    DWORD dwFileSizeLow = INVALID_FILE;
    DWORD dwFileSizeHigh = INVALID_FILE;

    if(fHandle)
    {
        dwFileSizeLow = FtpGetFileSize(fHandle,
                                       &dwFileSizeHigh);


        InternetCloseHandle(fHandle);
    }

    llFileSize.LowPart = dwFileSizeLow;
    llFileSize.HighPart = dwFileSizeHigh;

    return llFileSize.QuadPart;
}

bool FTP::GetFileData(TSTR lpszFileName, WIN32_FIND_DATA& data)
{
    if(!m_connected)
        return false;

    HINTERNET fHandle = FtpFindFirstFile(m_hFtpSession,
                                         (LPTSTR)lpszFileName.c_str(),
                                         &data,
                                         INTERNET_FLAG_HYPERLINK |
                                         INTERNET_FLAG_DONT_CACHE |
                                         INTERNET_FLAG_RELOAD |
                                         INTERNET_FLAG_RESYNCHRONIZE,
                                         NULL);
    if(!fHandle)
    {
        memset(&data, 0, sizeof(WIN32_FIND_DATA));
        return false;
    }

    InternetCloseHandle(fHandle);

    return true;
}

LIST FTP::SearchDir(TSTR lpszSearchStr)
{
    if(!m_connected)
        return LIST();

    LIST files;
    WIN32_FIND_DATA data;

    HINTERNET fHandle = FtpFindFirstFile(m_hFtpSession,
                                         (LPTSTR)lpszSearchStr.c_str(),
                                         &data,
                                         INTERNET_FLAG_HYPERLINK |
                                         INTERNET_FLAG_DONT_CACHE |
                                         INTERNET_FLAG_RELOAD |
                                         INTERNET_FLAG_RESYNCHRONIZE,
                                         NULL);
    if(!fHandle)
        return files;

    files.push_back(data.cFileName);

    while(InternetFindNextFile(fHandle, &data))
        files.push_back(data.cFileName);

    InternetCloseHandle(fHandle);

    return files;
}

bool FTP::Command(TSTR lpszCommand)
{
    return !m_connected ?
        false
        : FtpCommand(m_hFtpSession,
                     FALSE,
                     FTP_TRANSFER_TYPE_BINARY,
                     (LPTSTR)lpszCommand.c_str(),
                     0,
                     NULL) == TRUE;
}


int FTP::GetLastError(void)
{
    return (int)GetLastError();
}

#endif
