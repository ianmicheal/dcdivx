/**************************************************************************************
 *                                                                                    *
 * This application contains code from OpenDivX and is released as a "Larger Work"    *
 * under that license. Consistant with that license, this application is released     *
 * under the GNU General Public License.                                              *
 *                                                                                    *
 * The OpenDivX license can be found at: http://www.projectmayo.com/opendivx/docs.php *
 * The GPL can be found at: http://www.gnu.org/copyleft/gpl.html                      *
 *                                                                                    *
 *                                                                                    *
 * Authors:																			  *
 *          Marc Dukette                                                              *
 **************************************************************************************/

#include "InputMedia.h"
//include "winbase.h"
//#include <windows.h>
//#include <stdlib.h>
//enum {
//
//	INPUT_TYPE_FILE,INPUT_TYPE_CACHEFILE,
//	INPUT_TYPE_HTTP
//};

/*
 * File mode enum
 */

//enum {
//
//	INPUT_OPEN_ASCII,
//	INPUT_OPEN_BINARY
//};

/*
 * Seek Enum
 */

enum {

	INPUT_SEEK_SET,
	INPUT_SEEK_CUR,
	INPUT_SEEK_END
};


	file_t file;
	int filesize;
	/*
 	 * HTTP/FTP stuff 
	 */


	int     lastReadPos;





/*
 * The main class
 */

int InputMediaOpen(char* lpFilename, int mode, int type, int reserve, int maxsize) 
{
	if(lpFilename) 
	{
	
		::file       = 0;
		//::mode       = -1;

		lastReadPos=0;
		::file =fs_open((char*)lpFilename,O_RDONLY);
		if (::file==0 ) return 0;
		
		filesize=fs_total(::file); //GetFileSize(::file,NULL);

	}
	return filesize;
}

void InputMediaClose() 
{
	if(::file) {

		fs_close(::file);
		::file=0;
	}
}


int InputMediaRead(char *data, unsigned int size)
{
		

	if ((size=fs_read(file,(void *) data,size)))
	{
		::lastReadPos+=size;
		return size;
	}
	return 0;
}


int InputMediaSeek(int size, unsigned int method)
{

			switch(method)
			{
			case INPUT_SEEK_SET:
				::lastReadPos = size;
			//	SetFilePointer(::file, size,NULL, FILE_BEGIN);
				fs_seek(::file,size,SEEK_SET);
				return size;
				break;

			
			case INPUT_SEEK_CUR:

				if(!size) {
					return ::lastReadPos;
				}
				else {
					::lastReadPos += size;
					//SetFilePointer(::file, size, NULL,FILE_CURRENT);
					fs_seek(::file,size,SEEK_CUR);
					return ::lastReadPos;
				}
				break;

			}
	return 0;
}




