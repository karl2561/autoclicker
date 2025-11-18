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

int ascii_to_evcode(const char *buf, size_t len)
{
	if (len == 0 || buf == NULL)
		return -1;

	if (len == 1) {
		if (buf[0] >= 'a' && buf[0] <= 'z')
			return KEY_A + (buf[0] - 'a');
		if (buf[0] >= 'A' && buf[0] <= 'Z')
			return KEY_A + (buf[0] - 'A');
		if (buf[0] == '0')
			return KEY_0;
		if (buf[0] >= '0' && buf[0] <= '9')
			return KEY_1 + (buf[0] - '1'); // note: '0' is KEY_0

		switch (buf[0]) {
		case '-':
			return KEY_MINUS;
		case '=':
			return KEY_EQUAL;
		case '[':
			return KEY_LEFTBRACE;
		case ']':
			return KEY_RIGHTBRACE;
		case ';':
			return KEY_SEMICOLON;
		case '\'':
			return KEY_APOSTROPHE;
		case '`':
			return KEY_GRAVE;
		case '\\':
			return KEY_BACKSLASH;
		case ',':
			return KEY_COMMA;
		case '.':
			return KEY_DOT;
		case '/':
			return KEY_SLASH;
		case ' ':
			return KEY_SPACE;
		case '\t':
			return KEY_TAB;
		case '\n':
		case '\r':
			return KEY_ENTER;
		case '\033':
			return KEY_ESC;
		case '\b':
			return KEY_BACKSPACE;
		case '\177':
			return KEY_DELETE;
		}
	}

	// F1-F4 (ESC O P..S)
	if (len == 3 && buf[0] == 0x1B && buf[1] == 'O') {
		switch (buf[2]) {
		case 'P':
			return KEY_F1;
		case 'Q':
			return KEY_F2;
		case 'R':
			return KEY_F3;
		case 'S':
			return KEY_F4;
		}
	}

	// F5-F12 (ESC [ 1 5 ~ .. ESC [ 2 4 ~)
	if (len == 5 && buf[0] == 0x1B && buf[1] == '[' && buf[4] == '~') {
		if (buf[2] == '1') {
			switch (buf[3]) {
			case '5':
				return KEY_F5;
			case '7':
				return KEY_F6;
			case '8':
				return KEY_F7;
			case '9':
				return KEY_F8;
			}
		}
		if (buf[2] == '2') {
			switch (buf[3]) {
			case '0':
				return KEY_F9;
			case '1':
				return KEY_F10;
			case '3':
				return KEY_F11;
			case '4':
				return KEY_F12;
			}
		}
	}

	if (len == 3 && buf[0] == '\033' && buf[1] == '[') {
		switch (buf[2]) {
		case 'A':
			return KEY_UP;
		case 'B':
			return KEY_DOWN;
		case 'C':
			return KEY_RIGHT;
		case 'D':
			return KEY_LEFT;
		}
	}

	if (len >= 3 && buf[0] == '\033' && buf[1] == '[') {
		if (len == 3 && buf[2] == 'H')
			return KEY_HOME;
		if (len == 3 && buf[2] == 'F')
			return KEY_END;
		if (len == 4 && buf[2] == '1' && buf[3] == '~')
			return KEY_HOME;
		if (len == 4 && buf[2] == '5' && buf[3] == '~')
			return KEY_PAGEUP;
		if (len == 4 && buf[2] == '6' && buf[3] == '~')
			return KEY_PAGEDOWN;
		if (len == 4 && buf[2] == '2' && buf[3] == '~')
			return KEY_INSERT;
		if (len == 4 && buf[2] == '3' && buf[3] == '~')
			return KEY_DELETE;
	}

	return -1; // unknown
}
