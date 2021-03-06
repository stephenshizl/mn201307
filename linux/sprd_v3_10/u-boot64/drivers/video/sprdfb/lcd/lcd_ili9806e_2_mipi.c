/* drivers/video/sprdfb/lcd_ili9806e_2_mipi.c
 *
 * Support for ili9806e_2 mipi LCD device
 *
 * Copyright (C) 2010 Spreadtrum
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "../sprdfb_chip_common.h"
#include "../sprdfb.h"
#include "../sprdfb_panel.h"

#define printk printf

//#define  LCD_DEBUG
#ifdef LCD_DEBUG
#define LCD_PRINT printk
#else
#define LCD_PRINT(...)
#endif

#define MAX_DATA   48

typedef struct LCM_Init_Code_tag {
	unsigned int tag;
	unsigned char data[MAX_DATA];
}LCM_Init_Code;

typedef struct LCM_force_cmd_code_tag{
	unsigned int datatype;
	LCM_Init_Code real_cmd_code;
}LCM_Force_Cmd_Code;

#define LCM_TAG_SHIFT 24
#define LCM_TAG_MASK  ((1 << 24) -1)
#define LCM_SEND(len) ((1 << LCM_TAG_SHIFT)| len)
#define LCM_SLEEP(ms) ((2 << LCM_TAG_SHIFT)| ms)
//#define ARRAY_SIZE(array) ( sizeof(array) / sizeof(array[0]))

#define LCM_TAG_SEND  (1<< 0)
#define LCM_TAG_SLEEP (1 << 1)

static LCM_Init_Code init_data[] = {

#if 1 //old
//****************************************************************************//
//****************************** Page 1 Command ******************************//
//****************************************************************************//
	{LCM_SEND(8), {6, 0, 0xFF,0xFF,0x98,0x06,0x04,0x01}},     // Change to Page 1

	{LCM_SEND(2), {0x08,0x10}},                 // output SDA
	{LCM_SEND(2), {0x21,0x01}},                 // DE = 1 Active
	{LCM_SEND(2), {0x30,0x01}},                 // 480 X 854
	{LCM_SEND(2), {0x31,0x02}},                 // 2-dot Inversion
	{LCM_SEND(2), {0x40,0x11}},                 // DDVDH/DDVDL 
	{LCM_SEND(2), {0x41,0x33}},                 // DDVDH/DDVDL CP
	{LCM_SEND(2), {0x42,0x03}},                 // VGH/VGL 
	{LCM_SEND(2), {0x43,0x09}},                 // VGH CP 
	{LCM_SEND(2), {0x44,0x04}},                 // VGL CP
	{LCM_SEND(2), {0x50,0x78}},                 //    VGMP
	{LCM_SEND(2), {0x51,0x78}},                  //   VGMN
	{LCM_SEND(2), {0x52,0x00}},                   //Flicker
	{LCM_SEND(2), {0x53,0x80}},                   //Flicker
	{LCM_SEND(2), {0x57,0x50}},                 // SDTI
	{LCM_SEND(2), {0x60,0x07}},                 // SDTI
	{LCM_SEND(2), {0x61,0x00}},                // CRTI
	{LCM_SEND(2), {0x62,0x08}},                 // EQTI
	{LCM_SEND(2), {0x63,0x00}},                // PCTI

//++++++++++++++++++ Gamma Setting ++++++++++++++++++//
	{LCM_SEND(2), {0xA0,0x00}},  // Gamma 0 
	{LCM_SEND(2), {0xA1,0x05}},  // Gamma 4 
	{LCM_SEND(2), {0xA2,0x04}},  // Gamma 8
	{LCM_SEND(2), {0xA3,0x06}},  // Gamma 16
	{LCM_SEND(2), {0xA4,0x02}},  // Gamma 24
	{LCM_SEND(2), {0xA5,0x0E}},  // Gamma 52
	{LCM_SEND(2), {0xA6,0x0A}},  // Gamma 80
	{LCM_SEND(2), {0xA7,0x02}},  // Gamma 108
	{LCM_SEND(2), {0xA8,0x09}},  // Gamma 147
	{LCM_SEND(2), {0xA9,0x0E}},  // Gamma 175
	{LCM_SEND(2), {0xAA,0x0A}},  // Gamma 203
	{LCM_SEND(2), {0xAB,0x01}},  // Gamma 231
	{LCM_SEND(2), {0xAC,0x00}},  // Gamma 239
	{LCM_SEND(2), {0xAD,0x13}},  // Gamma 247
	{LCM_SEND(2), {0xAE,0x0E}},  // Gamma 251
	{LCM_SEND(2), {0xAF,0x00}},  // Gamma 255

///==============Nagitive
	{LCM_SEND(2), {0xC0,0x00}},  // Gamma 0 
	{LCM_SEND(2), {0xC1,0x0A}},  // Gamma 4
	{LCM_SEND(2), {0xC2,0x16}},  // Gamma 8
	{LCM_SEND(2), {0xC3,0x12}},  // Gamma 16
	{LCM_SEND(2), {0xC4,0x09}},  // Gamma 24
	{LCM_SEND(2), {0xC5,0x03}},  // Gamma 52
	{LCM_SEND(2), {0xC6,0x03}},  // Gamma 80
	{LCM_SEND(2), {0xC7,0x0A}},  // Gamma 108
	{LCM_SEND(2), {0xC8,0x05}},  // Gamma 147
	{LCM_SEND(2), {0xC9,0x04}},  // Gamma 175
	{LCM_SEND(2), {0xCA,0x1C}},  // Gamma 203
	{LCM_SEND(2), {0xCB,0x0C}},  // Gamma 231
	{LCM_SEND(2), {0xCC,0x16}},  // Gamma 239
	{LCM_SEND(2), {0xCD,0x20}},  // Gamma 247
	{LCM_SEND(2), {0xCE,0x13}},  // Gamma 251
	{LCM_SEND(2), {0xCF,0x00}},  // Gamma 255

//+++++++++++++++++++++++++++++++++++++++++++++++++++//

//****************************************************************************//
//****************************** Page 6 Command ******************************//
//****************************************************************************//
	{LCM_SEND(8), {6, 0, 0xFF,0xFF,0x98,0x06,0x04,0x06}},      // Change to Page 6

	{LCM_SEND(2), {0x00,0x20}},
	{LCM_SEND(2), {0x01,0x04}},
	{LCM_SEND(2), {0x02,0x00}},    
	{LCM_SEND(2), {0x03,0x0A}},
	{LCM_SEND(2), {0x04,0x09}},
	{LCM_SEND(2), {0x05,0x09}},
	{LCM_SEND(2), {0x06,0x98}},    
	{LCM_SEND(2), {0x07,0x02}},
	{LCM_SEND(2), {0x08,0x07}},
	{LCM_SEND(2), {0x09,0x80}},    
	{LCM_SEND(2), {0x0A,0x00}},    
	{LCM_SEND(2), {0x0B,0x00}},    
	{LCM_SEND(2), {0x0C,0x09}},
	{LCM_SEND(2), {0x0D,0x09}},
	{LCM_SEND(2), {0x0E,0x00}},
	{LCM_SEND(2), {0x0F,0x00}},
    
	{LCM_SEND(2), {0x10,0x77}},
	{LCM_SEND(2), {0x11,0xF0}},
	{LCM_SEND(2), {0x12,0x01}},
	{LCM_SEND(2), {0x13,0x00}},
	{LCM_SEND(2), {0x14,0x00}},
	{LCM_SEND(2), {0x15,0x43}},
	{LCM_SEND(2), {0x16,0x0B}},
	{LCM_SEND(2), {0x17,0x00}},
	{LCM_SEND(2), {0x18,0x00}},
	{LCM_SEND(2), {0x19,0x00}},
	{LCM_SEND(2), {0x1A,0x00}},
	{LCM_SEND(2), {0x1B,0x00}},
	{LCM_SEND(2), {0x1C,0x00}},
	{LCM_SEND(2), {0x1D,0x00}},

	{LCM_SEND(2), {0x20,0x01}},
	{LCM_SEND(2), {0x21,0x23}},
	{LCM_SEND(2), {0x22,0x45}},
	{LCM_SEND(2), {0x23,0x67}},
	{LCM_SEND(2), {0x24,0x01}},
	{LCM_SEND(2), {0x25,0x23}},
	{LCM_SEND(2), {0x26,0x45}},
	{LCM_SEND(2), {0x27,0x67}},
    
	{LCM_SEND(2), {0x30,0x13}},
	{LCM_SEND(2), {0x31,0x22}},
	{LCM_SEND(2), {0x32,0x22}},
	{LCM_SEND(2), {0x33,0x22}},
	{LCM_SEND(2), {0x34,0x22}},
	{LCM_SEND(2), {0x35,0xBB}},
	{LCM_SEND(2), {0x36,0xAA}},
	{LCM_SEND(2), {0x37,0xDD}},
	{LCM_SEND(2), {0x38,0xCC}},
	{LCM_SEND(2), {0x39,0x22}},
	{LCM_SEND(2), {0x3A,0x66}},
	{LCM_SEND(2), {0x3B,0x22}},
	{LCM_SEND(2), {0x3C,0x88}},
	{LCM_SEND(2), {0x3D,0x22}},
	{LCM_SEND(2), {0x3E,0x22}},
	{LCM_SEND(2), {0x3F,0x22}},
	{LCM_SEND(2), {0x40,0x22}},

	{LCM_SEND(8), {6, 0, 0xFF,0xFF,0x98,0x06,0x04,0x07}},      // Change to Page 7

	{LCM_SEND(2), {0x18,0x1D}},
	{LCM_SEND(2), {0x02,0x77}},
	{LCM_SEND(2), {0x06,0x13}},

	{LCM_SEND(8), {6, 0, 0xFF,0xFF,0x98,0x06,0x04,0x06}},     // Change to Page 6

	{LCM_SEND(2), {0x52,0x10}},
	{LCM_SEND(2), {0x53,0x10}},

	{LCM_SEND(8), {6, 0, 0xFF,0xFF,0x98,0x06,0x04,0x00}},      // Change to Page 0

	{LCM_SEND(2), {0x35,0x00}},                // Sleep-Out
                                                  
	{LCM_SEND(1), {0x11}},                            
                                                
	{LCM_SLEEP(120)},
                   
		                                            
	{LCM_SEND(1), {0x29}},                            
		                                            
	{LCM_SLEEP(100)},
#else
	//yi xia wei 9806e+boe(jinshijie)
	{LCM_SEND(8),{6,0,0xFF,0xFF,0x98,0x06,0x04,0x01}},    // Change to Page 1
	{LCM_SEND(2),{0x08,0x10}},                 // output SDA
	{LCM_SEND(2),{0x30,0x01}},                 // 480 X 854
	{LCM_SEND(2),{0x31,0x02}},                 // Column Inversion
	{LCM_SEND(2),{0x60,0x07}},                 // SDTI
	{LCM_SEND(2),{0x61,0x06}},                // CRTI
	{LCM_SEND(2),{0x62,0x06}},                 // EQTI
	{LCM_SEND(2),{0x63,0x04}},                // PCTI
	{LCM_SEND(2),{0x40,0x17}},                // BT  +2.5/-3 pump for DDVDH-L
	{LCM_SEND(2),{0x41,0x66}},                  // DVDDH DVDDL clamp  5.39  -5.32
	{LCM_SEND(2),{0x42,0x02}},              // VGH/VGL -1.9 1.63
	{LCM_SEND(2),{0x43,0x06}},                 // VGHL 13.58
	{LCM_SEND(2),{0x44,0x09}},                 // VGL -11.89
	{LCM_SEND(2),{0x50,0x80}},                 // VGMP
	{LCM_SEND(2),{0x51,0x80}},                // VGMN
	{LCM_SEND(2),{0x52,0x00}},                   //Flicker
	{LCM_SEND(2),{0x53,0x56}},                 //Flicker  //0x60
	{LCM_SEND(2),{0x54,0x00}},                   //Flicker
	{LCM_SEND(2),{0x55,0x57}},                 //Flicker  //0x60  
	{LCM_SEND(2),{0x57,0x50}},                 //Low voltage detection 
             
	{LCM_SEND(2),{0xA0,0x00}},  // Gamma 0 /255
	{LCM_SEND(2),{0xA1,0x01}},  // Gamma 4 /251
	{LCM_SEND(2),{0xA2,0x0F}},  // Gamma 8 /247
	{LCM_SEND(2),{0xA3,0x0B}},  // Gamma 16	/239
	{LCM_SEND(2),{0xA4,0x03}},  // Gamma 24 /231
	{LCM_SEND(2),{0xA5,0x0A}},  // Gamma 52 / 203
	{LCM_SEND(2),{0xA6,0x01}},  // Gamma 80 / 175
	{LCM_SEND(2),{0xA7,0x04}},  // Gamma 108 /147
	{LCM_SEND(2),{0xA8,0x08}},  // Gamma 147 /108
	{LCM_SEND(2),{0xA9,0x0A}},  // Gamma 175 / 80
	{LCM_SEND(2),{0xAA,0x17}},  // Gamma 203 / 52
	{LCM_SEND(2),{0xAB,0x13}},  // Gamma 231 / 24
	{LCM_SEND(2),{0xAC,0x1F}},  // Gamma 239 / 16
	{LCM_SEND(2),{0xAD,0x0C}},  // Gamma 247 / 8
	{LCM_SEND(2),{0xAE,0x0D}},  // Gamma 251 / 4
	{LCM_SEND(2),{0xAF,0x00}},  // Gamma 255 / 0
           
	{LCM_SEND(2),{0xC0,0x00}},  // Gamma 0 
	{LCM_SEND(2),{0xC1,0x05}},  // Gamma 4
	{LCM_SEND(2),{0xC2,0x1C}},  // Gamma 8
	{LCM_SEND(2),{0xC3,0x07}},  // Gamma 16
	{LCM_SEND(2),{0xC4,0x07}},  // Gamma 24
	{LCM_SEND(2),{0xC5,0x02}},  // Gamma 52
	{LCM_SEND(2),{0xC6,0x0C}},  // Gamma 80
	{LCM_SEND(2),{0xC7,0x05}}, // Gamma 108
	{LCM_SEND(2),{0xC8,0x03}},  // Gamma 147
	{LCM_SEND(2),{0xC9,0x0A}},  // Gamma 175
	{LCM_SEND(2),{0xCA,0x13}},  // Gamma 203
	{LCM_SEND(2),{0xCB,0x07}},  // Gamma 231
	{LCM_SEND(2),{0xCC,0x1C}},  // Gamma 239
	{LCM_SEND(2),{0xCD,0x1B}},  // Gamma 247
	{LCM_SEND(2),{0xCE,0x0A}},  // Gamma 251
	{LCM_SEND(2),{0xCF,0x00}},  // Gamma 255

	{LCM_SEND(8),{6,0,0xFF,0xFF,0x98,0x06,0x04,0x07}},     // Change to Page 7
	{LCM_SEND(2),{0xE1,0x79}},
	{LCM_SEND(2),{0x18,0x1D}},
	{LCM_SEND(2),{0x02,0x77}},
	{LCM_SEND(2),{0x17,0x22}},  

	{LCM_SEND(8),{6,0,0xFF,0xFF,0x98,0x06,0x04,0x06}},     // Change to Page 6
	{LCM_SEND(2),{0x00,0x20}},
	{LCM_SEND(2),{0x01,0x04}},//05
	{LCM_SEND(2),{0x02,0x00}},    
	{LCM_SEND(2),{0x03,0x00}},
	{LCM_SEND(2),{0x04,0x01}},
	{LCM_SEND(2),{0x05,0x01}},
	{LCM_SEND(2),{0x06,0x88}},    
	{LCM_SEND(2),{0x07,0x04}},
	{LCM_SEND(2),{0x08,0x01}},
	{LCM_SEND(2),{0x09,0x90}},    
	{LCM_SEND(2),{0x0A,0x03}},  //04  
	{LCM_SEND(2),{0x0B,0x01}},    
	{LCM_SEND(2),{0x0C,0x01}},
	{LCM_SEND(2),{0x0D,0x01}},
	{LCM_SEND(2),{0x0E,0x00}},
	{LCM_SEND(2),{0x0F,0x00}},
	{LCM_SEND(2),{0x10,0x55}},
	{LCM_SEND(2),{0x11,0x53}},//50
	{LCM_SEND(2),{0x12,0x01}},
	{LCM_SEND(2),{0x13,0x0D}}, //0C
	{LCM_SEND(2),{0x14,0x0D}},
	{LCM_SEND(2),{0x15,0x43}},
	{LCM_SEND(2),{0x16,0x0B}},
	{LCM_SEND(2),{0x17,0x00}},
	{LCM_SEND(2),{0x18,0x00}},
	{LCM_SEND(2),{0x19,0x00}},
	{LCM_SEND(2),{0x1A,0x00}},
	{LCM_SEND(2),{0x1B,0x00}},
	{LCM_SEND(2),{0x1C,0x00}},
	{LCM_SEND(2),{0x1D,0x00}},

	{LCM_SEND(2),{0x20,0x01}},
	{LCM_SEND(2),{0x21,0x23}},
	{LCM_SEND(2),{0x22,0x45}},
	{LCM_SEND(2),{0x23,0x67}},
	{LCM_SEND(2),{0x24,0x01}},
	{LCM_SEND(2),{0x25,0x23}},
	{LCM_SEND(2),{0x26,0x45}},
	{LCM_SEND(2),{0x27,0x67}},

	{LCM_SEND(2),{0x30,0x02}},
	{LCM_SEND(2),{0x31,0x22}},
	{LCM_SEND(2),{0x32,0x11}},
	{LCM_SEND(2),{0x33,0xAA}},
	{LCM_SEND(2),{0x34,0xBB}},
	{LCM_SEND(2),{0x35,0x66}},
	{LCM_SEND(2),{0x36,0x00}},
	{LCM_SEND(2),{0x37,0x22}},
	{LCM_SEND(2),{0x38,0x22}},
	{LCM_SEND(2),{0x39,0x22}},
	{LCM_SEND(2),{0x3A,0x22}},
	{LCM_SEND(2),{0x3B,0x22}},
	{LCM_SEND(2),{0x3C,0x22}},
	{LCM_SEND(2),{0x3D,0x22}},
	{LCM_SEND(2),{0x3E,0x22}},
	{LCM_SEND(2),{0x3F,0x22}},
	{LCM_SEND(2),{0x40,0x22}},
	{LCM_SEND(2),{0x52,0x10}},
	{LCM_SEND(2),{0x53,0x10}},

	{LCM_SEND(8),{6,0,0xFF,0xFF,0x98,0x06,0x04,0x00}},     // Change to Page 0
	{LCM_SEND(2),{0x35,0x00}},
	{LCM_SEND(2),{0x36,0x00}},  //xy-direction 0x03
	{LCM_SEND(2),{0x3A,0x77}},
	{LCM_SEND(2),{0x55,0x80}},

	{LCM_SEND(2),{0x11,0x00}},                                                 
	{LCM_SLEEP(120)},//{REGFLAG_DELAY, 120, {}},                                                                  
	{LCM_SEND(2),{0x29,0x00}},    
	
	{LCM_SLEEP(100)},//{REGFLAG_END_OF_TABLE, 0x00, {}}


#endif

 };

static LCM_Force_Cmd_Code rd_prep_code[]={
	{0x39, {LCM_SEND(8), {0x6, 0, 0xFF, 0xFF, 0x98, 0x06,0x04,0x01}}},
	{0x37, {LCM_SEND(2), {0x3, 0}}},
};

static LCM_Init_Code disp_on =  {LCM_SEND(1), {0x29}};

static LCM_Init_Code sleep_in =  {LCM_SEND(1), {0x10}};

static LCM_Init_Code sleep_out =  {LCM_SEND(1), {0x11}};

static int32_t ili9806e_2_mipi_init(struct panel_spec *self)
{
	int32_t i;
	LCM_Init_Code *init = init_data;
	unsigned int tag;

	mipi_set_cmd_mode_t mipi_set_cmd_mode = self->info.mipi->ops->mipi_set_cmd_mode;
	mipi_gen_write_t mipi_gen_write = self->info.mipi->ops->mipi_gen_write;

         mipi_set_lp_mode_t mipi_set_lp_mode = self->info.mipi->ops->mipi_set_lp_mode;
         mipi_set_hs_mode_t mipi_set_hs_mode = self->info.mipi->ops->mipi_set_hs_mode;
	mipi_eotp_set_t mipi_eotp_set = self->info.mipi->ops->mipi_eotp_set;
	LCD_PRINT("lcd_ili9806e_2_init\n");

	//mipi_set_cmd_mode();

	mdelay(5);
	mipi_set_lp_mode();
	//mipi_eotp_set(1,0);
	for(i = 0; i < ARRAY_SIZE(init_data); i++){
		tag = (init->tag >>24);
		if(tag & LCM_TAG_SEND){
			mipi_gen_write(init->data, (init->tag & LCM_TAG_MASK));
			udelay(20);
		}else if(tag & LCM_TAG_SLEEP){
			mdelay((init->tag & LCM_TAG_MASK));
		}
		init++;
	}

	mdelay(5);
	mipi_set_hs_mode();
	return 0;
}

static uint32_t ili9806e_2_readid(struct panel_spec *self)
{
        /*Jessica TODO: need read id*/
        int32_t i = 0;
        uint32_t j =0;
        LCM_Force_Cmd_Code * rd_prepare = rd_prep_code;
        uint8_t read_data[3] = {0};
        int32_t read_rtn = 0;
        unsigned int tag = 0;
        mipi_set_cmd_mode_t mipi_set_cmd_mode = self->info.mipi->ops->mipi_set_cmd_mode;
        mipi_force_write_t mipi_force_write = self->info.mipi->ops->mipi_force_write;
        mipi_force_read_t mipi_force_read = self->info.mipi->ops->mipi_force_read;
        mipi_set_lp_mode_t mipi_set_lp_mode = self->info.mipi->ops->mipi_set_lp_mode;
        mipi_set_hs_mode_t mipi_set_hs_mode = self->info.mipi->ops->mipi_set_hs_mode;
        mipi_eotp_set_t mipi_eotp_set = self->info.mipi->ops->mipi_eotp_set;

	//return 0x6; //cg liyun 20130329

	printk("lcd_ili9806e_2_mipi read id!\n");
	mipi_set_lp_mode();

	//mipi_set_cmd_mode();
	for(j = 0; j < 4; j++){
		rd_prepare = rd_prep_code;
		for(i = 0; i < ARRAY_SIZE(rd_prep_code); i++){
			tag = (rd_prepare->real_cmd_code.tag >> 24);
			if(tag & LCM_TAG_SEND){
				mipi_force_write(rd_prepare->datatype, rd_prepare->real_cmd_code.data, (rd_prepare->real_cmd_code.tag & LCM_TAG_MASK));
			}else if(tag & LCM_TAG_SLEEP){
				mdelay((rd_prepare->real_cmd_code.tag & LCM_TAG_MASK));
			}
			rd_prepare++;
		}
		mdelay(50);
		read_rtn = mipi_force_read(0x00, 1,(uint8_t *)&read_data[0]);//huafeizhoyu141030 add
		read_rtn = mipi_force_read(0x01, 1,(uint8_t *)&read_data[1]);//huafeizhoyu141030 add
		read_rtn = mipi_force_read(0x02, 1,(uint8_t *)&read_data[2]);//huafeizhoyu141030 mod
		printk("lcd_ili9806e_mipi read id value is 0x%x,0x%x,0x%x!\n", read_data[0],read_data[1],read_data[2]);//huafeizhoyu141030 mod

		if((0x98 == read_data[0])&&(0x06 == read_data[1])&&(0x04 == read_data[2])){//add 150907 mod
		//if((0x04 == read_data[2])){
			printk("lcd_ili9806e_2_mipi read id success!\n");
			return 0x980602;
		}
	}

	mdelay(5);
	mipi_set_hs_mode();
	return 0;
}

static struct panel_operations lcd_ili9806e_2_mipi_operations = {
	.panel_init = ili9806e_2_mipi_init,
	.panel_readid = ili9806e_2_readid,
};

static struct timing_rgb lcd_ili9806e_2_mipi_timing = {
	.hfp = 60,  /* unit: pixel */// 100
	.hbp = 80,//80
	.hsync = 60,//6
	.vfp = 20, /*unit: line*/
	.vbp = 14,
	.vsync =6, //6,
};

static struct info_mipi lcd_ili9806e_2_mipi_info = {
	.work_mode  = SPRDFB_MIPI_MODE_VIDEO,
	.video_bus_width = 24, /*18,16*/
	.lan_number = 2,
	.phy_feq = 500*1000,
	.h_sync_pol = SPRDFB_POLARITY_POS,
	.v_sync_pol = SPRDFB_POLARITY_POS,
	.de_pol = SPRDFB_POLARITY_POS,
	.te_pol = SPRDFB_POLARITY_POS,
	.color_mode_pol = SPRDFB_POLARITY_NEG,
	.shut_down_pol = SPRDFB_POLARITY_NEG,
	.timing = &lcd_ili9806e_2_mipi_timing,
	.ops = NULL,
};

struct panel_spec lcd_ili9806e_2_mipi_spec = {
	//.cap = PANEL_CAP_NOT_TEAR_SYNC,
	.width = 480,
	.height = 854,
	.fps = 60,
	.type = LCD_MODE_DSI,
	.direction = LCD_DIRECT_NORMAL,
	.info = {
		.mipi = &lcd_ili9806e_2_mipi_info
	},
	.ops = &lcd_ili9806e_2_mipi_operations,
};


