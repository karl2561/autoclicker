#include "conversion.h"

#include <linux/input-event-codes.h>
#include <stddef.h>

const char *kctc(int code)
{
	switch (code) {
	// Letters
	case KEY_A:
		return "a";
	case KEY_B:
		return "b";
	case KEY_C:
		return "c";
	case KEY_D:
		return "d";
	case KEY_E:
		return "e";
	case KEY_F:
		return "f";
	case KEY_G:
		return "g";
	case KEY_H:
		return "h";
	case KEY_I:
		return "i";
	case KEY_J:
		return "j";
	case KEY_K:
		return "k";
	case KEY_L:
		return "l";
	case KEY_M:
		return "m";
	case KEY_N:
		return "n";
	case KEY_O:
		return "o";
	case KEY_P:
		return "p";
	case KEY_Q:
		return "q";
	case KEY_R:
		return "r";
	case KEY_S:
		return "s";
	case KEY_T:
		return "t";
	case KEY_U:
		return "u";
	case KEY_V:
		return "v";
	case KEY_W:
		return "w";
	case KEY_X:
		return "x";
	case KEY_Y:
		return "y";
	case KEY_Z:
		return "z";

	// Numbers
	case KEY_1:
		return "1";
	case KEY_2:
		return "2";
	case KEY_3:
		return "3";
	case KEY_4:
		return "4";
	case KEY_5:
		return "5";
	case KEY_6:
		return "6";
	case KEY_7:
		return "7";
	case KEY_8:
		return "8";
	case KEY_9:
		return "9";
	case KEY_0:
		return "0";

	// Function keys
	case KEY_F1:
		return "F1";
	case KEY_F2:
		return "F2";
	case KEY_F3:
		return "F3";
	case KEY_F4:
		return "F4";
	case KEY_F5:
		return "F5";
	case KEY_F6:
		return "F6";
	case KEY_F7:
		return "F7";
	case KEY_F8:
		return "F8";
	case KEY_F9:
		return "F9";
	case KEY_F10:
		return "F10";
	case KEY_F11:
		return "F11";
	case KEY_F12:
		return "F12";

	// Symbols (main keyboard)
	case KEY_MINUS:
		return "-";
	case KEY_EQUAL:
		return "=";
	case KEY_LEFTBRACE:
		return "[";
	case KEY_RIGHTBRACE:
		return "]";
	case KEY_SEMICOLON:
		return ";";
	case KEY_APOSTROPHE:
		return "'";
	case KEY_GRAVE:
		return "`";
	case KEY_BACKSLASH:
		return "\\";
	case KEY_COMMA:
		return ",";
	case KEY_DOT:
		return ".";
	case KEY_SLASH:
		return "/";
	case KEY_SPACE:
		return " ";
	case KEY_TAB:
		return "TAB";
	case KEY_ENTER:
		return "ENTER";
	case KEY_ESC:
		return "ESC";
	case KEY_BACKSPACE:
		return "BACKSPACE";

	// Modifiers
	case KEY_LEFTSHIFT:
	case KEY_RIGHTSHIFT:
		return "SHIFT";
	case KEY_LEFTCTRL:
	case KEY_RIGHTCTRL:
		return "CTRL";
	case KEY_LEFTALT:
	case KEY_RIGHTALT:
		return "ALT";
	case KEY_CAPSLOCK:
		return "CAPSLOCK";

	// Navigation
	case KEY_UP:
		return "UP";
	case KEY_DOWN:
		return "DOWN";
	case KEY_LEFT:
		return "LEFT";
	case KEY_RIGHT:
		return "RIGHT";
	case KEY_HOME:
		return "HOME";
	case KEY_END:
		return "END";
	case KEY_PAGEUP:
		return "PAGEUP";
	case KEY_PAGEDOWN:
		return "PAGEDOWN";
	case KEY_INSERT:
		return "INSERT";
	case KEY_DELETE:
		return "DELETE";

	// Misc
	case KEY_PRINT:
		return "PRINTSCREEN";
	case KEY_PAUSE:
		return "PAUSE";
	case KEY_SCROLLLOCK:
		return "SCROLLLOCK";

	case BTN_LEFT:
		return "Left Mouse Button";
	case BTN_RIGHT:
		return "Right Mouse Button";

	default:
		return "UNKNOWN";
	}
}
