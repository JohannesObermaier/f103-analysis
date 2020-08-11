#include <stdint.h>
uint32_t volatile * const rccApb2 = (uint32_t *) 0x40021018u;
uint32_t volatile * const ioAModerH = (uint32_t *) 0x40010804u;
uint32_t volatile * const uart1Ctrl = (uint32_t *) 0x40013800u;

void readChar( uint8_t const chr );
void writeByte( uint8_t b );
void writeStr( uint8_t const * const str );
void writeChar( uint8_t const chr );
void writeWordLe( uint32_t const word );
void writeWordBe( uint32_t const word );
void readCmd( uint8_t const * const cmd);
uint32_t hexToInt(uint8_t const * const hex);
void readMem(uint32_t const addr, uint32_t const len);
void writeMem(uint32_t const addr, uint32_t const data);

uint8_t const strHelp[] = 	"Help\r\n-----------\r\n"
				"ADDR, VAL, LEN: 32-bit Hex encoded:\r\n e.g., 0A1337FF\r\n"
				"-----------\r\n"
				"R ADDR LEN - Read 32-bit word\r\n"
				"W ADDR VAL - Write 32-bit word\r\n"
				"D          - Dump all flash memory\r\n"
				"S          - Reboot\r\n"
				"E          - Exit\r\n"
				"H          - Show help \r\n"
				"---------------\r\n";

int main(void)
{
	/* IOA + USART1 clock enable */
	*rccApb2 = (uint32_t) 0x4004u;

	/* USART1 TX+RX (PA9, PX10) on */
	*ioAModerH = (uint32_t) 0x444444B4u;

	/* config and enable uart */
	uart1Ctrl[2] = 0x00000341u;
	uart1Ctrl[3] = 0x0000200Cu;
	
	writeStr("\r\n##############################\r\n"
		 "# Low-Level Shell v0.1 alpha #\r\n"
		 "# CPU-ID: 0x");
	uint32_t cpuid = *((uint32_t *) 0xe000ed00);
	writeWordBe(cpuid);
	writeStr(                    "         #\r\n");
	writeStr("##############################\r\n\r\n");
	writeStr("> ");

	while (1)
	{
		if ((uart1Ctrl[0] & 0x80u) && (0))
		{
			uart1Ctrl[1] = 0x33u;
		}

		if (uart1Ctrl[0] & 0x20u)
		{
			uint32_t data = uart1Ctrl[1];
			readChar((uint8_t) data);
		}
	}
}

/* hex must have length 8 */
uint32_t hexToInt(uint8_t const * const hex)
{
	uint32_t ind = 0u;
	uint32_t res = 0u;

	for (ind = 0; ind < 8; ++ind)
	{
		uint8_t chr = hex[ind];
		uint32_t val = 0u;

		res <<= 4u;

		if ((chr >= '0') && (chr <= '9'))
		{
			val = chr - '0';
		}
		else if ((chr >= 'a') && (chr <= 'f'))
		{
			val = chr - 'a' + 0x0a;
		}
		else if ((chr >= 'A') && (chr <= 'F'))
		{
			val = chr - 'A' + 0x0a;
		}
		else
		{
			val = 0u;
		}
		res |= val;
	}

	return res;
}

void readChar( uint8_t const chr )
{
	#define CMDBUF_LEN (64u)
	static uint8_t cmdbuf[CMDBUF_LEN] = {0u};
	static uint32_t cmdInd = 0u;

	switch (chr)
	{
		case '\n':
		case '\r':
			cmdbuf[cmdInd] = 0u;
			if (cmdInd != 0)
			{
				writeStr("\r\n");
			}
			readCmd(cmdbuf);
			cmdInd = 0u;
			writeStr("\r\n> ");
			{
				uint32_t ind = 0u;
				for (ind = 0; ind<CMDBUF_LEN; ++ind)
				{
					cmdbuf[ind]=0x00u;
				}
			}
		break;

		case 8:
		case 255:
		case 127: /* TODO backspace */
			if (cmdInd > 0u)
				--cmdInd;
			writeChar(chr);
		break;

		default:
			if (cmdInd < (CMDBUF_LEN - 1))
			{
				cmdbuf[cmdInd] = chr;
				++cmdInd;
				writeChar(chr);
			}
		break;
	}
}

void readCmd( uint8_t const * const cmd )
{
	switch (cmd[0])
	{
		case 0:
			return;
			break;

		/* read 32-bit command */
		case 'r':
		case 'R':
			/* r 08000000 00000100 */
			readMem(hexToInt(&cmd[2]), hexToInt(&cmd[11]));
			break;

		/* write 32-bit command */
		case 'w':
		case 'W':
			/* w 20000000 12345678 */
			writeMem(hexToInt(&cmd[2]), hexToInt(&cmd[11]));
			break;

		/* Dump all flash */
		case 'd':
		case 'D':
			writeStr("\r\n\r\n");
			{
				uint32_t const * addr = (uint32_t*) 0x08000000;
				uint32_t br = 8u;
				while (((uintptr_t) addr) < (0x08000000 + 64u * 1024u))
				{
					if (br == 8u)
					{
						writeStr("\r\n[");
						writeWordBe((uint32_t) addr);
						writeStr("]: ");
						br = 0u;
					}

					writeWordBe(*addr);
					writeChar(' ');
					++addr;
					++br;
				}
			}
			writeStr("\r\n\r\n");
			break;

		/* Help command */
		case 'h':
		case 'H':
			writeStr(strHelp);
			break;

		/* Reboot */
		case 's':
		case 'S':
			writeStr("Rebooting...\r\n\r\n");
			*((uint32_t *) 0xE000ED0C) = 0x05FA0004u;
			break;

		/* exit */
		case 'e':
		case 'E':
			writeStr("Bye.\r\n");
			while (1)
			{
				__asm__ volatile("wfi");
			}
			break;


		default:
			writeStr("Unknown command: ");
			writeStr(cmd);
			writeStr("\r\n");
			break;
	}
}
	
const uint8_t txtMap[] = "0123456789ABCDEF";

void writeByte( uint8_t b )
{
	writeChar(txtMap[b >> 4]);
	writeChar(txtMap[b & 0x0F]);
}


void writeStr( uint8_t const * const str )
{
	uint32_t ind = 0u;

	while (str[ind])
	{
		writeChar(str[ind]);
		++ind;
	}
}

void writeChar( uint8_t const chr )
{
	while (!(uart1Ctrl[0] & 0x80u))
	{
		/* wait */
	}

	uart1Ctrl[1] = chr;
}

void writeWordLe( uint32_t const word )
{
	writeByte((word & 0x000000FF));
	writeByte((word & 0x0000FF00) >> 8);
	writeByte((word & 0x00FF0000) >> 16);
	writeByte((word & 0xFF000000) >> 24);
}

void writeWordBe( uint32_t const word )
{
	writeByte((word & 0xFF000000) >> 24);
	writeByte((word & 0x00FF0000) >> 16);
	writeByte((word & 0x0000FF00) >> 8);
	writeByte((word & 0x000000FF));

}

void alertCrash( uint32_t crashId )
{
	writeStr("!!! EXCEPTION !!!\r\nID: ");
	writeByte(crashId);
	writeStr("\r\nRestart required!\r\n\r\n");
	*((uint32_t *) 0xE000ED0C) = 0x05FA0004u;
	while (1);
}

void readMem(uint32_t const addr, uint32_t const len)
{
	uint32_t it = 0u;
	uint32_t addrx = 0u;
	uint32_t lenx = 0u;

	lenx = len;
	if (lenx == 0u)
	{
		lenx = 4u;
	}

	for (it = 0u; it < (lenx / 4u); ++it)
	{
		addrx = addr + it*4u;
		writeStr("Read [");
		writeWordBe(addrx);
		writeStr("]: ");
		writeWordBe(*((uint32_t*)addrx));
		writeStr("\r\n");
	}
}

void writeMem(uint32_t const addr, uint32_t const data)
{
	writeStr("Write [");
	writeWordBe(addr);
	writeStr("]: ");
	writeWordBe(data);
	*((uint32_t*) addr) = data;
	writeStr("\r\n");
}
