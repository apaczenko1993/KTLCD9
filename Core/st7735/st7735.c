/* vim: set ai et ts=4 sw=4: */
#include "stm32f1xx_hal.h"
#include <stm32f1xx.h>

#include "st7735.h"
#include "malloc.h"
#include "string.h"

#define DELAY 0x80

extern TIM_HandleTypeDef htim1;   // Deklaracja zewnętrznego timera
extern void delay_us(uint16_t us);  // Deklaracja zewnętrznej funkcji

// based on Adafruit ST7735 library for Arduino
static const uint8_t
  init_cmds1[] = {            // Init for 7735R, part 1 (red or green tab)
    15,                       // 15 commands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
      150,                    //     150 ms delay
    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
      255,                    //     500 ms delay
    ST7735_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
      0x01, 0x2C, 0x2D,       //     Dot inversion mode
      0x01, 0x2C, 0x2D,       //     Line inversion mode
    ST7735_INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
      0x07,                   //     No inversion
    ST7735_PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
      0xA2,
      0x02,                   //     -4.6V
      0x84,                   //     AUTO mode
    ST7735_PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
      0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
    ST7735_PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
      0x0A,                   //     Opamp current small
      0x00,                   //     Boost frequency
    ST7735_PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
      0x8A,                   //     BCLK/2, Opamp current small & Medium low
      0x2A,  
    ST7735_PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
      0x8A, 0xEE,
    ST7735_VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
      0x0E,
    ST7735_INVOFF , 0      ,  // 13: Don't invert display, no args, no delay
    ST7735_MADCTL , 1      ,  // 14: Memory access control (directions), 1 arg:
      ST7735_ROTATION,        //     row addr/col addr, bottom to top refresh
    ST7735_COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
      0x05 },                 //     16-bit color

  init_cmds2[] = {            // Init for 7735R, part 2 (1.44" display)
    2,                        //  2 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F,             //     XEND = 127
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x9F },           //     XEND = 159

  init_cmds3[] = {            // Init for 7735R, part 3 (red or green tab)
    4,                        //  4 commands in list:
    ST7735_GMCTRP1, 16      , //  1: Gamma Adjustments (pos. polarity), 16 args, no delay:
      0x02, 0x1c, 0x07, 0x12,
      0x37, 0x32, 0x29, 0x2d,
      0x29, 0x25, 0x2B, 0x39,
      0x00, 0x01, 0x03, 0x10,
    ST7735_GMCTRN1, 16      , //  2: Gamma Adjustments (neg. polarity), 16 args, no delay:
      0x03, 0x1d, 0x07, 0x06,
      0x2E, 0x2C, 0x29, 0x2D,
      0x2E, 0x2E, 0x37, 0x3F,
      0x00, 0x00, 0x02, 0x10,
    ST7735_NORON  ,    DELAY, //  3: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ST7735_DISPON ,    DELAY, //  4: Main screen turn on, no args w/delay
      100 };                  //     100 ms delay

void ST7735_Select() {
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
	delay_us(1);
}

void ST7735_Unselect() {
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
	delay_us(1);
}

static void ST7735_WriteCommand(uint8_t cmd) {


    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET);  // RS/DCX = 0 dla komendy
    delay_us(1);

    LCD_DATA_PORT->ODR = cmd;  // Ustaw dane na magistrali D0-D7
    delay_us(1);

    HAL_GPIO_WritePin(LCD_WR_GPIO_Port, LCD_WR_Pin, GPIO_PIN_RESET);  // WR = 0, inicjacja zapisu
    delay_us(1);
    HAL_GPIO_WritePin(LCD_WR_GPIO_Port, LCD_WR_Pin, GPIO_PIN_SET);    // WR = 1, zakończenie zapisu
    delay_us(1);
}

static void ST7735_WriteData(uint8_t* buff, size_t buff_size) {

	HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);  // RS/DCX = 1 dla danych
	delay_us(1);

    while (buff_size--) {
        LCD_DATA_PORT->ODR = *buff++;  // Ustaw dane na magistrali D0-D7
        delay_us(1);  // Opóźnienie po ustawieniu danych (TDHT)

        HAL_GPIO_WritePin(LCD_WR_GPIO_Port, LCD_WR_Pin, GPIO_PIN_RESET);  // WR = 0, inicjacja zapisu
        delay_us(1);  // Opóźnienie po WR = 0 (TWRL)
        HAL_GPIO_WritePin(LCD_WR_GPIO_Port, LCD_WR_Pin, GPIO_PIN_SET);    // WR = 1, zakończenie zapisu
        delay_us(1);  // Opóźnienie po WR = 1 (TWRH)
    }
}

static void ST7735_ExecuteCommandList(const uint8_t *addr) {
    uint8_t numCommands, numArgs;
    uint16_t ms;

    numCommands = *addr++;
    while(numCommands--) {
        uint8_t cmd = *addr++;
        ST7735_WriteCommand(cmd);

        numArgs = *addr++;
        ms = numArgs & DELAY;
        numArgs &= ~DELAY;
        if(numArgs) {
            ST7735_WriteData((uint8_t*)addr, numArgs);
            addr += numArgs;
        }

        if(ms) {
            ms = *addr++;
            if(ms == 255) ms = 500;
            HAL_Delay(ms);
        }
    }
}

static void ST7735_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {

    ST7735_WriteCommand(ST7735_CASET);
    delay_us(1);  // Opóźnienie po wysłaniu komendy (TCS)

    uint8_t data[] = { 0x00, x0 + ST7735_XSTART, 0x00, x1 + ST7735_XSTART };
    ST7735_WriteData(data, sizeof(data));
    delay_us(1);  // Opóźnienie po wysłaniu danych (TDST)

    ST7735_WriteCommand(ST7735_RASET);
    delay_us(1);  // Opóźnienie po wysłaniu komendy (TCS)

    data[1] = y0 + ST7735_YSTART;
    data[3] = y1 + ST7735_YSTART;
    ST7735_WriteData(data, sizeof(data));
    delay_us(1);  // Opóźnienie po wysłaniu danych (TDST)

    ST7735_WriteCommand(ST7735_RAMWR);
    delay_us(1);  // Opóźnienie po wysłaniu komendy (TCS)
}

void ST7735_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
    if((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT))
        return;

    //ST7735_Select(); // funckja z delayem 1us

    ST7735_SetAddressWindow(x, y, x+1, y+1);  // wysylanie komend

    uint8_t data[] = { color >> 8, color & 0xFF };
    ST7735_WriteData(data, sizeof(data));

    //ST7735_Unselect();

}

void ST7735_Init() {
    ST7735_Select();
    //ST7735_Reset();
    ST7735_ExecuteCommandList(init_cmds1);
    ST7735_ExecuteCommandList(init_cmds2);
    ST7735_ExecuteCommandList(init_cmds3);
    ST7735_Unselect();
}



static void ST7735_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor) {
    uint32_t i, b, j;
    ST7735_Select();
    ST7735_SetAddressWindow(x, y, x+font.width-1, y+font.height-1);

    for(i = 0; i < font.height; i++) {
        b = font.data[(ch - 32) * font.height + i];
        for(j = 0; j < font.width; j++) {
            if((b << j) & 0x8000)  {
                uint8_t data[] = { color >> 8, color & 0xFF };
                ST7735_WriteData(data, sizeof(data));
            } else {
                uint8_t data[] = { bgcolor >> 8, bgcolor & 0xFF };
                ST7735_WriteData(data, sizeof(data));
            }
        }
    }
    ST7735_Unselect();
}

void ST7735_WriteString(uint16_t x, uint16_t y, const char* str, FontDef font, uint16_t color, uint16_t bgcolor) {
    ST7735_Select();

    while(*str) {
        if(x + font.width >= ST7735_WIDTH) {
            x = 0;
            y += font.height;
            if(y + font.height >= ST7735_HEIGHT) {
                break;
            }

            if(*str == ' ') {
                // skip spaces in the beginning of the new line
                str++;
                continue;
            }
        }

        ST7735_WriteChar(x, y, *str, font, color, bgcolor);
        x += font.width;
        str++;
    }

    ST7735_Unselect();
}

void ST7735_FillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {

	ST7735_Select(); // funckja z delayem 1us

	// Sprawdzenie granic prostokąta
	    if (x >= ST7735_WIDTH || y >= ST7735_HEIGHT) return;
	    if (x + w > ST7735_WIDTH) w = ST7735_WIDTH - x;
	    if (y + h > ST7735_HEIGHT) h = ST7735_HEIGHT - y;

	    // Wybór wyświetlacza i ustawienie okna adresowego
	    //ST7735_Select();
	    ST7735_SetAddressWindow(x, y, x + w - 1, y + h - 1);

	    // Przygotowanie danych pikseli
	    uint8_t data[] = { color >> 8, color & 0xFF };

	    //HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, GPIO_PIN_SET);

	    // Wysyłanie danych pikseli w blokach
	    for (uint16_t row = 0; row < h; ++row) {
	        for (uint16_t col = 0; col < w; ++col) {
	            ST7735_WriteData(data, sizeof(data));
	        }
	    }

	    // Odseparowanie wyświetlacza
	  ST7735_Unselect();
}

void ST7735_FillRectangleFast(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    if((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT)) return;
    if((x + w - 1) >= ST7735_WIDTH) w = ST7735_WIDTH - x;
    if((y + h - 1) >= ST7735_HEIGHT) h = ST7735_HEIGHT - y;

    ST7735_Select();
    ST7735_SetAddressWindow(x, y, x+w-1, y+h-1);

    uint8_t pixel[] = { color >> 8, color & 0xFF };
    uint8_t *line = malloc(w * sizeof(pixel));
    for(x = 0; x < w; ++x)
        memcpy(line + x * sizeof(pixel), pixel, sizeof(pixel));

    for(y = h; y > 0; y--)
        ST7735_WriteData(line, w * sizeof(pixel));

    free(line);
    ST7735_Unselect();
}

void ST7735_FillScreen(uint16_t color) {
    ST7735_FillRectangle(0, 0, ST7735_WIDTH, ST7735_HEIGHT, color);
}

void ST7735_FillScreenFast(uint16_t color) {
    ST7735_FillRectangleFast(0, 0, ST7735_WIDTH, ST7735_HEIGHT, color);
}

void ST7735_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data) {
    if((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT)) return;
    if((x + w - 1) >= ST7735_WIDTH) return;
    if((y + h - 1) >= ST7735_HEIGHT) return;

    ST7735_Select();
    ST7735_SetAddressWindow(x, y, x+w-1, y+h-1);
    ST7735_WriteData((uint8_t*)data, sizeof(uint16_t)*w*h);
    ST7735_Unselect();
}

void ST7735_InvertColors(bool invert) {
    ST7735_Select();
    ST7735_WriteCommand(invert ? ST7735_INVON : ST7735_INVOFF);
    ST7735_Unselect();
}

void ST7735_SetGamma(GammaDef gamma)
{
	ST7735_Select();
	ST7735_WriteCommand(ST7735_GAMSET);
	ST7735_WriteData((uint8_t *) &gamma, sizeof(gamma));
	ST7735_Unselect();
}
