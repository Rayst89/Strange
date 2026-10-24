#pragma once

namespace Debug
{
	extern void Print( const char* fmt, ... );
}

//#ifdef _DEBUG
//#define printf_s( fmt, ... ) Debug::Print( fmt, __VA_ARGS__ )
//#else
//#define DEBUG( fmt, ... )
//#endif // _DEBUG