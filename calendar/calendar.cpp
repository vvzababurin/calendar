// calendar.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"

#include <stdio.h>
#include <time.h>
#include <stdio.h>
#include <memory.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fcntl.h>

#define _MYUNICODE

bool default_lang = true;

#ifdef _WIN32

#include <io.h>
#include <windows.h>
#include <winnls.h>
#include <conio.h>

#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#define DISABLE_NEWLINE_AUTO_RETURN  0x0008

void activateVirtualTerminal()
{       
	HANDLE handleOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD consoleMode;
	GetConsoleMode( handleOut , &consoleMode);
	consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	consoleMode |= DISABLE_NEWLINE_AUTO_RETURN;            
	SetConsoleMode( handleOut , consoleMode );
}

#else 

#undef _MYUNICODE

#include <unistd.h>
#include <termios.h>

char getch(void)
{
	char buf = 0;
	struct termios old = {0};
	fflush(stdout);
	if(tcgetattr(0, &old) < 0)
		perror("tcsetattr()");
	old.c_lflag &= ~ICANON;
	old.c_lflag &= ~ECHO;
	old.c_cc[VMIN] = 1;
	old.c_cc[VTIME] = 0;
	if(tcsetattr(0, TCSANOW, &old) < 0)
		perror("tcsetattr ICANON");
	if(read(0, &buf, 1) < 0)
		perror("read()");
	old.c_lflag |= ICANON;
	old.c_lflag |= ECHO;
	if(tcsetattr(0, TCSADRAIN, &old) < 0)
		perror("tcsetattr ~ICANON");
	return buf;
}

#endif

#ifdef _MYUNICODE 
	#define _WT(c) L##c
	#define WTEXT(c) L##c

	typedef wchar_t WTCHAR;

#ifdef _WIN32
	#define _wtsprintf wsprintf
	#define _wtprintf wprintf_s
	#define _wtscanf wscanf_s
	#define _wtgetch _getch
	#define _wtoupper towupper
	#define _wtopen _wfopen
	#define _wttell ftell
	#define _wtseek fseek
	#define _wtread fread
	#define _wtstrlen wcslen
	#define _wtstrcmp wcscmp
	#define _wtstrdup _wcsdup
#else
	#define _wtsprintf sprintf
	#define _wtprintf printf
	#define _wtscanf scanf
	#define _wtgetch getch
	#define _wtoupper toupper
	#define _wtopen fopen
	#define _wttell ftell
	#define _wtseek fseek
	#define _wtread fread
	#define _wtstrlen strlen
	#define _wtstrcmp strcmp
	#define _wtstrdup strdup
#endif

#else 
	#define _WT(c) c
	#define WTEXT(c) c

	typedef char WTCHAR;

#ifdef _WIN32
	#define _wtsprintf sprintf
	#define _wtprintf printf_s
	#define _wtscanf scanf_s
	#define _wtgetch _getch
	#define _wtoupper toupper
	#define _wtopen fopen
	#define _wttell ftell
	#define _wtseek fseek
	#define _wtread fread
	#define _wtstrlen strlen
	#define _wtstrcmp strcmp
	#define _wtstrdup _strdup
#else
	#define _wtsprintf sprintf
	#define _wtprintf printf
	#define _wtscanf scanf
	#define _wtgetch getch
	#define _wtoupper toupper
	#define _wtopen fopen
	#define _wttell ftell
	#define _wtseek fseek
	#define _wtread fread
	#define _wtstrlen strlen
	#define _wtstrcmp strcmp
	#define _wtstrdup strdup
#endif

#endif

typedef struct _translateLine
{
	WTCHAR* szName;
	WTCHAR* szValue;
} translateLine;

translateLine lang[] = {
	{ _WT("Label01"), _WT("Day of week to search ( -1 any, monday - 1, tuesday - 2, wednesday - 3 and so on ): ") },
	{ _WT("Label02"), _WT("Number of days from date ( -1 doesn't matter ): ") },
	{ _WT("Label03"), _WT("Year to search ( -1 any ): ") },
	{ _WT("Label04"), _WT("Month to search ( -1 any ): ") },
	{ _WT("Label05"), _WT("Day of month to search ( -1 any ): ") },
	{ _WT("Label06"), _WT("%u days from the specified date is: \n") },
	{ _WT("Label07"), _WT("( Ferma's day ) ") },
	{ _WT("Label08"), _WT("January ") },
	{ _WT("Label09"), _WT("February ") },
	{ _WT("Label10"), _WT("March ") },
	{ _WT("Label11"), _WT("April ") },
	{ _WT("Label12"), _WT("May ") },
	{ _WT("Label13"), _WT("June ") },
	{ _WT("Label14"), _WT("July ") },
	{ _WT("Label15"), _WT("August ") },
	{ _WT("Label16"), _WT("September ") },
	{ _WT("Label17"), _WT("October ") },
	{ _WT("Label18"), _WT("November ") },
	{ _WT("Label19"), _WT("December ") },
	{ _WT("Label20"), _WT("( Ferma's month ) ") },
	{ _WT("Label21"), _WT("monday %04u") },
	{ _WT("Label22"), _WT("tuesday %04u") },
	{ _WT("Label23"), _WT("wednesday %04u") },
	{ _WT("Label24"), _WT("thursday %04u") },
	{ _WT("Label25"), _WT("friday %04u") },
	{ _WT("Label26"), _WT("saturday %04u") },
	{ _WT("Label27"), _WT("sunday %04u") },
	{ _WT("Label28"), _WT(" ( Ferma's year )\n") },
	{ _WT("Label29"), _WT("%08u days from the birth of Christ, day of year %03u, %04u year\n") },
	{ _WT("Label30"), _WT("month") },
	{ _WT("Label31"), _WT(" ( Ferma's month )\n") },
	{ _WT("Label32"), _WT("%04u - leap year\n") },
	{ _WT("Label33"), _WT("%04u - not leap year\n") },
	{ _WT("Label34"), _WT("Ferma's day from the birth of Christ") },
	{ _WT("Label35"), _WT("Outcomes including ( %04u ):\n") },
	{ _WT("Label36"), _WT("%u Leap years\n") },
	{ _WT("Label37"), _WT("%u Not leap years\n") },
	{ _WT("Label38"), _WT("Recalculation? ( y/n ): ") },
	{ _WT("Label39"), _WT("%08u - %s") },
	{ NULL, NULL }
};

WTCHAR* getValue( const WTCHAR* label )
{
	int i = 0;
	while( lang[i].szName != NULL ) {
		if ( _wtstrcmp( label, lang[i].szName ) == 0 )
			return lang[i].szValue;
		i++;
	}	
	return NULL;
}

void setValue( const WTCHAR* label, const WTCHAR* value )
{
	int i = 0;
	while( lang[i].szName != NULL ) {
		if ( _wtstrcmp( label, lang[i].szName ) == 0 ) {
			lang[i].szValue = _wtstrdup( value );
			break;
		}
		i++;
	}
}

#define _LBL(c)				getValue(_WT(c))
#define _STLBL(c,e)			setValue(_WT(c),_WT(e))

//////////////////////////////////////////////////////
// TODO: translate to other lang 
//////////////////////////////////////////////////////
void loadLocaleLang()
{
	static WTCHAR filename[256];
	memset(filename, 0, sizeof(WTCHAR) * 256);

	WTCHAR* buff = NULL;

#ifdef _WIN32
	#ifdef _MYUNICODE
	int nLocaleInfo = GetLocaleInfoW(LOCALE_SYSTEM_DEFAULT, LOCALE_SNAME, 0, 0);
	#else
	int nLocaleInfo = GetLocaleInfoA(LOCALE_SYSTEM_DEFAULT, LOCALE_SNAME, 0, 0);
	#endif
	if (nLocaleInfo > 0) 
	{
		buff = (WTCHAR*)malloc((nLocaleInfo + 1) * sizeof(WTCHAR));
		if (buff) {
#ifdef _MYUNICODE			
			GetLocaleInfoW(LOCALE_SYSTEM_DEFAULT, LOCALE_SNAME, buff, nLocaleInfo);
#else
			GetLocaleInfoA(LOCALE_SYSTEM_DEFAULT, LOCALE_SNAME, buff, nLocaleInfo);
#endif
			_wtsprintf(filename, _WT("translate/%s.lang"), buff);
			free(buff);
		}
		else 
		{
			// TODO: fatal crash
		}
	} 
	else
	{
		_wtsprintf(filename, _WT("translate/en-US.lang"));
	}
#else
	buff = getenv("LANG");
	if ( _wtstrlen(buff) > 0 )
	{
		char* ch = buff;
		while (*ch != 0) {
			if (*ch == '_') *ch = '-';
			else if (*ch == '.') {
				*ch = 0x0;
				break;
			}
			ch++;
		}
		_wtsprintf(filename, _WT("translate/%s.lang"), buff);
	}
	else
	{
		_wtsprintf(filename, _WT("translate/en-US.lang"));
	}	
	
#endif

	FILE* f = _wtopen( filename, _WT("rb") );
	if (f != NULL) {
		default_lang = false;
		_wtseek( f, 0, SEEK_END );
		long int nbytes = _wttell(f);
		_wtseek( f, 0, SEEK_SET );
		buff = (WTCHAR*)malloc( nbytes );
		size_t rbytes = _wtread( buff, 1, nbytes, f ); 
		if ( rbytes != nbytes ) default_lang = true;
		fclose(f);
	} 

	int bs = 0;  // Backslash
	int dq = 0;	 // Double quotas
	int cr = 0;	 // CR
	int lf = 0;	 // LF
	int ff = 0;  // First flag name

	std::string t_name = "";
	std::string t_value = "";
	std::string tag = "";

	if ( default_lang == false ) 
	{
		char* ch = (char*)buff;
		do {
			if ( bs == 0 ) 
			{
				if ( *ch == '\\' ) {
					// set backslash is not null
					bs = 1;
					continue;
				}
				if ( *ch == '"' ) {		// "
					dq++;
					continue;
				}
				if ( *ch == ':' && dq != 1 ) {		// :
					if ( dq == 2 ) {
					   t_name = tag;
					   tag = "";
					   dq = 0;
					}
					continue;
				}
				if ( *ch == ' ' && dq != 1 ) {
					continue;
				}
				if ( *ch == 0x0d ) {   // CR
					cr = 1;
					continue;
				}
				if ( *ch == 0x0a ) {   // LF
					if ( dq == 2 ) {
						t_value = tag;
						tag = "";

						#ifdef _MYUNICODE

						std::wstring t_name_w;
						std::wstring t_value_w;  

						int wcount = 0;

						wcount = MultiByteToWideChar(1251, 0, t_name.c_str(), -1, NULL, 0);
						t_name_w.resize( wcount );
						MultiByteToWideChar(1251, 0, t_name.c_str(), -1, &t_name_w[0], wcount);

						wcount = MultiByteToWideChar(1251, 0, t_value.c_str(), -1, NULL, 0);
						t_value_w.resize( wcount );
						MultiByteToWideChar(1251, 0, t_value.c_str(), -1, &t_value_w[0], wcount);

						setValue( t_name_w.c_str(), t_value_w.c_str() );
						#else
						setValue( t_name.c_str(), t_value.c_str() );
						#endif

						dq = 0;
					}
					cr = 0;
					continue;
				}

				cr = 0;	 // CR
				lf = 0;	 // LF
				ff = 0;  // First flag name

			} else if ( bs == 1 ) {
				if ( *ch == 'n' ) {
					tag += '\n';	
				} else if( *ch == 'r' ) {
					tag += '\r';	
				} else if( *ch == 't' ) {
					tag += '\t';	
				}
				bs = 0;			
				continue;
			}

			tag = tag + *ch;

		} while( *(ch++) != NULL );

		if ( buff != NULL ) free( buff );
	}

	// _wtprintf( _WT("default lang = %s\n"), ( default_lang == true ) ? _WT("true") : _WT("false") );
}

enum COLORS {
	NC=-1,
	BLACK,
	RED,
	GREEN,
	YELLOW,
	BLUE,
	MAGENTA,
	CYAN,
	WHITE,
};

/**
* Colorize terminal colors ANSI escape sequences.
*
* @param font font color (-1 to 7), see COLORS enum
* @param font color intensity ( true or false )
* @param back background color (-1 to 7), see COLORS enum
* @param back color intensity ( true or false )
* @param style font style (1==bold, 4==underline)
**/

const WTCHAR *colorize(int font, bool fintensity = false, int back = -1, bool bintensity = false, int style = -1) 
{
	static WTCHAR code[20];
	if (font >= 0) font += 30;
	else font = 0;
	if ( fintensity ) font += 60;
	if (back >= 0) back += 40;
	else back = 0;
	if ( bintensity ) back += 60;
	if (back > 0 && style > 0) {
		_wtsprintf( code, _WT("\033[%d;%d;%dm"), font, back, style );
	} else if (back > 0) {
		_wtsprintf( code, _WT("\033[%d;%dm"), font, back );
	} else {
		_wtsprintf( code, _WT("\033[%dm"), font );
	}
	return code;
}

void calendar()
{
	int dayofweek = 4;
	int monthofyear = 0;
	int dayofmonth = 0;
	int sy = -1;
	int sd = -1;
	int sm = -1;
	int dw = -1;
	int ef = -1;
	int dd = -1;
	int cd = 0;
	int v29 = 0;
	int vvy = 0;
	int nvy = 0;
	int dc = 0;
	WTCHAR wch = 'y';
loop:
	dayofweek = 4;
	monthofyear = 0;
	dayofmonth = 0;
	sy = -1;
	sd = -1;
	sm = -1;
	dw = -1;
	ef = -1;
	cd = 0;
	v29 = 0;
	vvy = 0;
	nvy = 0;
	dc = 0;
	wch = 'y';
	///////////////////////////////////////////////////
	// начало от рождения
	///////////////////////////////////////////////////
	_wtprintf( _LBL("Label01") );
	_wtscanf( _WT("%d"), &dw );
	_wtprintf( _LBL("Label02") );
	_wtscanf( _WT("%d"), &dd );
	_wtprintf( _LBL("Label03") );
	_wtscanf( _WT("%d"), &sy );
	_wtprintf( _LBL("Label04") );
	_wtscanf( _WT("%d"), &sm );
	_wtprintf( _LBL("Label05") );
	_wtscanf( _WT("%d"), &sd );
	_wtprintf( _WT("\n") );
	/////////////////////////////////////////////////////////
	// FILE* f = _wfreopen(L"calendar.out", L"w", stdout);
	/////////////////////////////////////////////////////////
	dw = ( dw != -1 ) ? --dw : dw;
	sm = ( sm != -1 ) ? --sm : sm;
	time_t ctime = time( NULL );
	time_t maxyear = 1970 + ctime / 31537970;
	for ( int i = 0; i <= maxyear; i++ ) 
	{
		if ( i % 4 == 0 ) {
			v29 = 1;
			vvy++;
		} else {
			v29 = 0;        
			nvy++;
		}
		int maxdays = (v29 == 1) ? 366 : 365;
		for ( int j = 1; j <= maxdays; j++ ) 
		{                            
			if ( monthofyear == 0 && dayofmonth == 31 ) {
				++monthofyear;
				dayofmonth = 0;
			} else if ( monthofyear == 1 && dayofmonth == ( (v29 == 1) ? 29 : 28 ) ) {
				++monthofyear;
				dayofmonth = 0;
			} else if ( monthofyear == 2 && dayofmonth == 31 ) {
				++monthofyear;
				dayofmonth = 0;
			} else if ( monthofyear == 3 && dayofmonth == 30 ) {
				++monthofyear;
				dayofmonth = 0;
			} else if ( monthofyear == 4 && dayofmonth == 31 ) {
				++monthofyear;
				dayofmonth = 0;
			} else if ( monthofyear == 5 && dayofmonth == 30 ) {
				++monthofyear;
				dayofmonth = 0;
			} else if ( monthofyear == 6 && dayofmonth == 31 ) {
				++monthofyear;
				dayofmonth = 0;
			} else if ( monthofyear == 7 && dayofmonth == 31 ) {
				++monthofyear;
				dayofmonth = 0;
			} else if ( monthofyear == 8 && dayofmonth == 30 ) {
				++monthofyear;
				dayofmonth = 0;
			} else if ( monthofyear == 9 && dayofmonth == 31 ) {
				++monthofyear;
				dayofmonth = 0;
			} else if ( monthofyear == 10 && dayofmonth == 30 ) {
				++monthofyear;
				dayofmonth = 0;
			} else if ( monthofyear == 11 && dayofmonth == 31 ) {
				monthofyear = 0;
				dayofmonth = 0;
			}
			dc++;
			dayofmonth++;
			if ( ef == 1 ) cd = cd + 1;
			if ( ( ( dw == dayofweek || dw == -1 ) &&
				( sy == i || sy == -1 ) && 
				( sm == monthofyear || sm == -1 ) &&
				( sd == dayofmonth || sd == -1 ) ) || ( dd == cd ) ) 
			{
				ef = 1;
				_wtprintf( colorize( WHITE, true ) );
				if ( dd == cd )  _wtprintf( _LBL("Label06"), dd );
				_wtprintf( colorize( WHITE, true ) );
				_wtprintf( _WT("%02u "), dayofmonth );
				_wtprintf( colorize( GREEN, true ) );
				if ( dayofmonth % 4 == 2 ) _wtprintf( _LBL("Label07") );
				_wtprintf( colorize( WHITE, true ) );
				if ( monthofyear == 0 ) _wtprintf( _LBL("Label08") );
				if ( monthofyear == 1 ) _wtprintf( _LBL("Label09") );
				if ( monthofyear == 2 ) _wtprintf( _LBL("Label10") );
				if ( monthofyear == 3 ) _wtprintf( _LBL("Label11") );
				if ( monthofyear == 4 ) _wtprintf( _LBL("Label12") );
				if ( monthofyear == 5 ) _wtprintf( _LBL("Label13") );
				if ( monthofyear == 6 ) _wtprintf( _LBL("Label14") );
				if ( monthofyear == 7 ) _wtprintf( _LBL("Label15") );
				if ( monthofyear == 8 ) _wtprintf( _LBL("Label16") );
				if ( monthofyear == 9 ) _wtprintf( _LBL("Label17") );
				if ( monthofyear == 10 ) _wtprintf( _LBL("Label18") );
				if ( monthofyear == 11 ) _wtprintf( _LBL("Label19") );
				_wtprintf( colorize( RED, true ) );
				if ( ( monthofyear + 1 ) % 4 == 2 ) _wtprintf( _LBL("Label20") );
				_wtprintf( colorize( WHITE, true ) );
				if ( dayofweek == 0 ) _wtprintf( _LBL("Label21"), i );
				if ( dayofweek == 1 ) _wtprintf( _LBL("Label22"), i );
				if ( dayofweek == 2 ) _wtprintf( _LBL("Label23"), i );
				if ( dayofweek == 3 ) _wtprintf( _LBL("Label24"), i );
				if ( dayofweek == 4 ) _wtprintf( _LBL("Label25"), i );
				if ( dayofweek == 5 ) _wtprintf( _LBL("Label26"), i );
				if ( dayofweek == 6 ) _wtprintf( _LBL("Label27"), i );
				_wtprintf( colorize( BLUE, true ) );
				if ( i % 4 == 2 ) _wtprintf( _LBL("Label28") );
				else _wtprintf( _WT("\n") );
				_wtprintf( colorize( YELLOW, true ) );
				_wtprintf( _LBL("Label29"), dc, j, i );
				_wtprintf( colorize( WHITE, true ) );
				int mt = i * 12 + (monthofyear + 1);
				_wtprintf( _LBL("Label39"), mt, _LBL("Label30") );
				_wtprintf( colorize( BLUE, true ) );
				if (mt % 4 == 2) _wtprintf( _LBL("Label31") );
				else _wtprintf( _WT("\n") );
				_wtprintf( colorize( CYAN, true ) );
				if (v29 == 1)
					_wtprintf( _LBL("Label32"), i);
				else
					_wtprintf( _LBL("Label33"), i);
				_wtprintf( colorize( MAGENTA, true ) );
				if ( dc % 4 == 2 ) _wtprintf( _LBL("Label39"), dc, _LBL("Label34") );
				_wtprintf( _WT("\n") );
			}
			( dayofweek == 6 ) ? dayofweek = 0 : dayofweek++;
		}
	}

	_wtprintf( colorize( WHITE, true ) );

	_wtprintf( _LBL("Label35"), maxyear );
	_wtprintf( _LBL("Label36"), vvy );
	_wtprintf(_LBL("Label37"), nvy );

	_wtprintf( _WT("\n") );

loop2:
	_wtprintf( _LBL("Label38") );

	wch = _wtgetch();  
	wch = _wtoupper( wch );

	_wtprintf( _WT("%c\n"), wch, 1);
	_wtprintf( _WT("\n") );

	if ( wch == 'Y' || wch == 'y' ) goto loop;
	if ( wch == 'N' || wch == 'n' ) return;

	_wtscanf( _WT("%*[^\r\n]") );

	goto loop2;
}

#ifdef _WIN32
	int wmain(int argc, wchar_t* argv[])
#else
	int main(int argc, char* argv[])
#endif
{
#ifdef _WIN32
	#ifdef _MYUNICODE
		_setmode(_fileno(stdout), _O_U8TEXT);
		_setmode(_fileno(stdin),  _O_U8TEXT);
		_setmode(_fileno(stderr), _O_U8TEXT);

		SetConsoleCP(65001);
		SetConsoleOutputCP(65001);
	#else
		_setmode(_fileno(stdout), _O_TEXT);
		_setmode(_fileno(stdin),  _O_TEXT);
		_setmode(_fileno(stderr), _O_TEXT);

		SetConsoleCP(1251);
		SetConsoleOutputCP(1251);
	#endif
	activateVirtualTerminal();    
#endif

	loadLocaleLang();
			
	_wtprintf( _WT("%s"), colorize( WHITE, true ) );
	calendar();

	return 0;
}

