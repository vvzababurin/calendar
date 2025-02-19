// calendar.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"

#include <stdio.h>
#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fcntl.h>

#define _MYUNICODE

#ifdef _WIN32

#include <io.h>
#include <windows.h>
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
#else
	#define _wtsprintf sprintf
	#define _wtprintf printf
	#define _wtscanf scanf
	#define _wtgetch getch
	#define _wtoupper toupper
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
#else
	#define _wtsprintf sprintf
	#define _wtprintf printf
	#define _wtscanf scanf
	#define _wtgetch getch
	#define _wtoupper toupper
#endif

#endif

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

    _wtprintf( _WT("Искомый день недели ( -1 - любой, понедельник - 1, вторник - 2, среда - 3 и т.д. ): ") );
    _wtscanf( _WT("%d"), &dw );

    _wtprintf( _WT("Количество дней с даты ( -1 всё равно ): ") );
    _wtscanf( _WT("%d"), &dd );

    _wtprintf( _WT("Искомый год ( -1 любой ): ") );
    _wtscanf( _WT("%d"), &sy );

    _wtprintf( _WT("Искомый месяц ( -1 любой ): ") );
    _wtscanf( _WT("%d"), &sm );

    _wtprintf( _WT("Искомый день месяца ( -1 любой ): ") );
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

			if ( dd == cd )  _wtprintf( _WT("%u дней с вышеуказанной даты это: \n"), dd );
					   
			_wtprintf( colorize( WHITE, true ) );

			_wtprintf( _WT("%02u "), dayofmonth );

			_wtprintf( colorize( GREEN, true ) );

			if ( dayofmonth % 4 == 2 ) _wtprintf( _WT("( день ферма ) ") );

			_wtprintf( colorize( WHITE, true ) );

			if ( monthofyear == 0 ) _wtprintf( _WT("января ") );
			if ( monthofyear == 1 ) _wtprintf( _WT("февраля ") );
			if ( monthofyear == 2 ) _wtprintf( _WT("марта ") );
			if ( monthofyear == 3 ) _wtprintf( _WT("апреля ") );
			if ( monthofyear == 4 ) _wtprintf( _WT("мая ") );
			if ( monthofyear == 5 ) _wtprintf( _WT("июня ") );
			if ( monthofyear == 6 ) _wtprintf( _WT("июля ") );
			if ( monthofyear == 7 ) _wtprintf( _WT("августа ") );
			if ( monthofyear == 8 ) _wtprintf( _WT("сентября ") );
			if ( monthofyear == 9 ) _wtprintf( _WT("октября ") );
			if ( monthofyear == 10 ) _wtprintf( _WT("ноября ") );
			if ( monthofyear == 11 ) _wtprintf( _WT("декабря ") );

			_wtprintf( colorize( RED, true ) );

			if ( ( monthofyear + 1 ) % 4 == 2 ) _wtprintf( _WT("( месяц ферма ) ") );

			_wtprintf( colorize( WHITE, true ) );

			if ( dayofweek == 0 ) _wtprintf( _WT("понедельник %04u"), i );
			if ( dayofweek == 1 ) _wtprintf( _WT("вторник %04u"), i );
			if ( dayofweek == 2 ) _wtprintf( _WT("среда %04u"), i );
			if ( dayofweek == 3 ) _wtprintf( _WT("четверг %04u"), i );
			if ( dayofweek == 4 ) _wtprintf( _WT("пятница %04u"), i );
			if ( dayofweek == 5 ) _wtprintf( _WT("суббота %04u"), i );
			if ( dayofweek == 6 ) _wtprintf( _WT("воскресенье %04u"), i );
				_wtprintf( colorize( YELLOW, true ) );

			if ( i % 4 == 2 ) 
			_wtprintf( _WT(" ( год ферма )\n") );
				else
			_wtprintf( _WT("\n") );

			_wtprintf( colorize( YELLOW, true ) );

			_wtprintf( _WT("%08u день от рождества христова, день года %03u, %04u год\n"), dc, j, i );

			_wtprintf( colorize( WHITE, true ) );

			int mt = i * 12 + (monthofyear + 1);

			_wtprintf( _WT("%08u - %s"), mt, _WT("месяц") );

			_wtprintf( colorize( BLUE, true ) );

			if (mt % 4 == 2)
				_wtprintf( _WT(" ( месяц ферма )\n") );
			else

			_wtprintf(_WT("\n"));

			_wtprintf( colorize( CYAN, true ) );

			if (v29 == 1)
				_wtprintf(_WT("%04u - високосный\n"), i);
			else
				_wtprintf(_WT("%04u - не високосный\n"), i);
			
			_wtprintf( colorize( MAGENTA, true ) );

			if ( dc % 4 == 2 ) _wtprintf( _WT("%08u - %s\n"), dc, _WT("день ферма от рождества христова"));

			_wtprintf( _WT("\n") );
		}
        
		( dayofweek == 6 ) ? dayofweek = 0 : dayofweek++;
	    }
	}

	_wtprintf( colorize( WHITE, true ) );

	_wtprintf( _WT("Итого, включая ( %04u ):\n"), maxyear );
	_wtprintf( _WT("Високосных %u лет\n"), vvy );
	_wtprintf( _WT("Не високосных %u лет\n"), nvy );

	_wtprintf( _WT("\n") );

loop2:
	_wtprintf( _WT("Повторить раcчёт? ( y/n ): ") );

	wch = _wtgetch();  
	wch = _wtoupper( wch );

	_wtprintf( _WT("%c\n"), wch, 1);
	_wtprintf( _WT("\n") );

	if ( wch == 'Y' || wch == 'y' ) goto loop;
	if ( wch == 'N' || wch == 'n' ) return;

	_wtscanf( _WT("%*[^\r\n]") );

	goto loop2;
}

/*
void regulatorVout()
{
    float vref = 1.3f;
    float r1 = 680.0f;
    float r2 = 10000.0f;
    float iadj = 0.000050f;     // 50μA, 50мкА
    float vout = 0.0f;
    wchar_t wch = 'y';
    int p = 0;
epq1:
    wprintf_s( L"Напряжение между контактами выхода и регулирования V(ref): " );
    if ( wscanf_s( L"%f", &vref ) == 0 ) {
        wscanf_s( L"%*[^\r\n]" );
        wprintf_s( L"Введите номинал напряжения...\n" );
        goto epq1;
    }
    wscanf_s( L"%*[^\r\n]" );
epq2:    
    wprintf_s( L"Сопротивление R1: " );
    if ( wscanf_s( L"%f", &r1 ) == 0 ) {
        wscanf_s( L"%*[^\r\n]" );
        wprintf_s( L"Введите номинал сопротивления...\n" );
        goto epq2;
    }
    wscanf_s( L"%*[^\r\n]" );
epq3:    
    wprintf_s( L"Сопротивление R2: " );
    if ( wscanf_s( L"%f", &r2 ) == 0 ) {
        wscanf_s( L"%*[^\r\n]" );
        wprintf_s( L"Введите номинал сопротивления...\n" );
        goto epq3;
    }
    wscanf_s( L"%*[^\r\n]" );
epq4:
    wprintf_s( L"Сила тока регулирования I(adj): " );
    if ( wscanf_s( L"%f", &iadj ) == 0 ) {
        wscanf_s( L"%*[^\r\n]" );
        wprintf_s( L"Введите номинал сопротивления...\n" );
        goto epq4;
    } 
    wscanf_s( L"%*[^\r\n]" );        
    vout = vref * ( 1 + r2 / r1 ) + iadj * r2;
    wprintf_s( L"\n" );
    wprintf_s( L"Максимальное напряжение на выходе: %.6f\n\n", vout );
ep2:
    wprintf_s( L"Повторить раcчёт? (y/n): " );
    wch = _getwch();  
    wprintf_s( L"%c\n\n", wch, 1);
    if ( wch == 'Y' || wch == 'y' ) goto epq1;
    if ( wch == 'N' || wch == 'n' ) return;
    wscanf_s( L"%*[^\r\n]" );
    goto ep2;
}

*/

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
			
	_wtprintf( _WT("%s"), colorize( WHITE, true ) );

	///////////////////////////////////
	calendar();

//    regulatorVout();

	return 0;
}

