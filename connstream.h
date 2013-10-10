/*
 * Author   : Mark Zammit
 * Contact  : markzammit@me.com
 */

 /** Connection Stream Interface
  *
  * Provides a generic interface for various connection
  * streams. This can possibly be replaced with a Windows
  * filesytem wrapper or an FTP handler or an SSH handler.
  * The terminology and usage is fairly generic.
  *
  * The main purpose of this is to allow for an application
  * to easily implement multiple connection stream options
  * easily as well as grant flexibility for additional streams.
  * Because this is an interface class the class can act
  * as a pointer to any class which has inheritied it so
  * no magic needs to be done with void pointers.
  *
  * E.G. Usage:
  *     class teststream : public connstream
  *     {
  *         public:
  *         teststream() {}
  *         ~teststream() {}
  *         connect() { .. // some magic here // ..}
  *         ....
  *     }
  *
  *     connstream * c = new teststream;
  */

#ifndef _CONNSTREAM_H
#define _CONNSTREAM_H

/* UNICODE SUPPORT */
#if defined(UNICODE) || defined(_UNICODE_)

#ifndef TSTR
#define TSTR	std::wstring
#endif

#ifndef _T(x)
#define _T(x)   ( L##x )
#endif

 /* ASCII Support */
#else

#ifndef TSTR
#define	TSTR    std::string
#endif

#ifndef _T(x)
#define _T(x)   ( x )
#endif

#endif

// Suggested return for getfilesize()
#define INVALID_FILE    -1

#include <string>

class connstream
{
    public:
        connstream() {};
        virtual ~connstream(void) {};
    public:
        /// CONNECTION METHODS ///
		/** bool connect(TSTR, TSTR, TSTR)
		 *  Connects/Opens to stream.
		 *      @url    : Fully qualified connection path mandatory
		 *      @uid    : User id optional
		 *      @pwd    : Password optional
		 *      @port   : Port optional
		 *  Returns : @true on connect of @false on failure
		 */
		 /** bool disconect(void)
		 *  Disconnects/Closes open stream.
		 *
		 *  Returns : @true on successful disconnect or @false if already disconnected
		 */
		virtual bool connect(TSTR, TSTR = _T(""), TSTR = _T(""), int = 0) = 0;
		virtual bool disconnect(void) = 0;

		/// GET/PUSH METHODS ///
		/** Uploads a file from host location to host location.
		 *      @location   : Client relative file location
		 *      @rename     : File name to be saved on host
		 *  Returns : @true on successful upload or @false on failed upload, could be
		 *              because file already exists on host or because there is no active connection.
		 */
		 /** Downloads a file from host location to client location.
		 *      @location   : Client relative file location
		 *      @rename     : File name to be saved on host
		 *  Returns : @true on successful upload or @false on failed upload, could be
		 *              because file already exists on client or because there is no active connection.
		 */
		virtual bool upload(TSTR, TSTR) = 0;
		virtual bool download(TSTR, TSTR) = 0;

		/// DIRECTORY METHODS ///
		/** bool cd(TSTR)
		 *  Change Directory to either a fully qualified path or relative path.
		 *      @dir  : Directory on host to change to
		 *  Returns : @true on succcessful change or @false if no directory exists or
		 *              if there is no active connection.
		 */
		 /** bool mkdir(TSTR)
		 *  Creates Directory for either a fully qualified path or relative path.
		 *      @dir  : Directory on host to create
		 *  Returns : @true on succcessful directory creation or @false if create failed or
		 *              if there is no active connection.
		 */
		 /** bool rmdir(TSTR)
		 *  Removes Directory for either a fully qualified path or relative path.
		 *      @dir  : Directory on host to remove
		 *  Returns : @true on succcessful removal or @false if directory doesn't exist or
		 *              if there is no active connection.
		 */
		 /** TSTR getcd(void)
		 *  Retrieves current directory path
		 *
		 *  Returns : currently pointed to host directory as a TSTR for unicode support
		 */
		virtual bool cd(TSTR) = 0;
		virtual bool mkdir(TSTR) = 0;
		virtual bool rmdir(TSTR) = 0;
		virtual TSTR getcd(void) = 0;

        /// FILE HANDLING METHODS ///
        /** bool remove(TSTR)
         *  Removes a file from FTP host.
         *      @filename   : Name of file to be deleted on host.
         *  Returns : @true of file was successfully removed or @false if no file exists or
         *              if there is no active connection.
         */
         /** bool rename(TSTR, TSTR)
         *  Renames a file on FTP host.
         *      @oldFilename    : Name of file to be renamed on host.
         *      @newFilename    : Name for file to be renamed to on host.
         *  Returns : @true of file was successfully renamed or @false if no file exists or
         *              if there is no active connection.
         */
         /** LONGLONG getfilesize(TSTR)
         *  File size of existing host file.
         *      @filename   : Name of file to retrieve size of on host.
         *  Returns : File size as a long long or @INVALID_FILE if file does not exist or
         *              if there is no active connection.
         */
		virtual bool        remove(TSTR) = 0;
		virtual bool        rename(TSTR, TSTR) = 0;
		virtual long long   getfilesize(TSTR) = 0;

        /// MISCELLANEOUS METHODS ///
         /** bool command(TSTR)
         *  Executes a user defined command based on stream type
         *      @command    : Command to be executed based of stream interface.
         *
         *  Returns : @true if command was successfully exeucted or @false if not or
         *              if there is no active connection.
         */
		virtual bool command(TSTR) = 0;

    protected:
        bool    m_connected;
};

#endif // _CONNSTREAM_H
