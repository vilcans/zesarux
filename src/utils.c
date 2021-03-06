/*
    ZEsarUX  ZX Second-Emulator And Released for UniX
    Copyright (C) 2013 Cesar Hernandez Bano

    This file is part of ZEsarUX.

    ZEsarUX is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <time.h>



#if defined(__APPLE__)
        #include <sys/syslimits.h>
#endif


#include "cpu.h"
#include "utils.h"
#include "debug.h"
#include "joystick.h"
#include "compileoptions.h"
#include "tape.h"
#include "snap.h"
#include "screen.h"
#include "realjoystick.h"
#include "menu.h"
#include "z88.h"
#include "audio.h"
#include "playtzx.h"
#include "tape_tzx.h"
#include "operaciones.h"
#include "zx8081.h"
#include "ay38912.h"
#include "autoselectoptions.h"
#include "ulaplus.h"
#include "textspeech.h"
#include "spectra.h"
#include "zxuno.h"
#include "spritechip.h"
#include "timex.h"
#include "cpc.h"
#include "timer.h"
#include "sam.h"
#include "divmmc.h"
#include "divide.h"
#include "superupgrade.h"
#include "chardetect.h"
#include "zx8081.h"
#include "jupiterace.h"
#include "mmc.h"
#include "ide.h"
#include "zxpand.h"
#include "diviface.h"
#include "divmmc.h"
#include "divide.h"
#include "dandanator.h"
#include "ql.h"
#include "m68k.h"
#include "remote.h"
#include "ula.h"
#include "snap_rzx.h"
#include "scmp.h"
#include "mk14.h"
#include "esxdos_handler.h"
#include "tbblue.h"
#include "tsconf.h"
#include "kartusho.h"
#include "mdvtool.h"
#include "betadisk.h"
#include "multiface.h"
#include "pd765.h"
#include "baseconf.h"
#include "settings.h"

//Archivo usado para entrada de teclas
FILE *ptr_input_file_keyboard;
//Nombre archivo
char input_file_keyboard_name_buffer[PATH_MAX];
//Puntero que apunta al nombre
char *input_file_keyboard_name=NULL;
//Si esta insertado
z80_bit input_file_keyboard_inserted;
//Si esta en play (y no pausado)
z80_bit input_file_keyboard_playing;

//Pausa en valores de 1/50 segundos
int input_file_keyboard_delay=5;
//Contador actual
int input_file_keyboard_delay_counter=0;
//Pendiente de leer siguiente tecla
z80_bit input_file_keyboard_pending_next;
//Si lo siguiente es pausa o tecla
z80_bit input_file_keyboard_is_pause;
//Si hay que enviar pausa despues de cada pulsacion o no
z80_bit input_file_keyboard_send_pause;

//modo turbo de spool file
z80_bit input_file_keyboard_turbo={0};

//Si se guarda la configuracion al salir del programa
z80_bit save_configuration_file_on_exit={0};

//ultima tecla leida
unsigned char input_file_keyboard_last_key;

void parse_customfile_options(void);


int write_rom_nested_id_poke_byte;
int write_rom_nested_id_poke_byte_no_time;


//Utilidades externas
char external_tool_sox[PATH_MAX]="/usr/bin/sox";
char external_tool_unzip[PATH_MAX]="/usr/bin/unzip";
char external_tool_gunzip[PATH_MAX]="/bin/gunzip";
char external_tool_tar[PATH_MAX]="/bin/tar";
char external_tool_unrar[PATH_MAX]="/usr/bin/unrar";


//tablas de conversion de teclado
struct x_tabla_teclado tabla_teclado_numeros[]={
//0
                                        {&puerto_61438, 1},
                                        {&puerto_63486, 1},
                                        {&puerto_63486, 2},
                                        {&puerto_63486, 4},
//4
                                        {&puerto_63486, 8},
                                        {&puerto_63486, 16},
                                        {&puerto_61438, 16},
                                        {&puerto_61438, 8},
//8
                                        {&puerto_61438, 4},
                                        {&puerto_61438, 2}
};

struct x_tabla_teclado tabla_teclado_letras[]={
        {&puerto_65022,1},   //A
        {&puerto_32766,16},
        {&puerto_65278,8},
        {&puerto_65022,4},   //D

                                        {&puerto_64510, 4},
                                        {&puerto_65022, 8},
                                        {&puerto_65022, 16},
                                        {&puerto_49150, 16},
                                        {&puerto_57342, 4},
                                        {&puerto_49150, 8},
                                        {&puerto_49150, 4},
                                        {&puerto_49150, 2},
                                        {&puerto_32766, 4},
//M
                                        {&puerto_32766, 8},
                                        {&puerto_57342, 2},
                                        {&puerto_57342, 1},
                                        {&puerto_64510, 1},
                                        {&puerto_64510, 8},
                                        {&puerto_65022, 2},
                                        {&puerto_64510, 16},
//U
                                        {&puerto_57342, 8},
                                        {&puerto_65278, 16},
                                        {&puerto_64510, 2},
                                        {&puerto_65278, 4},
                                        {&puerto_57342, 16},
                                        {&puerto_65278, 2}


};

struct x_tabla_teclado z88_tabla_teclado_numeros[]={
	{&blink_kbd_a13,1}, //0
	{&blink_kbd_a13,32},
	{&blink_kbd_a12,32},
	{&blink_kbd_a11,32},
	{&blink_kbd_a10,32},
	{&blink_kbd_a9,32},	//5
	{&blink_kbd_a8,32},
	{&blink_kbd_a8,2},
	{&blink_kbd_a8,1},
	{&blink_kbd_a11,1}

};

struct x_tabla_teclado cpc_tabla_teclado_numeros[]={
        {&cpc_keyboard_table[4],1}, //0
        {&cpc_keyboard_table[8],1},
        {&cpc_keyboard_table[8],2},
        {&cpc_keyboard_table[7],2},
        {&cpc_keyboard_table[7],1},
        {&cpc_keyboard_table[6],2},     //5
        {&cpc_keyboard_table[6],1},
        {&cpc_keyboard_table[5],2},
        {&cpc_keyboard_table[5],1},
        {&cpc_keyboard_table[4],2}

};





struct x_tabla_teclado z88_tabla_teclado_letras[]={
	{&blink_kbd_a13,8}, //A
	{&blink_kbd_a9,4},
	{&blink_kbd_a11,4},
	{&blink_kbd_a11,8}, //D
	{&blink_kbd_a11,16},
	{&blink_kbd_a10,8},
	{&blink_kbd_a9,8},
	{&blink_kbd_a8,8}, //H
	{&blink_kbd_a9,1},
	{&blink_kbd_a10,2},
	{&blink_kbd_a11,2},
	{&blink_kbd_a13,2}, //L
	{&blink_kbd_a12,2},
	{&blink_kbd_a8,4}, //N
	{&blink_kbd_a10,1},
	{&blink_kbd_a12,1}, //P
	{&blink_kbd_a13,16},
	{&blink_kbd_a10,16},
	{&blink_kbd_a12,8},
	{&blink_kbd_a9,16}, //T
	{&blink_kbd_a9,2},
	{&blink_kbd_a10,4},
	{&blink_kbd_a12,16},
	{&blink_kbd_a12,4}, //X
	{&blink_kbd_a8,16},
	{&blink_kbd_a13,4}
};


struct x_tabla_teclado cpc_tabla_teclado_letras[]={
	{&cpc_keyboard_table[8],32}, //A
	{&cpc_keyboard_table[6],64},
        {&cpc_keyboard_table[7],64},
        {&cpc_keyboard_table[7],32}, //D
        {&cpc_keyboard_table[7],4},
        {&cpc_keyboard_table[6],32},
        {&cpc_keyboard_table[6],16},
        {&cpc_keyboard_table[5],16}, //H
        {&cpc_keyboard_table[4],8},
        {&cpc_keyboard_table[5],32},
        {&cpc_keyboard_table[4],32},
        {&cpc_keyboard_table[4],16}, //L
        {&cpc_keyboard_table[4],64},
        {&cpc_keyboard_table[5],64}, //N
        {&cpc_keyboard_table[4],4},
        {&cpc_keyboard_table[3],8}, //P
        {&cpc_keyboard_table[8],8},
        {&cpc_keyboard_table[6],4},
        {&cpc_keyboard_table[7],16},
        {&cpc_keyboard_table[6],8}, //T
        {&cpc_keyboard_table[5],4},
        {&cpc_keyboard_table[6],128},
        {&cpc_keyboard_table[7],8},
        {&cpc_keyboard_table[7],128}, //X
        {&cpc_keyboard_table[5],8},
        {&cpc_keyboard_table[8],128}
};

// ================================== matrix ============================
//        0      1      2      3      4      5      6      7
//  +-------------------------------------------------------
// 0|    F4     F1      5     F2     F3     F5      4      7
// 1|   Ret   Left     Up    Esc  Right      \  Space   Down
// 2|     ]      z      .      c      b  Pound      m      '
// 3|     [   Caps      k      s      f      =      g      ;
// 4|     l      3      h      1      a      p      d      j
// 5|     9      w      i    Tab      r      -      y      o
// 6|     8      2      6      q      e      0      t      u
// 7| Shift   Ctrl    Alt      x      v      /      n      ,

//89l6ihverantyd
struct x_tabla_teclado ql_tabla_teclado_letras[]={
	      {&ql_keyboard_table[4],16}, //A
	      {&ql_keyboard_table[2],16},
        {&ql_keyboard_table[2],8},
        {&ql_keyboard_table[4],64}, //D
        {&ql_keyboard_table[6],16},//E
        {&ql_keyboard_table[3],16},
        {&ql_keyboard_table[3],64}, //G
        {&ql_keyboard_table[4],4}, //H
        {&ql_keyboard_table[5],4}, //I
        {&ql_keyboard_table[4],128}, //J
        {&ql_keyboard_table[3],4}, //k
        {&ql_keyboard_table[4],1}, //L
        {&ql_keyboard_table[2],64},
        {&ql_keyboard_table[7],64}, //N
        {&ql_keyboard_table[5],128},
        {&ql_keyboard_table[4],32}, //P
        {&ql_keyboard_table[6],8},
        {&ql_keyboard_table[5],16}, //R
        {&ql_keyboard_table[3],8},
        {&ql_keyboard_table[6],64}, //T
        {&ql_keyboard_table[6],128},
        {&ql_keyboard_table[7],16}, //V
        {&ql_keyboard_table[5],2},
        {&ql_keyboard_table[7],8}, //X
        {&ql_keyboard_table[5],64}, //Y
        {&ql_keyboard_table[2],2}
};

// ================================== matrix ============================
//        0      1      2      3      4      5      6      7
//  +-------------------------------------------------------
// 0|    F4     F1      5     F2     F3     F5      4      7
// 1|   Ret   Left     Up    Esc  Right      \  Space   Down
// 2|     ]      z      .      c      b  Pound      m      '
// 3|     [   Caps      k      s      f      =      g      ;
// 4|     l      3      h      1      a      p      d      j
// 5|     9      w      i    Tab      r      -      y      o
// 6|     8      2      6      q      e      0      t      u
// 7| Shift   Ctrl    Alt      x      v      /      n      ,

struct x_tabla_teclado ql_tabla_teclado_numeros[]={
	      {&ql_keyboard_table[6],32}, //0
        {&ql_keyboard_table[4],8},
        {&ql_keyboard_table[6],2},
        {&ql_keyboard_table[4],2}, //3
        {&ql_keyboard_table[0],64},
        {&ql_keyboard_table[0],4}, //5
        {&ql_keyboard_table[6],4},
        {&ql_keyboard_table[0],128}, //7
        {&ql_keyboard_table[6],1},
        {&ql_keyboard_table[5],1} //9
};


struct x_tabla_teclado mk14_tabla_teclado_numeros[]={
        {&mk14_keystatus[0],128}, //0
        {&mk14_keystatus[1],128},
        {&mk14_keystatus[2],128},
        {&mk14_keystatus[3],128},
        {&mk14_keystatus[4],128},
        {&mk14_keystatus[5],128},     //5
        {&mk14_keystatus[6],128},
        {&mk14_keystatus[7],128},
        {&mk14_keystatus[0],64},
        {&mk14_keystatus[1],64}

};

/*

Lista fabricantes
Science of Cambridge
Sinclair
Amstrad
Timex Sinclair
Investronica
Microdigital Eletronica
Zxuno team
Chloe Corporation
Jeff Braine
Cambridge computers
Jupiter cantab
Miles Gordon Tech
Pentagon


*/
char *array_fabricantes[]={
  "Science of Cambridge",
	"Sinclair Research",
  "Amstrad",
	"Timex Sinclair",
	"Investronica",
	"Microdigital Eletronica",
  "Cambridge Computers",
  "Miles Gordon Technology",
  "Pentagon",
  "Chloe Corporation",
  "Mario Prato",
  "Jeff Braine",
	"ZXUno Team",
        "NedoPC",
  "TS Labs",
	"VTrucco/FB Labs",
	"Jupiter Cantab"
        
};

char *array_fabricantes_hotkey[]={
        "Scie~~nce of Cambridge",
        "~~Sinclair Research",
        "~~Amstrad",
        "~~Timex Sinclair",
        "~~Investronica",
        "~~Microdigital Eletronica",
        "Cam~~bridge Computers",
        "Miles ~~Gordon Technology",
        "~~Pentagon",
        "~~Chloe Corporation",
        "Ma~~rio Prato",
        "~~Jeff Braine",
        "~~ZXUno Team",
        "N~~edoPC",
        "TS ~~Labs",
        "~~VTrucco/FB Labs",
        "J~~upiter Cantab"
        


};

//Si letra es espacio->no hay letra
char array_fabricantes_hotkey_letra[]="nsatimbgpcrjzelvu";




//Array de maquinas por fabricante, acabado en 255
int array_maquinas_sinclair[]={
	120,121,0,1,6,160,255
};

int array_maquinas_timex_sinclair[]={
	17,255
};

int array_maquinas_cambridge_computers[]={
	130,255
};

int array_maquinas_investronica[]={
	2,20,7,255
};

int array_maquinas_microdigital_electronica[]={
	3,4,5,255
};

int array_maquinas_amstrad[]={
	8,9,10,11,12,13,MACHINE_ID_SPECTRUM_P3_40,MACHINE_ID_SPECTRUM_P3_41,MACHINE_ID_SPECTRUM_P3_SPA,140,255
};

int array_maquinas_jupiter_cantab[]={
	122,255
};

int array_maquinas_zxuno_team[]={
	14,255
};

int array_maquinas_chloe_corporation[]={
	15,16,255
};

int array_maquinas_jeff_braine[]={
	18,255
};

int array_maquinas_tbblue[]={
	19,255
};

int array_maquinas_miles_gordon[]={
	150,255
};

int array_maquinas_pentagon[]={
	21,255
};

int array_maquinas_marioprato[]={
	22,255
};

int array_maquinas_tslabs[]={
	MACHINE_ID_TSCONF,255
};

int array_maquinas_nedopc[]={
	MACHINE_ID_BASECONF,255
};

int array_maquinas_science_of_cambridge[]={
	MACHINE_ID_MK14_STANDARD,255
};

//Retorna array a maquinas segun fabricante
int *return_maquinas_fabricante(int fabricante)
{
	switch (fabricante) {
		case FABRICANTE_SINCLAIR:
			return array_maquinas_sinclair;
		break;

		case FABRICANTE_TIMEX_SINCLAIR:
			return array_maquinas_timex_sinclair;
		break;

		case FABRICANTE_CAMBRIDGE_COMPUTERS:
			return array_maquinas_cambridge_computers;
		break;

		case FABRICANTE_INVESTRONICA:
			return array_maquinas_investronica;
		break;

		case FABRICANTE_MICRODIGITAL_ELECTRONICA:
			return array_maquinas_microdigital_electronica;
		break;

		case FABRICANTE_AMSTRAD:
			return array_maquinas_amstrad;
		break;

		case FABRICANTE_JUPITER_CANTAB:
			return array_maquinas_jupiter_cantab;
		break;

		case FABRICANTE_ZXUNO_TEAM:
			return array_maquinas_zxuno_team;
		break;

		case FABRICANTE_CHLOE_CORPORATION:
			return array_maquinas_chloe_corporation;
		break;

		case FABRICANTE_JEFF_BRAINE:
			return array_maquinas_jeff_braine;
		break;

		case FABRICANTE_TBBLUE:
			return array_maquinas_tbblue;
		break;

		case FABRICANTE_MILES_GORDON:
			return array_maquinas_miles_gordon;
		break;

		case FABRICANTE_PENTAGON:
			return array_maquinas_pentagon;
		break;

    case FABRICANTE_MARIOPRATO:
      return array_maquinas_marioprato;
    break;

    case FABRICANTE_TSLABS:
      return array_maquinas_tslabs;
    break;

        case FABRICANTE_NEDOPC:
      return array_maquinas_nedopc;
    break;

    case FABRICANTE_SCIENCE_OF_CAMBRIDGE:
      return array_maquinas_science_of_cambridge;
    break;

		default:
			cpu_panic("Unknown machines made by manufacturer");
		break;
	}

	//Aqui no se llega nunca. Pero para evitar warning del compilador
	return NULL;
}


//Retorna fabricante segun tipo maquina
int return_fabricante_maquina(int maquina)
{
	switch (maquina) {
		case 0:
		case 1:
		case 6:
		case 120:
		case 121:
		case 160:
			return FABRICANTE_SINCLAIR;
		break;

		case 17:
			return FABRICANTE_TIMEX_SINCLAIR;
		break;

		case 130:
			return FABRICANTE_CAMBRIDGE_COMPUTERS;
		break;

		case 2:
		case 7:
		case 20:
			return FABRICANTE_INVESTRONICA;
		break;

		case 3:
		case 4:
		case 5:
			return FABRICANTE_MICRODIGITAL_ELECTRONICA;
		break;

		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case MACHINE_ID_SPECTRUM_P3_40:
		case MACHINE_ID_SPECTRUM_P3_41:
		case MACHINE_ID_SPECTRUM_P3_SPA:
		case 140:
			return FABRICANTE_AMSTRAD;
		break;

		case 122:
			return FABRICANTE_JUPITER_CANTAB;
		break;

		case 14:
			return FABRICANTE_ZXUNO_TEAM;
		break;

		case 15:
		case 16:
			return FABRICANTE_CHLOE_CORPORATION;
		break;

		case 18:
			return FABRICANTE_JEFF_BRAINE;
		break;

		case 19:
			return FABRICANTE_TBBLUE;
		break;

		case 150:
			return FABRICANTE_MILES_GORDON;
		break;

		case 21:
			return FABRICANTE_PENTAGON;
		break;

    case MACHINE_ID_CHROME:
      return FABRICANTE_MARIOPRATO;
    break;

    case MACHINE_ID_TSCONF:
      return FABRICANTE_TSLABS;
    break;

        case MACHINE_ID_BASECONF:
      return FABRICANTE_NEDOPC;
    break;

    case MACHINE_ID_MK14_STANDARD:
      return FABRICANTE_SCIENCE_OF_CAMBRIDGE;
    break;

		default:
			cpu_panic ("Unknown manufacturer for machine id");
		break;
	}

	//Aqui no se llega nunca. Pero para evitar warning del compilador
	return 0;
}


//Retorna posicion dentro del array de maquinas indicado, posicion en array. 255 si no se encuentra
int return_machine_position(int *array_maquinas,int id_maquina)
{
	int index=0;
	while (array_maquinas[index]!=255) {
		if (id_maquina==array_maquinas[index]) return index;
		index++;
	}

	return 255;
}

z80_bit debug_parse_config_file;


#ifdef EMULATE_CPU_STATS
//Arrays de contadores de numero de veces instrucciones usadas
unsigned int stats_codsinpr[256];
unsigned int stats_codpred[256];
unsigned int stats_codprcb[256];
unsigned int stats_codprdd[256];
unsigned int stats_codprfd[256];
unsigned int stats_codprddcb[256];
unsigned int stats_codprfdcb[256];
//codprddfd.c  codprddfd.o  codpred.c    codpred.o

void util_stats_increment_counter(unsigned int *stats_array,int index)
{
	stats_array[index]++;
}

unsigned int util_stats_get_counter(unsigned int *stats_array,int index)
{
	return stats_array[index];
}

void util_stats_set_counter(unsigned int *stats_array,int index,unsigned int value)
{
	stats_array[index]=value;
}


//Buscar dentro del indice el de mayor valor
int util_stats_find_max_counter(unsigned int *stats_array)
{
	int index_max=0;
	unsigned int value_max=0;

	int i;
	unsigned int value;

	for (i=0;i<256;i++) {
		value=util_stats_get_counter(stats_array,i);
		if (value>value_max) {
			value_max=value;
			index_max=i;
		}
	}

	return index_max;
}


void util_stats_init(void)
{

        debug_printf (VERBOSE_INFO,"Initializing CPU Statistics Counter Array");

        int i;

        for (i=0;i<256;i++) {
                util_stats_set_counter(stats_codsinpr,i,0);
                util_stats_set_counter(stats_codpred,i,0);
                util_stats_set_counter(stats_codprcb,i,0);
                util_stats_set_counter(stats_codprdd,i,0);
                util_stats_set_counter(stats_codprfd,i,0);
                util_stats_set_counter(stats_codprddcb,i,0);
                util_stats_set_counter(stats_codprfdcb,i,0);
        }
}

//Retornar la suma de todos los contadores
unsigned int util_stats_sum_all_counters(void)
{

	unsigned int total=0;
	int i;

        for (i=0;i<256;i++) {
                total +=util_stats_get_counter(stats_codsinpr,i);
                total +=util_stats_get_counter(stats_codpred,i);
                total +=util_stats_get_counter(stats_codprcb,i);
                total +=util_stats_get_counter(stats_codprdd,i);
                total +=util_stats_get_counter(stats_codprfd,i);
                total +=util_stats_get_counter(stats_codprddcb,i);
                total +=util_stats_get_counter(stats_codprfdcb,i);
        }


	return total;

}


#endif








//Obtiene la extension de filename y la guarda en extension. Extension sin el punto
//filename debe ser nombre de archivo, sin incluir directorio
void util_get_file_extension(char *filename,char *extension)
{

        //obtener extension del nombre
        //buscar ultimo punto
        //parar si se encuentra / o \ de division de carpeta

        char caracter_carpeta='/';
#ifdef MINGW
        caracter_carpeta='\\';    
#endif


        int j;
        j=strlen(filename);
        if (j==0) extension[0]=0;
        else {
                for (;j>=0 && filename[j]!='.' && filename[j]!=caracter_carpeta;j--);

                if (filename[j]==caracter_carpeta) {
                        extension[0]=0; //no hay extension
                }

                else {
                        if (j>=0) strcpy(extension,&filename[j+1]);
                        else extension[0]=0;
                }
        }

        debug_printf (VERBOSE_DEBUG,"Filename: [%s] Extension: [%s]",filename,extension);
}

//Obtiene el nombre de filename sin extension y la guarda en filename_without_extension.
//filename debe ser nombre de archivo, sin incluir directorio
//TODO: busca hasta el primer punto. quiza deberia tener en cuenta el punto desde el final... o no..
void util_get_file_without_extension(char *filename,char *filename_without_extension)
{

	int i;

	for (i=0;filename[i]!=0 && filename[i]!='.';i++) {
		filename_without_extension[i]=filename[i];
	}

	filename_without_extension[i]=0;

        //printf ("Filename: %s without Extension: %s\n",filename,filename_without_extension);
}



//Obtiene el nombre y extension (quitamos el directorio)
void util_get_file_no_directory(char *filename,char *file_no_dir)
{
	//buscamos / o barra invertida windows

        int j;
        j=strlen(filename);
        if (j==0) file_no_dir[0]=0;
	else {
		for (;j>=0 && filename[j]!='/' && filename[j]!='\\' ;j--);

		strcpy(file_no_dir,&filename[j+1]);
	}
}

//Compara la extension indicada con la del archivo, sin distinguir mayusculas
//Devuelve valor de strcasecmp -> 0 igual, otros->diferente
//Extension sin el punto
int util_compare_file_extension(char *filename,char *extension_compare)
{
	char extension[NAME_MAX];

	util_get_file_extension(filename,extension);

	return strcasecmp(extension,extension_compare);
}



//Retorna el directorio de una ruta completa, ignorando barras repetidas del final
void util_get_dir(char *ruta,char *directorio)
{
        int i;

        if (ruta==NULL) {
                debug_printf (VERBOSE_DEBUG,"ruta NULL");
                directorio[0]=0;
                return;
        }

	//buscar barra final / (o \ de windows)
        for (i=strlen(ruta)-1;i>=0;i--) {
                if (ruta[i]=='/' || ruta[i]=='\\') break;
        }

        //Ubicarse en el primer caracter no /
        for (;i>=0;i--) {
                if (ruta[i]!='/' && ruta[i]!='\\') break;
        }        

        if (i>=0) {

                //TODO. porque esto no va con strcpy??? Da segmentation fault en mac, en cintas de revenge.p
                int j;
                for (j=0;j<=i;j++) {
                        directorio[j]=ruta[j];
                }
                //sleep(1);
                //printf ("despues de strncpy\n");
                directorio[i+1]='/';
                directorio[i+2]=0;
                //printf ("directorio final: %s\n",directorio);
        }

        else {
                //printf ("directorio vacio\n");
                //directorio vacio
                directorio[0]=0;
        }

}

//Compone path completo a un archivo teniendo el directorio y nombre
//tiene en cuenta si directorio es vacio
//lo mete en fullpath
void util_get_complete_path(char *dir,char *name,char *fullpath)
{
	if (dir[0]==0) {
		//directorio nulo
		sprintf (fullpath,"%s",name);
	}
	else {
		sprintf (fullpath,"%s/%s",dir,name);
	}
}

void util_press_menu_symshift(int pressrelease)
{
        if (pressrelease) menu_symshift.v=1;
        else menu_symshift.v=0;
}


//En spectrum, desactiva symbol shift. En zx80/81, desactiva mayusculas
void clear_symshift(void)
{
        if (MACHINE_IS_ZX8081) puerto_65278 |=1;
        else puerto_32766 |=2;

        //Desactivamos tecla symshift para menu
        menu_symshift.v=0;        

}


//En spectrum, activa symbol shift. En zx80/81, activa mayusculas
void set_symshift(void)
{
//puerto_65278   db    255  ; V    C    X    Z    Sh    ;0
//puerto_32766    db              255  ; B    N    M    Simb Space ;7

        if (MACHINE_IS_ZX8081) puerto_65278  &=255-1;
        else puerto_32766 &=255-2;

        //Activamos tecla symshift para menu
        menu_symshift.v=1;

}

void set_shift_z88(void)
{
	//A14 (#6) | HELP   LSH     TAB     DIA     MENU    ,       ;       '
	blink_kbd_a14 &= 255-64;
}

void clear_shift_z88(void)
{
        //A14 (#6) | HELP   LSH     TAB     DIA     MENU    ,       ;       '
        blink_kbd_a14 |= 64;
}




//Abre un archivo en solo lectura buscandolo en las rutas:
//1) ruta actual
//2) ../Resources/
//3) INSTALLPREFIX/
//normalmente usado para cargar roms
//modifica puntero FILE adecuadamente
void old_open_sharedfile(char *archivo,FILE **f)
{
        char buffer_nombre[1024];
	strcpy(buffer_nombre,archivo);

        //ruta actual
        debug_printf(VERBOSE_INFO,"Looking for file %s at current dir",buffer_nombre);
        *f=fopen(buffer_nombre,"rb");

        //sino, en ../Resources
        if (!(*f)) {
                sprintf(buffer_nombre,"../Resources/%s",archivo);
                debug_printf(VERBOSE_INFO,"Looking for file %s",buffer_nombre);
                *f=fopen(buffer_nombre,"rb");

                //sino, en INSTALLPREFIX/share/zesarux
                if (!(*f)) {
                        sprintf(buffer_nombre,"%s/%s/%s",INSTALL_PREFIX,"/share/zesarux/",archivo);
                        debug_printf(VERBOSE_INFO,"Looking for file %s",buffer_nombre);
                        *f=fopen(buffer_nombre,"rb");
                }
        }


}


//Busca un archivo buscandolo en las rutas:
//1) ruta actual
//2) ../Resources/
//3) INSTALLPREFIX/
//normalmente usado para cargar roms
//Retorna ruta archivo en ruta_final (siempre que no sea NULL), valor retorno no 0. Si no existe, valor retorno 0
int find_sharedfile(char *archivo,char *ruta_final)
{
        char buffer_nombre[PATH_MAX];
	strcpy(buffer_nombre,archivo);

	int existe;

        //ruta actual
        debug_printf(VERBOSE_INFO,"Looking for file %s at current dir",buffer_nombre);
        existe=si_existe_archivo(buffer_nombre);

        //sino, en ../Resources
        if (!existe) {
                sprintf(buffer_nombre,"../Resources/%s",archivo);
                debug_printf(VERBOSE_INFO,"Looking for file %s",buffer_nombre);
                existe=si_existe_archivo(buffer_nombre);

                //sino, en INSTALLPREFIX/share/zesarux
                if (!existe) {
                        sprintf(buffer_nombre,"%s/%s/%s",INSTALL_PREFIX,"/share/zesarux/",archivo);
                        debug_printf(VERBOSE_INFO,"Looking for file %s",buffer_nombre);
                        existe=si_existe_archivo(buffer_nombre);
                }
        }

	if (existe) {
		if (ruta_final!=NULL) strcpy(ruta_final,buffer_nombre);
		debug_printf(VERBOSE_INFO,"Found on path %s",buffer_nombre);
	}

	return existe;
}


//Abre un archivo en solo lectura buscandolo en las rutas:
//1) ruta actual
//2) ../Resources/
//3) INSTALLPREFIX/
//normalmente usado para cargar roms
//modifica puntero FILE adecuadamente
void open_sharedfile(char *archivo,FILE **f)
{
        char buffer_nombre[PATH_MAX];

	int existe=find_sharedfile(archivo,buffer_nombre);
	if (existe) {
		*f=fopen(buffer_nombre,"rb");
	}
 
	else *f=NULL;

}


//Devuelve puntero a archivo si ha podido encontrar el archivo y abrirlo en escritura.
//NULL
void open_sharedfile_write_open(char *archivo,FILE **f)
{
	//Asumimos NULL
	*f=NULL;

	if (!si_existe_archivo(archivo)) return;

	*f=fopen(archivo,"wb");

}

//Abre un archivo en escritura buscandolo en las rutas:
//1) ruta actual
//2) ../Resources/
//3) INSTALLPREFIX/
//normalmente usado para cargar roms
//modifica puntero FILE adecuadamente
//Primero mira que el archivo exista y luego intenta abrirlo para escritura
void open_sharedfile_write(char *archivo,FILE **f)
{
        char buffer_nombre[1024];

        //ruta actual
        debug_printf(VERBOSE_INFO,"Looking for file %s at current dir",archivo);
        open_sharedfile_write_open(archivo,f);

        //sino, en ../Resources
        if (!(*f)) {
                sprintf(buffer_nombre,"../Resources/%s",archivo);
                debug_printf(VERBOSE_INFO,"Looking for file %s",buffer_nombre);
                open_sharedfile_write_open(buffer_nombre,f);

                //sino, en INSTALLPREFIX/share/zesarux
                if (!(*f)) {
                        sprintf(buffer_nombre,"%s/%s/%s",INSTALL_PREFIX,"/share/zesarux/",archivo);
                        debug_printf(VERBOSE_INFO,"Looking for file %s",buffer_nombre);
                        open_sharedfile_write_open(buffer_nombre,f);
                }
        }

	//printf ("file: %d\n",*f);

}

void get_compile_info(char *s)
{
        sprintf (s,"%s",
        "Compilation date: " COMPILATION_DATE "\n"
	"\n"
	"Compilation system: " COMPILATION_SYSTEM "\n"
	"\n"
        "Configure command: " CONFIGURE_OPTIONS "\n"
	"\n"
        "Compile variables: " COMPILE_VARIABLES "\n"
	"\n"
        "Compile INITIALCFLAGS: " COMPILE_INITIALCFLAGS "\n"
        "Compile INITIALLDFLAGS: " COMPILE_INITIALLDFLAGS "\n"
        "Compile FINALCFLAGS: " COMPILE_FINALCFLAGS "\n"
        "Compile FINALLDFLAGS: " COMPILE_FINALLDFLAGS "\n"
	"\n"
        "Install PREFIX: " INSTALL_PREFIX "\n"
        );

	if (strlen(s)>=MAX_COMPILE_INFO_LENGTH) cpu_panic("Error. MAX_COMPILE_INFO_LENGTH reached");
}

void show_compile_info(void)
{
        char buffer[MAX_COMPILE_INFO_LENGTH];
        get_compile_info(buffer);
        printf ("%s",buffer);
}


//Segun tecla de entrada, genera puerto de teclado, lo pone o lo borra
void ascii_to_keyboard_port_set_clear(unsigned tecla,int pressrelease)
{

	//printf ("ascii_to_keyboard_port_set_clear: tecla: %d pressrelease: %d\n",tecla,pressrelease);

                                if (tecla>='A' && tecla<='Z') {
					if (MACHINE_IS_SPECTRUM || MACHINE_IS_ACE) {
                                        	//mayus. para Spectrum
						if (pressrelease) {
	                                        	puerto_65278 &=255-1;
						}
						else {
							puerto_65278 |=1;
						}
					}

					if (MACHINE_IS_Z88) {
						//mayus. para Z88
						//A14 (#6) | HELP   LSH     TAB     DIA     MENU    ,       ;       '
						if (pressrelease) {
							blink_kbd_a14 &=255-64;
						}
						else {
							blink_kbd_a14 |=64;
						}
					}

					if (MACHINE_IS_CPC) {
						if (pressrelease) {
							cpc_keyboard_table[2] &=255-32;
						}
						else {
							cpc_keyboard_table[2] |=32;
						}
					}

       	                                tecla=tecla+('a'-'A');
                                }


        //printf ("Tecla buena: %d  \n",c);
	switch(tecla) {

                                case 27:
                                        //printf ("Alt\n");
                                break;

				case 32:
                                	if (pressrelease) {
						puerto_32766 &=255-1;
                                        	blink_kbd_a13 &= (255-64);
						cpc_keyboard_table[5] &= (255-128);
					}
	                                else {
						puerto_32766 |=1;
                                        	blink_kbd_a13 |= 64;
						cpc_keyboard_table[5] |= 128;
					}

        	                break;


	                        case 13:
	                        case 10:
        	                        if (pressrelease) {
						puerto_49150 &=255-1;
						blink_kbd_a8 &= (255-64);
						//Enter "grande" del cpc
						cpc_keyboard_table[2] &= (255-4);
					}

                	                else {
						puerto_49150 |=1;
						blink_kbd_a8 |= 64;
						cpc_keyboard_table[2] |= 4;
					}


                        	break;

                                default:
					convert_numeros_letras_puerto_teclado(tecla,pressrelease);
                                break;
	}


	//simbolos para ZX Spectrum
	if (MACHINE_IS_SPECTRUM) {
		switch (tecla) {
                                case '!':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_63486 &=255-1;
					}
					else {
						clear_symshift();
						puerto_63486 |=1;
					}
                                break;

                                //Enviar caps shift con 128
                                case 128:
                                        if (pressrelease) {
						//mayus
						puerto_65278 &=255-1;
                                        }
                                        else {
						//mayus
						puerto_65278 |=1;
                                        }
				break;

                                //Enviar symbol shift con 129
                                case 129:
                                        if (pressrelease) {
						set_symshift();
                                        }
                                        else {
						clear_symshift();
                                        }
				break;

				//Enviar Shift + 1 (edit).
				/*
				En curses va bien
				Es stdout habria que hacer el | , luego el texto y enter, todo a la vez, en la misma linea,
				porque si se hace | y enter, se edita la linea y al dar enter se entra la linea sin modificar
				*/
				case '|':
                                        if (pressrelease) {
						//mayus
						puerto_65278 &=255-1;
                                                puerto_63486 &=255-1;
                                        }
                                        else {
						//mayus
						puerto_65278 |=1;
                                                puerto_63486 |=1;
                                        }
				break;


                                //Symbol + 2
                                case '@':
                                        if (pressrelease) {
						set_symshift();
	                                        puerto_63486 &=255-2;
					}
					else {
						clear_symshift();
	                                        puerto_63486 |=2;
					}
                                break;

                                 //Symbol + 3
                                case '#':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_63486 &=255-4;
					}
					else {
	                                        clear_symshift();
        	                                puerto_63486 |=4;
					}

                                break;

                                case '$':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_63486 &=255-8;
					}
					else {
	                                        clear_symshift();
        	                                puerto_63486 |=8;
					}
                                break;


                                case '%':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_63486 &=255-16;
					}
					else {
	                                        clear_symshift();
        	                                puerto_63486 |=16;
					}
                                break;

                                case '&':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_61438 &=255-16;
					}
					else {
	                                        clear_symshift();
        	                                puerto_61438 |=16;
					}
                                break;

                                //Comilla simple
                                case '\'':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_61438 &=255-8;
					}
					else {
	                                        clear_symshift();
        	                                puerto_61438 |=8;
					}
                                break;

                                case '(':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_61438 &=255-4;
					}
					else {
	                                        clear_symshift();
        	                                puerto_61438 |=4;
					}
                                break;

                                case ')':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_61438 &=255-2;
					}
					else {
	                                        clear_symshift();
        	                                puerto_61438 |=2;
					}
                                break;

                                case '_':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_61438 &=255-1;
					}
					else {
	                                        clear_symshift();
        	                                puerto_61438 |=1;
					}
                                break;

                                case '*':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_32766 &=255-16;
					}
					else {
	                                        clear_symshift();
        	                                puerto_32766 |=16;
					}
                                break;

                                case '?':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_65278 &=255-8;
					}
					else {
	                                        clear_symshift();
        	                                puerto_65278 |=8;
					}
                                break;

                                //Esto equivale al simbolo de "flecha" arriba - exponente
                                case '^':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_49150 &=255-16;
					}
					else {
	                                        clear_symshift();
        	                                puerto_49150 |=16;
					}
                                break;

                                case '-':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_49150 &=255-8;
					}
					else {
	                                        clear_symshift();
        	                                puerto_49150 |=8;
					}
                                break;


                                case '+':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_49150 &=255-4;
					}
					else {
	                                        clear_symshift();
        	                                puerto_49150 |=4;
					}
                                break;

                                case '=':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_49150 &=255-2;
					}
					else {
	                                        clear_symshift();
        	                                puerto_49150 |=2;
					}
                                break;

                                case ';':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_57342 &=255-2;
					}
					else {
	                                        clear_symshift();
        	                                puerto_57342 |=2;
					}
                                break;

                                //Comilla doble
                                case '"':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_57342 &=255-1;
					}
					else {
	                                        clear_symshift();
        	                                puerto_57342 |=1;
					}
                                break;

                                case '<':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_64510 &=255-8;
					}
					else {
	                                        clear_symshift();
        	                                puerto_64510 |=8;
					}
                                break;

                                case '>':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_64510 &=255-16;
					}
					else {
	                                        clear_symshift();
        	                                puerto_64510 |=16;
					}
                                break;

                                case '/':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_65278 &=255-16;
					}
					else {
	                                        clear_symshift();
        	                                puerto_65278 |=16;
					}
                                break;

                                case ':':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_65278 &=255-2;
					}
					else {
	                                        clear_symshift();
        	                                puerto_65278 |=2;
					}
                                break;

                                case '.':
					if (pressrelease) {
						puerto_32766 &=255-2-4;
					}
					else {
						puerto_32766 |=2+4;
					}

                                break;

                                case ',':
					if (pressrelease) {
	                                        puerto_32766 &=255-2-8;
					}
					else {
	                                        puerto_32766 |=2+8;
					}
				break;


		}
	}

	//simbolos para Jupiter Ace
	else if (MACHINE_IS_ACE) {
		switch (tecla) {
                                case '!':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_63486 &=255-1;
					}
					else {
						clear_symshift();
						puerto_63486 |=1;
					}
                                break;



                                case '@':
                                        if (pressrelease) {
						set_symshift();
	                                        puerto_63486 &=255-2;
					}
					else {
						clear_symshift();
	                                        puerto_63486 |=2;
					}
                                break;

                                case '#':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_63486 &=255-4;
					}
					else {
	                                        clear_symshift();
        	                                puerto_63486 |=4;
					}

                                break;

                                case '$':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_63486 &=255-8;
					}
					else {
	                                        clear_symshift();
        	                                puerto_63486 |=8;
					}
                                break;


                                case '%':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_63486 &=255-16;
					}
					else {
	                                        clear_symshift();
        	                                puerto_63486 |=16;
					}
                                break;

                                case '&':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_61438 &=255-16;
					}
					else {
	                                        clear_symshift();
        	                                puerto_61438 |=16;
					}
                                break;

                                //Comilla simple
                                case '\'':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_61438 &=255-8;
					}
					else {
	                                        clear_symshift();
        	                                puerto_61438 |=8;
					}
                                break;

                                case '(':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_61438 &=255-4;
					}
					else {
	                                        clear_symshift();
        	                                puerto_61438 |=4;
					}
                                break;

                                case ')':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_61438 &=255-2;
					}
					else {
	                                        clear_symshift();
        	                                puerto_61438 |=2;
					}
                                break;

                                case '_':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_61438 &=255-1;
					}
					else {
	                                        clear_symshift();
        	                                puerto_61438 |=1;
					}
                                break;

                                case '*':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_32766 &=255-16;
					}
					else {
	                                        clear_symshift();
        	                                puerto_32766 |=16;
					}
                                break;

                                case '?':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_65278 &=255-8;
					}
					else {
	                                        clear_symshift();
        	                                puerto_65278 |=8;
					}
                                break;

				case '~':
					if (pressrelease) {
                                                set_symshift();
                                                puerto_65022 &=255-1;
                                        }
                                        else {
                                                clear_symshift();
                                                puerto_65022 |=1;
                                        }
                                break;

                                case '|':
                                        if (pressrelease) {
                                                set_symshift();
                                                puerto_65022 &=255-2;
                                        }
                                        else {
                                                clear_symshift();
                                                puerto_65022 |=2;
                                        }
                                break;

                                case '\\':
                                        if (pressrelease) {
                                                set_symshift();
                                                puerto_65022 &=255-4;
                                        }
                                        else {
                                                clear_symshift();
                                                puerto_65022 |=4;
                                        }
                                break;

                                case '{':
                                        if (pressrelease) {
                                                set_symshift();
                                                puerto_65022 &=255-8;
                                        }
                                        else {
                                                clear_symshift();
                                                puerto_65022 |=8;
                                        }
                                break;

                                case '}':
                                        if (pressrelease) {
                                                set_symshift();
                                                puerto_65022 &=255-16;
                                        }
                                        else {
                                                clear_symshift();
                                                puerto_65022 |=16;
                                        }
                                break;


                                //Esto equivale al simbolo de "flecha" arriba - exponente
                                case '^':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_49150 &=255-16;
					}
					else {
	                                        clear_symshift();
        	                                puerto_49150 |=16;
					}
                                break;

                                case '-':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_49150 &=255-8;
					}
					else {
	                                        clear_symshift();
        	                                puerto_49150 |=8;
					}
                                break;


                                case '+':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_49150 &=255-4;
					}
					else {
	                                        clear_symshift();
        	                                puerto_49150 |=4;
					}
                                break;

                                case '=':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_49150 &=255-2;
					}
					else {
	                                        clear_symshift();
        	                                puerto_49150 |=2;
					}
                                break;

                                case '[':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_57342 &=255-16;
					}
					else {
	                                        clear_symshift();
        	                                puerto_57342 |=16;
					}
                                break;

                                case ']':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_57342 &=255-8;
					}
					else {
	                                        clear_symshift();
        	                                puerto_57342 |=8;
					}
                                break;


				/* Simbolo copyright (C) . TODO
                                case '(C)':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_57342 &=255-4;
					}
					else {
	                                        clear_symshift();
        	                                puerto_57342 |=4;
					}
                                break;
				*/

                                case ';':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_57342 &=255-2;
					}
					else {
	                                        clear_symshift();
        	                                puerto_57342 |=2;
					}
                                break;

                                //Comilla doble
                                case '"':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_57342 &=255-1;
					}
					else {
	                                        clear_symshift();
        	                                puerto_57342 |=1;
					}
                                break;

                                case '<':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_64510 &=255-8;
					}
					else {
	                                        clear_symshift();
        	                                puerto_64510 |=8;
					}
                                break;

                                case '>':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_64510 &=255-16;
					}
					else {
	                                        clear_symshift();
        	                                puerto_64510 |=16;
					}
                                break;

                                case '/':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_65278 &=255-16;
					}
					else {
	                                        clear_symshift();
        	                                puerto_65278 |=16;
					}
                                break;

                                case ':':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_65278 &=255-2;
					}
					else {
	                                        clear_symshift();
        	                                puerto_65278 |=2;
					}
                                break;

                                case '.':
					if (pressrelease) {
						puerto_32766 &=255-2-4;
					}
					else {
						puerto_32766 |=2+4;
					}

                                break;

                                case ',':
					if (pressrelease) {
	                                        puerto_32766 &=255-2-8;
					}
					else {
	                                        puerto_32766 |=2+8;
					}
				break;


		}

	}


	//simbolos para ZX80
	else if (MACHINE_IS_ZX80) {
                switch (tecla) {
                                case '$':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_57342 &=255-8;
					}
					else {
	                                        clear_symshift();
        	                                puerto_57342 |=8;
					}
                                break;


                                case '(':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_57342 &=255-4;
					}
					else {
	                                        clear_symshift();
        	                                puerto_57342 |=4;
					}
                                break;

                                case ')':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_57342 &=255-2;
					}
					else {
	                                        clear_symshift();
        	                                puerto_57342 |=2;
					}
                                break;

                                case '*':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_57342 &=255-1;
					}
					else {
	                                        clear_symshift();
        	                                puerto_57342 |=1;
					}
                                break;

                                case '?':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_65278 &=255-8;
					}
					else {
	                                        clear_symshift();
        	                                puerto_65278 |=8;
					}

                                break;

                                //Esto equivale al simbolo de ** - exponente
                                case '^':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_49150 &=255-16;
					}
					else {
	                                        clear_symshift();
        	                                puerto_49150 |=16;
					}
                                break;

                                case '-':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_49150 &=255-8;
					}
					else {
	                                        clear_symshift();
        	                                puerto_49150 |=8;
					}
                                break;

                                case '+':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_49150 &=255-4;
					}
					else {
	                                        clear_symshift();
        	                                puerto_49150 |=4;
					}
                                break;

                                case '=':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_49150 &=255-2;
					}
					else {
	                                        clear_symshift();
        	                                puerto_49150 |=2;
					}
                                break;

                                case ';':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_65278 &=255-4;
					}
					else {
	                                        clear_symshift();
        	                                puerto_65278 |=4;
					}
                                break;

                                //Comilla doble
                                case '"':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_57342 &=255-16;
					}
					else {
	                                        clear_symshift();
        	                                puerto_57342 |=16;
					}
                                break;

                                case '<':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_32766 &=255-8;
					}
					else {
	                                        clear_symshift();
        	                                puerto_32766 |=8;
					}
                                break;

                                case '>':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_32766 &=255-4;
					}
					else {
	                                        clear_symshift();
        	                                puerto_32766 |=4;
					}
                                break;

                                case '/':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_65278 &=255-16;
					}
					else {
	                                        clear_symshift();
        	                                puerto_65278 |=16;
					}
                                break;

                                case ':':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_65278 &=255-2;
					}
					else {
	                                        clear_symshift();
        	                                puerto_65278 |=2;
					}
                                break;

                                case '.':
					if (pressrelease) {
						puerto_32766 &=255-2;
					}
					else {
						puerto_32766 |=2;
					}
                                break;

                                case ',':
                                        if (pressrelease) {
						set_symshift();
						puerto_32766 &=255-2;
					}
					else {
						clear_symshift();
						puerto_32766 |=2;
					}

                                break;

                }
        }

        //simbolos para ZX81
        else if (MACHINE_IS_ZX81) {
                switch (tecla) {

                                case '$':
					if (pressrelease) {
	                                        set_symshift();
						puerto_57342 &=255-8;
					}
					else {
	                                        clear_symshift();
						puerto_57342 |=8;
					}
                                break;


                                case '(':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_57342 &=255-4;
					}
					else {
	                                        clear_symshift();
        	                                puerto_57342 |=4;
					}
                                break;

                                case ')':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_57342 &=255-2;
					}
					else {
	                                        clear_symshift();
        	                                puerto_57342 |=2;
					}
                                break;

                                case '*':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_32766 &=255-16;
					}
					else {
	                                        clear_symshift();
        	                                puerto_32766 |=16;
					}
                                break;

                                case '?':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_65278 &=255-8;
					}
					else {
	                                        clear_symshift();
        	                                puerto_65278 |=8;
					}
                                break;

                                //Esto equivale al simbolo de ** - exponente
                                case '^':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_49150 &=255-16;
					}
					else {
	                                        clear_symshift();
        	                                puerto_49150 |=16;
					}
                                break;

                                case '-':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_49150 &=255-8;
					}
					else {
	                                        clear_symshift();
        	                                puerto_49150 |=8;
					}
                                break;

                                case '+':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_49150 &=255-4;
					}
					else {
	                                        clear_symshift();
        	                                puerto_49150 |=4;
					}
                                break;

                                case '=':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_49150 &=255-2;
					}
					else {
	                                        clear_symshift();
        	                                puerto_49150 |=2;
					}
                                break;

                                case ';':
					if (pressrelease) {
	                                        set_symshift();
	                                        puerto_65278 &=255-4;
					}
					else {
	                                        clear_symshift();
	                                        puerto_65278 |=4;
					}
                                break;

                                //Comilla doble
                                case '"':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_57342 &=255-1;
					}
					else {
	                                        clear_symshift();
        	                                puerto_57342 |=1;
					}
                                break;

                                case '<':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_32766 &=255-8;
					}
					else {
	                                        clear_symshift();
        	                                puerto_32766 |=8;
					}
                                break;

                                case '>':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_32766 &=255-4;
					}
					else {
	                                        clear_symshift();
        	                                puerto_32766 |=4;
					}
                                break;

                                case '/':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_65278 &=255-16;
					}
					else {
	                                        clear_symshift();
        	                                puerto_65278 |=16;
					}
                                break;

                                case ':':
					if (pressrelease) {
	                                        set_symshift();
        	                                puerto_65278 &=255-2;
					}
					else {
	                                        clear_symshift();
        	                                puerto_65278 |=2;
					}
                                break;

                                case '.':
					if (pressrelease) {
	                                        puerto_32766 &=255-2;
					}
					else {
	                                        puerto_32766 |=2;
					}

                                break;

                                case ',':
					if (pressrelease) {
	                                        set_symshift();
						puerto_32766 &=255-2;
					}
					else {
	                                        clear_symshift();
	                                        puerto_32766 |=2;
					}
				break;

                }
        }

        if (MACHINE_IS_Z88) {
                switch (tecla) {
                                case '!':
                                        if (pressrelease) {
						set_shift_z88();
						//A13 (#5) [      SPACE   1       Q       A       Z       L       0
						blink_kbd_a13 &=255-32;
                                        }
                                        else {
						clear_shift_z88();
						blink_kbd_a13 |=32;
                                        }
                                break;

				case '@':
                                        if (pressrelease) {
                                                set_shift_z88();
                                                blink_kbd_a12 &=255-32;
                                        }
                                        else {
                                                clear_shift_z88();
                                                blink_kbd_a12 |=32;
                                        }
                                break;

                                case '#':
                                        if (pressrelease) {
                                                set_shift_z88();
                                                blink_kbd_a11 &=255-32;
                                        }
                                        else {
                                                clear_shift_z88();
                                                blink_kbd_a11 |=32;
                                        }
                                break;

                                case '$':
                                        if (pressrelease) {
                                                set_shift_z88();
                                                blink_kbd_a10 &=255-32;
                                        }
                                        else {
                                                clear_shift_z88();
                                                blink_kbd_a10 |=32;
                                        }
                                break;

                                case '%':
                                        if (pressrelease) {
                                                set_shift_z88();
                                                blink_kbd_a9 &=255-32;
                                        }
                                        else {
                                                clear_shift_z88();
                                                blink_kbd_a9 |=32;
                                        }
                                break;

                                case '^':
                                        if (pressrelease) {
                                                set_shift_z88();
                                                blink_kbd_a8 &=255-32;
                                        }
                                        else {
                                                clear_shift_z88();
                                                blink_kbd_a8 |=32;
                                        }
                                break;

				//A8  (#0) | DEL    ENTER   6       Y       H       N       7       8
                                case '&':
                                        if (pressrelease) {
                                                set_shift_z88();
                                                blink_kbd_a8 &=255-2;
                                        }
                                        else {
                                                clear_shift_z88();
                                                blink_kbd_a8 |=2;
                                        }
                                break;

                                case '*':
                                        if (pressrelease) {
                                                set_shift_z88();
                                                blink_kbd_a8 &=255-1;
                                        }
                                        else {
                                                clear_shift_z88();
                                                blink_kbd_a8 |=1;
                                        }
                                break;

				//A11 (#3) | -      RGT     3       E       D       C       K       9
                                case '(':
                                        if (pressrelease) {
                                                set_shift_z88();
                                                blink_kbd_a11 &=255-1;
                                        }
                                        else {
                                                clear_shift_z88();
                                                blink_kbd_a11 |=1;
                                        }
                                break;

				//A13 (#5) | [      SPACE   1       Q       A       Z       L       0
                                case ')':
                                        if (pressrelease) {
                                                set_shift_z88();
                                                blink_kbd_a13 &=255-1;
                                        }
                                        else {
                                                clear_shift_z88();
                                                blink_kbd_a13 |=1;
                                        }
                                break;

				//A11 (#3) | -      RGT     3       E       D       C       K       9
				//A10 (#2) | =      DWN     4       R       F       V       J       O
				//A9  (#1) | \      UP      5       T       G       B       U       I
                                case '-':
                                        if (pressrelease) {
                                                blink_kbd_a11 &=255-128;
                                        }
                                        else {
                                                blink_kbd_a11 |=128;
                                        }
                                break;


                                case '_':
                                        if (pressrelease) {
                                                set_shift_z88();
                                                blink_kbd_a11 &=255-128;
                                        }
                                        else {
                                                clear_shift_z88();
                                                blink_kbd_a11 |=128;
                                        }
                                break;

                                case '=':
                                        if (pressrelease) {
                                                blink_kbd_a10 &=255-128;
                                        }
                                        else {
                                                blink_kbd_a10 |=128;
                                        }
                                break;


                                case '+':
                                        if (pressrelease) {
                                                set_shift_z88();
                                                blink_kbd_a10 &=255-128;
                                        }
                                        else {
                                                clear_shift_z88();
                                                blink_kbd_a10 |=128;
                                        }
                                break;

                                case '\\':
                                        if (pressrelease) {
                                                blink_kbd_a9 &=255-128;
                                        }
                                        else {
                                                blink_kbd_a9 |=128;
                                        }
                                break;


                                case '|':
                                        if (pressrelease) {
                                                set_shift_z88();
                                                blink_kbd_a9 &=255-128;
                                        }
                                        else {
                                                clear_shift_z88();
                                                blink_kbd_a9 |=128;
                                        }
                                break;

				//A13 (#5) | [      SPACE   1       Q       A       Z       L       0
				//A12 (#4) | ]      LFT     2       W       S       X       M
                                case '[':
                                        if (pressrelease) {
                                                blink_kbd_a13 &=255-128;
                                        }
                                        else {
                                                blink_kbd_a13 |=128;
                                        }
                                break;


                                case '{':
                                        if (pressrelease) {
                                                set_shift_z88();
                                                blink_kbd_a13 &=255-128;
                                        }
                                        else {
                                                clear_shift_z88();
                                                blink_kbd_a13 |=128;
                                        }
                                break;

                                case ']':
                                        if (pressrelease) {
                                                blink_kbd_a12 &=255-128;
                                        }
                                        else {
                                                blink_kbd_a12 |=128;
                                        }
                                break;


                                case '}':
                                        if (pressrelease) {
                                                set_shift_z88();
                                                blink_kbd_a12 &=255-128;
                                        }
                                        else {
                                                clear_shift_z88();
                                                blink_kbd_a12 |=128;
                                        }
                                break;


				//A14 (#6) | HELP   LSH     TAB     DIA     MENU    ,       ;       '
                                case ';':
                                        if (pressrelease) {
                                                blink_kbd_a14 &=255-2;
                                        }
                                        else {
                                                blink_kbd_a14 |=2;
                                        }
                                break;


                                case ':':
                                        if (pressrelease) {
                                                set_shift_z88();
                                                blink_kbd_a14 &=255-2;
                                        }
                                        else {
                                                clear_shift_z88();
                                                blink_kbd_a14 |=2;
                                        }
                                break;

                                //A14 (#6) | HELP   LSH     TAB     DIA     MENU    ,       ;       '
                                case '\'':
                                        if (pressrelease) {
                                                blink_kbd_a14 &=255-1;
                                        }
                                        else {
                                                blink_kbd_a14 |=1;
                                        }
                                break;


                                case '"':
                                        if (pressrelease) {
                                                set_shift_z88();
                                                blink_kbd_a14 &=255-1;
                                        }
                                        else {
                                                clear_shift_z88();
                                                blink_kbd_a14 |=1;
                                        }
                                break;



				//A15 (#7) | RSH    SQR     ESC     INDEX   CAPS    .       /       £
				//TODO Libra se envia como ...?
				/*
                                case 'XXXX':
                                        if (pressrelease) {
                                                blink_kbd_a15 &=255-1;
                                        }
                                        else {
                                                blink_kbd_a15 |=1;
                                        }
                                break;
				*/


                                case '~':
                                        if (pressrelease) {
                                                set_shift_z88();
                                                blink_kbd_a15 &=255-1;
                                        }
                                        else {
                                                clear_shift_z88();
                                                blink_kbd_a15 |=1;
                                        }
                                break;


                                //A14 (#6) | HELP   LSH     TAB     DIA     MENU    ,       ;       '
                                case ',':
                                        if (pressrelease) {
                                                blink_kbd_a14 &=255-4;
                                        }
                                        else {
                                                blink_kbd_a14 |=4;
                                        }
                                break;


                                case '<':
                                        if (pressrelease) {
                                                set_shift_z88();
                                                blink_kbd_a14 &=255-4;
                                        }
                                        else {
                                                clear_shift_z88();
                                                blink_kbd_a14 |=4;
                                        }
                                break;

				//A15 (#7) | RSH    SQR     ESC     INDEX   CAPS    .       /       £
                                case '.':
                                        if (pressrelease) {
                                                blink_kbd_a15 &=255-4;
                                        }
                                        else {
                                                blink_kbd_a15 |=4;
                                        }
                                break;


                                case '>':
                                        if (pressrelease) {
                                                set_shift_z88();
                                                blink_kbd_a15 &=255-4;
                                        }
                                        else {
                                                clear_shift_z88();
                                                blink_kbd_a15 |=4;
                                        }
                                break;

                                //A15 (#7) | RSH    SQR     ESC     INDEX   CAPS    .       /       £
                                case '/':
                                        if (pressrelease) {
                                                blink_kbd_a15 &=255-2;
                                        }
                                        else {
                                                blink_kbd_a15 |=2;
                                        }
                                break;


                                case '?':
                                        if (pressrelease) {
                                                set_shift_z88();
                                                blink_kbd_a15 &=255-2;
                                        }
                                        else {
                                                clear_shift_z88();
                                                blink_kbd_a15 |=2;
                                        }
                                break;



		}
	}



}

//Segun tecla de entrada, genera puerto de teclado, lo activa
void ascii_to_keyboard_port(unsigned tecla)
{
	ascii_to_keyboard_port_set_clear(tecla,1);
}

int input_file_keyboard_is_playing(void)
{
        if (input_file_keyboard_inserted.v && input_file_keyboard_playing.v) return 1;
        else return 0;
}

void insert_input_file_keyboard(void)
{
	input_file_keyboard_inserted.v=1;
        input_file_keyboard_playing.v=0;
}

void eject_input_file_keyboard(void)
{
        input_file_keyboard_inserted.v=0;
        input_file_keyboard_playing.v=0;

        //Si modo turbo, quitar
        if (input_file_keyboard_turbo.v) {
                reset_peek_byte_function_spoolturbo();
                input_file_keyboard_turbo.v=0;
        }
}

int input_file_keyboard_init(void)
{
	ptr_input_file_keyboard=fopen(input_file_keyboard_name,"rb");

	if (!ptr_input_file_keyboard) {
		debug_printf(VERBOSE_ERR,"Unable to open input file %s",input_file_keyboard_name);
		eject_input_file_keyboard();
		return 1;
	}

	insert_input_file_keyboard();

	return 0;

}

void input_file_keyboard_close(void)
{
	eject_input_file_keyboard();
	fclose(ptr_input_file_keyboard);
}


void reset_keyboard_ports(void)
{
        //inicializar todas las teclas a nada - 255
        puerto_65278=255;
        puerto_65022=255;
        puerto_64510=255;
        puerto_63486=255;
        puerto_61438=255;
        puerto_57342=255;
        puerto_49150=255;
        puerto_32766=255;
        puerto_especial1=255;
        puerto_especial2=255;
        puerto_especial3=255;
        puerto_especial4=255;

        puerto_especial_joystick=0;

	//De Z88
	blink_kbd_a15=255;
	blink_kbd_a14=255;
	blink_kbd_a13=255;
	blink_kbd_a12=255;
	blink_kbd_a11=255;
	blink_kbd_a10=255;
	blink_kbd_a9=255;
	blink_kbd_a8=255;

	//De CPC
	int i=0;
	for (i=0;i<16;i++) cpc_keyboard_table[i]=255;

	//De QL
	for (i=0;i<8;i++) ql_keyboard_table[i]=255;

        menu_symshift.v=0;
        menu_capshift.v=0;
        menu_backspace.v=0;
        menu_tab.v=0;


}


void input_file_keyboard_get_key(void)
{
                        if (input_file_keyboard_pending_next.v==1) {
                                input_file_keyboard_pending_next.v=0;
                                //leer siguiente tecla o enviar pausa (nada)
				if (input_file_keyboard_send_pause.v==1) {
					if (input_file_keyboard_is_pause.v==1) {
						reset_keyboard_ports();
						return;
					}
				}

                                int leidos=fread(&input_file_keyboard_last_key,1,1,ptr_input_file_keyboard);
				if (leidos==0) {
					debug_printf (VERBOSE_INFO,"Read 0 bytes of Input File Keyboard. End of file");
					eject_input_file_keyboard();
					reset_keyboard_ports();
					return;
				}

                        }


                        //Puertos a 0
                        reset_keyboard_ports();
                        //solo habilitar tecla indicada
			if (input_file_keyboard_send_pause.v==0) ascii_to_keyboard_port(input_file_keyboard_last_key);

			//Se envia pausa. Ver si ahora es pausa o tecla
			else {
				if (input_file_keyboard_is_pause.v==0) ascii_to_keyboard_port(input_file_keyboard_last_key);
			}


}


//Devuelve tamanyo minimo de ROM para el tipo de maquina indicado
int get_rom_size(int machine)
{
	//Maquinas 48k
	if (machine<=5) return 16384;

	//Maquinas 128k,+2
	else if (machine<=10) return 32768;

	//Maquinas +2A
	else if (machine<=13) return 65536;

	//Maquina ZX-Uno
  //Aunque rom actual es 8126, la antigua era de 214
	else if (machine==14) return 214;

	//Chloe 140SE
	else if (machine==15) return 32768;

	//Chloe 280SE
	else if (machine==16) return 32768;

	//Timex TS2068
	else if (machine==17) return 24576;

	//Prism
	else if (machine==18) return 320*1024;

	//TBBLUE
	else if (machine==19) return 8192;

        //ZX80
        else if (machine==120) return 4096;

	//ZX81
	else if (machine==121) return 8192;

	//Jupiter Ace
	else if (machine==122) return 8192;

	//Z88
	else if (machine==130) return 131072;

	//CPC 464
	else if (machine==MACHINE_ID_CPC_464) return 32768;

	//SAM
	else if (machine==150) return 32768;

  //QL
  else if (machine==MACHINE_ID_QL_STANDARD) return 16384;

	else cpu_panic("Unknown machine on get_rom_size");


	//Aqui no se llegara nunca. Solo para que no salte warning al compilar
	return 0;
}



//1 si ok
//0 si error
int configfile_read_aux(char *configfile,char *mem)
{

        //Avisar si tamanyo grande
        if (get_file_size(configfile) > (long int)MAX_SIZE_CONFIG_FILE) cpu_panic("Configuration file is larger than maximum size allowed");

	FILE *ptr_configfile;
        ptr_configfile=fopen(configfile,"rb");


        if (!ptr_configfile) {
                printf("Unable to open configuration file %s\n",configfile);
                return 0;
        }

        int leidos=fread(mem,1,MAX_SIZE_CONFIG_FILE,ptr_configfile);

        //Poner un codigo 0 al final
        mem[leidos]=0;


        fclose(ptr_configfile);

        return 1;

}

//Devuelve 1 si ok
//0 si error
int util_get_configfile_name(char *configfile)
{

  #ifndef MINGW
  	char *directorio_home;
  	directorio_home=getenv("HOME");
  	if ( directorio_home==NULL) {
                  //printf("Unable to find $HOME environment variable to open configuration file\n");
  		return 0;
  	}

  	sprintf(configfile,"%s/%s",directorio_home,ZESARUX_CONFIG_FILE);

  #else
  	char *homedrive;
  	char *homepath;
  	homedrive=getenv("HOMEDRIVE");
  	homepath=getenv("HOMEPATH");

  	if (homedrive==NULL || homepath==NULL) {
  		//printf("Unable to find HOMEDRIVE or HOMEPATH environment variables to open configuration file\n");
                  return 0;
          }

  	sprintf(configfile,"%s\\%s\\%s",homedrive,homepath,ZESARUX_CONFIG_FILE);

  #endif

  return 1;

}


//Devuelve 1 si ok
//0 si error
int util_create_sample_configfile(int additional)
{

char configfile[PATH_MAX];
	FILE *ptr_configfile;

if (util_get_configfile_name(configfile)==0)  {
  printf("Unable to find $HOME, or HOMEDRIVE or HOMEPATH environment variables to open configuration file\n");
  return 0;
}

  ptr_configfile=fopen(configfile,"wb");
              if (!ptr_configfile) {
                      printf("Unable to create sample configuration file %s\n",configfile);
                      return 0;
              }

  char *sample_config=
    ";ZEsarUX sample configuration file\n"
    ";\n"
    ";Lines beginning with ; or # are ignored\n"
    ";You can specify here the same options passed on command line, for example:\n"
    ";--verbose 2\n"
    ";Options can be written in quotation marks, for example:\n"
    ";--joystickemulated \"OPQA Space\"\n"
    ";Options can be written on the same line or different lines, like:\n"
    ";--verbose 2 --machine 128k\n"
    ";Or:\n"
    ";--verbose 2\n"
    ";--machine 128k\n"
    ";Or even like this:\n"
    ";--verbose\n"
    ";2\n"
    ";--machine\n"
    ";128k\n"
    "\n"
    ";Run zesarux with --help or --experthelp to see all the options\n"
    "\n"
    ;

  fwrite(sample_config, 1, strlen(sample_config), ptr_configfile);

  if (additional)  {
	char *sample_config_additional="--saveconf-on-exit"
	    "\n"
	    "\n"
	;

	fwrite(sample_config_additional, 1, strlen(sample_config_additional), ptr_configfile);
   }


              fclose(ptr_configfile);

              return 1;

}

/*int util_write_config_add_string(char *config_settings,char *string_add)
{
    int longitud=strlen(string_add)+2; //Agregar texto y salto linea
    sprintf (config_settings,"%s \n",string_add);

    return longitud;
}
*/

#define MAX_CONFIG_SETTING (PATH_MAX*2)

int util_write_config_add_string (char *config_settings, const char * format , ...)
{

	//tamaño del buffer bastante mas grande que el valor constante definido
    char string_add[MAX_CONFIG_SETTING+100];


    va_list args;
    va_start (args, format);
    vsprintf (string_add,format, args);
    va_end (args);

    int longitud=strlen(string_add)+2; //Agregar texto y salto linea
    sprintf (config_settings,"%s \n",string_add);

    return longitud;

}


//Retorna un string con valor X,+X o -X segun button_type
void util_write_config_aux_realjoystick(int button_type, int button, char *texto)
{
  		if (button_type==0) {
  			sprintf(texto,"%d",button);
  		}
  		else if (button_type<0) {
  			sprintf(texto,"-%d",button);
  		}

  		else {
  			sprintf(texto,"+%d",button);
  		}
}

void util_copy_path_delete_last_slash(char *origen, char *destino)
{
    //Quitar barras del final si las hay
    strcpy(destino,origen);
    int i=strlen(destino);
    i--;
    while (i>0 && 
        (destino[i]=='/' || destino[i]=='\\')
        ) {
            destino[i]=0;
            i--;
    }

}

//void util_save_config_normalize_path


//1 si ok
//0 si error
int util_write_configfile(void)
{
  //Sobreescribimos archivos con settings actuales
  debug_printf (VERBOSE_INFO,"Writing configuration file");

  //Creamos archivo de configuracion de ejemplo y agregamos settings aparte

  if (util_create_sample_configfile(0)==0) {
    debug_printf(VERBOSE_ERR,"Cannot write configuration file");
    return 0;
  }

  //Agregamos contenido
  char config_settings[20000]; //Esto es mas que suficiente

  char buffer_temp[MAX_CONFIG_SETTING]; //Para cadenas temporales
  //char buffer_temp2[MAX_CONFIG_SETTING]; //Para cadenas temporales

  int indice_string=0;

  int i;

//Macro para no repetir tantas veces lo mismo
#define ADD_STRING_CONFIG indice_string +=util_write_config_add_string(&config_settings[indice_string]

  //sprintf (buffer_temp,"--zoom %d",zoom_x);
  if (save_configuration_file_on_exit.v)      ADD_STRING_CONFIG,"--saveconf-on-exit");
                                              ADD_STRING_CONFIG,"--zoom %d",zoom_x);
  if (frameskip)                              ADD_STRING_CONFIG,"--frameskip %d",frameskip);


//Solo la agrego si no es 8, que esto es experimental
  if (menu_char_width!=8)		      ADD_STRING_CONFIG,"--menucharwidth %d",menu_char_width);

  if (screen_reduce_075.v)			ADD_STRING_CONFIG,"--reduce-075");



						ADD_STRING_CONFIG,"--reduce-075-offset-x %d",screen_reduce_offset_x);
						ADD_STRING_CONFIG,"--reduce-075-offset-y %d",screen_reduce_offset_y);

  if (screen_watermark_enabled.v)               ADD_STRING_CONFIG,"--enable-watermark");
                                        

                                                ADD_STRING_CONFIG,"--watermark-position %d",screen_watermark_position);


  if (autoframeskip.v==0)                     ADD_STRING_CONFIG,"--disable-autoframeskip");
  if (no_cambio_parametros_maquinas_lentas.v) ADD_STRING_CONFIG,"--nochangeslowparameters");
  if (ventana_fullscreen)                     ADD_STRING_CONFIG,"--fullscreen");
  if (verbose_level)                          ADD_STRING_CONFIG,"--verbose %d",verbose_level);
  if (windows_no_disable_console.v)           ADD_STRING_CONFIG,"--nodisableconsole");
  if (porcentaje_velocidad_emulador!=100)     ADD_STRING_CONFIG,"--cpuspeed %d",porcentaje_velocidad_emulador);
  if (zxuno_deny_turbo_bios_boot.v)           ADD_STRING_CONFIG,"--denyturbozxunoboot");

  if (tbblue_deny_turbo_rom.v)                ADD_STRING_CONFIG,"--denyturbotbbluerom");


  if (tbblue_fast_boot_mode.v)                ADD_STRING_CONFIG,"--tbblue-fast-boot-mode");


  if (cpu_random_r_register.v)                ADD_STRING_CONFIG,"--random-r-register");


  //no uso esto de momento if (tbblue_initial_123b_port>=0)            ADD_STRING_CONFIG,"--tbblue-123b-port %d",tbblue_initial_123b_port);

  if (zx8081_get_standard_ram()!=16)          ADD_STRING_CONFIG,"--zx8081mem %d",zx8081_get_standard_ram());
  if (get_ram_ace()!=19)                      ADD_STRING_CONFIG,"--acemem %d",get_ram_ace() );
  if (mem128_multiplicador!=1)                ADD_STRING_CONFIG,"--128kmem %d",mem128_multiplicador*128);

  if (simulate_screen_zx8081.v)               ADD_STRING_CONFIG,"--videozx8081 %d",umbral_simulate_screen_zx8081);
                                              ADD_STRING_CONFIG,"--ao %s",audio_driver_name);
                                              ADD_STRING_CONFIG,"--vo %s",scr_driver_name);
  if (noautoload.v)                           ADD_STRING_CONFIG,"--noautoload");

  if (fast_autoload.v)			      ADD_STRING_CONFIG,"--fastautoload");


  if (ay_speech_enabled.v==0)                 ADD_STRING_CONFIG,"--disableayspeech");
  if (ay_envelopes_enabled.v==0)              ADD_STRING_CONFIG,"--disableenvelopes");
  if (audio_noreset_audiobuffer_full.v)       ADD_STRING_CONFIG,"--noreset-audiobuffer-full");


  if (silence_detector_setting.v)	      ADD_STRING_CONFIG,"--enable-silencedetector");
  else 					      ADD_STRING_CONFIG,"--disable-silencedetector");


  if (border_enabled.v==0)                    ADD_STRING_CONFIG,"--disableborder");
  if (mouse_pointer_shown.v==0)               ADD_STRING_CONFIG,"--hidemousepointer");
  if (mouse_menu_disabled.v)                  ADD_STRING_CONFIG,"--disablemenumouse");

  if (kempston_mouse_emulation.v)             ADD_STRING_CONFIG,"--enablekempstonmouse");

                                              ADD_STRING_CONFIG,"--kempstonmouse-sens %d",kempston_mouse_factor_sensibilidad);


  if (core_spectrum_uses_reduced.v)           ADD_STRING_CONFIG,"--spectrum-reduced-core");
  else                                        ADD_STRING_CONFIG,"--no-spectrum-reduced-core");


  if (menu_footer==0)                         ADD_STRING_CONFIG,"--disablefooter");
  if (menu_multitarea==0)                     ADD_STRING_CONFIG,"--disablemultitaskmenu");


  //if (screen_bw_no_multitask_menu.v==0)       ADD_STRING_CONFIG,"--disablebw-no-multitask");

  get_machine_config_name_by_number(buffer_temp,current_machine_type);
  if (buffer_temp[0]!=0) {
                                              ADD_STRING_CONFIG,"--machine %s",buffer_temp);
  }
  if (video_fast_mode_emulation.v)            ADD_STRING_CONFIG,"--videofastblack");
  if (zx8081_vsync_sound.v)                   ADD_STRING_CONFIG,"--zx8081vsyncsound");
  if (ram_in_8192.v)                          ADD_STRING_CONFIG,"--zx8081ram8K2000");
  if (ram_in_32768.v)                         ADD_STRING_CONFIG,"--zx8081ram16K8000");
  if (ram_in_49152.v)                         ADD_STRING_CONFIG,"--zx8081ram16KC000");
  if (autodetect_wrx.v)                       ADD_STRING_CONFIG,"--autodetectwrx");
  if (wrx_present.v)                          ADD_STRING_CONFIG,"--wrx");
                                              ADD_STRING_CONFIG,"--vsync-minimum-length %d",minimo_duracion_vsync);
  if (chroma81.v)                             ADD_STRING_CONFIG,"--chroma81");


  if (quickload_file[0]!=0) {
	util_get_dir(quickload_file,buffer_temp);
 	ADD_STRING_CONFIG,"--smartloadpath \"%s\"",buffer_temp);
  }

  if (snapshot_autosave_interval_quicksave_directory[0]!=0) {
        //Rutas que son directorios, llamar a util_copy_path_delete_last_slash 
        //Rutas que apuntan a archivos (o directorios por linea de comandos pero que en menu almacenan archivos), llamar a util_get_dir
        //printf ("dir quicksave: %s\n",snapshot_autosave_interval_quicksave_directory);
        util_copy_path_delete_last_slash(snapshot_autosave_interval_quicksave_directory,buffer_temp);
        //printf ("dir quicksave final: %s\n",buffer_temp);

 	ADD_STRING_CONFIG,"--quicksavepath \"%s\"",buffer_temp);
  }
  

  if (binary_file_load[0]!=0) {
        //printf ("dir binary_file_load: %s\n",binary_file_load);
	util_get_dir(binary_file_load,buffer_temp);
        //printf ("dir binary_file_load final: %s\n",buffer_temp);
 	ADD_STRING_CONFIG,"--loadbinarypath \"%s\"",buffer_temp);
  }

  if (binary_file_save[0]!=0) {
        util_get_dir(binary_file_save,buffer_temp);
 	ADD_STRING_CONFIG,"--savebinarypath \"%s\"",buffer_temp);
  }  

  if (zxuno_flash_spi_name[0])                ADD_STRING_CONFIG,"--zxunospifile \"%s\"",zxuno_flash_spi_name);
  if (zxuno_flash_persistent_writes.v)     ADD_STRING_CONFIG,"--zxunospi-persistent-writes");


  if (zxuno_flash_write_protection.v)         ADD_STRING_CONFIG,"--zxunospi-write-protection");

  //TODO printerbitmapfile
  //TODO printertextfile
  //TODO redefinekey

  if (recreated_zx_keyboard_support.v)	      ADD_STRING_CONFIG,"--recreatedzx");

  if (autoload_snapshot_on_start.v)           ADD_STRING_CONFIG,"--autoloadsnap");
  if (autosave_snapshot_on_exit.v)            ADD_STRING_CONFIG,"--autosavesnap");
  if (autosave_snapshot_path_buffer[0]!=0) {
    //Quitar barra del final si la hay
    //strcpy(buffer_temp,autosave_snapshot_path_buffer);
    //i=strlen(buffer_temp);
    //if (i>1 && buffer_temp[i-1]=='/') buffer_temp[i-1]=0;
    util_copy_path_delete_last_slash(autosave_snapshot_path_buffer,buffer_temp);
        ADD_STRING_CONFIG,"--autosnappath \"%s\"",buffer_temp);
  }


  if (emulator_tmpdir_set_by_user[0]!=0) {
	//Quitar barra del final si la hay
	util_copy_path_delete_last_slash(emulator_tmpdir_set_by_user,buffer_temp);
 	ADD_STRING_CONFIG,"--tempdir \"%s\"",buffer_temp);
  }

  if (sna_setting_no_change_machine.v)      ADD_STRING_CONFIG,"--sna-no-change-machine");


  if (texto_artistico.v==0)                   ADD_STRING_CONFIG,"--disablearttext");
                                              ADD_STRING_CONFIG,"--arttextthresold %d",umbral_arttext);
  if (chardetect_printchar_enabled.v)         ADD_STRING_CONFIG,"--enableprintchartrap");
  if (stdout_simpletext_automatic_redraw.v)   ADD_STRING_CONFIG,"--autoredrawstdout");
  if (screen_text_accept_ansi)                ADD_STRING_CONFIG,"--sendansi");

                                              ADD_STRING_CONFIG,"--textfps %d",50/scrstdout_simpletext_refresh_factor);


  if (chardetect_line_width!=32)              ADD_STRING_CONFIG,"--linewidth %d",chardetect_line_width);
  if (chardetect_detect_char_enabled.v && trap_char_detection_routine_number==TRAP_CHAR_DETECTION_ROUTINE_AUTOMATIC)
                                              ADD_STRING_CONFIG,"--automaticdetectchar");
  if (chardetect_second_trap_char_dir)        ADD_STRING_CONFIG,"--secondtrapchar %d",chardetect_second_trap_char_dir);
  if (chardetect_third_trap_char_dir)         ADD_STRING_CONFIG,"--thirdtrapchar %d",chardetect_third_trap_char_dir);
  if (chardetect_line_width_wait_space.v)     ADD_STRING_CONFIG,"--linewidthwaitspace");
  if (chardetect_second_trap_sum32.v)         ADD_STRING_CONFIG,"--secondtrapsum32");
  if (textspeech_filter_program!=NULL)        ADD_STRING_CONFIG,"--textspeechprogram \"%s\"",textspeech_filter_program);
  if (textspeech_stop_filter_program!=NULL)   ADD_STRING_CONFIG,"--textspeechstopprogram \"%s\"",textspeech_stop_filter_program);
  if (textspeech_filter_program_wait.v)       ADD_STRING_CONFIG,"--textspeechwait");
  if (textspeech_also_send_menu.v)            ADD_STRING_CONFIG,"--textspeechmenu");
  if (textspeech_timeout_no_enter)            ADD_STRING_CONFIG,"--textspeechtimeout %d",textspeech_timeout_no_enter);
                                              ADD_STRING_CONFIG,"--tool-sox-path \"%s\"",external_tool_sox);
                                              ADD_STRING_CONFIG,"--tool-unzip-path \"%s\"",external_tool_unzip);
                                              ADD_STRING_CONFIG,"--tool-gunzip-path \"%s\"",external_tool_gunzip);
                                              ADD_STRING_CONFIG,"--tool-tar-path \"%s\"",external_tool_tar);
                                              ADD_STRING_CONFIG,"--tool-unrar-path \"%s\"",external_tool_unrar);
  if (mmc_file_name[0]!=0)                    ADD_STRING_CONFIG,"--mmc-file \"%s\"",mmc_file_name);
  if (mmc_enabled.v)                          ADD_STRING_CONFIG,"--enable-mmc");
  if (divmmc_mmc_ports_enabled.v)             ADD_STRING_CONFIG,"--enable-divmmc-ports");

  if (mmc_write_protection.v)		      ADD_STRING_CONFIG,"--mmc-write-protection");
  if (mmc_persistent_writes.v==0)	      ADD_STRING_CONFIG,"--mmc-no-persistent-writes");

  //Los settings de  mmc paging no guardarlo si maquina es tbblue, pues acaba activando el divmmc paging en tbblue y entonces esto provoca que no arranque la tbblue rom
  if (!MACHINE_IS_TBBLUE) {
    if (divmmc_diviface_enabled.v)            ADD_STRING_CONFIG,"--enable-divmmc-paging");
    if (divmmc_mmc_ports_enabled.v && divmmc_diviface_enabled.v)
                                              ADD_STRING_CONFIG,"--enable-divmmc");
  }

  if (divmmc_rom_name[0]!=0)                  ADD_STRING_CONFIG,"--divmmc-rom \"%s\"",divmmc_rom_name);




  if (ide_file_name[0]!=0)                    ADD_STRING_CONFIG,"--ide-file \"%s\"",ide_file_name);
  if (ide_enabled.v)                          ADD_STRING_CONFIG,"--enable-ide");
  if (ide_write_protection.v)		      ADD_STRING_CONFIG,"--ide-write-protection");
  if (ide_persistent_writes.v==0)	      ADD_STRING_CONFIG,"--ide-no-persistent-writes");
  if (divide_ide_ports_enabled.v)             ADD_STRING_CONFIG,"--enable-divide-ports");
  if (divide_diviface_enabled.v)              ADD_STRING_CONFIG,"--enable-divide-paging");

  if (divide_ide_ports_enabled.v && divide_diviface_enabled.v)
                                              ADD_STRING_CONFIG,"--enable-divide");
  if (divide_rom_name[0]!=0)                  ADD_STRING_CONFIG,"--divide-rom \"%s\"",divide_rom_name);


  if (zxmmc_emulation.v)                      ADD_STRING_CONFIG,"--enable-zxmmc");
  if (eight_bit_simple_ide_enabled.v)         ADD_STRING_CONFIG,"--enable-8bit-ide");
  if (zxpand_enabled.v)                       ADD_STRING_CONFIG,"--enable-zxpand");
  if (zxpand_root_dir[0]!=0)                  ADD_STRING_CONFIG,"--zxpand-root-dir \"%s\"",zxpand_root_dir);

  if (esxdos_handler_enabled.v)               ADD_STRING_CONFIG,"--enable-esxdos-handler");
  if (esxdos_handler_root_dir[0]!=0)          ADD_STRING_CONFIG,"--esxdos-root-dir \"%s\"",esxdos_handler_root_dir);

  if (ql_microdrive_floppy_emulation)         ADD_STRING_CONFIG,"--enable-ql-mdv-flp");
  if (ql_mdv1_root_dir[0]!=0)                 ADD_STRING_CONFIG,"--ql-mdv1-root-dir \"%s\"",ql_mdv1_root_dir);
  if (ql_mdv2_root_dir[0]!=0)                 ADD_STRING_CONFIG,"--ql-mdv2-root-dir \"%s\"",ql_mdv2_root_dir);
  if (ql_flp1_root_dir[0]!=0)                 ADD_STRING_CONFIG,"--ql-flp1-root-dir \"%s\"",ql_flp1_root_dir);



  if (dandanator_rom_file_name[0]!=0)         ADD_STRING_CONFIG,"--dandanator-rom \"%s\"",dandanator_rom_file_name);
  if (dandanator_enabled.v)                   ADD_STRING_CONFIG,"--enable-dandanator");
  if (kartusho_rom_file_name[0]!=0)         ADD_STRING_CONFIG,"--kartusho-rom \"%s\"",kartusho_rom_file_name);
  if (kartusho_enabled.v)                   ADD_STRING_CONFIG,"--enable-kartusho");

  if (betadisk_enabled.v)                   ADD_STRING_CONFIG,"--enable-betadisk");

  if (trd_file_name[0]!=0)                    ADD_STRING_CONFIG,"--trd-file \"%s\"",trd_file_name);
  if (trd_enabled.v)                          ADD_STRING_CONFIG,"--enable-trd");
  if (trd_write_protection.v)		      ADD_STRING_CONFIG,"--trd-write-protection");
  if (trd_persistent_writes.v==0)	      ADD_STRING_CONFIG,"--trd-no-persistent-writes");


 
  if (dskplusthree_file_name[0]!=0)           ADD_STRING_CONFIG,"--dsk-file \"%s\"",dskplusthree_file_name);		
  if (dskplusthree_emulation.v)               ADD_STRING_CONFIG,"--enable-dsk");
  if (dskplusthree_write_protection.v)	      ADD_STRING_CONFIG,"--dsk-write-protection");
  if (dskplusthree_persistent_writes.v==0)    ADD_STRING_CONFIG,"--dsk-no-persistent-writes");

  if (superupgrade_rom_file_name[0]!=0)       ADD_STRING_CONFIG,"--superupgrade-flash \"%s\"",superupgrade_rom_file_name);
  if (superupgrade_enabled.v)                 ADD_STRING_CONFIG,"--enable-superupgrade");


                                              ADD_STRING_CONFIG,"--showfiredbreakpoint %d",debug_show_fired_breakpoints_type);


  if (debug_breakpoints_enabled.v)            ADD_STRING_CONFIG,"--enable-breakpoints");

  if (debug_shows_invalid_opcode.v)           ADD_STRING_CONFIG,"--show-invalid-opcode");


  if (debug_breakpoints_cond_behaviour.v==0)  ADD_STRING_CONFIG,"--brkp-always");


  if (debug_settings_show_screen.v)           ADD_STRING_CONFIG,"--show-display-debug");


  if (menu_debug_registers_if_showscan.v)     ADD_STRING_CONFIG,"--show-electron-debug");
 



  for (i=0;i<MAX_BREAKPOINTS_CONDITIONS;i++) {
		if (debug_breakpoints_conditions_array[i][0]!=0)
                                              ADD_STRING_CONFIG,"--set-breakpoint %d \"%s\"",i+1,debug_breakpoints_conditions_array[i]);
	}

  for (i=0;i<MAX_BREAKPOINTS_CONDITIONS;i++) {
    if (debug_breakpoints_actions_array[i][0]!=0)
                                              ADD_STRING_CONFIG,"--set-breakpointaction %d \"%s\"",i+1,debug_breakpoints_actions_array[i]);
  }

  for (i=0;i<65536;i++) {
		if (mem_breakpoint_array[i]!=0)
                                              ADD_STRING_CONFIG,"--set-mem-breakpoint %04XH %d",i,mem_breakpoint_array[i]);
	}  

  if (hardware_debug_port.v)                  ADD_STRING_CONFIG,"--hardware-debug-ports");

  if (autoselect_snaptape_options.v==0)       ADD_STRING_CONFIG,"--noautoselectfileopt");
  if (screen_show_splash_texts.v==0)          ADD_STRING_CONFIG,"--nosplash");
  if (screen_show_cpu_usage.v)                ADD_STRING_CONFIG,"--cpu-usage");
  if (opcion_no_splash.v)                     ADD_STRING_CONFIG,"--nowelcomemessage");

  if (menu_hide_vertical_percentaje_bar.v) ADD_STRING_CONFIG,"--hide-menu-percentage-bar");

  if (rainbow_enabled.v)                      ADD_STRING_CONFIG,"--realvideo");

  if (autodetect_rainbow.v==0)                ADD_STRING_CONFIG,"--no-detect-realvideo");

  //if (tsconf_si_render_spritetile_rapido.v)   ADD_STRING_CONFIG,"--tsconf-fast-render");


  if (gigascreen_enabled.v)                   ADD_STRING_CONFIG,"--enablegigascreen");
  if (video_interlaced_mode.v)                ADD_STRING_CONFIG,"--enableinterlaced");

  if (ulaplus_presente.v)                     ADD_STRING_CONFIG,"--enableulaplus");
  if (spectra_enabled.v)                      ADD_STRING_CONFIG,"--enablespectra");
  if (timex_video_emulation.v)                ADD_STRING_CONFIG,"--enabletimexvideo");
  if (timex_mode_512192_real.v==0)	      ADD_STRING_CONFIG,"--disablerealtimex512");


  if (spritechip_enabled.v)                   ADD_STRING_CONFIG,"--enablezgx");
  if (beeper_enabled.v==0)                    ADD_STRING_CONFIG,"--disablebeeper");
  if (beeper_real_enabled==0)                 ADD_STRING_CONFIG,"--disablerealbeeper");
  if (ay_retorna_numero_chips()>1)            ADD_STRING_CONFIG,"--totalaychips %d",ay_retorna_numero_chips() );

  if (ay3_stereo_mode>0)                      ADD_STRING_CONFIG,"--ay-stereo-mode %d",ay3_stereo_mode);

                                              ADD_STRING_CONFIG,"--ay-stereo-channel A %d",ay3_custom_stereo_A);
                                              ADD_STRING_CONFIG,"--ay-stereo-channel B %d",ay3_custom_stereo_B);
                                              ADD_STRING_CONFIG,"--ay-stereo-channel C %d",ay3_custom_stereo_C);

  if (audiodac_enabled.v)                     ADD_STRING_CONFIG,"--enableaudiodac");
                                              ADD_STRING_CONFIG,"--audiodactype \"%s\"",audiodac_types[audiodac_selected_type].name);

  if (snow_effect_enabled.v)                  ADD_STRING_CONFIG,"--snoweffect");
  if (audiovolume!=100)                       ADD_STRING_CONFIG,"--audiovolume %d",audiovolume);
  if (ay_player_exit_emulator_when_finish.v)  ADD_STRING_CONFIG,"--ayplayer-end-exit");
  if (ay_player_repeat_file.v==0)             ADD_STRING_CONFIG,"--ayplayer-end-no-repeat");
  if (ay_player_limit_infinite_tracks!=0)     ADD_STRING_CONFIG,"--ayplayer-inf-length %d",ay_player_limit_infinite_tracks/50);
  if (ay_player_limit_any_track!=0)           ADD_STRING_CONFIG,"--ayplayer-any-length %d",ay_player_limit_any_track/50);
  if (ay_player_cpc_mode.v)                   ADD_STRING_CONFIG,"--ayplayer-cpc");

  //Este setting lo permitimos siempre, aunque no se haya compilado driver sdl, pues es una variable global, aunque no se verá en la ayuda,
  if (sdl_raw_keyboard_read.v)                ADD_STRING_CONFIG,"--sdlrawkeyboard");
  
  if (tape_loading_simulate.v)                ADD_STRING_CONFIG,"--simulaterealload");
  if (tape_loading_simulate_fast.v)           ADD_STRING_CONFIG,"--simulaterealloadfast");
  if (screen_gray_mode&1)                     ADD_STRING_CONFIG,"--blue");
  if (screen_gray_mode&2)                     ADD_STRING_CONFIG,"--green");
  if (screen_gray_mode&4)                     ADD_STRING_CONFIG,"--red");
  if (inverse_video.v)                        ADD_STRING_CONFIG,"--inversevideo");
  if (spectrum_1648_use_real_palette.v)       ADD_STRING_CONFIG,"--realpalette");

  if (tooltip_enabled.v==0)                   ADD_STRING_CONFIG,"--disabletooltips");


  if (menu_limit_menu_open.v)                 ADD_STRING_CONFIG,"--limitopenmenu");

  if (menu_filesel_hide_dirs.v)         ADD_STRING_CONFIG,"--hide-dirs");

  if (menu_desactivado.v)                     ADD_STRING_CONFIG,"--disablemenu");

  if (menu_force_writing_inverse_color.v)     ADD_STRING_CONFIG,"--forcevisiblehotkeys");
  if (force_confirm_yes.v)                    ADD_STRING_CONFIG,"--forceconfirmyes");
                                              ADD_STRING_CONFIG,"--gui-style \"%s\"",definiciones_estilos_gui[estilo_gui_activo].nombre_estilo);

					      ADD_STRING_CONFIG,"--last-version \"%s\"",BUILDNUMBER);

  if (do_no_show_changelog_when_update.v)     ADD_STRING_CONFIG,"--no-show-changelog");


  if (parameter_disablebetawarning[0])        ADD_STRING_CONFIG,"--disablebetawarning \"%s\"",parameter_disablebetawarning);


  for (i=0;i<MAX_F_FUNCTIONS_KEYS;i++) {
    enum defined_f_function_ids accion=defined_f_functions_keys_array[i];
    if (accion!=F_FUNCION_DEFAULT) {
                                              ADD_STRING_CONFIG,"--def-f-function F%d \"%s\"",i+1,defined_f_functions_array[accion].texto_funcion);
    }
  }


  if (input_file_keyboard_name!=NULL && input_file_keyboard_inserted.v)         ADD_STRING_CONFIG,"--keyboardspoolfile \"%s\"",input_file_keyboard_name);

  if (input_file_keyboard_playing.v)           ADD_STRING_CONFIG,"--keyboardspoolfile-play");


                                              ADD_STRING_CONFIG,"--joystickemulated \"%s\"",joystick_texto[joystick_emulation]);

  if (remote_protocol_enabled.v)              ADD_STRING_CONFIG,"--enable-remoteprotocol");
                                              ADD_STRING_CONFIG,"--remoteprotocol-port %d",remote_protocol_port);

  if (realjoystick_disabled.v==1)              ADD_STRING_CONFIG,"--disablerealjoystick");


  //real joystick buttons to events
  for (i=0;i<MAX_EVENTS_JOYSTICK;i++) {
  	if (realjoystick_events_array[i].asignado.v) {
  		char texto_button[20];
  		int button_type;
  		button_type=realjoystick_events_array[i].button_type;

  		util_write_config_aux_realjoystick(button_type, realjoystick_events_array[i].button, texto_button);

  		ADD_STRING_CONFIG,"--joystickevent %s %s",texto_button,realjoystick_event_names[i]);
  	}
  }

  //text osd keyboard
  for (i=0;i<osd_adv_kbd_defined;i++) {
          //Truco para poder poner " en el texto. Con barra invertida
          if (!strcmp(osd_adv_kbd_list[i],"\"")) ADD_STRING_CONFIG,"--text-keyboard-add \\");
	else ADD_STRING_CONFIG,"--text-keyboard-add \"%s\"",osd_adv_kbd_list[i]);
  }

                                        ADD_STRING_CONFIG,"--text-keyboard-length %d",adventure_keyboard_key_length);
		


  //real joystick buttons to keys
  for (i=0;i<MAX_KEYS_JOYSTICK;i++) {
  	if (realjoystick_keys_array[i].asignado.v) {
  		char texto_button[20];
  		int button_type;
  		z80_byte caracter;
  		caracter=realjoystick_keys_array[i].caracter;
  		button_type=realjoystick_keys_array[i].button_type;

		util_write_config_aux_realjoystick(button_type, realjoystick_keys_array[i].button, texto_button);	

  		ADD_STRING_CONFIG,"--joystickkeybt %s %d",texto_button,caracter);
  	}
  }

  //joystickkeyev no lo estoy autoguardando, esto es mas indicado para archivos .config
  if (realjoystick_clear_keys_on_smartload.v) ADD_STRING_CONFIG,"--clearkeylistonsmart");
  if (quickexit.v)                            ADD_STRING_CONFIG,"--quickexit");

  //Guardar si hay algo que Guardar
  if (indice_string) {
    char configfile[PATH_MAX];
  	 FILE *ptr_configfile;

     if (util_get_configfile_name(configfile)==0)  {
       debug_printf(VERBOSE_ERR,"Unable to find $HOME, or HOMEDRIVE or HOMEPATH environment variables to open configuration file");
       return 0;
     }

     ptr_configfile=fopen(configfile,"a+");
     if (!ptr_configfile) {
                        debug_printf(VERBOSE_ERR,"Cannot write configuration file %s",configfile);
                        return 0;
      }

    fwrite(config_settings, 1, strlen(config_settings), ptr_configfile);


      fclose(ptr_configfile);
    }
    return 1;

}

//Leer archivo de configuracion en buffer
//Devuelve 0 si no existe
//Max 64 kb
int configfile_read(char *mem)
{



	char configfile[PATH_MAX];

  if (util_get_configfile_name(configfile)==0)  {
    printf("Unable to find $HOME, or HOMEDRIVE or HOMEPATH environment variables to open configuration file\n");
    return 0;
  }


	//Ver si archivo existe

	if (!si_existe_archivo(configfile)) {
		printf("Configuration file %s not found\nCreating a new one\n",configfile);

    if (util_create_sample_configfile(1)==0) return 0;

	}

	return configfile_read_aux(configfile,mem);

}

//Retorna puntero a caracter leido que hace de final (sea final de linea o de archivo)
char *configfile_end_line(char *m)
{

	while ( (*m!='\n') && (*m!=0) ) {
		m++;
	}

	return m;
}

//Retorna puntero a caracter leido que hace de final (sea final de linea o de archivo)
char *configfile_next_field(char *m,int comillas_iniciales)
{

	//Hasta final de linea, o final de texto, o espacio (y no comilla previa), o comillas
       while (
		(*m!='\n') &&
		(*m!='\r') &&
		(*m!=0) &&
		(*m!='"')
	 ) {

		if (comillas_iniciales==0 && *m==' ') break;

                //Dejamos que se puedan escapar unas " si van precedidas de barra invertida
                //if ( (*m)=='\\') {
                //        if (*(m+1)=='"') m++;
                //}

                m++;
        }

        return m;
}


void configfile_parse_lines(char *mem,char *p_argv[],int *p_argc)
{
	char caracter;
	int argumentos=*p_argc;

	//para compatibilidad con rutinas de leer parametros, argv[0] no lo usamos
	p_argv[0]="";
	argumentos++;

	do {
		caracter=*mem;

		//Fin
		if (caracter==0) {
			*p_argc=argumentos;
			return;
		}

		//Comentarios
		if (caracter==';' || caracter=='#') {
			mem=configfile_end_line(mem);
			if (*mem) mem++;
		}

		//Estamos en salto de linea o espacio o retorno de carro, saltamos
		else if (caracter=='\n' || caracter==' ' || caracter=='\r' ) mem++;

		else {

			int comillas_iniciales=0;

			//Parametro correcto
			//Ver si comillas iniciales
			if (caracter=='"') {
				comillas_iniciales=1;
				mem++;
				//printf ("comillas iniciales en %s\n",mem);
			}


			p_argv[argumentos]=mem;
			argumentos++;
                        if (argumentos>MAX_PARAMETERS_CONFIG_FILE) {
                                cpu_panic("Maximum parameters in config file reached");
                        }

			mem=configfile_next_field(mem,comillas_iniciales);

			//Poner codigo 0 al final
			if (*mem) {
				*mem=0;
				mem++;
			}
		}

	} while (1);

}


char *configfile_argv[MAX_PARAMETERS_CONFIG_FILE];
int configfile_argc=0;

//Convertir archivo de configuracion leido en argv

//Ignorar lineas que empiecen con ; o #
//Separar lineas y espacios en diferentes parametros; tener en cuenta texto que haya entre comillas, no separar por espacios
//Cada final de parametro tendra codigo 0
void configfile_parse(void)
{
	char *mem_config=malloc(MAX_SIZE_CONFIG_FILE+1);
        //printf ("mem_config init: %p\n",mem_config);

        if (mem_config==NULL) {
		cpu_panic("Unable to allocate memory for configuration file");
	}

	if (configfile_read(mem_config)==0) {
		//No hay archivo de configuracion. Parametros vacios
		configfile_argv[0]="";
		configfile_argc=1;
                //free(mem_config);
		return;
	}


	configfile_parse_lines(mem_config,&configfile_argv[0],&configfile_argc);

	//Mostrar cada parametro. Ignoramos el primero (numero 0) pues es vacio
	if (debug_parse_config_file.v) {
		int i;
		for (i=1;i<configfile_argc;i++) {
			printf ("Debug: Configfile, parameter %d = [%s]\n",i,configfile_argv[i]);
		}
	}

        //TODO: Este free da segmentation fault. Por que??? Revisar los otros free de leer archivos de config que estan comentados tambien
        //printf ("mem_config in free: %p\n",mem_config);
        //free(mem_config);
	return;
}

char *custom_config_mem_pointer=NULL;

//Parsear archivo indicado de configuracion
void configfile_parse_custom_file(char *archivo)
{

	//Ver si hay que asignar memoria
	//Debe quedar esta memoria asignada al salir de este procedimiento, pues lo usa en parse_customfile_options
	if (custom_config_mem_pointer==NULL) {
		debug_printf (VERBOSE_DEBUG,"Allocating memory to read custom config file");
		custom_config_mem_pointer=malloc(MAX_SIZE_CONFIG_FILE+1);
	}

	else {
		debug_printf (VERBOSE_DEBUG,"No need to allocate memory to read custom config file, we allocated it before");
	}


        if (custom_config_mem_pointer==NULL) {
                cpu_panic("Unable to allocate memory for configuration file");
        }

	//Valor inicial
	configfile_argc=0;


        if (configfile_read_aux(archivo,custom_config_mem_pointer)==0) {
                //No hay archivo de configuracion. Parametros vacios
                configfile_argv[0]="";
                configfile_argc=1;
                //free(custom_config_mem_pointer);
                return;
        }


        configfile_parse_lines(custom_config_mem_pointer,&configfile_argv[0],&configfile_argc);

        //Mostrar cada parametro. Ignoramos el primero (numero 0) pues es vacio
        if (debug_parse_config_file.v) {
                int i;
                for (i=1;i<configfile_argc;i++) {
                        printf ("Debug: Custom Configfile, parameter %d = [%s]\n",i,configfile_argv[i]);
                }
        }

        //free(custom_config_mem_pointer);
        return;
}


//Parsear archivo de configuracion indicado, usado en insertar cinta, snap, etc
void parse_custom_file_config(char *archivo)
{

	configfile_parse_custom_file(archivo);
	argc=configfile_argc;
	argv=configfile_argv;
	puntero_parametro=0;

	parse_customfile_options();
}


tecla_redefinida lista_teclas_redefinidas[MAX_TECLAS_REDEFINIDAS];


void clear_lista_teclas_redefinidas(void)
{
	int i;
	for (i=0;i<MAX_TECLAS_REDEFINIDAS;i++) {
		lista_teclas_redefinidas[i].tecla_original=0;
	}
	debug_printf (VERBOSE_DEBUG,"Cleared redefined keys table");
}

//Poder redefinir una tecla. Retorna 0 si no hay redefinicion. Retorna 1 si lo hay
z80_byte util_redefinir_tecla(z80_byte tecla)
{

	int i;
	z80_byte tecla_original,tecla_redefinida;
	for (i=0;i<MAX_TECLAS_REDEFINIDAS;i++) {
		tecla_original=lista_teclas_redefinidas[i].tecla_original;
		tecla_redefinida=lista_teclas_redefinidas[i].tecla_redefinida;
		if (tecla_original) {
			if (tecla_original==tecla) return tecla_redefinida;
		}
	}

	return 0;


}

//Agregar una tecla a la lista de redefinicion. Retorna 0 si ok, 1 si error (lista llena por ejemplo)
int util_add_redefinir_tecla(z80_byte p_tecla_original, z80_byte p_tecla_redefinida)
{

	int i;
        z80_byte tecla_original;
	//z80_byte tecla_redefinida;

	for (i=0;i<MAX_TECLAS_REDEFINIDAS;i++) {
		tecla_original=lista_teclas_redefinidas[i].tecla_original;
                //tecla_redefinida=lista_teclas_redefinidas[i].tecla_redefinida;
		if (tecla_original==0) {
			lista_teclas_redefinidas[i].tecla_original=p_tecla_original;
			lista_teclas_redefinidas[i].tecla_redefinida=p_tecla_redefinida;
			debug_printf (VERBOSE_DEBUG,"Added key %d to be %d",p_tecla_original,p_tecla_redefinida);
			return 0;
		}
	}

	debug_printf (VERBOSE_ERR,"Maximum redefined keys (%d)",MAX_TECLAS_REDEFINIDAS);
	return 1;
}


void convert_numeros_letras_puerto_teclado_continue(z80_byte tecla,int pressrelease)
{


       //Si teclado recreated, y menu cerrado
       /*if (!menu_abierto && recreated_zx_keyboard_support.v) {

               enum util_teclas tecla_final;
               int pressrelease_final;
               //printf ("Entrada a convertir tecla recreated desde convert_numeros_letras_puerto_teclado_continue\n");
               recreated_zx_spectrum_keyboard_convert(tecla, &tecla_final, &pressrelease_final);
               if (tecla_final) {
               		//printf ("redefinicion de tecla. antes: %d despues: %d\n",tecla,tecla_final);
                       tecla=tecla_final;
                       pressrelease=pressrelease_final;

			if (tecla==UTIL_KEY_CAPS_SHIFT) {
				//printf ("Pulsada caps shift (%d)\n",UTIL_KEY_CAPS_SHIFT);
				util_set_reset_key(tecla,pressrelease);
				return;
			}

                       //Si sigue estando entre a-z y 0-9 enviar tal cual. Si no, llamar a util_set_reset_key
                       if (
                       	(tecla>='a' && tecla<='z') ||
                       	(tecla>='0' && tecla<='9') 
                       	)
                       {
                       	//Nada. Prefiero escribir la condicion asi que no poner un negado
                       	//printf("Tecla final es entre az y 09. generar puerto spectrum\n");
                       }


                       else {
                       	//util_set_reset_key(tecla,pressrelease);
                       	//printf ("Enviar a util_set_reset_key_convert_recreated_yesno sin convertir\n");
                       	util_set_reset_key_convert_recreated_yesno(tecla,pressrelease,0);
                       	return;
                       }
               }
       }*/

       convert_numeros_letras_puerto_teclado_continue_after_recreated(tecla,pressrelease);
}



void convert_numeros_letras_puerto_teclado_continue_after_recreated(z80_byte tecla,int pressrelease)
{

	//Redefinicion de teclas
	z80_byte tecla_redefinida;
	tecla_redefinida=util_redefinir_tecla(tecla);
	if (tecla_redefinida) tecla=tecla_redefinida;

  if (tecla>='a' && tecla<='z') {
      int indice=tecla-'a';

      z80_byte *puerto;
      puerto=tabla_teclado_letras[indice].puerto;

      z80_byte mascara;
      mascara=tabla_teclado_letras[indice].mascara;

                                        //printf ("asignamos tecla mediante array indice=%d puerto=%x mascara=%d - ",indice,puerto,mascara);

      if (pressrelease) *puerto &=255-mascara;
      else *puerto |=mascara;


					if (MACHINE_IS_Z88) {
						puerto=z88_tabla_teclado_letras[indice].puerto;
						mascara=z88_tabla_teclado_letras[indice].mascara;

						if (pressrelease) *puerto &=255-mascara;
						else *puerto |=mascara;
					}

					if (MACHINE_IS_CPC) {
						puerto=cpc_tabla_teclado_letras[indice].puerto;
                                                mascara=cpc_tabla_teclado_letras[indice].mascara;

                                                if (pressrelease) *puerto &=255-mascara;
                                                else *puerto |=mascara;
          }

          if (MACHINE_IS_QL) {
            puerto=ql_tabla_teclado_letras[indice].puerto;
            mascara=ql_tabla_teclado_letras[indice].mascara;

            if (pressrelease) *puerto &=255-mascara;
            else *puerto |=mascara;
          }

          if (MACHINE_IS_MK14) {
            //Solo algunas letras
            /*
            mk14_keystatus

            Matriz teclado
             128 64  32  16

              0  8   -   A	mk14_keystatus[0]
            	1  9   -   B	mk14_keystatus[1]
            	2  -   GO  C	mk14_keystatus[2]
            	3  -   MEM D	mk14_keystatus[3]
            	4  -   ABR -	mk14_keystatus[4]
            	5  -   -   -	mk14_keystatus[5]
            	6  -   -   E	mk14_keystatus[6]
            	7  -   TER F	mk14_keystatus[7]

            GO: mapeado a G
            MEM: mapeado a M
            ABR: mapeado a Z
            TERM: mapeado a T
            */

            mascara=255;

            if (tecla=='a') {
              puerto=&mk14_keystatus[0];
              mascara=16;
            }

            if (tecla=='b') {
              puerto=&mk14_keystatus[1];
              mascara=16;
            }

            if (tecla=='c') {
              puerto=&mk14_keystatus[2];
              mascara=16;
            }

            if (tecla=='d') {
              puerto=&mk14_keystatus[3];
              mascara=16;
            }

            if (tecla=='e') {
              puerto=&mk14_keystatus[6];
              mascara=16;
            }

            if (tecla=='f') {
              puerto=&mk14_keystatus[7];
              mascara=16;
            }

            if (tecla=='g') {
              puerto=&mk14_keystatus[2];
              mascara=32;
            }

            if (tecla=='m') {
              puerto=&mk14_keystatus[3];
              mascara=32;
            }

            if (tecla=='z') {
              puerto=&mk14_keystatus[4];
              mascara=32;
            }

            if (tecla=='t') {
              puerto=&mk14_keystatus[7];
              mascara=32;
            }


            if (mascara!=255) {
              if (pressrelease) *puerto &=255-mascara;
              else *puerto |=mascara;
            }
          }


    }

  if (tecla>='0' && tecla<='9') {
  int indice=tecla-'0';

  z80_byte *puerto;
  puerto=tabla_teclado_numeros[indice].puerto;

  z80_byte mascara;
  mascara=tabla_teclado_numeros[indice].mascara;

                                        //printf ("asignamos tecla mediante array indice=%d puerto=%x mascara=%d - ",indice,puerto,mascara);

                                        if (pressrelease) *puerto &=255-mascara;
                                        else *puerto |=mascara;



  if (MACHINE_IS_Z88) {
                                                puerto=z88_tabla_teclado_numeros[indice].puerto;
                                                mascara=z88_tabla_teclado_numeros[indice].mascara;

                                                if (pressrelease) *puerto &=255-mascara;
                                                else *puerto |=mascara;
  }

	if (MACHINE_IS_CPC) {
                                                puerto=cpc_tabla_teclado_numeros[indice].puerto;
                                                mascara=cpc_tabla_teclado_numeros[indice].mascara;

                                                if (pressrelease) *puerto &=255-mascara;
                                                else *puerto |=mascara;
  }

  if (MACHINE_IS_QL) {
                                          puerto=ql_tabla_teclado_numeros[indice].puerto;
                                          mascara=ql_tabla_teclado_numeros[indice].mascara;

                                          if (pressrelease) *puerto &=255-mascara;
                                          else *puerto |=mascara;
  }

  if (MACHINE_IS_MK14) {
                                                puerto=mk14_tabla_teclado_numeros[indice].puerto;
                                                mascara=mk14_tabla_teclado_numeros[indice].mascara;

                                                if (pressrelease) *puerto &=255-mascara;
                                                else *puerto |=mascara;
  }


 }

}




void insert_tape_cmdline(char *s)
{
        tapefile=s;
}


void insert_snap_cmdline(char *s)
{
        snapfile=s;
}

//retorna 1 si ok
int quickload_valid_extension(char *nombre) {
	if (
                   !util_compare_file_extension(nombre,"zx")
                || !util_compare_file_extension(nombre,"sp")
                || !util_compare_file_extension(nombre,"zsf")
                || !util_compare_file_extension(nombre,"spg")
                || !util_compare_file_extension(nombre,"trd")
                || !util_compare_file_extension(nombre,"z80")
                || !util_compare_file_extension(nombre,"tzx")
                || !util_compare_file_extension(nombre,"sna")
                || !util_compare_file_extension(nombre,"tap")

                || !util_compare_file_extension(nombre,"smp")
                || !util_compare_file_extension(nombre,"wav")
                || !util_compare_file_extension(nombre,"rwa")


                || !util_compare_file_extension(nombre,"o")
                || !util_compare_file_extension(nombre,"80")
                || !util_compare_file_extension(nombre,"p")
                || !util_compare_file_extension(nombre,"81")
                || !util_compare_file_extension(nombre,"z81")
                || !util_compare_file_extension(nombre,"epr")
                || !util_compare_file_extension(nombre,"63")
                || !util_compare_file_extension(nombre,"eprom")
		|| !util_compare_file_extension(nombre,"flash")
		|| !util_compare_file_extension(nombre,"ace")
		|| !util_compare_file_extension(nombre,"dck")
		|| !util_compare_file_extension(nombre,"cdt")
    || !util_compare_file_extension(nombre,"ay")
    || !util_compare_file_extension(nombre,"scr")
    || !util_compare_file_extension(nombre,"rzx")
	) {
		return 1;
	}


	return 0;


}


//Indica que quickload esta adivinando tipo tzx (si real o standard)
//Se lee desde rutina tape_tzx para meter error en pantalla o no
z80_bit quickload_guessing_tzx_type={0};

void quickload_standard_tape(char *nombre)
{
                if (noautoload.v==0 && !MACHINE_IS_TBBLUE) { //TODO: desactivamos autoload en TBBLUE

			//De momento sam coupe solo carga en real tape
			if (MACHINE_IS_SAM) {
		                realtape_name=nombre;
		                realtape_insert();
		                return;

			}

			//Extension tap tambien es de jupiter ace
			if (!MACHINE_IS_ACE) {

	                        //Si no estamos en spectrum, cambiar maquina
        	                if (!(MACHINE_IS_SPECTRUM)) {
                	                //spectrum 128k
                        	        current_machine_type=6;
                                	set_machine(NULL);
	                                //el reset machine viene desde tape_init
        	                }
			}

                        //establecer parametros por defecto
                        set_machine_params();
                }

                insert_tape_cmdline(nombre);
                tape_init();


}

void quickload_real_tape(char *nombre)
{
                if (noautoload.v==0) {
                        //Si no estamos en spectrum, cambiar maquina
                        if (!(MACHINE_IS_SPECTRUM)) {
                                //spectrum 128k
                                current_machine_type=6;
                                set_machine(NULL);
                                //el reset machine viene desde tape_init
                        }

                        //establecer parametros por defecto
                        set_machine_params();
                }

                realtape_name=nombre;

                realtape_insert();

}

void quickload_common_set_spectrum(void)
{
                if (noautoload.v==0) {
                        //Si no estamos en spectrum, cambiar maquina
                        if (!(MACHINE_IS_SPECTRUM)) {
                                //spectrum 128k
                                current_machine_type=6;
                                set_machine(NULL);
                                //el reset machine viene desde tape_init
                        }

                        //establecer parametros por defecto
                        set_machine_params();
                }

}

//Funcionamiento de quickload:
//Desactivar realvideo
//Limpiar asignacion de joystick a teclas (pero no de eventos), si hay opcion de que lo haga
//Si es snapshot, detecta parametros de ese snap(mediante ssl) y carga el snapshot con los parametros de maquina indicados en el archivo snap
//Si es cinta:
//Si tiene autoload:
//  Si es cinta de spectrum y maquina seleccionada no es spectrum, reset a spectrum 128k
//  si es cinta de zx80 o zx81, cambiar a maquina zx80 o zx81 correspondiente
//  establecer parametros por defecto de la maquina actual
//Insertar cinta. Detectar parametros por defecto de esa cinta(mediante ssl) y detectar tambien si se debe ejecutar en maquina concreta, como Robocop2
//Si tiene autoload, resetear antes maquina para que se pueda ejecutar comando LOAD

//En el caso de Z88, si esta en Z88 activo, se inserta cartucho en slot 1 sin alterar resto de slots. Si no esta Z88 activo, se cambia a Z88

int quickload_continue(char *nombre) {

	//Si hay cinta realtape insertada, quitarla
        if (realtape_inserted.v) realtape_eject();

	//Si hay cartucho de timex insertado, expulsarlo
	//Realmente no comprobamos si hay cartucho pues no hay ningun flag que indique insertado o no. Simplemente liberamos memoria dock
	if (MACHINE_IS_TIMEX_TS2068) {
		timex_empty_dock_space();
	}

	//Quitar top speed si es que estabamos cargando algo antes a maxima velocidad
	top_speed_timer.v=0;

	//Poner modo turbo a 1. En Z88 no hacer esto, pues desasigna los slots , porque llama a set_machine_params y este a init_z88_memory_slots
	if (!MACHINE_IS_Z88) {
		cpu_turbo_speed=1;
		cpu_set_turbo_speed();
	}



#ifdef EMULATE_RASPBERRY
	//desactivar realvideo por defecto en raspberry. Si hace falta, el realvideo se activara desde el archivo snapshot o en la deteccion por ssl
	disable_rainbow();
#endif

	//expulsar cinta insertada
	eject_tape_load();
	tapefile=NULL;


	if (realjoystick_clear_keys_on_smartload.v) {
	        //Eliminar asignacion de eventos joystick a teclas.
		realjoystick_clear_keys_array();
	}

        if (
                           !util_compare_file_extension(nombre,"zx")
                        || !util_compare_file_extension(nombre,"sp")
                        || !util_compare_file_extension(nombre,"zsf")
                        || !util_compare_file_extension(nombre,"spg")
                        || !util_compare_file_extension(nombre,"z80")
                        || !util_compare_file_extension(nombre,"sna")
                        || !util_compare_file_extension(nombre,"ace")
                        || !util_compare_file_extension(nombre,"rzx")
        ) {


                insert_snap_cmdline(nombre);
		snapshot_load();

                return 0;
        }

        //cinta de spectrum tzx, la mayoria (excepto rwa)
        else if (
                   !util_compare_file_extension(nombre,"tzx")

        ) {

		debug_printf (VERBOSE_INFO,"Smartload. Guessing if standard tzx tape or real tape");

		//Ver si la cinta es estandard (ver si todos los tags id tzx son reconocidos)
		//o meterla como real tape

		//Mete la cinta como standard tape. Si da error de tzx id no reconocido, la meteremos como real tape

                insert_tape_cmdline(nombre);
                tape_init();

		//Leer hasta final
		char buffer_temp[65536];
		int final=0;
		quickload_guessing_tzx_type.v=1;
		while (!final) {
			z80_int cinta_longitud;
                        cinta_longitud=tape_block_readlength();
                        if (cinta_longitud==0) final=1;
			else {
				tape_block_read(buffer_temp,cinta_longitud);
			}
		}

		//expulsar cinta insertada
		eject_tape_load();
		tapefile=NULL;

		quickload_guessing_tzx_type.v=0;

		debug_printf (VERBOSE_INFO,"Smartload. End guessing process");

		if (tzx_read_returned_unknown_id.v) {
			debug_printf (VERBOSE_INFO,"Tzx tape will be loaded as real tape");
			quickload_real_tape(nombre);
		}

		else {
			debug_printf (VERBOSE_INFO,"Tzx tape will be loaded as standard tape");
			quickload_standard_tape(nombre);
		}

                return 0;
        }


	//cintas de spectrum, la mayoria (excepto rwa)
        else if (
                   !util_compare_file_extension(nombre,"tap")
                || !util_compare_file_extension(nombre,"rwa")

        ) {

		quickload_standard_tape(nombre);

                return 0;
        }


	//wav y smp suponemos real audio tape Spectrum
        else if (
                   !util_compare_file_extension(nombre,"wav")
                || !util_compare_file_extension(nombre,"smp")

        ) {
		quickload_real_tape(nombre);

                return 0;
        }




        //cintas de zx80, las cargamos como cintas
        else if (
                   !util_compare_file_extension(nombre,"o")
                || !util_compare_file_extension(nombre,"80")

        ) {


		if (noautoload.v==0) {
			//Si no estamos en zx80, cambiar maquina
        	        if (!(MACHINE_IS_ZX80)) {
                	        current_machine_type=120;
                        	set_machine(NULL);
	                }

                        //establecer parametros por defecto
                        set_machine_params();
                }


                insert_tape_cmdline(nombre);
                tape_init();

                return 0;
        }

        //cintas de zx81, las cargamos como cintas
        else if (
                   !util_compare_file_extension(nombre,"p")
                || !util_compare_file_extension(nombre,"81")
                || !util_compare_file_extension(nombre,"z81")

        ) {

		if (noautoload.v==0) {
	                //Si no estamos en zx81, cambiar maquina
        	        if (!(MACHINE_IS_ZX81)) {
                	        current_machine_type=121;
                        	set_machine(NULL);
	                        //el reset machine viene desde tape_init
        	        }

                        //establecer parametros por defecto
                        set_machine_params();
		}


                insert_tape_cmdline(nombre);
                tape_init();

                return 0;
        }

	//dock cartridges de Timex
	else if (
		!util_compare_file_extension(nombre,"dck")
	) {
		//Aqui el autoload da igual. cambiamos siempre a timex si conviene
		if (!MACHINE_IS_TIMEX_TS2068) {
			current_machine_type=17;
			set_machine(NULL);

                                //establecer parametros por defecto. Incluido quitar slots de memoria
                           set_machine_params();

                          reset_cpu();
                }

		timex_insert_dck_cartridge(nombre);

		return 0;

	}

	//TRD
	else if (
		!util_compare_file_extension(nombre,"trd")
	) {
		//Aqui el autoload da igual. cambiamos siempre a Pentagon si conviene
		if (!MACHINE_IS_SPECTRUM) {
			current_machine_type=MACHINE_ID_PENTAGON;
			set_machine(NULL);

                        //establecer parametros por defecto. Incluido quitar slots de memoria
                        set_machine_params();
                        reset_cpu();
                      
                       
                }
                               
                betadisk_enable();
                trd_insert_disk(nombre);


		return 0;

	}        


  //Archivos ay
  else if (
    !util_compare_file_extension(nombre,"ay")
  ) {
    ay_player_load_and_play(nombre);
    return 0;

  }

  else if (
    !util_compare_file_extension(nombre,"scr")
  ) {
    quickload_common_set_spectrum();
    load_screen(nombre);
    return 0;

  }


	//cintas de CPC
	else if (
                !util_compare_file_extension(nombre,"cdt")
        ) {
		//Aqui el autoload da igual. cambiamos siempre a cpc si conviene
                if (!MACHINE_IS_CPC) {
			current_machine_type=140;
                        set_machine(NULL);

                                //establecer parametros por defecto. Incluido quitar slots de memoria
                           set_machine_params();

                          reset_cpu();
                }

                realtape_name=nombre;

                realtape_insert();

                return 0;

        }


	//eprom cards de Z88
        else if (
                   !util_compare_file_extension(nombre,"epr")
		|| !util_compare_file_extension(nombre,"63")
		|| !util_compare_file_extension(nombre,"eprom")

        ) {

		//Aqui el autoload da igual. cambiamos siempre a Z88 si conviene
                //if (noautoload.v==0) {
                        //Si no estamos en z88, cambiar maquina
                        if (!(MACHINE_IS_Z88)) {
                                current_machine_type=130;
                                set_machine(NULL);

                        	//establecer parametros por defecto. Incluido quitar slots de memoria
	                        set_machine_params();

				reset_cpu();
			}

                //}


		//Insertar rom en slot libre
		debug_printf (VERBOSE_INFO,"Finding free slots");

		int slot;
		for (slot=1;slot<=3 && z88_memory_slots[slot].size!=0;slot++);


		//si no hay ninguno libre, meterlo en slot 1
		if (slot==4) {
			//debug_printf (VERBOSE_INFO,"No free slots. Using slot 1");
			debug_printf (VERBOSE_ERR,"No free slots to use. Inserting new card on slot 1");
			slot=1;
		}

		z88_load_eprom_card(nombre,slot);


                return 0;
        }


	//flash cards de Z88
        else if (
                   !util_compare_file_extension(nombre,"flash")

        ) {

                //Aqui el autoload da igual. cambiamos siempre a Z88 si conviene
                //if (noautoload.v==0) {
                        //Si no estamos en z88, cambiar maquina
                        if (!(MACHINE_IS_Z88)) {
                                current_machine_type=130;
                                set_machine(NULL);

                                //establecer parametros por defecto. Incluido quitar slots de memoria
                                set_machine_params();

                                reset_cpu();
                        }

                //}


                //Insertar flash en slot 3
                z88_load_flash_intel_card(nombre,3);


                return 0;
        }




        //Error. no reconocido
        return 1;

}



int quickload(char *nombre) {

	//Recordar estado de algunos interfaces para activarlos despues
	z80_bit antes_divmmc_diviface_enabled;
	antes_divmmc_diviface_enabled.v=divmmc_diviface_enabled.v;

	z80_bit antes_divide_diviface_enabled;
	antes_divide_diviface_enabled.v=divide_diviface_enabled.v;

	z80_bit antes_superupgrade_enabled;
	antes_superupgrade_enabled.v=superupgrade_enabled.v;


	int retorno=quickload_continue(nombre);

	//En tbblue, setting de divmmc desactivado se gestiona mediante bit de registro
	/*
	z80_byte diven=tbblue_registers[6]&16;
	*/
	if (MACHINE_IS_TBBLUE) {
		if ((tbblue_registers[6]&16)==0) antes_divmmc_diviface_enabled.v=0;
	}

       if (antes_divmmc_diviface_enabled.v)  {
		debug_printf (VERBOSE_DEBUG,"Reenabling divmmc as it was enabled before quickload");
		divmmc_diviface_disable();
                divmmc_diviface_enable();
        }

       if (antes_divide_diviface_enabled.v)  {
		debug_printf (VERBOSE_DEBUG,"Reenabling divide as it was enabled before quickload");
		divide_diviface_disable();
                divide_diviface_enable();
        }

	if (antes_superupgrade_enabled.v && superupgrade_enabled.v==0) {
		debug_printf (VERBOSE_DEBUG,"Reenabling superupgrade as it was enabled before quickload");
		superupgrade_enable(0);
	}

	return retorno;

}


//Retorna 1 si existe archivo
//0 si no
int si_existe_archivo(char *nombre)
{


                //Ver si archivo existe y preguntar
                struct stat buf_stat;

                if (stat(nombre, &buf_stat)==0) {

			return 1;

                }

	return 0;

}


//Retorna tamanyo archivo
long int get_file_size(char *nombre)
{
        struct stat buf_stat;

                if (stat(nombre, &buf_stat)!=0) {
                        debug_printf(VERBOSE_INFO,"Unable to get status of file %s",nombre);
			return 0;
                }

                else {
			//printf ("file size: %ld\n",buf_stat.st_size);
			return buf_stat.st_size;
                }
}

//0 desconocido o inexistente
//1 normal
//2 directorio
int get_file_type_from_name(char *nombre)
{
  struct stat buf_stat;

          if (stat(nombre, &buf_stat)!=0) {
                  debug_printf(VERBOSE_INFO,"Unable to get status of file %s",nombre);
return 0;
          }

          else {
//printf ("file size: %ld\n",buf_stat.st_size);
return get_file_type_from_stat(&buf_stat);
          }
}


//Retorna fecha de un archivo en valores de punteros
//Devuelve 1 si error
//anyo tal cual: 2017, etc
int get_file_date_from_stat(struct stat *buf_stat,int *hora,int *minuto,int *segundo,int *dia,int *mes,int *anyo)
{



          struct tm *foo;

#if defined(__APPLE__)
          struct timespec *d;
          d=&buf_stat->st_mtimespec;
          //foo = gmtime((const time_t *)d);
          foo = localtime((const time_t *)d);
#else
          struct time_t *d;
          d=&buf_stat->st_mtime;
          //foo = gmtime((const time_t *)d);
          foo = localtime((const time_t *)d);
#endif

//printf("Year: %d\n", foo->tm_year);
//printf("Month: %d\n", foo->tm_mon);
//printf("Day: %d\n", foo->tm_mday);
//printf("Hour: %d\n", foo->tm_hour);
//printf("Minute: %d\n", foo->tm_min);
//printf("Second: %d\n", foo->tm_sec);

*hora=foo->tm_hour;
*minuto=foo->tm_min;
*segundo=foo->tm_sec;

*dia=foo->tm_mday;
*mes=foo->tm_mon+1;
*anyo=foo->tm_year+1900;

          return 0;


}

//Retorna fecha de un archivo en valores de punteros
//Devuelve 1 si error
//anyo tal cual: 2017, etc
int get_file_date_from_name(char *nombre,int *hora,int *minuto,int *segundo,int *dia,int *mes,int *anyo)
{
  struct stat buf_stat;

          if (stat(nombre, &buf_stat)!=0) {
                  debug_printf(VERBOSE_INFO,"Unable to get status of file %s",nombre);
return 1;
          }


          get_file_date_from_stat(&buf_stat,hora,minuto,segundo,dia,mes,anyo);


          return 0;


}


//Retorna -1 si hay algun error
//retorna bytes leidos si ok
int lee_archivo(char *nombre,char *buffer,int max_longitud)
{

	int leidos;

                FILE *ptr_archivo;
                ptr_archivo=fopen(nombre,"rb");
                if (!ptr_archivo) {
                        debug_printf (VERBOSE_DEBUG,"Can not open %s",nombre);
                        return -1;
                }

                leidos=fread(buffer,1,max_longitud,ptr_archivo);
                fclose(ptr_archivo);

		return leidos;

}

void util_set_reset_key_cpc_keymap(enum util_teclas_cpc_keymap tecla,int pressrelease)
{
        switch (tecla) {
			case UTIL_KEY_CPC_MINUS:
                                if (pressrelease) {
                                        cpc_keyboard_table[3] &=255-2;
                                }
                                else {
                                        cpc_keyboard_table[3] |=2;
                                }
                        break;

			case UTIL_KEY_CPC_CIRCUNFLEJO:
                                if (pressrelease) {
                                        cpc_keyboard_table[3] &=255-1;
                                }
                                else {
                                        cpc_keyboard_table[3] |=1;
                                }
                        break;

			case UTIL_KEY_CPC_ARROBA:
                                if (pressrelease) {
                                        cpc_keyboard_table[3] &=255-4;
                                }
                                else {
                                        cpc_keyboard_table[3] |=4;
                                }
                        break;

			case UTIL_KEY_CPC_BRACKET_LEFT:
                                if (pressrelease) {
                                        cpc_keyboard_table[2] &=255-2;
                                }
                                else {
                                        cpc_keyboard_table[2] |=2;
                                }
                        break;

			case UTIL_KEY_CPC_COLON:
                                if (pressrelease) {
                                        cpc_keyboard_table[3] &=255-32;
                                }
                                else {
                                        cpc_keyboard_table[3] |=32;
                                }
                        break;

			case UTIL_KEY_CPC_SEMICOLON:
                                if (pressrelease) {
                                        cpc_keyboard_table[3] &=255-16;
                                }
                                else {
                                        cpc_keyboard_table[3] |=16;
                                }
                        break;

			case UTIL_KEY_CPC_BRACKET_RIGHT:
                                if (pressrelease) {
                                        cpc_keyboard_table[2] &=255-8;
                                }
                                else {
                                        cpc_keyboard_table[2] |=8;
                                }
                        break;

			case UTIL_KEY_CPC_COMMA:
                                if (pressrelease) {
                                        cpc_keyboard_table[4] &=255-128;
                                }
                                else {
                                        cpc_keyboard_table[4] |=128;
                                }
                        break;

			case UTIL_KEY_CPC_PERIOD:


                                if (pressrelease) {
                                        cpc_keyboard_table[3] &=255-128;
                                }
                                else {
                                        cpc_keyboard_table[3] |=128;
                                }
                        break;

			case UTIL_KEY_CPC_SLASH:
                                if (pressrelease) {
                                        cpc_keyboard_table[3] &=255-64;
                                }
                                else {
                                        cpc_keyboard_table[3] |=64;
                                }
                        break;

			case UTIL_KEY_CPC_BACKSLASH:
                                if (pressrelease) {
                                        cpc_keyboard_table[2] &=255-64;
                                }
                                else {
                                        cpc_keyboard_table[2] |=64;
                                }
                        break;

	}

}

void util_set_reset_key_z88_keymap(enum util_teclas_z88_keymap tecla,int pressrelease)
{
	switch (tecla) {
                        case UTIL_KEY_Z88_MINUS:
                                if (pressrelease) {
                                        blink_kbd_a11 &= 255-128;
                                }
                                else {
                                        blink_kbd_a11 |= 128;
                                }
                        break;

                        case UTIL_KEY_Z88_EQUAL:
                                if (pressrelease) {
                                        blink_kbd_a10 &= 255-128;
                                }
                                else {
                                        blink_kbd_a10 |= 128;
                                }
                        break;

                        case UTIL_KEY_Z88_BACKSLASH:
                                if (pressrelease) {
                                        blink_kbd_a9 &= 255-128;
                                }
                                else {
                                        blink_kbd_a9 |= 128;
                                }
                        break;



                        case UTIL_KEY_Z88_BRACKET_LEFT:
                                if (pressrelease) {
                                        blink_kbd_a13 &= 255-128;
                                }
                                else {
                                        blink_kbd_a13 |= 128;
                                }
                        break;

                        case UTIL_KEY_Z88_BRACKET_RIGHT:
                                if (pressrelease) {
                                        blink_kbd_a12 &= 255-128;
                                }
                                else {
                                        blink_kbd_a12 |= 128;
                                }
                        break;

                        case UTIL_KEY_Z88_SEMICOLON:
                                if (pressrelease) {
                                        blink_kbd_a14 &= 255-2;
                                }
                                else {
                                        blink_kbd_a14 |= 2;
                                }
                        break;

                        case UTIL_KEY_Z88_APOSTROPHE:
                                if (pressrelease) {
                                        blink_kbd_a14 &= 255-1;
                                }
                                else {
                                        blink_kbd_a14 |= 1;
                                }
                        break;

                        case UTIL_KEY_Z88_POUND:
                                if (pressrelease) {
                                        blink_kbd_a15 &= 255-1;
                                }
                                else {
                                        blink_kbd_a15 |= 1;
                                }
                        break;

                        case UTIL_KEY_Z88_COMMA:
                                if (pressrelease) {

                                        blink_kbd_a14 &= (255-4);
                                }
                                else {

                                        blink_kbd_a14 |= 4;
                                }
                        break;

                        case UTIL_KEY_Z88_PERIOD:
                                 if (pressrelease) {
                                                blink_kbd_a15 &= (255-4);
                                        }
                                        else {
                                                blink_kbd_a15 |= 4;
                                        }
                        break;

                        case UTIL_KEY_Z88_SLASH:
                                 if (pressrelease) {
                                                blink_kbd_a15 &= (255-2);
                                        }
                                        else {
                                                blink_kbd_a15 |= 2;
                                        }
                        break;

	}
}


void util_set_reset_key_common_keymap(enum util_teclas_common_keymap tecla,int pressrelease)
{
        switch (tecla) {
                        case UTIL_KEY_COMMON_KEYMAP_MINUS:
                                if (pressrelease) {
                                        puerto_teclado_sam_eff9 &= 255-32;
                                        ql_keyboard_table[5] &= 255-32;
                                        //printf ("tecla minus\n");
                                }
                                else {
                                        puerto_teclado_sam_eff9 |= 32;
                                        ql_keyboard_table[5] |= 32;
                                }
                        break;

                        case UTIL_KEY_COMMON_KEYMAP_EQUAL:
                                if (pressrelease) {
                                        puerto_teclado_sam_eff9 &= 255-64;
                                        ql_keyboard_table[3] &= 255-32;
                                }
                                else {
                                        puerto_teclado_sam_eff9 |= 64;
                                        ql_keyboard_table[3] |= 32;
                                }
                        break;

                        case UTIL_KEY_COMMON_KEYMAP_BACKSLASH:
                                if (pressrelease) {
                                  // 6|   Ret   Left     Up    Esc  Right      \  Space   Down     ql_keyboard_table[1]
                                  ql_keyboard_table[1] &=255-32;
                                }
                                else {
                                  ql_keyboard_table[1] |=32;
                                }
                        break;



                        case UTIL_KEY_COMMON_KEYMAP_BRACKET_LEFT:
                                if (pressrelease) {
					                             puerto_teclado_sam_dff9 &= 255-32;
                                       //// 4|     [   Caps      k      s      f      =      g      ;     ql_keyboard_table[3]
                                       ql_keyboard_table[3] &=255-1;
                                }
                                else {
					                             puerto_teclado_sam_dff9 |= 32;
                                       ql_keyboard_table[3] |=1;
                                }
                        break;


			case UTIL_KEY_COMMON_KEYMAP_BRACKET_RIGHT:
                                if (pressrelease) {
					                             puerto_teclado_sam_dff9 &= 255-64;
                                       // 5|     ]      z      .      c      b  Pound      m      '     ql_keyboard_table[2]
                                       ql_keyboard_table[2] &=255-1;
                                }
                                else {
                                        puerto_teclado_sam_dff9 |= 64;
                                        ql_keyboard_table[2] |=1;
                                }
                        break;

                        case UTIL_KEY_COMMON_KEYMAP_SEMICOLON:
                                if (pressrelease) {
                                      // 4|     [   Caps      k      s      f      =      g      ;     ql_keyboard_table[3]
                                      ql_keyboard_table[3] &=255-128;
                                }
                                else {
                                      ql_keyboard_table[3] |=128;
                                }
                        break;

                        case UTIL_KEY_COMMON_KEYMAP_APOSTROPHE:
                                if (pressrelease) {
                                        puerto_teclado_sam_bff9 &= 255-32;
                                        // 5|     ]      z      .      c      b  Pound      m      '     ql_keyboard_table[2]
                                        ql_keyboard_table[2] &=255-128;
                                }
                                else {
                                        puerto_teclado_sam_bff9 |= 32;
                                        ql_keyboard_table[2] |=128;
                                }
                        break;

                        case UTIL_KEY_COMMON_KEYMAP_POUND:
                                if (pressrelease) {
                                        puerto_teclado_sam_bff9 &= 255-64;
                                        //// 5|     ]      z      .      c      b  Pound      m      '     ql_keyboard_table[2]
                                        ql_keyboard_table[2] &=255-32;
                                }
                                else {
                                        puerto_teclado_sam_bff9 |= 64;
                                        ql_keyboard_table[2] |=32;
                                }
                        break;


			case UTIL_KEY_COMMON_KEYMAP_COMMA:
                                if (pressrelease) {
					                              puerto_teclado_sam_7ff9 &= 255-32;
                                        ql_keyboard_table[7] &=(255-128);

                                }
                                else {
					                             puerto_teclado_sam_7ff9 |= 32;
                                       ql_keyboard_table[7] |=128;
                                }
                        break;

                        case UTIL_KEY_COMMON_KEYMAP_PERIOD:
                                 if (pressrelease) {
					                              puerto_teclado_sam_7ff9 &= 255-64;
                                        ql_keyboard_table[2] &=(255-4);
                                 }
                                 else {
					                              puerto_teclado_sam_7ff9 |= 64;
                                        ql_keyboard_table[2] |=4;
                                 }
                        break;

                        case UTIL_KEY_COMMON_KEYMAP_SLASH:
                                 if (pressrelease) {
					                              puerto_teclado_sam_7ff9 &= 255-128;
                                        // 0| Shift   Ctrl    Alt      x      v      /      n      ,     ql_keyboard_table[7]
                                        ql_keyboard_table[7] &=255-32;
                                 }
                                 else {
					                              puerto_teclado_sam_7ff9 |= 128;
                                        ql_keyboard_table[7] |=32;
                                 }
                        break;

        }
}



z80_bit chloe_keyboard_pressed_shift={0};
//z80_bit chloe_keyboard_pressed_alt={0};
z80_bit chloe_keyboard_pressed_winkey={0};

//Tecla pulsada. Valor de util_teclas
enum util_teclas chloe_keyboard_pressed_tecla=UTIL_KEY_NONE;

//Valor en ascii
z80_byte chloe_keyboard_pressed_tecla_ascii=0;


//Tabla de teclas a enviar con modificador en chloe
//Todas se corresponen a symbol shift + algo

struct s_teclas_chloe_numeros {
	z80_byte ascii_origen;
	z80_byte ascii_destino;
};

typedef struct s_teclas_chloe_numeros teclas_chloe_numeros;

#define TOTAL_TECLAS_CHLOE_NUMEROS 10
teclas_chloe_numeros tabla_teclas_chloe_numeros[TOTAL_TECLAS_CHLOE_NUMEROS]={
	{'1','1'},
	{'2','2'},
	{'3','3'},
	{'4','4'},
	{'5','5'},
	{'6','h'},
	{'7','6'},
	{'8','b'},
	{'9','8'},
	{'0','9'},
};

void util_set_reset_key_chloe_keymap(enum util_teclas_chloe_keymap tecla,int pressrelease)
{
	//printf ("envio de tecla extendida por keymap. tecla: %d pressrelease: %d\n",tecla,pressrelease);

	//todas generan sym shift
	if (pressrelease==0) {
		reset_keyboard_ports();
		return;
	}


	//Se pulsan las teclas
	//Teclas que vienen de keymap, como -_ etc
	switch (tecla) {
		case UTIL_KEY_CHLOE_MINUS:
			set_symshift();
			if (chloe_keyboard_pressed_shift.v) convert_numeros_letras_puerto_teclado_continue('0',1);
			else  convert_numeros_letras_puerto_teclado_continue('j',1);
		break;

		case UTIL_KEY_CHLOE_EQUAL:
			set_symshift();
			if (chloe_keyboard_pressed_shift.v) convert_numeros_letras_puerto_teclado_continue('k',1);
			else convert_numeros_letras_puerto_teclado_continue('l',1);
		break;


		case UTIL_KEY_CHLOE_BRACKET_LEFT:
			set_symshift();
			if (chloe_keyboard_pressed_shift.v) convert_numeros_letras_puerto_teclado_continue('f',1);
			else convert_numeros_letras_puerto_teclado_continue('y',1);
		break;

		case UTIL_KEY_CHLOE_BRACKET_RIGHT:
			set_symshift();
			if (chloe_keyboard_pressed_shift.v) convert_numeros_letras_puerto_teclado_continue('g',1);
			else convert_numeros_letras_puerto_teclado_continue('u',1);
		break;

		case UTIL_KEY_CHLOE_SEMICOLON:
			set_symshift();
			if (chloe_keyboard_pressed_shift.v) convert_numeros_letras_puerto_teclado_continue('z',1);
			else convert_numeros_letras_puerto_teclado_continue('o',1);
		break;

		case UTIL_KEY_CHLOE_APOSTROPHE:
			set_symshift();
			if (chloe_keyboard_pressed_shift.v) convert_numeros_letras_puerto_teclado_continue('p',1);
			else convert_numeros_letras_puerto_teclado_continue('7',1);
		break;

		case UTIL_KEY_CHLOE_POUND:
			set_symshift();
			if (chloe_keyboard_pressed_shift.v) convert_numeros_letras_puerto_teclado_continue('s',1);
			else convert_numeros_letras_puerto_teclado_continue('d',1);
		break;

		case UTIL_KEY_CHLOE_SLASH:
			set_symshift();
			if (chloe_keyboard_pressed_shift.v) convert_numeros_letras_puerto_teclado_continue('c',1);
			else convert_numeros_letras_puerto_teclado_continue('v',1);
		break;

		case UTIL_KEY_CHLOE_COMMA:
			set_symshift();
                        if (chloe_keyboard_pressed_shift.v) convert_numeros_letras_puerto_teclado_continue('r',1);
                        else convert_numeros_letras_puerto_teclado_continue('n',1);
		break;

		case UTIL_KEY_CHLOE_PERIOD:
			set_symshift();
                        if (chloe_keyboard_pressed_shift.v) convert_numeros_letras_puerto_teclado_continue('t',1);
                        else convert_numeros_letras_puerto_teclado_continue('m',1);
		break;

		case UTIL_KEY_CHLOE_LEFTZ:
			set_symshift();
                        if (chloe_keyboard_pressed_shift.v) convert_numeros_letras_puerto_teclado_continue('a',1);
                        else convert_numeros_letras_puerto_teclado_continue('x',1);
		break;

		case UTIL_KEY_CHLOE_BACKSLASH:
			//Esto se mapea a true video (shift +3) e inverse video (shift+4)
			//Activar mayus
			puerto_65278  &=255-1;

			if (chloe_keyboard_pressed_shift.v) convert_numeros_letras_puerto_teclado_continue('4',1);
			else convert_numeros_letras_puerto_teclado_continue('3',1);
		break;




	}
}


void util_set_reset_key_chloe(void)
{
	//printf ("Shift: %d non ascii key: %d ascii: %d\n",chloe_keyboard_pressed_shift.v,chloe_keyboard_pressed_tecla,chloe_keyboard_pressed_tecla_ascii);

	if (chloe_keyboard_pressed_winkey.v) {
		set_symshift();
	}

				if (chloe_keyboard_pressed_shift.v==0) {
					//Tecla sin shift
					//Enviar tecla tal cual sin modificador
					//printf ("Enviar tecla sin shift\n");
					if (chloe_keyboard_pressed_tecla!=UTIL_KEY_NONE) {
						//printf ("tecla no es ascii: %d\n",chloe_keyboard_pressed_tecla);
						util_set_reset_key_continue(chloe_keyboard_pressed_tecla,1);
					}
					else {
						//printf ("tecla es ascii: %d\n",chloe_keyboard_pressed_tecla_ascii);
						convert_numeros_letras_puerto_teclado_continue(chloe_keyboard_pressed_tecla_ascii,1);
					}
					return;
				}


				//Tecla con shift

                                if (chloe_keyboard_pressed_tecla==UTIL_KEY_NONE && chloe_keyboard_pressed_tecla_ascii==0) {
					//No tecla pulsada. Volver
                                        return;
                                }

                                else {
                                        //Tecla con shift
					//printf ("Pulsado shift y tecla %d o %d ascii\n",chloe_keyboard_pressed_tecla,chloe_keyboard_pressed_tecla_ascii);

					//Ver si es tecla ascii
					if (chloe_keyboard_pressed_tecla_ascii) {
						//printf ("Es tecla ascii\n");
						int i;

						//Ver si es tecla numerica
						for (i=0;i<TOTAL_TECLAS_CHLOE_NUMEROS;i++) {
							if (tabla_teclas_chloe_numeros[i].ascii_origen==chloe_keyboard_pressed_tecla_ascii) {
								set_symshift();
								convert_numeros_letras_puerto_teclado_continue(tabla_teclas_chloe_numeros[i].ascii_destino,1);
								return;
							}
						}

						//No es tecla numerica. Enviar mayusculas y ascii
						util_set_reset_key_continue(UTIL_KEY_SHIFT_L,1);
						convert_numeros_letras_puerto_teclado_continue(chloe_keyboard_pressed_tecla_ascii,1);
						return;
					}

					//No es tecla ascii
					//printf ("No es tecla ascii\n");


					if (chloe_keyboard_pressed_tecla!=UTIL_KEY_NONE) util_set_reset_key_continue(chloe_keyboard_pressed_tecla,1);
					else convert_numeros_letras_puerto_teclado_continue(chloe_keyboard_pressed_tecla_ascii,1);



                                        return;
                                }
}


//Decir si el raton funciona sobre el menu
int si_menu_mouse_activado(void)
{
  if (kempston_mouse_emulation.v) return 0;
  if (gunstick_emulation) return 0;
  if (mouse_menu_disabled.v) return 0;

  return 1;
}


//Numero de veces que se ha pulsado la tecla de abrir el menu
int util_if_open_just_menu_times=0;

//Contador que se incrementa cada vez desde timer.c (50 veces por segundo)
unsigned int util_if_open_just_menu_counter=0;

//Contador inicial de la primera pulsacion de tecla de abrir menu
unsigned int util_if_open_just_menu_initial_counter=0;

//Accion de abrir menu (F5, boton joystick) que ademas controla si el boton esta limitado para que abra el menu solo cuando se pulsa 3 veces seguidas, por ejemplo, en 1 segundo
int util_if_open_just_menu(void)
{
	if (menu_limit_menu_open.v==0) return 1;

	//esta limitado el uso de F5, hay que pulsar 3 veces en el ultimo segundo
	if (util_if_open_just_menu_times==0) {
		util_if_open_just_menu_initial_counter=util_if_open_just_menu_counter;
	}

	util_if_open_just_menu_times++;
	debug_printf (VERBOSE_DEBUG,"Pressed open menu key: %d times",util_if_open_just_menu_times);

	//Si llega a 3 veces, ver si la diferencia del contador es menor o igual que 50
	if (util_if_open_just_menu_times==3) {
		util_if_open_just_menu_times=0;
		int diferencia=util_if_open_just_menu_counter-util_if_open_just_menu_initial_counter;
		debug_printf (VERBOSE_DEBUG,"Time elapsed between the third keypress and the first one (in 1/50s): %d",diferencia);
		if (diferencia<=50) return 1;
	}

	return 0;
}


//Si esta emulacion de kempston mouse activa, no se puede usar el raton para el menu
void util_set_reset_mouse(enum util_mouse_buttons boton,int pressrelease)
{

  switch(boton)
  {
    case UTIL_MOUSE_LEFT_BUTTON:
      if (pressrelease) {
        mouse_left=1;

        if (si_menu_mouse_activado() ) {
          //Si no esta menu abierto, hace accion de abrir menu
          if (menu_abierto==0) menu_fire_event_open_menu();
          else {
            //Si esta menu abierto, es como enviar enter
            util_set_reset_key(UTIL_KEY_ENTER,1);
          }
        }

      }
      else {
        mouse_left=0;

        if (si_menu_mouse_activado()) {

          if (menu_abierto==1) {
            //Si esta menu abierto, es como enviar enter
            util_set_reset_key(UTIL_KEY_ENTER,0);
          }
        }
      }
    break;

    case UTIL_MOUSE_RIGHT_BUTTON:
      if (pressrelease) {
        mouse_right=1;
        //Si esta menu abierto, hace como ESC

        if (si_menu_mouse_activado() ) {
          //Si no esta menu abierto, hace accion de abrir menu
          if (menu_abierto==0) menu_fire_event_open_menu();
          else {
            //Si esta menu abierto, es como enviar ESC
            util_set_reset_key(UTIL_KEY_ESC,1);
          }
        }

      }
      else {
        mouse_right=0;
        //Si esta menu abierto, hace como ESC
        if (si_menu_mouse_activado()) {
          if (menu_abierto) util_set_reset_key(UTIL_KEY_ESC,0);
        }
      }
    break;

  }


}


void util_set_reset_key_handle_chloe_ascii(enum util_teclas tecla,int pressrelease)
{
        
                //printf ("teclado es chloe. tecla=%d\n",tecla);
                //tecla ascii
       //Tecla no es shift. comentario absurdo???
                        //Ver si se pulsa o se libera
                        if (pressrelease==0) {
                                //Se libera
				chloe_keyboard_pressed_tecla_ascii=0;
                                //Liberar todas teclas de puertos
                                reset_keyboard_ports();
                        }
                        else {
                                //Se pulsa
                                //Si solo pulsado shift pero no una tecla diferente, no hacer nada, solo activar modificador
                                chloe_keyboard_pressed_tecla_ascii=tecla;
                                util_set_reset_key_chloe();
                        }

}

void util_set_reset_key_convert_recreated_yesno(enum util_teclas tecla,int pressrelease,int convertrecreated)
{


	//Para poder evitar que se vuelva a convertir cuando se llama desde convert_numeros_letras_puerto_teclado_continue
	if (convertrecreated) {
		//Si teclado recreated, y menu cerrado
		if (!menu_abierto && recreated_zx_keyboard_support.v) {

			enum util_teclas tecla_final;
			int pressrelease_final;

			//Si es mayusculas
			if (tecla==UTIL_KEY_SHIFT_L) {
				//printf ("Pulsada shift L real\n");
				if (pressrelease) recreated_zx_keyboard_pressed_caps.v=1;
				else recreated_zx_keyboard_pressed_caps.v=0;
				return;
			}

			//printf ("Entrada a convertir tecla recreated desde util_set_reset_key\n");
			recreated_zx_spectrum_keyboard_convert(tecla, &tecla_final, &pressrelease_final);
			if (tecla_final) {
				tecla=tecla_final;
				pressrelease=pressrelease_final;
			}
		}
	}

	if (chloe_keyboard.v) {
		//Ver si tecla que se pulsa es shift
		if (tecla==UTIL_KEY_SHIFT_L || tecla==UTIL_KEY_SHIFT_R) {
			//Ver si se pulsa o se libera
			if (pressrelease==0) {
				//Se libera
				chloe_keyboard_pressed_shift.v=0;
				util_set_reset_key_continue(tecla,pressrelease);
				return;
			}
			else {
				//Se pulsa
				//Si solo pulsado shift pero no una tecla diferente, no hacer nada, solo activar modificador
				chloe_keyboard_pressed_shift.v=1;
				util_set_reset_key_chloe();
				return;
			}
		}

		//Ver si se pulsa tecla Windows (o cmd en mac)
		else if (tecla==UTIL_KEY_WINKEY) {
			//Ver si se pulsa o se libera
                        if (pressrelease==0) {
				//printf ("Liberada winkey\n");
				chloe_keyboard_pressed_winkey.v=0;
				clear_symshift();
				//Liberar todas teclas de puertos en caso de driver cocoa
				//Esto sucede porque esto es tecla cmd y en cocoa,
				//cuando se libera cmd, no se envian liberacion de otras teclas pulsadas
				if (strcmp(scr_driver_name,"cocoa")==0) {
					//printf ("Liberando todas teclas en driver cocoa\n");
					chloe_keyboard_pressed_tecla=UTIL_KEY_NONE;
					chloe_keyboard_pressed_tecla_ascii=0;
					reset_keyboard_ports();
	                                return;
				}
				util_set_reset_key_continue(tecla,pressrelease);
                                return;
                        }
                        else {
                                //Se pulsa
				//printf ("utils. enviando winkey - symbol shift\n");
				chloe_keyboard_pressed_winkey.v=1;
				util_set_reset_key_chloe();
                                return;
                        }
		}

		else if (tecla==UTIL_KEY_ALT_L || tecla==UTIL_KEY_ALT_R) {
			//Se pulsa alt
			if (pressrelease==0) {
				//Liberar todas teclas de puertos
                                reset_keyboard_ports();
                                return;
                        }
                        else {
                                //Se pulsa
				//Enviar sym+shift
				set_symshift();
				puerto_65278  &=255-1;
				return;
			}
		}

		else if (tecla==UTIL_KEY_CONTROL_L || tecla==UTIL_KEY_CONTROL_R)  {
			//En Chloe, Control es shift+9
			if (pressrelease==0) {
                                //Liberar todas teclas de puertos
                                reset_keyboard_ports();
                                return;
                        }
                        else {
                                //Se pulsa
                                //Shift
                                puerto_65278  &=255-1;
                                //9
                                puerto_61438  &=255-2;
				return;
			}
		}


		else if (tecla==UTIL_KEY_TAB && menu_abierto==0) {
                       //En Chloe, TAB es shift+1. Pero con menu cerrado
			if (pressrelease==0) {
                                //Liberar todas teclas de puertos
                                reset_keyboard_ports();
                                return;
                        }
                        else {
                                //Shift
                                puerto_65278  &=255-1;
                                //1
                                puerto_63486  &=255-1;
				return;
			}
                }


		else if (tecla==UTIL_KEY_ESC) {
		      //ESC en Chloe es BREAK
			if (pressrelease==0) {
                                //Liberar todas teclas de puertos
                                reset_keyboard_ports();
                                return;
                        }
                        else {
                        	puerto_65278 &=255-1;
				puerto_32766 &=255-1;
				//Y indicar tecla ESC y otra funcion de esc (liberar cola speech)
				util_set_reset_key_continue(UTIL_KEY_ESC,1);
                                return;
                        }
		}



		else {
			//Ver si se pulsa o se libera
                        if (pressrelease==0) {
                                //Se libera
                                chloe_keyboard_pressed_tecla=UTIL_KEY_NONE;
				//Liberar todas teclas de puertos
				reset_keyboard_ports();
                                return;
                        }
                        else {
                                //Se pulsa
                                //Si solo pulsado shift pero no una tecla diferente, no hacer nada, solo activar modificador
                                chloe_keyboard_pressed_tecla=tecla;
                                util_set_reset_key_chloe();
				return;
                        }
                }


	}

	else {
		util_set_reset_key_continue(tecla,pressrelease);
	}

}




void util_set_reset_key(enum util_teclas tecla,int pressrelease)
{
        if (chloe_keyboard.v && tecla>=32 && tecla<=127) {
                util_set_reset_key_handle_chloe_ascii(tecla,pressrelease);
                return;
        }
        else {
        //printf ("util_set_reset_key tecla: %d pressrelease: %d\n",tecla,pressrelease);
       util_set_reset_key_convert_recreated_yesno(tecla,pressrelease,1);
        }
}

void convert_numeros_letras_puerto_teclado(z80_byte tecla,int pressrelease)
{



	//else {
	        convert_numeros_letras_puerto_teclado_continue(tecla,pressrelease);
	//}


}






//Retorna no 0 si hay redefinicion de tecla F (no es default)
int util_set_reset_key_continue_f_functions(enum util_teclas tecla,int pressrelease)
{

  //printf ("tecla: %d pressrelease: %d menu_abierto: %d\n",tecla,pressrelease,menu_abierto);

  //Aunque este el menu abierto, procesar estas teclas. Asi esto permite que la tecla de OSD aparezca 
  //desde el menu
  //if (menu_abierto) return 0;
  //Ver si la tecla F esta asignada
  //enum defined_f_function_ids defined_f_functions_keys_array[MAX_F_FUNCTIONS_KEYS]

  int indice;

  switch (tecla) {
    case UTIL_KEY_F1:
      indice=0;
    break;

    case UTIL_KEY_F2:
      indice=1;
    break;

    case UTIL_KEY_F3:
      indice=2;
    break;

    case UTIL_KEY_F4:
      indice=3;
    break;

    case UTIL_KEY_F5:
      indice=4;
      //printf ("F5!!\n");
    break;

    case UTIL_KEY_F6:
      indice=5;
    break;

    case UTIL_KEY_F7:
      indice=6;
    break;

    case UTIL_KEY_F8:
      indice=7;
    break;

    case UTIL_KEY_F9:
      indice=8;
    break;

    case UTIL_KEY_F10:
      indice=9;
    break;

    case UTIL_KEY_F11:
      indice=10;
    break;

    case UTIL_KEY_F12:
      indice=11;
    break;

    case UTIL_KEY_F13:
      indice=12;
    break;

    case UTIL_KEY_F14:
      indice=13;
    break;

    case UTIL_KEY_F15:
      indice=14;
    break;

    default:
      return 0;
    break;
  }

  enum defined_f_function_ids accion=defined_f_functions_keys_array[indice];

  debug_printf (VERBOSE_DEBUG,"Key: F%d Action: %s",indice+1,defined_f_functions_array[accion].texto_funcion);

  if (accion!=F_FUNCION_DEFAULT) {
    if (pressrelease) {
      //Activar funcion f en menu
      menu_button_f_function.v=1;
      menu_button_f_function_index=indice;
      menu_abierto=1;


    }
    return 1;
  }

  return 0;
}


//Aqui solo se activan/desactivan bits de puerto_especial para teclas F
void util_set_reset_key_continue_tecla_f(enum util_teclas tecla,int pressrelease)
{

switch(tecla)
{

  //F1 pulsado
  case UTIL_KEY_F1:

          if (pressrelease) {
                  puerto_especial2 &=255-1;
          }
          else {
                  puerto_especial2 |=1;
          }
  break;

  //F2 pulsado
  case UTIL_KEY_F2:

          if (pressrelease) {
                  puerto_especial2 &=255-2;
          }
          else {
                  puerto_especial2 |=2;
          }
  break;


  case UTIL_KEY_F3:

          if (pressrelease) {
                  puerto_especial2 &=255-4;
          }
          else {
                  puerto_especial2 |=4;
          }
  break;


       case UTIL_KEY_F4:
           if (pressrelease) {
             puerto_especial2 &=255-8;
           }

           else {
             puerto_especial2 |=8;
           }
      break;


  case UTIL_KEY_F5:

          if (pressrelease) {
                  puerto_especial2 &=255-16;
          }
          else {
                  puerto_especial2 |=16;
          }
  break;


  case UTIL_KEY_F6:

          if (pressrelease) {
                  puerto_especial3 &=255-1;
          }
          else {
                  puerto_especial3 |=1;
          }
  break;


  case UTIL_KEY_F7:

          if (pressrelease) {
                  puerto_especial3 &=255-2;
          }
          else {
                  puerto_especial3 |=2;
          }
  break;


  case UTIL_KEY_F8:
        if (pressrelease) {
               puerto_especial3 &=255-4;
        }
        else {
              puerto_especial3 |=4;
        }

  break;


  case UTIL_KEY_F9:

          if (pressrelease) {
              puerto_especial3 &=255-8;
          }
          else {
              puerto_especial3 |=8;
          }

  break;


  case UTIL_KEY_F10:

          if (pressrelease) {
                  puerto_especial3 &=255-16;
          }
          else {
                  puerto_especial3 |=16;
          }
  break;


  case UTIL_KEY_F11:

          if (pressrelease) {
                  puerto_especial4 &=255-1;
          }
          else {
                  puerto_especial4 |=1;
          }
  break;


  case UTIL_KEY_F12:

          if (pressrelease) {
                  puerto_especial4 &=255-2;
          }
          else {
                  puerto_especial4 |=2;
          }
  break;


  case UTIL_KEY_F13:

          if (pressrelease) {
                  puerto_especial4 &=255-4;
          }
          else {
                  puerto_especial4 |=4;
          }
  break;


  case UTIL_KEY_F14:

          if (pressrelease) {
                  puerto_especial4 &=255-8;
          }
          else {
                  puerto_especial4 |=8;
          }
  break;


  case UTIL_KEY_F15:

          if (pressrelease) {
                  puerto_especial4 &=255-16;
          }
          else {
                  puerto_especial4 |=16;
          }
  break;

  default:
    //Para que no se queje el compilador
  break;

}

//printf ("puerto especial 4 en set: %d\n",puerto_especial4);

}

void util_set_reset_key_continue(enum util_teclas tecla,int pressrelease)
{

  //Activar bits de puertos_especiales para teclas F
  //Hay que hacerlo asi para que el menu se vea notificado de pulsacion o no pulsacion de esas teclas, estén o no asignadas a funciones
  //Sirve por ejemplo para que si tenemos mapeado F13 a hard reset, al pulsar F13, cuando se llama a menu_espera_no_tecla, que funcione
  util_set_reset_key_continue_tecla_f(tecla,pressrelease);

  //Ver si hay teclas F redefinidas
  if (util_set_reset_key_continue_f_functions(tecla,pressrelease)) return;

        //temp reasignacion
        //if (tecla==UTIL_KEY_ALT_R) tecla=UTIL_KEY_ENTER;

	switch (tecla) {
                        case UTIL_KEY_SPACE:
                       	case 32:
                                if (pressrelease) {
                                        puerto_32766 &=255-1;
                                        blink_kbd_a13 &= (255-64);
					                              cpc_keyboard_table[5] &= (255-128);
                                        ql_keyboard_table[1] &= (255-64);
                                }
                                else {
                                        puerto_32766 |=1;
                                        blink_kbd_a13 |= 64;
					                              cpc_keyboard_table[5] |= 128;
                                        ql_keyboard_table[1] |= 64;
                                }
                        break;

                        case UTIL_KEY_ENTER:
                                if (pressrelease) {
                                        puerto_49150 &=255-1;
                                        blink_kbd_a8 &= (255-64);
					                              cpc_keyboard_table[0] &= (255-64);
                                        ql_keyboard_table[1] &= (255-1);

					//Avisar de envio enter especial para rutinas de speech, para que envien sonido
					textspeech_send_new_line();

                                }

                                else {
                                        puerto_49150 |=1;
                                        blink_kbd_a8 |=64;
					                              cpc_keyboard_table[0] |= 64;
                                        ql_keyboard_table[1] |= 1;
                                }


					//if (MACHINE_IS_CPC) {
					//	printf ("Temporal en CPC Enter es Fire\n");
//&49     DEL     Joy 1 Fire 3 (CPC only) Joy 1 Fire 2    Joy1 Fire 1     Joy1 right      Joy1 left       Joy1 down       Joy1 up
					//	if (pressrelease) cpc_keyboard_table[9] &=(255-16);
					//	else cpc_keyboard_table[9] |=16;
					//}



                        break;

                        case UTIL_KEY_SHIFT_L:
                        case UTIL_KEY_CAPS_SHIFT:

                                if (pressrelease) menu_capshift.v=1;
                                else menu_capshift.v=0;

                                if (pressrelease) {
                                        puerto_65278  &=255-1;
                                        blink_kbd_a14 &= (255-64);
					                              cpc_keyboard_table[2] &=255-32;
                                        ql_keyboard_table[7] &= (255-1);
                                }
                                else  {
                                        puerto_65278 |=1;
                                        blink_kbd_a14 |= 64;
                                        ql_keyboard_table[7] |= 1;
					cpc_keyboard_table[2] |=32;
                                }
                        break;

                        case UTIL_KEY_SHIFT_R:
                                if (pressrelease) menu_capshift.v=1;
                                else menu_capshift.v=0;

                                if (pressrelease) {
                                        puerto_65278  &=255-1;
                                        blink_kbd_a15 &= (255-128);
					                              cpc_keyboard_table[2] &=255-32;
                                        ql_keyboard_table[7] &= (255-1);
                                }
                                else  {
                                        puerto_65278 |=1;
                                        blink_kbd_a15 |= 128;
					                              cpc_keyboard_table[2] |=32;
                                        ql_keyboard_table[7] |= 1;
                                }
                        break;

                        case UTIL_KEY_ALT_L:

                                util_press_menu_symshift(pressrelease);

                                //printf ("Pulsado ctrl o Alt");
                                if (MACHINE_IS_ZX8081) {
                                       //para zx80/81
                                        //aqui hace lo mismo que mayusculas
                                       if (pressrelease) puerto_65278  &=255-1;
                                        else  puerto_65278 |=1;
                                }


                                else {

//puerto_32766    db              255  ; B    N    M    Simb Space ;7

                                        if (pressrelease) {
                                                puerto_32766  &=255-2;
                                                blink_kbd_a15 &= (255-64);

						//ALT en CPC es CLR
						cpc_keyboard_table[2] &=(255-1);
                                                ql_keyboard_table[7] &= (255-4);
                                        }


                                        else  {
                                                puerto_32766 |=2;
                                                blink_kbd_a15 |= 64;

						//ALT en CPC es CLR
						cpc_keyboard_table[2] |=1;
                                                ql_keyboard_table[7] |= 4;
                                        }
                                }
                        break;

                        case UTIL_KEY_ALT_R:
                                util_press_menu_symshift(pressrelease);

                                //printf ("Pulsado ctrl o Alt");
                                if (MACHINE_IS_ZX8081) {
                                       //para zx80/81
                                        //aqui hace lo mismo que mayusculas
                                       if (pressrelease) puerto_65278  &=255-1;
                                        else  puerto_65278 |=1;
                                }


				//En SAM coupe, alt derecha es Edit
				else if (MACHINE_IS_SAM) {
					if (pressrelease) {
						puerto_teclado_sam_bff9 &= 255-128;
					}
					else	puerto_teclado_sam_bff9 |=128;
				}


                                else {

//puerto_32766    db              255  ; B    N    M    Simb Space ;7

                                        if (pressrelease) {
                                                puerto_32766  &=255-2;
                                                blink_kbd_a15 &= (255-64);

                                                //ALT en CPC es CLR
                                                cpc_keyboard_table[2] &=(255-1);
                                                ql_keyboard_table[7] &= (255-4);
                                        }


                                        else  {
                                                puerto_32766 |=2;
                                                blink_kbd_a15 |= 64;

                                                //ALT en CPC es CLR
                                                cpc_keyboard_table[2] |=1;
                                                ql_keyboard_table[7] |= 4;
                                        }
                                }
                        break;


                        case UTIL_KEY_CONTROL_R:
                        case UTIL_KEY_CONTROL_L:                

                                util_press_menu_symshift(pressrelease);

                                //printf ("Pulsado ctrl");
                                if (MACHINE_IS_ZX8081) {
                                       //para zx80/81
                                        //aqui hace lo mismo que mayusculas
                                        if (pressrelease) puerto_65278  &=255-1;
                                        else  puerto_65278 |=1;
                                }

				//En sam es otra tecla

				else if (MACHINE_IS_SAM) {
					if (pressrelease) puerto_65534 &=255-1;
					else puerto_65534 |=1;
				}


                                else {



//puerto_32766    db              255  ; B    N    M    Simb Space ;7

                                        if (pressrelease) {
                                                puerto_32766  &=255-2;
                                                blink_kbd_a14 &= (255-16);
						cpc_keyboard_table[2] &=(255-128);
                                                ql_keyboard_table[7] &= (255-2);


                                        }


                                        else  {
                                                puerto_32766 |=2;
                                                blink_kbd_a14 |= 16;
						cpc_keyboard_table[2] |=128;
                                                ql_keyboard_table[7] |= 2;

                                        }
                                }
                        break;

                        //Teclas que generan doble pulsacion
                        case UTIL_KEY_BACKSPACE:
                                if (pressrelease) menu_backspace.v=1;
                                else menu_backspace.v=0;

                                if (MACHINE_IS_SAM) {
                                        if (pressrelease) puerto_teclado_sam_eff9 &= (255-128);
                                        else              puerto_teclado_sam_eff9 |= 128;
                                }

				else {
	                                if (pressrelease) {
        	                                puerto_65278 &=255-1;
                	                        puerto_61438 &=255-1;
                        	                blink_kbd_a8 &= (255-128);
						                              cpc_keyboard_table[9] &=(255-128);
                                          ql_pressed_backspace=1;

	                                }
        	                        else {
                	                        puerto_65278 |=1;
                        	                puerto_61438 |=1;
                                	        blink_kbd_a8 |= 128;
						                              cpc_keyboard_table[9] |=128;
                                          ql_pressed_backspace=0;
	                                }
				}
                        break;


                        case UTIL_KEY_HOME:
                                if (pressrelease) {
                                        joystick_set_fire();

					//Home en cpc es copy
					cpc_keyboard_table[1] &=(255-2);
                                }
                                else {
                                        joystick_release_fire();

					//Home en cpc es copy
					cpc_keyboard_table[1] |=2;
                                }
                        break;

                        case UTIL_KEY_LEFT:
                                if (pressrelease) {
                                        //puerto_65278 &=255-1;
                                        //puerto_63486 &=255-16;
                                        joystick_set_left();
                                        blink_kbd_a12 &= (255-64);
					cpc_keyboard_table[1] &=(255-1);

          // 1|   Ret   Left     Up    Esc  Right      \  Space   Down
                                        ql_keyboard_table[1] &= (255-2);
                                }
                                else {
                                        //puerto_65278 |=1;
                                        //puerto_63486 |=16;
                                        joystick_release_left();
                                        blink_kbd_a12 |= 64;
					                              cpc_keyboard_table[1] |=1;
                                        ql_keyboard_table[1] |= 2;
                                }
                        break;
                        case UTIL_KEY_RIGHT:
                                if (pressrelease) {
                                        //puerto_65278 &=255-1;
                                        //puerto_61438 &=255-4;
                                        joystick_set_right();
                                        blink_kbd_a11 &= (255-64);
					                              cpc_keyboard_table[0] &=(255-2);
                                        ql_keyboard_table[1] &= (255-16);
                                }
                                else {
                                        //puerto_65278 |=1;
                                        //puerto_61438 |=4;
                                        joystick_release_right();
                                        blink_kbd_a11 |= 64;
                                        cpc_keyboard_table[0] |=2;
                                        ql_keyboard_table[1] |= 16;
                                }
                        break;

                        case UTIL_KEY_DOWN:
                                if (pressrelease) {
                                        //puerto_65278 &=255-1;
                                        //puerto_61438 &=255-16;
                                        joystick_set_down();

                                        blink_kbd_a10 &= (255-64);
					cpc_keyboard_table[0] &=(255-4);
                                        ql_keyboard_table[1] &= (255-128);

                                }
                                else {
                                        //puerto_65278 |=1;
                                        //puerto_61438 |=16;
                                        joystick_release_down();

                                        blink_kbd_a10 |= 64;
					cpc_keyboard_table[0] |=4;
                                        ql_keyboard_table[1] |= 128;
                                }
                        break;

                        case UTIL_KEY_UP:
                                if (pressrelease) {
                                        //puerto_65278 &=255-1;
                                        //puerto_61438 &=255-8;
                                        joystick_set_up();
                                        blink_kbd_a9 &= (255-64);
					cpc_keyboard_table[0] &=(255-1);
                                        ql_keyboard_table[1] &= (255-4);
                                }
                                else {
                                        //puerto_65278 |=1;
                                        //puerto_61438 |=8;
                                        joystick_release_up();
                                        blink_kbd_a9 |= 64;
					cpc_keyboard_table[0] |=1;
                                        ql_keyboard_table[1] |=4;
                                }
                        break;

                        case UTIL_KEY_TAB:
                                if (pressrelease) {
                                        menu_tab.v=1; 
                                        //printf ("Pulsado TAB\n");
                                }
                                else {
                                        menu_tab.v=0;
                                        //printf ("Liberado TAB\n");
                                }

				if (MACHINE_IS_SAM) {
					if (pressrelease) puerto_teclado_sam_f7f9 &= (255-64);
					else              puerto_teclado_sam_f7f9 |= 64;
				}

				else {
	                                if (pressrelease) {
                                        	puerto_65278 &=255-1;
                                	        puerto_32766 &=255-2;
                        	                blink_kbd_a14 &= (255-32);
						                              cpc_keyboard_table[8]&= (255-16);
                                          // 2|     9      w      i    Tab      r      -      y      o     ql_keyboard_table[5]
                                          ql_keyboard_table[5] &=(255-8);
        	                        }



	                                else {
                                	        puerto_65278 |=1;
                        	                puerto_32766 |=2;
                	                        blink_kbd_a14 |= 32;
						                              cpc_keyboard_table[8]|= 16;
                                          ql_keyboard_table[5] |=8;
	                                }
				}
                        break;

                        case UTIL_KEY_CAPS_LOCK:
				if (MACHINE_IS_SAM) {
                                        if (pressrelease) puerto_teclado_sam_f7f9 &= (255-128);
                                        else              puerto_teclado_sam_f7f9 |= 128;
                                }

				else {

	                                if (pressrelease) {
        	                                puerto_65278 &=255-1;
                	                        puerto_63486 &=255-2;
                        	                blink_kbd_a15 &= (255-8);
						                              cpc_keyboard_table[8] &=(255-64);
                                          // 4|     [   Caps      k      s      f      =      g      ;     ql_keyboard_table[3]
                                          ql_keyboard_table[3] &=(255-2);
	                                }
        	                        else {
                	                        puerto_65278 |=1;
                        	                puerto_63486 |=2;
                                	        blink_kbd_a15 |= 8;
						                              cpc_keyboard_table[8] |=64;
                                          ql_keyboard_table[3] |=2;
	                                }
				}
                        break;

                        case UTIL_KEY_COMMA:
                        case ',':
                                util_press_menu_symshift(pressrelease);
                                //Nota: para tanto , como . hay un problema en zx80/81 con la tecla en el menu,
                                //pues no genera sym +m y n como cabria esperar
                                //en ese caso, hay que pulsar ctrl/alt + m o n          
				if (MACHINE_IS_ZX8081) {
					if (pressrelease) {
						puerto_32766 &=255-2;
						puerto_65278 &=255-1;
					}
					else {
						puerto_32766 |=2;
						puerto_65278 |=1;
					}
				}


				else {


	                                if (pressrelease) {
        	                                puerto_32766 &=255-2-8;

                	                }
                        	        else {
                                	        puerto_32766 |=2+8;

	                                }


					//Temp para CPC. Quiza hay que tener en cuenta keymap
					if (pressrelease) {
                                                cpc_keyboard_table[4] &=255-128;
                                        }
                                        else {
                                                cpc_keyboard_table[4] |=128;
                                        }
				}
                        break;


                        //Punto
                        case UTIL_KEY_PERIOD:
                        case '.':
                                util_press_menu_symshift(pressrelease);
                                //Nota: para tanto , como . hay un problema en zx80/81 con la tecla en el menu,
                                //pues no genera sym +m y n como cabria esperar
                                //en ese caso, hay que pulsar ctrl/alt + m o n                                
                                if (MACHINE_IS_ZX8081) {
                                        //para zx80/81
                                                if (pressrelease) puerto_32766  &=255-2;
                                        else  puerto_32766 |=2;
                                }

                                else {
                                        if (pressrelease) {
                                                puerto_32766 &=255-2-4;

                                        }
                                        else {
                                                puerto_32766 |=2+4;

                                        }
                                }
                        break;

                        case UTIL_KEY_MINUS:

                                if (pressrelease) {
                                        set_symshift();
                                        puerto_49150 &=255-8;
                                }
                                else {
                                        clear_symshift();
                                        puerto_49150 |=8;
                                }
                        break;
                       case UTIL_KEY_PLUS:
                                if (pressrelease) {
                                        set_symshift();
                                        puerto_49150 &=255-4;
                                }
                                else {
                                        clear_symshift();
                                        puerto_49150 |=4;
                                }
                        break;

                        case UTIL_KEY_SLASH:
                                if (pressrelease) {
                                        set_symshift();
                                        puerto_65278 &=255-16;
                                }
                                else {
                                        clear_symshift();
                                        puerto_65278 |=16;
                                }
                        break;

                        case UTIL_KEY_ASTERISK:
                                if (pressrelease) {
                                        set_symshift();
                                        puerto_32766 &=255-16;
                                }
                                else {
                                        clear_symshift();
                                        puerto_32766 |=16;
                                }
                        break;

                        //F1 pulsado
                        case UTIL_KEY_F1:

                                if (pressrelease) {
                                        blink_kbd_a14 &= (255-128);
                                        ql_keyboard_table[0] &= (255-2);
                                }
                                else {
                                        blink_kbd_a14 |= 128;
                                        ql_keyboard_table[0] |= 2;
                                }
                        break;

                        //F2 pulsado
                        case UTIL_KEY_F2:

                                if (pressrelease) {
                                        blink_kbd_a15 &= (255-16);
                                        ql_keyboard_table[0] &= (255-8);
                                }
                                else {
                                        blink_kbd_a15 |= 16;
                                        ql_keyboard_table[0] |= 8;
                                }
                        break;

//// 7|    F4     F1      5     F2     F3     F5      4      7
                        //F3 pulsado
                        case UTIL_KEY_F3:

                                if (pressrelease) {
                                        blink_kbd_a14 &= (255-8);
                                        ql_keyboard_table[0] &= (255-16);
                                }
                                else {
                                        blink_kbd_a14 |= 8;
                                        ql_keyboard_table[0] |= 16;
                                }
                        break;

			                    //F4 pulsado. Volcar pantalla a speech
			                       case UTIL_KEY_F4:
				                         if (pressrelease) {
                                   textspeech_enviar_speech_pantalla();
                                   ql_keyboard_table[0] &= (255-1);
                                 }

                                 else {
                                   ql_keyboard_table[0] |= 1;
                                 }
			                      break;

                        //F5 pulsado
                        case UTIL_KEY_F5:

                                if (pressrelease) {
					if (util_if_open_just_menu() )  menu_fire_event_open_menu();
                                        ql_keyboard_table[0] &= (255-32); //Caso especial F5 y QL
                                }
                                else {
                                        ql_keyboard_table[0] |= 32;
                                }
                        break;

                        //z80_byte puerto_especial3=255; //  F10 F9 F8 F7 F6

                        //F6 pulsado. De momento no hace nada
                        case UTIL_KEY_F6:

                                if (pressrelease) {

                                }
                                else {

                                }
                        break;

                        //F7 pulsado. Uso del simulador de joystick si esta habilitado
                        case UTIL_KEY_F7:

                                if (pressrelease) {
					if (simulador_joystick) {
						simulador_joystick_forzado=1;
					}
                                }
                                else {

                                }
                        break;


			//F8 pulsado. On Screen keyboard, De momento solo en spectrum
                        case UTIL_KEY_F8:
				                      if (pressrelease) {
				                            menu_abierto=1;
				                             menu_button_osdkeyboard.v=1;
                     	        }
                              else {
                              }

                        break;


                        //F9 pulsado. quickload
                        case UTIL_KEY_F9:

                                if (pressrelease) {
                                        menu_abierto=1;
                                        menu_button_quickload.v=1;
                                }
                                else {
                                }

                        break;

                        //F10 pulsado
                        case UTIL_KEY_F10:

                                if (pressrelease) {
                                }
                                else {
                                }
                        break;


                        //z80_byte puerto_especial3=255; //  F10 F9 F8 F7 F6


                        //z80_byte puerto_especial4=255; //  F15 F14 F13 F12 F11

                        //F11 pulsado. De momento no hace nada
                        case UTIL_KEY_F11:

                                if (pressrelease) {
                                }
                                else {
                                }
                        break;

                        //F12 pulsado. De momento no hace nada
                        case UTIL_KEY_F12:

                                if (pressrelease) {
                                }
                                else {
                                }
                        break;

                        //F13 pulsado. De momento no hace nada
                        case UTIL_KEY_F13:

                                if (pressrelease) {
                                }
                                else {
                                }
                        break;

                        //F14 pulsado. De momento no hace nada
                        case UTIL_KEY_F14:

                                if (pressrelease) {
                                }
                                else {
                                }
                        break;

                        //F15 pulsado. De momento no hace nada
                        case UTIL_KEY_F15:

                                if (pressrelease) {
                                }
                                else {
                                }
                        break;




                        //ESC pulsado. Para Z88 y tecla menu
                        case UTIL_KEY_ESC:
//A15 (#7) | RSH    SQR     ESC     INDEX   CAPS    .       /       £
                          //ESC para Spectrum es BREAK, siempre que no este text to speech habilitado
                        if (MACHINE_IS_SPECTRUM && textspeech_filter_program==NULL) {
                          if (pressrelease) {
                            puerto_65278 &=255-1;
                            puerto_32766 &=255-1;
                          }
                          else {
                            puerto_65278 |=1;
                            puerto_32766 |=1;
                          }
                        }


				if (pressrelease) {
					blink_kbd_a15 &= (255-32);
					puerto_especial1 &=255-1;
					cpc_keyboard_table[8] &=(255-4);
					puerto_teclado_sam_f7f9 &= (255-32);
          // 6|   Ret   Left     Up    Esc  Right      \  Space   Down     ql_keyboard_table[1]
          ql_keyboard_table[1] &= (255-8);

	                                //Vaciamos cola speech
        	                        textspeech_empty_speech_fifo();

				}
				else {
					blink_kbd_a15 |=32;
					puerto_especial1 |=1;
					cpc_keyboard_table[8] |=4;
					puerto_teclado_sam_f7f9 |= 32;
                                        ql_keyboard_table[1] |= 8;
				}



			break;

                        //PgUP
                        case UTIL_KEY_PAGE_UP:

                                if (pressrelease) {
                                        puerto_especial1 &=255-2;
                                }
                                else {
                                        puerto_especial1 |=2;
                                }
                        break;

                        //PgDn
                        case UTIL_KEY_PAGE_DOWN:

                                if (pressrelease) {
                                        puerto_especial1 &=255-4;
                                }
                                else {
                                        puerto_especial1 |=4;
                                }
                        break;


			//Teclas del keypad
			case UTIL_KEY_KP0:
				if (pressrelease) {
					cpc_keyboard_table[1] &=(255-128);
					puerto_teclado_sam_dff9 &=(255-128);
				}
				else {
					cpc_keyboard_table[1] |=128;
					puerto_teclado_sam_dff9 |=128;
				}
			break;

			case UTIL_KEY_KP1:
				if (pressrelease) {
					cpc_keyboard_table[1] &=(255-32);
					puerto_teclado_sam_fef9 &=(255-32);
				}
				else {
					cpc_keyboard_table[1] |=32;
					puerto_teclado_sam_fef9 |=32;
				}
			break;

			case UTIL_KEY_KP2:
				if (pressrelease) {
					cpc_keyboard_table[1] &=(255-64);
					puerto_teclado_sam_fef9 &=(255-64);
				}
				else {
					cpc_keyboard_table[1] |=64;
					puerto_teclado_sam_fef9 |=64;
				}
			break;

			case UTIL_KEY_KP3:
				if (pressrelease) {
					cpc_keyboard_table[0] &=(255-32);
					puerto_teclado_sam_fef9 &=(255-128);
				}
				else {
					cpc_keyboard_table[0] |=32;
					puerto_teclado_sam_fef9 |=128;
				}
			break;

			case UTIL_KEY_KP4:
				if (pressrelease) {
					cpc_keyboard_table[2] &=(255-16);
					puerto_teclado_sam_fdf9 &=(255-32);
				}
				else {
					cpc_keyboard_table[2] |=16;
					puerto_teclado_sam_fdf9 |=32;
				}
			break;

			case UTIL_KEY_KP5:
				if (pressrelease) {
					cpc_keyboard_table[1] &=(255-16);
					puerto_teclado_sam_fdf9 &=(255-64);
				}
				else {
					cpc_keyboard_table[1] |=16;
					puerto_teclado_sam_fdf9 |=64;
				}
			break;

			case UTIL_KEY_KP6:
				if (pressrelease) {
					cpc_keyboard_table[0] &=(255-16);
					puerto_teclado_sam_fdf9 &=(255-128);
				}
				else {
					cpc_keyboard_table[0] |=16;
					puerto_teclado_sam_fdf9 |=128;
				}
			break;

			case UTIL_KEY_KP7:
				if (pressrelease) {
					cpc_keyboard_table[1] &=(255-4);
					puerto_teclado_sam_fbf9 &=(255-32);
				}
				else {
					cpc_keyboard_table[1] |=4;
					puerto_teclado_sam_fbf9 |=32;
				}
			break;

			case UTIL_KEY_KP8:
				if (pressrelease) {
					cpc_keyboard_table[1] &=(255-8);
					puerto_teclado_sam_fbf9 &=(255-64);
				}
				else {
					cpc_keyboard_table[1] |=8;
					puerto_teclado_sam_fbf9 |=64;
				}
			break;

			case UTIL_KEY_KP9:
				if (pressrelease) {
					cpc_keyboard_table[0] &=(255-8);
					puerto_teclado_sam_fbf9 &=(255-128);
				}
				else {
					cpc_keyboard_table[0] |=8;
					puerto_teclado_sam_fbf9 |=128;
				}
			break;

			case UTIL_KEY_KP_COMMA:
				if (pressrelease) {
					cpc_keyboard_table[0] &=(255-128);
				}
				else {
					cpc_keyboard_table[0] |=128;
				}
			break;

			case UTIL_KEY_KP_ENTER:
				if (pressrelease) {
					cpc_keyboard_table[0] &=(255-64);
				}
				else {
					cpc_keyboard_table[0] |=64;
				}
			break;

			case UTIL_KEY_WINKEY:
				//Esto no se usa aqui, solo lo usa en la rutina de Chloe. Lo pongo para que no se queje de Warning al compilar
			break;


			case UTIL_KEY_NONE:
				//Esto no se usa aqui, solo lo usa en la rutina de Chloe. Lo pongo para que no se queje de Warning al compilar
				//Ademas esto identifica cuando no habia tecla previa en Chloe, no tiene sentido aqui en el switch gestionarlo
			break;

			//NO: Chapuza. Para gestion de z y b del teclado recreated
			//La solucion elegante seria que para activar un puerto de tecla, se usase solo util_set_reset_keys, 
			//y nadie llamase a convert_numeros_letras_puerto_teclado . Para ello, todas las teclas fuera de lo ascii (ctrl, alt, etc)
			//deben tener una numeracion fuera de lo ascii normal (128 en adelante?)
			//Esta chapuza que hago tambien esta provocando que se llame a la funcion de recreated_zx_spectrum_keyboard_convert
			//desde dos sitios, cuando deberia llamarse solo desde un sitio
			//case 'z':
			//case 'b':
			//	convert_numeros_letras_puerto_teclado_continue_after_recreated(tecla,pressrelease);
			//break;


			default:
				//Caso entre a-z y 0-9
			if (
                        (tecla>='a' && tecla<='z') ||
                        (tecla>='0' && tecla<='9')
			)
				{
					convert_numeros_letras_puerto_teclado_continue_after_recreated(tecla,pressrelease);
				}
			break;




	}
}



//Retorna numero parseado. Si acaba en H, se supone que es hexadecimal
//Si acaba en 
//Si empieza por ' o "" es un caracter (1 solo caracter, no un string)
//Valor de retorno unsigned de 32 bits
unsigned int parse_string_to_number(char *texto)
{
	int value;

	int l=strlen(texto);
	if (l==0) return 0;


	//Si empieza por ' o ""
	if (texto[0]=='\'' || texto[0]=='"') return texto[1];

	//sufijo. Buscar ultimo caracter antes de final de cadena o espacio. Asi podemos parsear cosas como "20H 32 34", y se interpretara solo el 20H
        int posicion_sufijo=0;
        for (;texto[posicion_sufijo]!=0 && texto[posicion_sufijo]!=' ';posicion_sufijo++);
        posicion_sufijo--;

  //int posicion_sufijo=l-1;


	char sufijo=texto[posicion_sufijo];
	if (sufijo=='H' || sufijo=='h') {
		//hexadecimal
		//quitamos sufijo y parseamos
		texto[posicion_sufijo]=0;
		value=strtol(texto, NULL, 16);
		//volvemos a dejar sufijo tal cual
		texto[posicion_sufijo]=sufijo;
		return value;
	}

        if (sufijo=='%') {
		//binario
		//quitamos sufijo y parseamos
		texto[posicion_sufijo]=0;
		value=strtol(texto, NULL, 2);
		//volvemos a dejar sufijo tal cual
		texto[posicion_sufijo]=sufijo;
		return value;
	}

	//decimal
        return atoi(texto);

}

/*int ascii_to_hexa(char c)
{
  if (c>='0' && c<='9') return c-'0';
  if (c>='a' && c<='f') return c-'a'+10;
  if (c>='A' && c<='F') return c-'A'+10;
  return 0;
}*/

//Retorna numero grande parseado. Si acaba en H, se supone que es hexadecimal
//Si empieza por ' o "" es un caracter (1 solo caracter, no un string)
/*//Valor de retorno unsigned de 64 bits
long long int parse_string_to_long_number(char *texto)
{
	long long int value;

  while (*texto && *texto!=' ') {
    printf("t: [%c]",*texto);
    value=value*16;
    value +=ascii_to_hexa(*texto);
    texto++;
  }

  return value;


}*/


//carpeta temporal usada por el emulador
char emulator_tmpdir[PATH_MAX]="";

//carpeta temporal que ha especificado el usuario
char emulator_tmpdir_set_by_user[PATH_MAX]="";

//Retorna directorio temporal para el usuario y lo crea
//Formato: /tmp/zesarux-<uid>
//donde <uid> es el uid del usuario
char *get_tmpdir_base(void)
{

	//Si lo ha cambiado el usuario
	if (emulator_tmpdir_set_by_user[0]!=0) strcpy(emulator_tmpdir,emulator_tmpdir_set_by_user);


	if (emulator_tmpdir[0]==0) {

#ifndef MINGW
		int uid=getuid();
		sprintf (emulator_tmpdir,"/tmp/zesarux-%d",uid);
#else
		//Obtener ruta temporal raiz c:\windows
		char windows_temp[PATH_MAX];

		char *env_tmp,*env_temp,*env_userprofile,*env_systemroot;
        	env_tmp=getenv("TMP");
        	env_temp=getenv("TEMP");
		env_userprofile=getenv("USERPROFILE");
		env_systemroot=getenv("SystemRoot");


		//Ir probando uno a uno
		if (env_tmp!=NULL) sprintf (windows_temp,"%s",env_tmp);
		else if (env_temp!=NULL) sprintf (windows_temp,"%s",env_temp);
		else if (env_userprofile!=NULL) sprintf (windows_temp,"%s",env_userprofile);
		else if (env_systemroot!=NULL) sprintf (windows_temp,"%s",env_systemroot);
		else {
			//como ultima instancia, c:\windows\temp
			sprintf (windows_temp,"%s","C:\\windows\\temp\\");
		}

		debug_printf (VERBOSE_DEBUG,"Windows Temporary dir: %s",windows_temp);

		//Obtener un uid unico para usuario
		char template_dir[PATH_MAX];
		sprintf (template_dir,"%s\\zesarux-XXXXXX",windows_temp);

		char *dir;
		dir=mkdtemp(template_dir);

                if (dir==NULL) {
                        debug_printf (VERBOSE_DEBUG,"Error getting temporary directory: %s",strerror(errno) );
                }
                else {
                        sprintf(emulator_tmpdir,"%s",dir);
			debug_printf (VERBOSE_DEBUG,"ZEsarUX Temporary dir: %s",emulator_tmpdir);
                }

#endif


		debug_printf (VERBOSE_INFO,"Creating new temporary directory %s",emulator_tmpdir);


#ifndef MINGW
	      mkdir(emulator_tmpdir,S_IRWXU);
#else
	      mkdir(emulator_tmpdir);
#endif


	}

	return emulator_tmpdir;
}

#define FRECUENCIA_SONIDO_RWA_FILES 15600

int convert_smp_to_rwa(char *origen, char *destino)
{

	FILE *ptr_origen;
	ptr_origen=fopen(origen,"rb");
	if (ptr_origen==NULL) {
		debug_printf (VERBOSE_ERR,"Error reading source file");
		return 1;
	}

        FILE *ptr_destino;
        ptr_destino=fopen(destino,"wb");
        if (ptr_destino==NULL) {
		debug_printf (VERBOSE_ERR,"Error creating target file: %s",destino);
		return 1;
	}

	unsigned char last_value=0;
	unsigned char previous_last_value=0;

	#define FREQ_SMP 11111

	int contador=FRECUENCIA_SONIDO_RWA_FILES;
	int frecuencia_origen=FREQ_SMP;

	while (!feof(ptr_origen)) {

		//"Sampleamos" frecuencia original a 15600

		contador=contador-frecuencia_origen;
		if (contador<=0) {
			fread(&last_value, 1,1 , ptr_origen);
			contador +=FRECUENCIA_SONIDO_RWA_FILES;
		}



		//fwrite(&last_value,1,1,ptr_destino);
		//en vez de escribir ultimo valor tal cual, hacer la media de los dos anteriores
		int media=(previous_last_value+last_value)/2;
		unsigned char media8=media;
		fwrite(&media8,1,1,ptr_destino);

		previous_last_value=last_value;
	}


	fclose(ptr_origen);
	fclose(ptr_destino);

	return 0;

}


int convert_wav_to_rwa(char *origen, char *destino)
{



	char sox_command[PATH_MAX];

	char sox_program[PATH_MAX];



	//sprintf (sox_program,"/usr/bin/sox");
	//sprintf (sox_command,"/usr/bin/sox \"%s\" -t .raw -r %d -b 8 -e unsigned -c 1 \"%s\"",origen,FRECUENCIA_SONIDO_RWA_FILES,destino);
	sprintf (sox_program,"%s",external_tool_sox);
	sprintf (sox_command,"%s \"%s\" -t .raw -r %d -b 8 -e unsigned -c 1 \"%s\"",external_tool_sox,origen,FRECUENCIA_SONIDO_RWA_FILES,destino);




	if (!si_existe_archivo(sox_program)) {
		debug_printf(VERBOSE_ERR,"Unable to find sox program: %s",sox_program);
		return 1;
	}


	debug_printf (VERBOSE_DEBUG,"Running %s command",sox_command);

	if (system (sox_command)==-1) {
		debug_printf (VERBOSE_DEBUG,"Error running command %s",sox_command);
		return 1;
	}

	return 0;

}

char tzx_frecuencia_sonido[10];

int convert_tzx_to_rwa(char *origen, char *destino)
{

        //char nombre_origen[NAME_MAX];
        //util_get_file_no_directory(origen,nombre_origen);

        //sprintf (destino,"%s/tmp_%s.rwa",get_tmpdir_base(),nombre_origen);
        //debug_printf (VERBOSE_INFO,"Creating temporary file %s",destino);

	//playtzx-0.12c/playtzx -au -freq 15600 Rambo.tzx Rambo.rwa
	char *argumentos[]={
		"playtzx","-au","-freq","","",""
	};

	//sprintf (tzx_frecuencia_sonido,"%d",FRECUENCIA_SONIDO);
	//frecuencia de sonido a generar archivo siempre 15600, independientemente del % de cpu
	sprintf (tzx_frecuencia_sonido,"%d",FRECUENCIA_SONIDO_RWA_FILES);


	argumentos[3]=tzx_frecuencia_sonido;
	argumentos[4]=origen;
	argumentos[5]=destino;

	return main_playtzx(6,argumentos);
}

void convert_o_p_to_rwa_write_silence(FILE *ptr_archivo,int segundos)
{
        int i;
        unsigned char escrito;

        escrito=128;
        for (i=0;i<FRECUENCIA_SONIDO_RWA_FILES*segundos;i++) {
                fwrite(&escrito,1,1,ptr_archivo);
        }

}


//escribe 1 o 0
void convert_o_p_to_rwa_write_bit(FILE *ptr_archivo,int bit,unsigned char valor_high,unsigned char valor_low)
{

/*
  0:  /\/\/\/\________
  1:  /\/\/\/\/\/\/\/\/\________

        //150 us high: byte:  1 1
	//byte 0 de punto medio
        //150 us low: byte: 0 0

        //silencio: 1300 us -> 22 bytes a 0


	Lo unico que depende de la frecuencia es esos 2 bytes a 1 y los dos bytes a 0 y los 22 bytes de silencio de final de bit

*/



	int longitud;

	//Bit a 1 son 9 ondas. Bit a 0 son 4 ondas
	if (bit) longitud=9;
	else longitud=4;

	z80_byte escrito;
	int i;
	for (i=0;i<longitud;i++) {

		//2 bytes arriba
		fwrite(&valor_high,1,1,ptr_archivo);
		fwrite(&valor_high,1,1,ptr_archivo);

		//1 byte 0. esto podria ser -1 a efectos practicos. pero se pone 0 para que "visualmente" quede mejor
		escrito=128;
		fwrite(&escrito,1,1,ptr_archivo);

		//2 bytes -1
		fwrite(&valor_low,1,1,ptr_archivo);
		fwrite(&valor_low,1,1,ptr_archivo);
	}

	escrito=128;
	for (i=0;i<22;i++) {
		fwrite(&escrito,1,1,ptr_archivo);
	}
}


void convert_o_p_to_rwa_write_byte(FILE *ptr_archivo,unsigned char leido)
{

#define ZX8081_AMPLITUD_BIT 50
                //meter los 8 bits. en orden, primero el mas alto
                int numerobit,bit;
                for (numerobit=0;numerobit<8;numerobit++) {
                        if (leido&128) bit=1;
                        else bit=0;

                        leido=leido<<1;


                        convert_o_p_to_rwa_write_bit(ptr_archivo,bit,128+ZX8081_AMPLITUD_BIT,128-ZX8081_AMPLITUD_BIT);
                }
}

int convert_o_p_to_rwa(char *origen, char *destino,int si_p)
{


/*
Bits and Bytes
Each byte consists of 8 bits (MSB first) without any start and stop bits, directly followed by the next byte. A "0" bit consists of four high pulses, a "1" bit of nine pulses, either one followed by a silence period.

  0:  /\/\/\/\________
  1:  /\/\/\/\/\/\/\/\/\________

Each pulse is split into a 150us High period, and 150us Low period. The duration of the silence between each bit is 1300us. The baud rate is thus 400 bps (for a "0" filled area) downto 250 bps (for a "1" filled area). Average medium transfer rate is approx. 307 bps (38 bytes/sec) for files that contain 50% of "0" and "1" bits each.

*/

	//en senyal rwa a 15600 hz, cada pulso de 150+150 us dura aprox 5 bytes -> 0,00032 s
	//300 us -> 1/1000000 * 300 = 0,0003

	//150 us high: byte:  1 1
	//byte 0 de punto medio
	//150 us low: byte: -1 -1

	//silencio: 1300 us -> 22 bytes a 0


        FILE *ptr_origen;
        ptr_origen=fopen(origen,"rb");
        if (ptr_origen==NULL) {
                debug_printf (VERBOSE_ERR,"Error reading source file");
                return 1;
        }

        FILE *ptr_destino;
        ptr_destino=fopen(destino,"wb");
        if (ptr_destino==NULL) {
                debug_printf (VERBOSE_ERR,"Error creating target file: %s",destino);
                return 1;
        }

	//metemos 7 segundos de silencio
	convert_o_p_to_rwa_write_silence(ptr_destino,7);

        unsigned char leido;


	//Si ZX81, metemos nombre al principio. Nombre es inventado, solo "A"
	if (si_p) {

	//Metemos nombre
	//unsigned char nombre='A';
	unsigned char nombre=38; //38 = A en ZX81
	nombre |=128;

		convert_o_p_to_rwa_write_byte(ptr_destino,nombre);
	}


        //metemos datos
        while (!feof(ptr_origen)) {
		fread(&leido, 1,1 , ptr_origen);
		convert_o_p_to_rwa_write_byte(ptr_destino,leido);
        }

	//metemos 3 segundos de silencio
	convert_o_p_to_rwa_write_silence(ptr_destino,3);

        fclose(ptr_origen);
        fclose(ptr_destino);

        return 0;
}


int convert_o_to_rwa(char *origen, char *destino)
{
	return convert_o_p_to_rwa(origen,destino,0);
}

int convert_p_to_rwa(char *origen, char *destino)
{
        return convert_o_p_to_rwa(origen,destino,1);
}


void convert_tap_to_rwa_write_silence(FILE *ptr_archivo,int segundos)
{
        int i;
        unsigned char escrito;

        escrito=128;
        for (i=0;i<FRECUENCIA_SONIDO_RWA_FILES*segundos;i++) {
                fwrite(&escrito,1,1,ptr_archivo);
        }

}



//escribe 1 o 0
void convert_tap_to_rwa_write_bit(FILE *ptr_archivo,int bit,unsigned char valor_high,unsigned char valor_low)
{


	int longitud;

	//Bit a 1 son 4 bytes high , 4 low
	if (bit==0) longitud=4;
	else longitud=8;

	//z80_byte escrito;
	int i;
	for (i=0;i<longitud;i++) {

		//bytes arriba
		fwrite(&valor_high,1,1,ptr_archivo);

	}

	for (i=0;i<longitud;i++) {

		//bytes abajo
		fwrite(&valor_low,1,1,ptr_archivo);

	}


}


void convert_tap_to_rwa_write_byte(FILE *ptr_archivo,unsigned char leido)
{

#define SPEC_AMPLITUD_BIT 50
                //meter los 8 bits. en orden, primero el mas alto
                int numerobit,bit;
                for (numerobit=0;numerobit<8;numerobit++) {
                        if (leido&128) bit=1;
                        else bit=0;

                        leido=leido<<1;


                        convert_tap_to_rwa_write_bit(ptr_archivo,bit,128+SPEC_AMPLITUD_BIT,128-SPEC_AMPLITUD_BIT);
                }
}

void convert_tap_to_rwa_write_pilot(FILE *ptr_archivo,z80_byte flag_bloque)
{

#define SPEC_AMPLITUD_PILOT 50

////Tono guia: 10 bytes high. 10 bytes low.

	int ondas;

        int longitud=10;

        z80_byte escrito;
        int i;

//8063 (Header) or 3223 (Data) Pilot Pulses
//flag 0: 8063/2
//flag 255: 3223/2

	int totalondas;

	int diferenciamaxmin=8063-3223;

	flag_bloque=255-flag_bloque;

	totalondas=3223+(diferenciamaxmin*flag_bloque/256);

	debug_printf (VERBOSE_DEBUG,"Number of pilot tone waves: %d",totalondas);

	for (ondas=0;ondas<totalondas/2;ondas++) {

	        for (i=0;i<longitud;i++) {
			escrito=128+SPEC_AMPLITUD_PILOT;
                	//bytes arriba
	                fwrite(&escrito,1,1,ptr_archivo);

        	}

	        for (i=0;i<longitud;i++) {
			escrito=128-SPEC_AMPLITUD_PILOT;

	                //bytes abajo
        	        fwrite(&escrito,1,1,ptr_archivo);

	        }
	}
}

void convert_tap_to_rwa_write_sync_false(FILE *ptr_archivo)
{

#define SPEC_AMPLITUD_SYNC_FALSE 50
        //Metemos onda "falsa" de sincronismo
        //convert_tap_to_rwa_write_sync_false(ptr_destino);
	//        //Primero sync pulse (high): 190us-> 3 bytes
        //Segundo sync pulse (low): 210us->3 bytes

	z80_byte escrito;
	escrito=128+SPEC_AMPLITUD_SYNC_FALSE;

	fwrite(&escrito,1,1,ptr_archivo);
	fwrite(&escrito,1,1,ptr_archivo);
	fwrite(&escrito,1,1,ptr_archivo);

        escrito=128-SPEC_AMPLITUD_SYNC_FALSE;

        fwrite(&escrito,1,1,ptr_archivo);
        fwrite(&escrito,1,1,ptr_archivo);
        fwrite(&escrito,1,1,ptr_archivo);
}




//Convierte archivo tap a rwa en destino indicado
int convert_tap_to_rwa(char *origen, char *destino)
{
        //char nombre_origen[NAME_MAX];
        //util_get_file_no_directory(origen,nombre_origen);

        //sprintf (destino,"%s/tmp_%s.rwa",get_tmpdir_base(),nombre_origen);
        //debug_printf (VERBOSE_INFO,"Creating temporary file %s",destino);




        FILE *ptr_origen;
        ptr_origen=fopen(origen,"rb");
        if (ptr_origen==NULL) {
                debug_printf (VERBOSE_ERR,"Error reading source file");
                return 1;
        }

        FILE *ptr_destino;
        ptr_destino=fopen(destino,"wb");
        if (ptr_destino==NULL) {
                debug_printf (VERBOSE_ERR,"Error creating target file: %s",destino);
                return 1;
        }


/*
	//Tono guia: 10 bytes high. 10 bytes low. cada pulso entero (+1 y -1 ) dura: 0.001280 segundos. medio pulso dura 0.000641 -> aprox 619us/pulse. cada byte son 64.102564 us
	//Primero sync pulse (high): 190us-> 3 bytes
	//Segundo sync pulse (low): 210us->3 bytes

	//bit a 0: 244 us -> 3.8 bytes-> 4 bytes a high. 4 bytes a low -> 8 total onda que dura el bit 0
	//bit a 1: 488 us-> 7.6 bytes -> 8 bytes a high. 8 bytes a low ->16 total onda que dura el bit 1


Spectrum Cassette Blocks
  Silence (low level)
  8063 (Header) or 3223 (Data) Pilot Pulses (2168 clks/pulse) (619us/pulse)-> 0.000619
  1st Sync Pulse (667 clks/pulse) (190us/pulse)
  2nd Sync Pulse (735 clks/pulse) (210us/pulse)
  Blocktype Byte (00h=Header, FFh=Data)
  Data Byte(s) (two 855 or 1710 clks/pulse per bit) (244 or 488 us/pulse)
  Checksum Byte (above Blocktype and Data Bytes XORed with each other)
  End pulse (opposite level of last pulse)
  Silence (low level)

*/

	//Bucle hasta que acabe tap
	//metemos 2 segundos de silencio
	convert_tap_to_rwa_write_silence(ptr_destino,2);

	while (!feof(ptr_origen)) {

        	unsigned char leido;



		//Leer longitud de lo que viene
		z80_int longitud_bloque;

		fread(&leido, 1,1 , ptr_origen);

		//aqui puede haber feof
		if (!feof(ptr_origen)) {

			longitud_bloque=leido;

		        fread(&leido, 1,1 , ptr_origen);
        		longitud_bloque +=leido*256;

			z80_byte flag_bloque;
			fread(&flag_bloque, 1,1 , ptr_origen);

			debug_printf (VERBOSE_INFO,"Generating audio tape block. Length (without flag and checksum): %d, flag: %d",longitud_bloque-2,flag_bloque);

			//Meter tono guia, longitud de tono guia depende de flag
			convert_tap_to_rwa_write_pilot(ptr_destino,flag_bloque);

			//Metemos onda "falsa" de sincronismo
			convert_tap_to_rwa_write_sync_false(ptr_destino);

			//Metemos flag
			convert_tap_to_rwa_write_byte(ptr_destino,flag_bloque);
			//Meter datos
			//bucle de longitud bloque.

			//restamos el flag que ya esta escrito
			longitud_bloque--;

			while (longitud_bloque>0) {


				fread(&leido, 1,1 , ptr_origen);
				convert_tap_to_rwa_write_byte(ptr_destino,leido);

				longitud_bloque--;

			}

			//fin bucle longitud bloque

			//metemos 1 segundo de silencio
			convert_tap_to_rwa_write_silence(ptr_destino,1);

			//seguir a siguiente bloque dentro de tap
		}

	}


        fclose(ptr_origen);
        fclose(ptr_destino);

        return 0;
}

//Crea carpeta temporal y asigna nombre para archivo temporal rwa
void convert_to_rwa_common_tmp(char *origen, char *destino)
{
        char nombre_origen[NAME_MAX];
        util_get_file_no_directory(origen,nombre_origen);

        sprintf (destino,"%s/tmp_%s.rwa",get_tmpdir_base(),nombre_origen);
        debug_printf (VERBOSE_INFO,"Creating temporary file %s",destino);
}


//Convierte archivo tap a rwa en carpeta temporal, generando nombre de archivo destino
int convert_tap_to_rwa_tmpdir(char *origen, char *destino)
{
	convert_to_rwa_common_tmp(origen,destino);

	return convert_tap_to_rwa(origen,destino);
}


//Convierte archivo tzx a rwa en carpeta temporal, generando nombre de archivo destino
int convert_tzx_to_rwa_tmpdir(char *origen, char *destino)
{
	convert_to_rwa_common_tmp(origen,destino);

        return convert_tzx_to_rwa(origen,destino);

}

int convert_smp_to_rwa_tmpdir(char *origen, char *destino)
{
        convert_to_rwa_common_tmp(origen,destino);

        return convert_smp_to_rwa(origen,destino);

}

int convert_wav_to_rwa_tmpdir(char *origen, char *destino)
{
        convert_to_rwa_common_tmp(origen,destino);

        return convert_wav_to_rwa(origen,destino);

}

int convert_o_to_rwa_tmpdir(char *origen, char *destino)
{
        convert_to_rwa_common_tmp(origen,destino);

        return convert_o_to_rwa(origen,destino);

}

int convert_p_to_rwa_tmpdir(char *origen, char *destino)
{
        convert_to_rwa_common_tmp(origen,destino);

        return convert_p_to_rwa(origen,destino);

}


//Retorna 0 si ok
//1 si archivo no encontrado
//2 si error leyendo
int load_binary_file(char *binary_file_load,int valor_leido_direccion,int valor_leido_longitud)
{
  int returncode=0;

  if (!si_existe_archivo(binary_file_load)) return 1;

                if (MACHINE_IS_SPECTRUM) {
                  if (valor_leido_longitud==0 || valor_leido_longitud>65536) valor_leido_longitud=65536;

                  //maximo hasta direccion 65535
                  if (valor_leido_direccion+valor_leido_longitud > 65535) valor_leido_longitud=65536-valor_leido_direccion;
                }

                else { //MOTOROLA
                  if (valor_leido_longitud==0) valor_leido_longitud=QL_MEM_LIMIT+1;
                }

                debug_printf(VERBOSE_INFO,"Loading %s file at %d address with maximum %d bytes",binary_file_load,valor_leido_direccion,valor_leido_longitud);



                                FILE *ptr_binaryfile_load;
                                  ptr_binaryfile_load=fopen(binary_file_load,"rb");
                                  if (!ptr_binaryfile_load)
                                {
                                      debug_printf (VERBOSE_ERR,"Unable to open Binary file %s",binary_file_load);
                                      returncode=2;

                                  }
                                else {

                                                int leidos=1;
                                                z80_byte byte_leido;
                                                while (valor_leido_longitud>0 && leidos>0) {
                                                        leidos=fread(&byte_leido,1,1,ptr_binaryfile_load);
                                                        if (leidos>0) {
                                                                //poke_byte_no_time(valor_leido_direccion,byte_leido);
                                                                poke_byte_z80_moto(valor_leido_direccion,byte_leido);
                                                                valor_leido_direccion++;
                                                                valor_leido_longitud--;
                                                        }
                                                }


                                  fclose(ptr_binaryfile_load);

                                }

    return returncode;

}


//extern char *mostrar_footer_game_name;

//Opciones para configuracion custom de archivo cinta/snap
void parse_customfile_options(void)
{

	//Si se ha leido algun parametro de --joystickkeybt o --joystickkeyev
	//Cuando se lee el primero, se inicializa la tabla a 0
	int leido_config_joystick_a_key=0;

	z80_bit added_some_osd_text_keyboard={0};

        while (!siguiente_parametro()) {

		debug_printf (VERBOSE_DEBUG,"Parsing setting %s",argv[puntero_parametro]);

                if (!strcmp(argv[puntero_parametro],"--realvideo")) {
                                enable_rainbow();
                }

		else if (!strcmp(argv[puntero_parametro],"--snoweffect")) {
                                snow_effect_enabled.v=1;
		}

		else if (!strcmp(argv[puntero_parametro],"--wrx")) {
			enable_wrx();
		}

		else if (!strcmp(argv[puntero_parametro],"--chroma81")) {
                        enable_chroma81();
		}

                else if (!strcmp(argv[puntero_parametro],"--vsync-minimum-length")) {
                                siguiente_parametro_argumento();
                                int valor=atoi(argv[puntero_parametro]);
                                if (valor<100 || valor>999) {
                                        debug_printf (VERBOSE_ERR,"Invalid vsync lenght value");
                                        return;
                                }
                                minimo_duracion_vsync=valor;
                }

		else if (!strcmp(argv[puntero_parametro],"--no-horiz-stabilization")) {
                        video_zx8081_estabilizador_imagen.v=0;
		}

		else if (!strcmp(argv[puntero_parametro],"--programname")) {
			siguiente_parametro_argumento();
			mostrar_footer_game_name=argv[puntero_parametro];
		}

		//Para que aparezca --programsettingsinfo tiene que haber --programname
		else if (!strcmp(argv[puntero_parametro],"--programsettingsinfo")) {
                        siguiente_parametro_argumento();
                        mostrar_footer_second_message=argv[puntero_parametro];
                }


		else if (!strcmp(argv[puntero_parametro],"--disableborder")) {
			debug_printf(VERBOSE_INFO,"End Screen");
			scr_end_pantalla();
			disable_border();
			screen_init_pantalla_and_others();
			debug_printf(VERBOSE_INFO,"Creating Screen");
		}

                else if (!strcmp(argv[puntero_parametro],"--enableborder")) {
                        debug_printf(VERBOSE_INFO,"End Screen");
                        scr_end_pantalla();
                        enable_border();
			screen_init_pantalla_and_others();
                        debug_printf(VERBOSE_INFO,"Creating Screen");
                }

		else if (!strcmp(argv[puntero_parametro],"--enableinterlace")) {
			enable_interlace();
		}

		else if (!strcmp(argv[puntero_parametro],"--disableinterlace")) {
			disable_interlace();
		}

		else if (!strcmp(argv[puntero_parametro],"--enableulaplus")) {
			enable_ulaplus();
		}

		else if (!strcmp(argv[puntero_parametro],"--disableulaplus")) {
			disable_ulaplus();
		}

                else if (!strcmp(argv[puntero_parametro],"--enablespectra")) {
                        spectra_enable();
                }

                else if (!strcmp(argv[puntero_parametro],"--disablespectra")) {
                        spectra_disable();
                }

		else if (!strcmp(argv[puntero_parametro],"--enabletimexvideo")) {
                        enable_timex_video();
		}

		else if (!strcmp(argv[puntero_parametro],"--disabletimexvideo")) {
                        disable_timex_video();
		}

		else if (!strcmp(argv[puntero_parametro],"--enablezgx")) {
                        spritechip_enable();
		}

		else if (!strcmp(argv[puntero_parametro],"--redefinekey")) {
				z80_byte tecla_original, tecla_redefinida;
                                siguiente_parametro_argumento();
                                tecla_original=parse_string_to_number(argv[puntero_parametro]);

                                siguiente_parametro_argumento();
                                tecla_redefinida=parse_string_to_number(argv[puntero_parametro]);

                                if (util_add_redefinir_tecla(tecla_original,tecla_redefinida)) {
					//Error volver sin leer mas parametros
                                        return;
                                }
		}

		else if (!strcmp(argv[puntero_parametro],"--clearredefinekey")) {
			clear_lista_teclas_redefinidas();
		}



		else if (!strcmp(argv[puntero_parametro],"--joystickemulated")) {
			siguiente_parametro_argumento();
			if (realjoystick_set_type(argv[puntero_parametro])) {
			//Error. Volver sin leer mas parametros
				return;
			}

		}

		else if (!strcmp(argv[puntero_parametro],"--joystickevent")) {
                                char *text_button;
                                char *text_event;

                                //obtener boton
                                siguiente_parametro_argumento();
                                text_button=argv[puntero_parametro];

                                //obtener evento
                                siguiente_parametro_argumento();
                                text_event=argv[puntero_parametro];

                                //Y definir el evento
                                if (realjoystick_set_button_event(text_button,text_event)) {
					//Error. Volver sin leer mas parametros
                                        return;
                                }


		}

		else if (!strcmp(argv[puntero_parametro],"--joystickkeybt")) {

				if (leido_config_joystick_a_key==0) {
					//Vaciar tabla
					realjoystick_clear_keys_array();
					//Puntero a 0
					joystickkey_definidas=0;
					//Decir que para las siguientes no se borra
					leido_config_joystick_a_key=1;
				}

                                char *text_button;
                                char *text_key;

                                //obtener boton
                                siguiente_parametro_argumento();
                                text_button=argv[puntero_parametro];

                                //obtener tecla
                                siguiente_parametro_argumento();
                                text_key=argv[puntero_parametro];

                                //Y definir el evento
                                if (realjoystick_set_button_key(text_button,text_key)) {
					//Error. Volver sin leer mas parametros
                                        return;
                                }

		}

                else if (!strcmp(argv[puntero_parametro],"--joystickkeyev")) {

                                if (leido_config_joystick_a_key==0) {
                                        //Vaciar tabla
                                        realjoystick_clear_keys_array();
                                        //Puntero a 0
                                        joystickkey_definidas=0;
                                        //Decir que para las siguientes no se borra
                                        leido_config_joystick_a_key=1;
                                }

                                char *text_event;
                                char *text_key;

                                //obtener boton
                                siguiente_parametro_argumento();
                                text_event=argv[puntero_parametro];

                                //obtener tecla
                                siguiente_parametro_argumento();
                                text_key=argv[puntero_parametro];

                                //Y definir el evento
                                if (realjoystick_set_event_key(text_event,text_key)) {
					//Error. Volver sin leer mas parametros
                                        return;
                                }

                }

                else if (!strcmp(argv[puntero_parametro],"--cleareventlist")) {
			realjoystick_clear_events_array();
		}


                  else if (!strcmp(argv[puntero_parametro],"--text-keyboard-add")) {
                                if (added_some_osd_text_keyboard.v==0) {
                                        util_clear_text_adventure_kdb();
                                        added_some_osd_text_keyboard.v=1;
                                        //printf ("Clearing text keyboard\n");
                                }
                                siguiente_parametro_argumento();
                                //printf ("Adding text keyboard %s\n",argv[puntero_parametro]);
                                util_add_text_adventure_kdb(argv[puntero_parametro]);
                        }

				else if (!strcmp(argv[puntero_parametro],"--text-keyboard-length")) {
						siguiente_parametro_argumento();
						int valor=parse_string_to_number(argv[puntero_parametro]);
						if (valor<10 || valor>100) {
                                        debug_printf (VERBOSE_ERR,"Invalid text-keyboard-length value\n");
                                        return;
                                }
						adventure_keyboard_key_length=valor;
				}

		 else if (!strcmp(argv[puntero_parametro],"--machine")) {
                                char *machine_name;
                                siguiente_parametro_argumento();
                                machine_name=argv[puntero_parametro];

                                if (set_machine_type_by_name(machine_name)) {
					                               //Error. Volver sin leer mas parametros
                                        return;
                                }

                                set_machine(NULL);
                                cold_start_cpu_registers();
                                reset_cpu();

		}

		else if (!strcmp(argv[puntero_parametro],"--zx8081vsyncsound")) {
			zx8081_vsync_sound.v=1;
		}

		else if (!strcmp(argv[puntero_parametro],"--zx8081mem")) {
                                siguiente_parametro_argumento();
                                int valor=atoi(argv[puntero_parametro]);
                                if (valor<1 || valor>16) {
                                        debug_printf (VERBOSE_ERR,"Invalid RAM value");
                                        return;
                                }
                                set_zx8081_ramtop(valor);
                }

                else if (!strcmp(argv[puntero_parametro],"--zx8081ram8K2000")) {
                                ram_in_8192.v=1;
                }

                else if (!strcmp(argv[puntero_parametro],"--zx8081ram16K8000")) {
                                enable_ram_in_32768();
                }

                else if (!strcmp(argv[puntero_parametro],"--zx8081ram16KC000")) {
                                enable_ram_in_49152();
                }

		else if (!strcmp(argv[puntero_parametro],"--gigascreen")) {
                                enable_gigascreen();
		}


		else if (!strcmp(argv[puntero_parametro],"--enablezx8081lnctradjust")) {
			video_zx8081_lnctr_adjust.v=1;
		}

		else if (!strcmp(argv[puntero_parametro],"--disablezx8081lnctradjust")) {
			video_zx8081_lnctr_adjust.v=0;
		}

		else if (!strcmp(argv[puntero_parametro],"--frameskip")) {
                                siguiente_parametro_argumento();
                                int valor=atoi(argv[puntero_parametro]);
                                if (valor<0 || valor>50) {
                                        debug_printf (VERBOSE_ERR,"Invalid frameskip value");
                                        return;
                                }
                                frameskip=valor;
                }

		else if (!strcmp(argv[puntero_parametro],"--aychip")) {
                                ay_chip_present.v=1;
		}



		//
		//Opcion no reconocida. Error
		//
                else {
                        debug_printf (VERBOSE_ERR,"Setting %s not allowed on custom config file",argv[puntero_parametro]);
                        return;
                }
        }


}

void customconfig_help(void)
{
	printf ("Custom config help:\n"
		"\n"
		"Custom config files are configuration files for every program or game you want.\n"
		"When you load a tape/snapshot, the emulator will search for a file, identical name of the file you load,\n"
		"but adding extension .config, and it will apply every parameter contained in the file.\n"
		"\n"
		"So, if you load for example: game.tap, it will be searched a file game.tap.config\n"
		"This config file is a text file you can edit; the format is like the " ZESARUX_CONFIG_FILE " main configuration file,\n"
		"but not every command line parameter allowed on main configuration file is allowed on a custom config file.\n"
		"There are some .config files included in this emulator, you can view them to have some examples."
		"\n"
		"The following are the allowed parameters on a custom config file, we don't describe the parameters that are the same as on command line:\n"
		"\n"

	"--machine id \n"
	"--programname text          Set the name of the tape/snapshot, shown on footer\n"
	"--programsettingsinfo text  Set the text for settings information, shown on footer\n"
	"--frameskip n\n"
	"--disableborder\n"
	"--enableborder              Enable border\n"
	"--aychip                    Enable AY-Chip\n"
	"--zx8081mem n\n"
	"--zx8081ram8K2000\n"
	"--zx8081ram16K8000\n"
	"--zx8081ram16KC000\n"
	"--zx8081vsyncsound\n"
	"--realvideo\n"
	"--snoweffect\n"
	"--enableinterlace           Enable interlace video mode\n"
	"--disableinterlace          Disable interlace video mode\n"
	"--enableulaplus             Enable ULAplus video modes\n"
	"--disableulaplus            Disable ULAplus video modes\n"
	"--enablespectra             Enable Spectra video modes\n"
	"--disablespectra            Disable Spectra video modes\n"
	"--enabletimexvideo          Enable Timex video modes\n"
	"--disabletimexvideo         Disable Timex video modes\n"
	"--enablezgx\n"
	"--wrx\n"
	"--chroma81\n"
	"--vsync-minimum-length n \n"
	"--no-horiz-stabilization    Disable Horizontal Stabilization\n"
	"--gigascreen                Enable GigaScreen emulation\n"
	"--enablezx8081lnctradjust   Enable LNCTR adjust on ZX80/81\n"
	"--disablezx8081lnctradjust  Disable LNCTR adjust on ZX80/81\n"
	"--redefinekey src dest\n"
	"--clearredefinekey          Clear redefine key table\n"
	"--joystickemulated type\n"
	"--joystickevent but evt\n"
	"--joystickkeybt but key\n"
	"--joystickkeyev evt key\n"
	"--text-keyboard-add text\n"
        "--text-keyboard-length n\n"
	"--cleareventlist\n"

	"\n"
	);
}

//Devuelve -1 si desconocida
int get_machine_id_by_name(char *machine_name)
{

  int return_machine;
                                if (!strcmp(machine_name,"16k")) return_machine=0;
                                else if (!strcasecmp(machine_name,"48k")) return_machine=1;
                                else if (!strcasecmp(machine_name,"48ks")) return_machine=20;
                                else if (!strcasecmp(machine_name,"Inves")) return_machine=2;
                                else if (!strcasecmp(machine_name,"TK90X")) return_machine=3;
                                else if (!strcasecmp(machine_name,"TK90XS")) return_machine=4;
                                else if (!strcasecmp(machine_name,"TK95")) return_machine=5;
                                else if (!strcasecmp(machine_name,"128k")) return_machine=6;
                                else if (!strcasecmp(machine_name,"128ks")) return_machine=7;
                                else if (!strcasecmp(machine_name,"P2")) return_machine=8;
                                else if (!strcasecmp(machine_name,"P2F")) return_machine=9;
                                else if (!strcasecmp(machine_name,"P2S")) return_machine=10;
                                else if (!strcasecmp(machine_name,"P2A40")) return_machine=11;
                                else if (!strcasecmp(machine_name,"P2A41")) return_machine=12;
                                else if (!strcasecmp(machine_name,"P2AS")) return_machine=13;
                                else if (!strcasecmp(machine_name,"ZXUNO")) return_machine=14;
                                else if (!strcasecmp(machine_name,"Chloe140")) return_machine=15;
                                else if (!strcasecmp(machine_name,"Chloe280")) return_machine=16;
                                else if (!strcasecmp(machine_name,"TS2068")) return_machine=17;
                                else if (!strcasecmp(machine_name,"Prism")) return_machine=18;
                                else if (!strcasecmp(machine_name,"TBBlue")) return_machine=19;
                                else if (!strcasecmp(machine_name,"Pentagon")) return_machine=21;
                                else if (!strcasecmp(machine_name,"Chrome")) return_machine=MACHINE_ID_CHROME;
                                else if (!strcasecmp(machine_name,"BaseConf")) return_machine=MACHINE_ID_BASECONF;
                                else if (!strcasecmp(machine_name,"TSConf")) return_machine=MACHINE_ID_TSCONF;
                                else if (!strcasecmp(machine_name,"P340")) return_machine=MACHINE_ID_SPECTRUM_P3_40;
                                else if (!strcasecmp(machine_name,"P341")) return_machine=MACHINE_ID_SPECTRUM_P3_41;
                                else if (!strcasecmp(machine_name,"P3S")) return_machine=MACHINE_ID_SPECTRUM_P3_SPA;
                                else if (!strcasecmp(machine_name,"ZX80")) return_machine=120;
                                else if (!strcasecmp(machine_name,"ZX81")) return_machine=121;
                                else if (!strcasecmp(machine_name,"ACE")) return_machine=122;
                                else if (!strcasecmp(machine_name,"Z88")) return_machine=130;
                                else if (!strcasecmp(machine_name,"CPC464")) return_machine=140;
                                else if (!strcasecmp(machine_name,"SAM")) return_machine=150;
                                else if (!strcasecmp(machine_name,"QL")) return_machine=160;
                                else if (!strcasecmp(machine_name,"MK14")) return_machine=MACHINE_ID_MK14_STANDARD;
                                else {
                                        debug_printf (VERBOSE_ERR,"Unknown machine %s",machine_name);
                                        return_machine=-1;
                                }

	return return_machine;
}



//Devuelve 0 si ok
int set_machine_type_by_name(char *machine_name)
{

  int maquina=get_machine_id_by_name(machine_name);
  if (maquina==-1) return 1;

  current_machine_type=maquina;


	return 0;
}


//Esta es la funcion inversa de la anterior. Devuelve "" si no se sabe numero de maquina
void get_machine_config_name_by_number(char *machine_name,int machine_number)
{

switch (machine_number) {
case 0:
strcpy(machine_name,"16k");
break;

case 1:
strcpy(machine_name,"48k");
break;

case 20:
strcpy(machine_name,"48ks");
break;

case 2:
strcpy(machine_name,"Inves");
break;

case 3:
strcpy(machine_name,"TK90X");
break;

case 4:
strcpy(machine_name,"TK90XS");
break;

case 5:
strcpy(machine_name,"TK95");
break;

case 6:
strcpy(machine_name,"128k");
break;

case 7:
strcpy(machine_name,"128ks");
break;

case 8:
strcpy(machine_name,"P2");
break;

case 9:
strcpy(machine_name,"P2F");
break;

case 10:
strcpy(machine_name,"P2S");
break;

case 11:
strcpy(machine_name,"P2A40");
break;

case 12:
strcpy(machine_name,"P2A41");
break;

case 13:
strcpy(machine_name,"P2AS");
break;

case 14:
strcpy(machine_name,"ZXUNO");
break;

case 15:
strcpy(machine_name,"Chloe140");
break;

case 16:
strcpy(machine_name,"Chloe280");
break;

case 17:
strcpy(machine_name,"TS2068");
break;

case 18:
strcpy(machine_name,"Prism");
break;

case 19:
strcpy(machine_name,"TBBlue");
break;

case 21:
strcpy(machine_name,"Pentagon");
break;

case MACHINE_ID_CHROME:
strcpy(machine_name,"Chrome");
break;

case MACHINE_ID_TSCONF:
strcpy(machine_name,"TSConf");
break;

case MACHINE_ID_BASECONF:
strcpy(machine_name,"BaseConf");
break;

case MACHINE_ID_SPECTRUM_P3_40:
strcpy(machine_name,"P340");
break;

case MACHINE_ID_SPECTRUM_P3_41:
strcpy(machine_name,"P341");
break;

case MACHINE_ID_SPECTRUM_P3_SPA:
strcpy(machine_name,"P3S");
break;

case 120:
strcpy(machine_name,"ZX80");
break;

case 121:
strcpy(machine_name,"ZX81");
break;

case 122:
strcpy(machine_name,"ACE");
break;

case 130:
strcpy(machine_name,"Z88");
break;

case 140:
strcpy(machine_name,"CPC464");
break;

case 150:
strcpy(machine_name,"SAM");
break;

case 160:
strcpy(machine_name,"QL");
break;

case MACHINE_ID_MK14_STANDARD:
strcpy(machine_name,"MK14");
break;

default:
machine_name[0]=0;
break;

}


}


//Alternativa a scandir para sistemas Mingw, que no implementan dicha funcion
int scandir_mingw(const char *dir, struct dirent ***namelist,
              int (*filter)(const struct dirent *),
              int (*compar)(const struct dirent **, const struct dirent **))
{

	#define MAX_ARCHIVOS_SCANDIR_MINGW 10000
        int archivos=0;

        //Puntero a cada archivo leido
        struct dirent *memoria_archivos;

        //Array de punteros.
        struct dirent **memoria_punteros;

        //Asignamos memoria
        memoria_punteros=malloc(sizeof(struct dirent *)*MAX_ARCHIVOS_SCANDIR_MINGW);


        if (memoria_punteros==NULL) {
                cpu_panic("Error allocating memory when reading directory");
        }

        *namelist=memoria_punteros;

        //int indice_puntero;


       struct dirent *dp;
       DIR *dfd;

       if ((dfd = opendir(dir)) == NULL) {
           debug_printf(VERBOSE_ERR,"Can't open directory %s", dir);
           return -1;
       }

       while ((dp = readdir(dfd)) != NULL) {

                debug_printf(VERBOSE_DEBUG,"scandir_mingw: file: %s",dp->d_name);

		if (filter(dp)) {

		        //Asignar memoria para ese fichero
		        memoria_archivos=malloc(sizeof(struct dirent));

		      if (memoria_archivos==NULL) {
                		cpu_panic("Error allocating memory when reading directory");
		        }

		        //Meter puntero
		        memoria_punteros[archivos]=memoria_archivos;

		        //Meter datos
		        memcpy(memoria_archivos,dp,sizeof( struct dirent ));
		        archivos++;

		        if (archivos>=MAX_ARCHIVOS_SCANDIR_MINGW) {
                		debug_printf(VERBOSE_ERR,"Error. Maximum files in directory reached: %d",MAX_ARCHIVOS_SCANDIR_MINGW);
		                return archivos;
		        }

		}


       }
       closedir(dfd);

	//lanzar qsort
	int (*funcion_compar)(const void *, const void *);

	funcion_compar=( int (*)(const void *, const void *)  )compar;

	qsort(memoria_punteros,archivos,sizeof( struct dirent *), funcion_compar);

        return archivos;
}


//Devuelve la letra en mayusculas
char letra_mayuscula(char c)
{
	if (c>='a' && c<='z') c=c-('a'-'A');
	return c;
}

//Devuelve la letra en minusculas
char letra_minuscula(char c)
{
        if (c>='A' && c<='Z') c=c+('a'-'A');
        return c;
}

//Convierte una string en minusculas
void string_a_minusculas(char *origen, char *destino)
{
	char letra;

	for (;*origen;origen++,destino++) {
		letra=*origen;
		letra=letra_minuscula(letra);
		*destino=letra;
	}

	*destino=0;
}

//Dice si ruta es absoluta. 1 si es absoluta. 0 si no
int si_ruta_absoluta(char *ruta)
{

#ifdef MINGW
	//En Windows
	//Si empieza por '\' es absoluta
        if (ruta[0]=='\\') return 1;

	//Si empieza por letra unidad: es absoluta
	if (strlen(ruta)>2) {
		//Solo mirar los :
		if (ruta[1]==':') return 1;
	}

        return 0;

#else
	//En Unix
	//Si empieza por '/' es absoluta
	if (ruta[0]=='/') return 1;
	else return 0;
#endif

}

//Retorna tipo de archivo segun valor d_type
//0: desconocido
//1: archivo normal (o symbolic link)
//2: directorio


int get_file_type_from_stat(struct stat *f)
{
  if (f->st_mode & S_IFDIR) return 2;
  else return 1;
}


//Retorna tipo de archivo segun valor d_type
//0: desconocido
//1: archivo normal (o symbolic link)
//2: directorio
//Entrada: d_type, nombre archivo. requisito es que el archivo se encuentre en directorio actual
//En Linux solo se usa d_type
//En Windows solo se usa nombre, y luego se obtiene st_mode

int get_file_type(int d_type, char *nombre)
{

	debug_printf (VERBOSE_DEBUG,"Name: %s type: %d constants: DT_DIR: %d DT_REG: %d DT_LNK: %d",
			nombre,d_type,DT_DIR,DT_REG,DT_LNK);

#ifndef MINGW


	if (d_type == DT_DIR) return 2;
	if (d_type == DT_REG || d_type == DT_LNK) return 1;

	return 0;
#else
/*
24 puede significar 16+8:
(obtenido de dirent.h de mingw)
_A_SUBDIR 0x10
_A_VOLID 0x8
cosa un tanto extraña....
y mas teniendo en cuenta que hay una definicion:

#define DT_UNKNOWN (_A_VOLID | _A_SUBDIR)

Esto aparece en archivos descargados de internet

*/

//temp tipo archivo para win
/*
        //Ver si archivo existe
        struct stat buf_stat;

        if (stat(d->d_name, &buf_stat)==0) {
                //printf ("%s st_mode: %d converted to d_type: %d constants: DT_DIR: %d DT_REG: %d DT_LNK: %d\n",d->d_name,buf_stat.st_mode,IFTODT(buf_stat.st_mode),DT_DIR,DT_REG,DT_LNK );
                debug_printf (VERBOSE_DEBUG,"%s st_mode: %d constants: S_IFDIR: %d DT_DIR: %d DT_REG: %d DT_LNK: %d",d->d_name,buf_stat.st_mode,S_IFDIR,DT_DIR,DT_REG,DT_LNK );
        }
        else {
                printf ("error stat para %s\n",d->d_name );
        }
*/

        struct stat buf_stat;

        if (stat(nombre, &buf_stat)==0) {
		debug_printf (VERBOSE_DEBUG,"Name: %s st_mode: %d constants: S_IFDIR: %d",nombre,buf_stat.st_mode,S_IFDIR);

		if (buf_stat.st_mode & S_IFDIR) return 2;
		else return 1;
	}


	//if (d_type == DT_DIR ) return 2;
	//if (d_type == DT_REG || d_type == DT_LNK || d_type ==24) return 1;

	//desconocido
        return 0;

#endif

}


void convert_relative_to_absolute(char *relative_path,char *final_path)
{
	//Directorio actual
	char directorio_actual[PATH_MAX];
	getcwd(directorio_actual,PATH_MAX);

	//Cambiamos a lo que dice el path relativo
	menu_filesel_chdir(relative_path);

	//Y en que directorio acabamos?
	getcwd(final_path,PATH_MAX);

	//Volver a directorio inicial
	menu_filesel_chdir(directorio_actual);

	//printf ("Convert relative to absolute path: relative: %s absolute: %s\n",relative_path,final_path);

	debug_printf (VERBOSE_DEBUG,"Convert relative to absolute path: relative: %s absolute: %s",relative_path,final_path);

}

int util_read_file_lines(char *memoria,char **lineas,int max_lineas)
{

	int linea=0;
	//char letra;

	while (linea<max_lineas && *memoria!=0) {
		lineas[linea++]=memoria;

		while (*memoria!='\n' && *memoria!='\r' && *memoria!=0) {
			//printf ("1 %d %c ",linea,*memoria);
			memoria++;
		}

		//meter un 0
		if (*memoria!=0) {
			*memoria=0;
			memoria++;
		}

		//saltar saltos de lineas sobrantes
		while ( (*memoria=='\n' || *memoria=='\r') && *memoria!=0) {
			//printf ("2 %d %c ",linea,*memoria);
			memoria++;
		}

		//printf (" memoria %p\n",memoria);
	}

	return linea;

}

int util_read_line_fields(char *memoria,char **campos)
{
	int campo_actual=0;
	while (*memoria!=0) {
		campos[campo_actual++]=memoria;

		while (*memoria!=' ' && *memoria!=0) {
			memoria++;
		}

                //meter un 0
                if (*memoria!=0) {
                        *memoria=0;
                        memoria++;
                }

		//saltar espacios adicionales
		while ( *memoria==' ' && *memoria!=0) {
			memoria++;
		}
	}

	return campo_actual;
}


int util_parse_pok_mem(char *memoria,struct s_pokfile **tabla_pokes)
{
/*
'N' means: this is the Next trainer,
'Y' means: this is the last line of the file (the rest of the file is ignored).
After the 'N' follows the name/description of this specific trainer. This string
may be up to 30 characters. There is no space between the 'N' and the string.
Emulator authors can use these strings to set up the selection entries.

The following lines, up to the next 'N' or 'Z' hold the POKEs to be applied for
this specific trainer. Again, the first character determines the content.
'M' means: this is not the last POKE (More)
'Z' means: this is the last POKE
The rest of a POKE line is built from

  bbb aaaaa vvv ooo

All values are decimal, separation is done by one or more spaces.

The field 'bbb' (128K memory bank) is built from
  bit 0-2 : bank value
        3 : ignore bank (1=yes, always set for 48K games)



Ejemplo:
NImmunity
M  8 44278  58   0
Z  8 44285  58   0
Y
*/


/*
Estructura:

int indice_accion: numero de orden de la accion poke. 0 para la primera, 1 para la segunda, etc
Un poke de inmunidad que sean dos pokes, tendran este numero igual (y si es el primero, sera 0)
char texto[40]: lo que hace ese poke
z80_byte banco;
z80_int direccion;
z80_byte valor;
z80_byte valor_orig;

*/

	//int indice_accion=0;

	char *lineas[MAX_LINEAS_POK_FILE];

	char ultimo_nombre_poke[MAX_LENGTH_LINE_POKE_FILE+1]="";

	int lineas_leidas=util_read_file_lines(memoria,lineas,MAX_LINEAS_POK_FILE);

	int i,j;

	int total_pokes=0;

	int final=0;

	int destino=0;

	for (i=0;i<lineas_leidas && final==0;i++) {
		debug_printf (VERBOSE_DEBUG,"line: %d text: %s",i,lineas[i]);
		char *campos[10];

		//Si linea pasa de 1024, limitar
		if (strlen(lineas[i]) >=1024 ) {
			debug_printf (VERBOSE_DEBUG,"Line %d too large. Truncating",i);
			lineas[i][1023]=0;
		}

		//copiar linea actual antes de trocear para recuperar luego nombre de poke en N si conviene

		char ultima_linea[1024];
		sprintf (ultima_linea,"%s",lineas[i]);
		//si linea execede maximo, truncar
		if (strlen(ultima_linea) > MAX_LENGTH_LINE_POKE_FILE) ultima_linea[MAX_LENGTH_LINE_POKE_FILE]=0;

		int total_campos=util_read_line_fields(lineas[i],campos);
		for (j=0;j<total_campos;j++) {
			//printf ("campo %d : [%s] ",j,campos[j]);
			//Si campo excede maximo, truncar
			if ( strlen(campos[j]) > MAX_LENGTH_LINE_POKE_FILE) campos[j][MAX_LENGTH_LINE_POKE_FILE]=0;
		}

		//Casos
		switch (campos[0][0]) {
			case 'N':
				//Texto de accion puede ser de dos palabras
				//sprintf (ultimo_nombre_poke,"%s %s",&campos[0][1], (total_campos>1 ? campos[1] : "")   );
				sprintf (ultimo_nombre_poke,"%s",&ultima_linea[1]);
			break;

			case 'M':
			case 'Z':
				//printf ("i: %d puntero %p\n",i,&tabla_pokes[i]->indice_accion);
				tabla_pokes[destino]->indice_accion=total_pokes;
				sprintf (tabla_pokes[destino]->texto,"%s",ultimo_nombre_poke);
				if (total_campos>=4) {
					tabla_pokes[destino]->banco=atoi(campos[1]);
					tabla_pokes[destino]->direccion=atoi(campos[2]);
					tabla_pokes[destino]->valor=atoi(campos[3]);
				}

				if (total_campos>=5) {
					tabla_pokes[destino]->valor_orig=atoi(campos[4]);
				}

				//Si era final de poke de accion, incrementar
				if (campos[0][0]=='Z') total_pokes++;

				destino++;

			break;


			case 'Y':
				final=1;
			break;

			default:
				//printf ("error format\n");
				return -1;
			break;
		}

		if (!final) {

		}


		//printf ("\n");
	}


	//return lineas_leidas;
	return destino;

}


//Retorna numero elementos del array
int util_parse_pok_file(char *file,struct s_pokfile **tabla_pokes)
{
	char *mem;

	mem=malloc(65536);


                FILE *ptr_pok;
                ptr_pok=fopen(file,"rb");

                if (!ptr_pok) {
                        debug_printf (VERBOSE_ERR,"Unable to open file %s",file);
                        return 0;
                }


                int leidos=fread(mem,1,65535,ptr_pok);

		//Fin de texto
		mem[leidos]=0;

		fclose(ptr_pok);


	int total=util_parse_pok_mem(mem,tabla_pokes);



	free(mem);

	return total;


}


//Usado en pok file
int util_poke(z80_byte banco,z80_int direccion,z80_byte valor)
{
	//Si estamos en maquina 48k
	if (MACHINE_IS_SPECTRUM_16_48) {
		if (banco<8) {
			debug_printf (VERBOSE_ERR,"This poke is for a 128k machine and we are not in 128k machine");
			return -1;
		}

		debug_printf (VERBOSE_DEBUG,"util_spectrum_poke. pokeing address %d with value %d",direccion,valor);
		poke_byte_no_time(direccion,valor);
	}


	//si maquina 128k
	else if (MACHINE_IS_SPECTRUM_128_P2_P2A_P3) {
		//ram_mem_table
		//Si banco >7, hacer poke normal
		if (banco>7) {
			debug_printf (VERBOSE_DEBUG,"util_spectrum_poke. pokeing address %d with value %d",direccion,valor);
			poke_byte_no_time(direccion,valor);
		}
		else {
			//Poke con banco de memoria
			direccion = direccion & 16383;
			z80_byte *puntero;
			puntero=ram_mem_table[banco]+direccion;
			debug_printf (VERBOSE_DEBUG,"util_spectrum_poke. pokeing bank %d address %d with value %d",banco,direccion,valor);
			*puntero=valor;
		}
	}

	else if (MACHINE_IS_ZXUNO)  {
		if (ZXUNO_BOOTM_ENABLED) {
			debug_printf (VERBOSE_ERR,"Can not poke in ZX-Uno mode with BOOTM enabled");
			return -1;
		}

                if (banco>7) {
                        debug_printf (VERBOSE_DEBUG,"util_spectrum_poke. pokeing address %d with value %d",direccion,valor);
                        poke_byte_no_time(direccion,valor);
                }
                else {
                        //Poke con banco de memoria
                        direccion = direccion & 16383;
                        z80_byte *puntero;
                        puntero=zxuno_sram_mem_table_new[banco]+direccion;
			debug_printf (VERBOSE_DEBUG,"util_spectrum_poke. pokeing bank %d address %d with value %d",banco,direccion,valor);
                        *puntero=valor;
                }
	}

	//resto de maquinas
	else if (MACHINE_IS_ZX8081 || MACHINE_IS_Z88) {
		debug_printf (VERBOSE_DEBUG,"util_spectrum_poke. pokeing address %d with value %d",direccion,valor);
		poke_byte_no_time(direccion,valor);
	}

	return 0;


}



//Busca archivo "archivo" en directorio, sin distinguir mayusculas/minusculas
//Si encuentra, lo mete en nombreencontrado, y devuelve no 0
//Si no encuentra, devuelve 0
int util_busca_archivo_nocase(char *archivo,char *directorio,char *nombreencontrado)
{

	debug_printf (VERBOSE_DEBUG,"Searching file %s in directory %s",archivo,directorio);

	//Saltar todas las / iniciales del archivo
	while ( (*archivo=='/' || *archivo=='\\') && (*archivo!=0) ) {
		archivo++;
	}


	struct dirent *busca_archivo_dirent;
	DIR *busca_archivo_dir=NULL;



	busca_archivo_dir = opendir(directorio);


	if (busca_archivo_dir==NULL) {
		return 0;
        }

        do {

                busca_archivo_dirent = readdir(busca_archivo_dir);

                if (busca_archivo_dirent == NULL) {
                        closedir(busca_archivo_dir);
			return 0;
                }

		if (!strcasecmp(archivo,busca_archivo_dirent->d_name)) {
			//Encontrado
			sprintf (nombreencontrado,"%s",busca_archivo_dirent->d_name);
			debug_printf (VERBOSE_DEBUG,"Found file %s",nombreencontrado);
			return 1;
		}


        } while(1);

	//Aqui no llega nunca
	return 0;


}


//Funciones Spool Turbo para modelos Spectrum
void peek_byte_spoolturbo_check_key(z80_int dir)
{
	//si dir=23560, enviar tecla de spool file
	z80_int lastk=23560;

        if (input_file_keyboard_is_playing() && dir==lastk) {
	                        z80_byte input_file_keyboard_last_key;

                                int leidos=fread(&input_file_keyboard_last_key,1,1,ptr_input_file_keyboard);
                                if (leidos==0) {
                                        debug_printf (VERBOSE_INFO,"Read 0 bytes of Input File Keyboard. End of file");
                                        eject_input_file_keyboard();
                                        reset_keyboard_ports();
                                }

                                //conversion de salto de linea
                                if (input_file_keyboard_last_key==10) input_file_keyboard_last_key=13;

                                poke_byte_no_time(lastk,input_file_keyboard_last_key);


        }
}

//Punteros a las funciones originales
z80_byte (*peek_byte_no_time_no_spoolturbo)(z80_int dir);
z80_byte (*peek_byte_no_spoolturbo)(z80_int dir);

z80_byte peek_byte_spoolturbo(z80_int dir)
{

	peek_byte_spoolturbo_check_key(dir);

	return peek_byte_no_spoolturbo(dir);
}

z80_byte peek_byte_no_time_spoolturbo(z80_int dir)
{

        peek_byte_spoolturbo_check_key(dir);

        return peek_byte_no_time_no_spoolturbo(dir);
}




void set_peek_byte_function_spoolturbo(void)
{

        debug_printf(VERBOSE_INFO,"Enabling spoolturbo on peek_byte");


	if (MACHINE_IS_SPECTRUM) {
		//Cambiar valores de repeticion de teclas
		poke_byte_no_time(23561,1);
		poke_byte_no_time(23562,1);
	}


        peek_byte_no_time_no_spoolturbo=peek_byte_no_time;
        peek_byte_no_time=peek_byte_no_time_spoolturbo;

        peek_byte_no_spoolturbo=peek_byte;
        peek_byte=peek_byte_spoolturbo;

}

void reset_peek_byte_function_spoolturbo(void)
{
	debug_printf(VERBOSE_INFO,"Resetting spoolturbo on peek_byte");

	//Restaurar valores de repeticion de teclas
	poke_byte_no_time(23561,35);
	poke_byte_no_time(23562,5);

        peek_byte_no_time=peek_byte_no_time_no_spoolturbo;
        peek_byte=peek_byte_no_spoolturbo;
}


//Funciones de Escritura en ROM
//Permitido en Spectrum 48k, Spectrum 16k, ZX80, ZX81, Jupiter Ace
//Punteros a las funciones originales
//void (*poke_byte_no_time_no_writerom)(z80_int dir,z80_byte value);
//void (*poke_byte_no_writerom)(z80_int dir,z80_byte value);


void poke_byte_writerom_exec(z80_int dir,z80_byte value)
{
	if (MACHINE_IS_SPECTRUM) {
		if (dir<16384) memoria_spectrum[dir]=value;
	}

	else if (MACHINE_IS_ZX80) {
                if (dir<4096) memoria_spectrum[dir]=value;
        }

	else if (MACHINE_IS_ZX81) {
                if (dir<8192) memoria_spectrum[dir]=value;
        }

	else if (MACHINE_IS_ACE) {
                if (dir<8192) memoria_spectrum[dir]=value;
        }

	else if (MACHINE_IS_SAM) {

                z80_byte *puntero;
                puntero=sam_return_segment_memory(dir);

                dir = dir & 16383;

                puntero=puntero+dir;
                *puntero=value;

	}
}

z80_byte poke_byte_writerom(z80_int dir,z80_byte value)
{

	poke_byte_writerom_exec(dir,value);

        //poke_byte_no_writerom(dir,value);
	debug_nested_poke_byte_call_previous(write_rom_nested_id_poke_byte,dir,value);

	//Para que no se queje el compilador
	return 0;

}

z80_byte poke_byte_no_time_writerom(z80_int dir,z80_byte value)
{

	poke_byte_writerom_exec(dir,value);

        //poke_byte_no_time_no_writerom(dir,value);
	debug_nested_poke_byte_no_time_call_previous(write_rom_nested_id_poke_byte_no_time,dir,value);

	//Para que no se queje el compilador
	return 0;
}




void set_poke_byte_function_writerom(void)
{

        debug_printf(VERBOSE_INFO,"Enabling Write on ROM on poke_byte");

        //poke_byte_no_time_no_writerom=poke_byte_no_time;
        //poke_byte_no_time=poke_byte_no_time_writerom;

        //poke_byte_no_writerom=poke_byte;
        //poke_byte=poke_byte_writerom;

        write_rom_nested_id_poke_byte=debug_nested_poke_byte_add(poke_byte_writerom,"Write rom poke_byte");
        write_rom_nested_id_poke_byte_no_time=debug_nested_poke_byte_no_time_add(poke_byte_no_time_writerom,"Write rom poke_byte_no_time");

}

void reset_poke_byte_function_writerom(void)
{
        debug_printf(VERBOSE_INFO,"Resetting Write on ROM on poke_byte");
        //poke_byte_no_time=poke_byte_no_time_no_writerom;
        //poke_byte=poke_byte_no_writerom;

        debug_nested_poke_byte_del(write_rom_nested_id_poke_byte);
        debug_nested_poke_byte_no_time_del(write_rom_nested_id_poke_byte_no_time);

}

//Retorna nombre de cinta spectrum (10 bytes)
void util_tape_get_name_header(z80_byte *tape,char *texto)
{
	int i;
	z80_byte caracter;

	for (i=0;i<10;i++) {
		caracter=*tape++;
		if (caracter<32 || caracter>126) caracter='.';

		*texto++=caracter;
	}

	*texto=0;
}


void util_tape_get_info_tapeblock(z80_byte *tape,z80_byte flag,z80_int longitud,char *texto)
{
	char buffer_nombre[11];

	z80_byte first_byte=tape[0];
		if (flag==0 && first_byte<=3 && longitud==19) {
			//Posible cabecera
			util_tape_get_name_header(&tape[1],buffer_nombre);

                        z80_int cabecera_longitud;
                        z80_int cabecera_inicio;

                        cabecera_longitud=value_8_to_16(tape[12],tape[11]);
                        cabecera_inicio=value_8_to_16(tape[14],tape[13]);
                        


			//Evitar el uso del caracter ":" para evitar generar nombres (en el expansor de archivos) con ":" que pueden dar problemas en windows
			switch (first_byte) {
				case 0:
					sprintf(texto,"Program %s",buffer_nombre);
				break;

				case 1:
					sprintf(texto,"Num array %s",buffer_nombre);
				break;

				case 2:
					sprintf(texto,"Char array %s",buffer_nombre);
				break;

				case 3:
                                        if (cabecera_longitud==6912 && cabecera_inicio==16384) sprintf(texto,"Screen$ %s",buffer_nombre);
                                                          //01234567890123456789012345678901
                                                          //Code: 1234567890 [16384,49152]
					else sprintf(texto,"Code %s [%d,%d]",buffer_nombre,cabecera_inicio,cabecera_longitud);
				break;

			}
		}

		else if (flag==0 && first_byte==3 && longitud==36) { 
			//Bloque de codigo fuente SPED
			util_tape_get_name_header(&tape[1],buffer_nombre);

                                     //01234567890123456789012345678901
                                     //SPED: 1234567890 (12/12/2099)
                        z80_byte sped_day=tape[18];
                        z80_byte sped_month=tape[19];
                        z80_byte sped_year=tape[20];

			sprintf(texto,"SPED %s (%d-%d-%d)",buffer_nombre,sped_day,sped_month,sped_year+1980);

		}


		else sprintf(texto,"Flag %d Length %d",flag,longitud-2); //Saltar los 2 bytes de flag y checksum
}

//Retorna texto descriptivo de informacion de cinta en texto. Cinta tipo tap
//Retorna longitud del bloque
int util_tape_tap_get_info(z80_byte *tape,char *texto)
{
	int longitud=value_8_to_16(tape[1],tape[0]);
	//Si longitud<2, es error
	int flag=tape[2];
	tape+=3;

	//char buffer_nombre[11];

	if (longitud<2) strcpy(texto,"Corrupt tape");
	else {
		util_tape_get_info_tapeblock(tape,flag,longitud,texto);

		
	}


	return longitud+2;
}


//Peek byte multiple para Z80 y Motorola
//Esta deberia ser la rutina preferida para llamarse desde menu y otros sitios
z80_byte peek_byte_z80_moto(unsigned int address)
{
  address=adjust_address_space_cpu(address);
  if (MACHINE_IS_QL) return ql_readbyte_no_ports(address);
  else return peek_byte_no_time(address);
}

void poke_byte_z80_moto(unsigned int address,z80_byte valor)
{
  address=adjust_address_space_cpu(address);
  if (MACHINE_IS_QL) ql_writebyte_no_ports(address,valor);
  else poke_byte_no_time(address,valor);
}

unsigned int get_ql_pc(void)
{
  return m68k_get_reg(NULL, M68K_REG_PC);
}

unsigned int get_pc_register(void)
{
  if (CPU_IS_SCMP) return scmp_m_PC.w.l;
  else if (CPU_IS_MOTOROLA) return get_ql_pc();
  else return reg_pc;
}

unsigned int adjust_address_space_cpu(unsigned int direccion)
{
  if (!CPU_IS_MOTOROLA) direccion &=0xFFFF;
  else {
        //direccion &=0xFFFFF; //20 bits direccion
        direccion &=QL_MEM_LIMIT; //si son 256k totales, esto vale 0x3FFFF
  }

  return direccion;
}


void convert_signed_unsigned(char *origen, unsigned char *destino,int longitud)
{
        int i;
        z80_byte leido;

        for (i=0;i<longitud;i++) {
                leido=*origen;
                leido=128+leido;

                *destino=leido;
                destino++;

                origen++;
        }

}

//Retorna 0 si ok
/*
Al descomprimir bloques de datos de RZX suele decir:
gunzip: invalid compressed data--crc error
Aunque descomprime bien. Sera porque en caso del RZX le agrego una cabecera manualmente
*/
int uncompress_gz(char *origen,char *destino)
{

  debug_printf(VERBOSE_INFO,"Uncompressing %s to %s",origen,destino);

//Descomprimir
char uncompress_command[PATH_MAX];
char uncompress_program[PATH_MAX];


sprintf (uncompress_program,"%s",external_tool_gunzip);
sprintf (uncompress_command,"cat %s | %s -c  > \"%s\" ",origen,external_tool_gunzip,destino);

if (system (uncompress_command)==-1) return 1;

return 0;
}

char *mingw_strcasestr(const char *arg1, const char *arg2)
{
   const char *a, *b;

   for(;*arg1;arg1++) {

     a = arg1;
     b = arg2;

     while((*a | 32) == (*b | 32)) {
	a++;
	b++;
       if(!*b)
         return (char *)arg1;
     }

   }

   return NULL;
}

char *util_strcasestr(char *string, char *string_a_buscar)
{

        //Debe coincidir desde el principio de string
#ifdef MINGW
        char *coincide=mingw_strcasestr(string,string_a_buscar);
#else
        char *coincide=strcasestr(string,string_a_buscar);
#endif

        return coincide;

}


void util_sprintf_address_hex(menu_z80_moto_int p,char *string_address)
{
  if (CPU_IS_MOTOROLA) {
    sprintf (string_address,"%06XH",p);
  }

  else {
    sprintf (string_address,"%04XH",p);
  }
}



//Separar comando con codigos 0 y rellenar array de parametros
int util_parse_commands_argvc(char *texto, char *parm_argv[], int maximo)
{

        int args=0;

        while (*texto) {
                //Inicio parametro
                parm_argv[args++]=texto;
                if (args==maximo) {
                        debug_printf(VERBOSE_DEBUG,"Max parameters reached (%d)",maximo);
                        return args;
                }

                //Ir hasta espacio o final
                while (*texto && *texto!=' ') {
                        texto++;
                }

                if ( (*texto)==0) return args;

                *texto=0; //Separar cadena
                texto++;
        }

        return args;
}

//Retorna 0 si ok. No 0 si error. Ancho expresado en pixeles. Alto expresado en pixeles
//Source es en crudo bytes monocromos. ppb sera 8 siempre
int util_write_pbm_file(char *archivo, int ancho, int alto, int ppb, z80_byte *source)
{

  FILE *ptr_destino;
  ptr_destino=fopen(archivo,"wb");

  if (ptr_destino==NULL) {
    debug_printf (VERBOSE_ERR,"Error writing pbm file");
    return 1;
  }


    //Escribir cabecera pbm
  	char pbm_header[20]; //Suficiente

  	sprintf (pbm_header,"P4\n%d %0d\n",ancho,alto);

  	fwrite(pbm_header,1,strlen(pbm_header),ptr_destino);


  int x,y;
  for (y=0;y<alto;y++) {
    for (x=0;x<ancho/ppb;x++) {
      fwrite(source,1,1,ptr_destino);
      source++;
    }
  }

  fclose(ptr_destino);

  return 0;
}


void util_truncate_file(char *filename)
{

	debug_printf(VERBOSE_INFO,"Truncating file %s",filename);

	FILE *ptr_destino;
	ptr_destino=fopen(filename,"wb");

  	if (ptr_destino==NULL) {
    		debug_printf (VERBOSE_ERR,"Error truncating file");
    		return;
    	}

    	fclose(ptr_destino);

}


//Retorna tamanyo de zona y actualiza puntero a memoria indicada
//Si es 0, no existe
//Attrib: bit 0: read, bit 1: write

unsigned int machine_get_memory_zone_attrib(int zone, int *readwrite)
{

  //Por defecto
  int size=0;

  //Zona 0, ram speccy
  switch (zone) {
    case 0:


      *readwrite=3; //1 read, 2 write
      //como fallback, por defecto en spectrum 48kb ram
      size=49152;

      if (MACHINE_IS_SPECTRUM_128_P2_P2A_P3) {
        size=131072*mem128_multiplicador;
      }

      //Vigilar condiciones que pueden cumplir mas de una maquina a la vez

      if (MACHINE_IS_ZXUNO) {
        size=ZXUNO_SRAM_PAGES*16384;
      }

      if (MACHINE_IS_TBBLUE) {
        size=2*1024*1024; //Retornamos siempre zona memoria 2 MB
      }

      if (MACHINE_IS_TSCONF) {
        size=TSCONF_RAM_PAGES*16384;
      }

        if (MACHINE_IS_BASECONF) {
        size=BASECONF_RAM_PAGES*16384;
      }

      if (MACHINE_IS_QL) {
        size=QL_MEM_LIMIT+1-131072;
      }

      if (MACHINE_IS_CPC_464) {
        size=65536;
      }

      if (MACHINE_IS_INVES) {
	size=65536;
      }

      if (MACHINE_IS_SAM) {
	size=512*1024; //512 kb para simplificar->siempre el maximo
      }

      if (MACHINE_IS_Z88) {
      	size=0; //Solo zona memoria de 4 mb en caso de z88
      }
 

    break;

    //Zona rom
    case 1:


      *readwrite=1; //1 read, 2 write
      //como fallback por defecto en spectrum 16kb rom
      size=16384;

      if (MACHINE_IS_SPECTRUM_128_P2) {
        size=32768;
      }

      if (MACHINE_IS_SPECTRUM_P2A_P3) {
        size=65536;
      }

      //Vigilar condiciones que pueden cumplir mas de una maquina a la vez

      if (MACHINE_IS_ZXUNO) {
        size=16384;
      }

      if (MACHINE_IS_TBBLUE) {
        size=8192;
      }

      if (MACHINE_IS_TSCONF) {
        size=TSCONF_ROM_PAGES*16384;
      }

      if (MACHINE_IS_BASECONF) {
        size=BASECONF_ROM_PAGES*16384;
      }

      if (MACHINE_IS_QL) {
        size=49152;
      }

      if (MACHINE_IS_CPC_464) {
        size=32768;
      }

      if (MACHINE_IS_SAM) {
	size=32768;
      }

      if (MACHINE_IS_Z88) {
      	size=0; //Solo zona memoria de 4 mb en caso de z88
      }


    break;

    //diviface rom
    case 2:
      if (diviface_enabled.v) {
        *readwrite=1;
        size=DIVIFACE_FIRMWARE_KB*1024;
      }
    break;

    //diviface ram
    case 3:
      if (diviface_enabled.v) {
        *readwrite=3;
        size=(get_diviface_total_ram())*1024;
      }
    break;

    //zxuno flash
    case 4:
      if (MACHINE_IS_ZXUNO) {
        *readwrite=3;
        size=ZXUNO_SPI_SIZE*1024;
      }
    break;

    //tsconf fmaps
    case 5:
      if (MACHINE_IS_TSCONF) {
        *readwrite=3;
        size=TSCONF_FMAPS_SIZE;
      }
    break;

    //kartusho
    case 6:
	if (kartusho_enabled.v) {
          *readwrite=1;      
	  size=KARTUSHO_SIZE;
	}
    break;

    //dandanator
    case 7:
        if (dandanator_enabled.v) {
          *readwrite=1;
          size=DANDANATOR_SIZE;
        }
    break;


    //Superupgrade ram
    case 8:
        if (superupgrade_enabled.v) {
          *readwrite=3;
          size=SUPERUPGRADE_RAM_SIZE;
        }
    break;

    //Superupgrade rom
    case 9:
        if (superupgrade_enabled.v) {
          *readwrite=1;
          size=SUPERUPGRADE_ROM_SIZE;
        }
    break;

    //Z88 memory
    case 10:
    	if (MACHINE_IS_Z88) {
                *readwrite=1;
    		size=4*1024*1024; //Zona entera de los 4 MB
    	}
    break;



    //Betadisk
    case 11:
    	if (betadisk_enabled.v) {
                *readwrite=1;
    		size=BETADISK_SIZE; 
    	}
    break;


    //Multiface rom
    case 12:
    	if (multiface_enabled.v) {
                *readwrite=1;
    		size=8192;
    	}
    break;

    //Multiface ram
    case 13:
    	if (multiface_enabled.v) {
                *readwrite=3;
    		size=8192;
    	}
    break;

    //tbblue sprites
    case 14:
      if (MACHINE_IS_TBBLUE) {
        *readwrite=3;
        size=TBBLUE_MAX_SPRITES*TBBLUE_SPRITE_SIZE;
      }
    break;    

    //tsconf sprites
    case 15:
      if (MACHINE_IS_TSCONF) {
        *readwrite=3;
        size=4*1024*1024; //Puede ser toda la ram
      }
    break;        

	//memory zone by file. 16
    case MEMORY_ZONE_NUM_FILE_ZONE:
      if (memory_zone_by_file_size>0) {
        *readwrite=3;
        size=memory_zone_by_file_size;
      }
    break;     

    //tbblue copper
    case MEMORY_ZONE_NUM_TBBLUE_COPPER:
      if (MACHINE_IS_TBBLUE) {
        *readwrite=3;
        size=TBBLUE_COPPER_MEMORY;
      }
    break;          

    case MEMORY_ZONE_NUM_TIMEX_EXROM:
      if (MACHINE_IS_TIMEX_TS2068) {
        *readwrite=1;
        size=8192;
      }
    break;      

    case MEMORY_ZONE_NUM_TIMEX_DOCK:
      if (MACHINE_IS_TIMEX_TS2068) {
        *readwrite=1;
        size=65536;
      }
    break;     

  }

  return size;

}


z80_byte *machine_get_memory_zone_pointer(int zone, int address)
{

  z80_byte *p=NULL;

  //Zona 0, ram speccy
  switch (zone) {
    case 0:

    //Caso normal 48k como fallback
          p=&memoria_spectrum[address+16384];




      if (MACHINE_IS_SPECTRUM_128_P2) {
        //Saltar los 32kb de rom
        p=&memoria_spectrum[address+32768];
      }

      if (MACHINE_IS_SPECTRUM_P2A_P3) {
        //Saltar los 64kb de rom
        p=&memoria_spectrum[address+65536];
      }

      //Vigilar condiciones que pueden cumplir mas de una maquina a la vez

      if (MACHINE_IS_ZXUNO) {
        //saltar los primeros 16 kb de la rom del zxuno
        p=&memoria_spectrum[address+16384];

      }

      if (MACHINE_IS_TBBLUE) {
        p=&memoria_spectrum[address];
      }

      if (MACHINE_IS_TSCONF) {
        z80_byte *start=tsconf_ram_mem_table[0];
        p=&start[address];
      }

      if (MACHINE_IS_BASECONF) {
        z80_byte *start=baseconf_ram_mem_table[0];
        p=&start[address];
      }

      if (MACHINE_IS_QL) {
        p=&memoria_spectrum[address+131072];
      }

      if (MACHINE_IS_CPC_464) {
        z80_byte *start=cpc_ram_mem_table[0];
        p=&start[address];
      }

      if (MACHINE_IS_INVES) {
	p=&memoria_spectrum[address];
      }

      if (MACHINE_IS_SAM) {
	z80_byte *start=sam_ram_memory[0];
        p=&start[address]; 
      }



    break;


    //Zona 1, rom speccy
    case 1:

      //Caso normal 48k como fallback
      p=&memoria_spectrum[address];

      //En resto de maquinas suele tambien estar al principio del puntero memoria_spectrum

      if (MACHINE_IS_TBBLUE) {
        p=&tbblue_fpga_rom[address];
      }

      if (MACHINE_IS_INVES) {
	p=&memoria_spectrum[65536+address];
      }

    break;


    //diviface rom
    case 2:
      if (diviface_enabled.v) {
        p=&diviface_memory_pointer[address];
      }
    break;

    //diviface ram
    case 3:
      if (diviface_enabled.v) {
        p=&diviface_ram_memory_pointer[address];
      }
    break;

    //zxuno flash
    case 4:
      if (MACHINE_IS_ZXUNO) {
        p=&memoria_spectrum[ (ZXUNO_ROM_SIZE+ZXUNO_SRAM_SIZE)*1024 + address ];
      }
    break;

    case 5:
      if (MACHINE_IS_TSCONF) {
        p=&tsconf_fmaps[address];
      }
    break;


    //kartusho
    case 6:
        if (kartusho_enabled.v) {
	p=&kartusho_memory_pointer[address];
      }
    break;

    //dandanator
    case 7:
        if (dandanator_enabled.v) {
        p=&dandanator_memory_pointer[address];
      }
    break;


    //superupgrade ram
    case 8:
        if (superupgrade_enabled.v) {
        p=&superupgrade_ram_memory_pointer[address];
      }
    break;

    //superupgrade rom
    case 9:
        if (superupgrade_enabled.v) {
        p=&superupgrade_rom_memory_pointer[address];
      }
    break;

    case 10:
    	if (MACHINE_IS_Z88) {
    		p=&memoria_spectrum[address];
    	}
    break;

    //Betadisk
    case 11:
	if (betadisk_enabled.v) {
    		p=&betadisk_memory_pointer[address];
    	}
    break;


    //Multiface rom
    case 12:
    	if (multiface_enabled.v) {
    		p=&multiface_memory_pointer[address];
    	}
    break;

    //Multiface ram
    case 13:
    	if (multiface_enabled.v) {
    		p=&multiface_memory_pointer[address+8192];
    	}
    break;


    //tbblue sprites
    case 14:
      if (MACHINE_IS_TBBLUE) {
        p=tbsprite_new_patterns;
        p=p+address;
      }
    break;    


    //tsconf sprites
    case 15:
      if (MACHINE_IS_TSCONF) {
        p=tsconf_ram_mem_table[0];
        int offset=tsconf_return_spritesgraphicspage();
        offset+=address;

        //limitar a 4 mb
        offset &=0x3FFFFF;
        p +=offset;
      }
    break;           

	//memory zone by file. 16
	case MEMORY_ZONE_NUM_FILE_ZONE:
		if (memory_zone_by_file_size>0) {
			p=&memory_zone_by_file_pointer[address];
		}
	break;

    //tbblue copper
    case MEMORY_ZONE_NUM_TBBLUE_COPPER:
      if (MACHINE_IS_TBBLUE) {
        p=tbblue_copper_memory;
        p=p+address;
      }
    break;      

    case MEMORY_ZONE_NUM_TIMEX_EXROM:
      if (MACHINE_IS_TIMEX_TS2068) {
        z80_byte *start=timex_ex_rom_mem_table[0];
        p=&start[address];              
      }
    break;      

    case MEMORY_ZONE_NUM_TIMEX_DOCK:
      if (MACHINE_IS_TIMEX_TS2068) {
        z80_byte *start=timex_dock_rom_mem_table[0];
        p=&start[address];              
      }
    break;        


  }

  return p;

}

//Maximo texto: 15 de longitud

void machine_get_memory_zone_name(int zone, char *name)
{

  //Por defecto
  strcpy(name,"Unknown zone");

  //Zona 0, ram speccy
  switch (zone) {
    case 0:
		 //123456789012345
      strcpy(name,"Machine RAM");


    break;


    case 1:
		 //123456789012345
      strcpy(name,"Machine ROM");


    break;

    case 2:
      if (diviface_enabled.v) {
		   //123456789012345
        strcpy(name,"Diviface eprom");
      }
    break;

    case 3:
      if (diviface_enabled.v) {
		   //123456789012345
        strcpy(name,"Diviface ram");
      }
    break;

    //zxuno flash
    case 4:
      if (MACHINE_IS_ZXUNO) {
		   //123456789012345
        strcpy(name,"ZX-Uno flash");
      }
    break;

    case 5:
      if (MACHINE_IS_TSCONF) {
		   //123456789012345
        strcpy(name,"TSConf Fmaps");
      }
    break;

    //kartusho
    case 6:
        if (kartusho_enabled.v) {
		           //123456789012345
		strcpy(name,"Kartusho rom");
	}
    break;

    //dandanator
    case 7:
        if (dandanator_enabled.v) {
		           //123456789012345
                strcpy(name,"Dandanator rom");
        }
    break;


	//superupgrade ram
    case 8:
	if (superupgrade_enabled.v) {
		           //123456789012345
		strcpy(name,"Superupgr. ram");
	}
    break;


        //superupgrade rom
    case 9:
        if (superupgrade_enabled.v) {
		           //123456789012345
                strcpy(name,"Superupgr. rom");
        }
    break;


    case 10:
    	if (MACHINE_IS_Z88) {
    		strcpy(name,"Full 4 MB");
    	}
    break;

    //Betadisk
    case 11:
	if (betadisk_enabled.v) {
			   //123456789012345
		strcpy(name,"Betadisk rom");
	}
    break;


    //Multiface rom
    case 12:
    	if (multiface_enabled.v) {
    			   //123456789012345
		strcpy(name,"Multiface rom");
        }
    break;

    
    //Multiface ram
    case 13:
    	if (multiface_enabled.v) {
    			   //123456789012345
		strcpy(name,"Multiface ram");
        }
    break;   

    case 14:
        if (MACHINE_IS_TBBLUE) {
          		   //123456789012345
		strcpy(name,"TBBlue sprites");          
        }
    break;

    //tsconf sprites
    case 15:
      if (MACHINE_IS_TSCONF) {
          		   //123456789012345
		strcpy(name,"TSConf sprites");   
      }
        break;                   

	//memory zone by file. 16
	case MEMORY_ZONE_NUM_FILE_ZONE:
		if (memory_zone_by_file_size>0) {
          		   //123456789012345
		strcpy(name,"File zone");   
		}
	break;

    //tbblue copper. 17
    case MEMORY_ZONE_NUM_TBBLUE_COPPER:
      if (MACHINE_IS_TBBLUE) {
          		   //123456789012345
		strcpy(name,"TBBlue copper");   
      }
    break;   

    case MEMORY_ZONE_NUM_TIMEX_EXROM:
      if (MACHINE_IS_TIMEX_TS2068) {
        strcpy(name,"Timex EXROM");             
      }
    break;      

    case MEMORY_ZONE_NUM_TIMEX_DOCK:
      if (MACHINE_IS_TIMEX_TS2068) {
        strcpy(name,"Timex Dock");               
      }
    break;             

  }


}



int machine_get_next_available_memory_zone(int zone)
{
  //Dado una zona actual, busca primera disponible. Si llega al final, retorna -1
  //char nombre[1024];
  int readwrite;

  do {
    //printf ("Zone: %d\n",zone);
    if (zone>=MACHINE_MAX_MEMORY_ZONES) return -1;
    unsigned int size=machine_get_memory_zone_attrib(zone,&readwrite);
    if (size>0) return zone;
    zone++;
  } while (1);

}

void util_delete(char *filename)
{
	unlink(filename);
}



//Extraido de http://rosettacode.org/wiki/CRC-32#C
z80_long_int util_crc32_calculation(z80_long_int crc, z80_byte *buf, size_t len)
{
        static z80_long_int table[256];
        static int have_table = 0;
        z80_long_int rem;
        z80_byte octet;
        int i, j;
        z80_byte *p, *q;
 
        /* This check is not thread safe; there is no mutex. */
        if (have_table == 0) {
                /* Calculate CRC table. */
                for (i = 0; i < 256; i++) {
                        rem = i;  /* remainder from polynomial division */
                        for (j = 0; j < 8; j++) {
                                if (rem & 1) {
                                        rem >>= 1;
                                        rem ^= 0xedb88320;
                                } else
                                        rem >>= 1;
                        }
                        table[i] = rem;
                }
                have_table = 1;
        }
 
        crc = ~crc;
        q = buf + len;
        for (p = buf; p < q; p++) {
                octet = *p;  /* Cast to unsigned octet. */
                crc = (crc >> 8) ^ table[(crc & 0xff) ^ octet];
        }
        return ~crc;
}


//Retorna cuantos bits estan a 0 en un byte
int util_return_ceros_byte(z80_byte valor)
{
	int i;
	int ceros=0;
	for (i=0;i<8;i++) {
		if ((valor&1)==0) ceros++;

		valor=valor>>1;
	}

	return ceros;
}


void util_byte_to_binary(z80_byte value,char *texto)
{
	int i;

	for (i=0;i<8;i++) {
		if (value&128) *texto='1';
		else *texto='0';

		texto++;

		value=value<<1;
	}

	*texto=0; //fin cadena
}

void util_save_file(z80_byte *origin, long int tamanyo_origen, char *destination_file)
{


        FILE *ptr_destination_file;
        ptr_destination_file=fopen(destination_file,"wb");

                if (!ptr_destination_file) {
                        debug_printf (VERBOSE_ERR,"Can not open %s",destination_file);
                        return;
        }

        //Leer byte a byte... Si, es poco eficiente
        while (tamanyo_origen) {
        	fwrite(origin,1,1,ptr_destination_file);
                origin++;
        	tamanyo_origen--;
	}
        fclose(ptr_destination_file);

}


void util_copy_file(char *source_file, char *destination_file)
{

	long int tamanyo_origen=get_file_size(source_file);

	FILE *ptr_source_file;
        ptr_source_file=fopen(source_file,"rb");
        if (!ptr_source_file) {
                        debug_printf (VERBOSE_ERR,"Can not open %s",source_file);
                        return;
        }

        FILE *ptr_destination_file;
        ptr_destination_file=fopen(destination_file,"wb");

                if (!ptr_destination_file) {
                        debug_printf (VERBOSE_ERR,"Can not open %s",destination_file);
                        return;
        }

        z80_byte byte_buffer;

        //Leer byte a byte... Si, es poco eficiente
        while (tamanyo_origen) {
        	fread(&byte_buffer,1,1,ptr_source_file);
        	fwrite(&byte_buffer,1,1,ptr_destination_file);
        	tamanyo_origen--;
	}
        fclose(ptr_source_file);
        fclose(ptr_destination_file);


}

int si_existe_editionnamegame(char *nombre_final)
{
	return find_sharedfile("editionnamegame.tap",nombre_final);
}

//Carga el juego designado como edition name game. Devuelve 0 si no encontrado (en caso por ejemplo que el juego
//esté denegada la distribución
int util_load_editionnamegame(void)
{
	char nombre_final[PATH_MAX];
	if (si_existe_editionnamegame(nombre_final)) {
		debug_printf(VERBOSE_INFO,"Loading name edition game %s",EMULATOR_GAME_EDITION);
		strcpy(quickload_file,nombre_final);
		quickfile=quickload_file;
		//Forzar autoload
		z80_bit pre_noautoload;
		pre_noautoload.v=noautoload.v;
		noautoload.v=0;
		quickload(quickload_file);

		noautoload.v=pre_noautoload.v;
		return 1;
	}
	else return 0;
}


int util_extract_mdv(char *mdvname, char *dest_dir)
{
	//echo "./mdvtool /Users/chernandezba/Downloads/psion/ABACUS.MDV export_all /tmp/"

	//qlay mdv must be file size 174930
	if (get_file_size(mdvname)!=174930) {
		debug_printf(VERBOSE_ERR,"I can only open QLAY mdv file format - must be exactly 174930 bytes in size");
		return 1;
	}

	char *argumentos[]={
                "mdvtool","","export_all",""
        };

        argumentos[1]=mdvname;
        argumentos[3]=dest_dir;

	return main_mdvtool(4,argumentos);
}

int util_extract_hdf(char *hdfname, char *dest_dir)
{

	int leidos;

	//unsigned char buffer_lectura[1024];
        //Asignamos bloque de 64kb de lectura
        z80_byte *buffer_lectura;

        buffer_lectura=malloc(65536);
        if (buffer_lectura==NULL) {
                cpu_panic("Unable to assign read buffer");
        }

        //Obtener archivo
        char archivo[PATH_MAX];
        char archivo_sin_extension[PATH_MAX];
        char archivo_destino[PATH_MAX];

        //Solo nombre sin directorio
        util_get_file_no_directory(hdfname,archivo);

        //Solo nombre sin extension
        util_get_file_without_extension(archivo,archivo_sin_extension);

        //De un hdf puede salir un mmc o un ide
        sprintf (archivo_destino,"%s/%s.mmcide",dest_dir,archivo_sin_extension);

        FILE *ptr_inputfile;
        ptr_inputfile=fopen(hdfname,"rb");

        if (ptr_inputfile==NULL) {
                debug_printf (VERBOSE_ERR,"Error opening input file %s",hdfname);
                return 1;
        }

	FILE *ptr_outputfile;
	ptr_outputfile=fopen(archivo_destino,"wb");

        if (ptr_outputfile==NULL) {
                debug_printf (VERBOSE_ERR,"Error opening output file %s",archivo_destino);
                return 1;
        }

        //printf ("Input: %s output: %s\n",hdfname,archivo_destino);

	// Leer offset a datos raw del byte de cabecera:
	//0x09 DOFS WORD Image data offset This is the absolute offset in the HDF file where the actual hard-disk data dump starts.
	//In HDF version 1.1 this is 0x216.

	//Leemos 11 bytes de la cabecera
        fread(buffer_lectura,1,11,ptr_inputfile);

	int offset_raw=buffer_lectura[9]+256*buffer_lectura[10];

	debug_printf (VERBOSE_DEBUG,"HDF Offset to raw data: %d",offset_raw);


	//Ya hemos leido 11 bytes del principio
	int saltar_bytes=offset_raw-11;

	//Saltar esos bytes
	fread(buffer_lectura,1,saltar_bytes,ptr_inputfile);

	//Y vamos leyendo bloques de 1024
	unsigned long int escritos=0;

	do {
	        leidos=fread(buffer_lectura,1,65536,ptr_inputfile);
		if (leidos>0) {
			fwrite(buffer_lectura,1,leidos,ptr_outputfile);
			escritos +=leidos;
			debug_printf (VERBOSE_DEBUG,"Writing temporary data %dKB",escritos/1024);
		}
	} while (leidos>0);

        //Y luego rellenar archivo a siguiente valor valido de archivo .ide
        //Nota: para .mmc ya nos sirve, dado que .mmc necesita que sea multiple de 256 kb
        unsigned long int valid_ide_sizes[]={
		 8*1024*1024,   //0
		 16*1024*1024,
		 32*1024*1024,
		 64*1024*1024,
		 128*1024*1024,
		 256*1024*1024, //5
		 512*1024*1024,
		 1024*1024*1024,  //7. 1 GB
                 2147483648L, //8. 2 GB
        };

        int total_sizes=9;
        //Ver si coincide con alguno exactamente
        int i;
        int coincide=0;
        for (i=0;i<total_sizes && !coincide;i++) {
                if (escritos==valid_ide_sizes[i]) coincide=1;
        }

        if (!coincide) {
                //No coincide. Ver en que tramo esta
                for (i=0;i<total_sizes;i++) {
                        if (escritos<valid_ide_sizes[i]) break;
                }

                if (i==total_sizes) {
                        debug_printf(VERBOSE_ERR,"Final IDE image too big");
                }

                else {
                        unsigned long int final_size=valid_ide_sizes[i];
                        unsigned long int rellenar=final_size-escritos;
                        debug_printf (VERBOSE_DEBUG,"Adding %d KB until normal image size (%d KB)",rellenar/1024,final_size/1024);
                        z80_byte byte_relleno=0xFF;
                        while (rellenar) {
                            fwrite(&byte_relleno,1,1,ptr_outputfile);  
                            rellenar--;
                        }
                }
        }


	fclose (ptr_inputfile);

        fclose(ptr_outputfile);

        free(buffer_lectura);

	return 0;




}

//Cambiar en cadena s, caracter orig por dest
void util_string_replace_char(char *s,char orig,char dest)
{
	while (*s) {
		if ( (*s)==orig) *s=dest;
		s++;
	}
}


//Agrega una cadena de texto a otra con salto de linea al final. Retorna longitud texto agregado contando salto de linea
int util_add_string_newline(char *destination,char *text_to_add)
{
	int longitud_texto=strlen(text_to_add)+1; //Agregar salto de linea
	sprintf (destination,"%s\n",text_to_add);
 	return longitud_texto;

}

//De una cadena de bytes, lo muestra como hexadecimal, sin espacios. Retorna cadena acabada en 0
//Completa con espacios hasta longitud
void util_binary_to_hex(z80_byte *origen, char *destino, int longitud_max, int longitud)
{
	int i;

	for (i=0;i<longitud_max && i<longitud;i++) {
		sprintf(destino,"%02X",*origen);

		origen++;
		destino+=2;
	}

	for (;i<longitud_max;i++) {
		*destino=' ';
		destino++;
		*destino=' ';
		destino++;
	}

	*destino=0;
}



//De una cadena de bytes, lo muestra como ascii, sin espacios. si hay caracter no imprimible, muestra . . . Retorna cadena acabada en 0
//Completa con espacios
void util_binary_to_ascii(z80_byte *origen, char *destino, int longitud_max, int longitud)
{
	int i;
	z80_byte caracter;

	for (i=0;i<longitud_max && i<longitud;i++) {
		caracter=*origen;
		if (caracter<32 || caracter>126) caracter='.';
		*destino=caracter;

		origen++;
		destino++;
	}

	for (;i<longitud_max;i++) {
                *destino=' ';
        }

        *destino=0;
}





/*void util_file_save(char *filename,z80_byte *puntero, long int tamanyo);
{
	           FILE *ptr_filesave;
                                  ptr_filesave=fopen(filename,"wb");
                                  if (!ptr_filesave)
                                {
                                      debug_printf (VERBOSE_ERR,"Unable to open file %s",filename);

                                  }
                                else {  
                                        
                                        fwrite(puntero,1,tamanyo,ptr_filesave);
                                                
                                  fclose(ptr_filesave);


                                }

}*/

void util_file_append(char *filename,z80_byte *puntero, int tamanyo)
{
                   FILE *ptr_filesave;
                                  ptr_filesave=fopen(filename,"ab");
                                  if (!ptr_filesave)
                                {
                                      debug_printf (VERBOSE_ERR,"Unable to open file %s",filename);

                                  }
                                else {

                                        fwrite(puntero,1,tamanyo,ptr_filesave);

                                  fclose(ptr_filesave);


                                }

}

/*
Funcion usada en compresion de datos
Dado un puntero de entrada, y la longitud del bloque, dice cuantas veces aparecer el primer byte, y que byte es
Retorna: numero de repeticiones (minimo 1) y el byte que es (modifica contenido puntero *byte_repetido)
*/

int util_get_byte_repetitions(z80_byte *memoria,int longitud,z80_byte *byte_repetido)
{
	int repeticiones=1;

	//Primer byte
	z80_byte byte_anterior;
	byte_anterior=*memoria;
	//printf ("byte anterior: %d\n",byte_anterior);

	memoria++;
	longitud--;

	while (longitud>0 && (*memoria)==byte_anterior) {
		//printf ("longitud: %d memoria: %p\n",longitud,memoria);
		repeticiones++;
		memoria++;
		longitud--;
	}

	*byte_repetido=byte_anterior;
	return repeticiones;

}

void util_write_repeated_byte(z80_byte *memoria,z80_byte byte_a_repetir,int longitud)
{
	while (longitud>0) {
		*memoria=byte_a_repetir;

		longitud--;
		memoria++;
	}
}


/*
Funcion de compresion de datos
Dado un puntero de entrada, la longitud del bloque, y el byte "magico" de repeticion (normalmente DD), y el puntero de salida, retorna esos datos comprimidos, mediante:
siendo XX el byte magico, cuando hay un bloque con repeticion (minimo 5 repeticiones) se retorna como:
XX XX YY ZZ, siendo YY el byte a repetir y ZZ el numero de repeticiones (0 veces significa 256 veces)
Si no hay repeticiones, se retornan los bytes tal cual
*casos a considerar:
**si en entrada hay byte XX repetido al menos 2 veces, se trata como repeticion, sin tener que llegar al minimo de 5
**si el bloque de salida ocupa mas que el de entrada (que hubiesen muchos XX XX en la entrada) , el proceso que llama aqui lo debe considerar, para dejar el bloque sin comprimir
**si se repite mas de 256 veces, trocear en varios

Retorna: longitud del bloque destino


Considerar caso DD 00 00 00 00 00 ...  -> genera DD    DD DD 00 5 
Habria que ver al generar repeticion, si anterior era DD, hay que convertir el DD anterior en DD DD DD 01
Debe generar:
DD DD DD 01  DD DD 00 05

O sea, tenemos el DD anterior, agregamos DD DD 01

con zxuno.flash es uno de estos casos


Peor de los casos:

A)
DD DD 00  DD DD 00  ....  6 bytes
DD DD DD 02     DD DD DD 02   8 bytes

B)
DD 00 00 00 00 00   DD 00 00 00 00 00  ... 12 bytes
DD DD DD 01  DD DD 00 5    DD DD DD 01  DD DD 00 5   .... 16 bytes

consideramos lo peor que sea el doble

*/

int util_compress_data_repetitions(z80_byte *origen,z80_byte *destino,int longitud,z80_byte magic_byte)
{

	int longitud_destino=0;

	int antes_es_magic_aislado=0; //Si el de antes era un byte magic (normalmente DD) asilado

	while (longitud) {		
		z80_byte byte_repetido;
		int repeticiones=util_get_byte_repetitions(origen,longitud,&byte_repetido);
		//printf ("Remaining size: %d Byte: %02X Repetitions: %d\n",longitud,byte_repetido,repeticiones);
		//if (longitud<0) exit(1);

		origen +=repeticiones;
		longitud -=repeticiones;
		
		//Hay repeticiones
		if (repeticiones>=5
			||
		(byte_repetido==magic_byte && repeticiones>1)
		) {
			//Escribir magic byte dos veces, byte a repetir, y numero repeticiones
			//Si repeticiones > 256, trocear
			while (repeticiones>0) {
				if (antes_es_magic_aislado) {
					destino[0]=magic_byte;
					destino[1]=magic_byte;
					destino[2]=1;

					destino +=3;
					longitud_destino +=3;
				}

				destino[0]=magic_byte;
				destino[1]=magic_byte;
				destino[2]=byte_repetido;
				z80_byte brep;
				if (repeticiones>255) brep=0;
				else brep=repeticiones;

				destino[3]=brep;

				//printf ("%d %02X %02X %02X %02X\n",longitud,magic_byte,magic_byte,byte_repetido,brep);

				destino +=4;
				longitud_destino +=4;

				repeticiones -=256;
			}

			antes_es_magic_aislado=0;
		}

		else {
			//No hay repeticiones
			if (repeticiones==1 && byte_repetido==magic_byte) antes_es_magic_aislado=1;
			else antes_es_magic_aislado=0;


			util_write_repeated_byte(destino,byte_repetido,repeticiones);
			//printf ("%d %02X(%d)\n",longitud,byte_repetido,repeticiones);

			destino +=repeticiones;
			longitud_destino +=repeticiones;



		}
	}

	return longitud_destino;
	
}



/*
Funcion de descompresion de datos
siendo XX el byte magico, cuando hay un bloque con repeticion (minimo 5 repeticiones) se retorna como:
XX XX YY ZZ, siendo YY el byte a repetir y ZZ el numero de repeticiones (0 veces significa 256 veces)

Retorna: longitud del bloque destino
*/

int util_uncompress_data_repetitions(z80_byte *origen,z80_byte *destino,int longitud,z80_byte magic_byte)
{
        int longitud_destino=0;

        while (longitud) {
		//Si primer y segundo byte son el byte magic
		int repeticion=0;
		if (longitud>=4) {
			if (origen[0]==magic_byte && origen[1]==magic_byte) {
				repeticion=1;

				z80_byte byte_a_repetir=origen[2];

				int longitud_repeticion;
				longitud_repeticion=origen[3];

				if (longitud_repeticion==0) longitud_repeticion=256;

				util_write_repeated_byte(destino,byte_a_repetir,longitud_repeticion);

				origen+=4;
				longitud-=4;

				destino+=longitud_repeticion;
				longitud_destino+=longitud_repeticion;
			}
		}

		if (!repeticion) {
			*destino=*origen;
			
			origen++;
			longitud--;

			destino++;
			longitud_destino++;
		}
	}

	return longitud_destino;
}



//Obtener coordenada x,y de una direccion de pantalla dada
//x entre 0..255 (aunque sera multiple de 8)
//y entre 0..191
void util_spectrumscreen_get_xy(z80_int dir,int *xdest,int *ydest)
{
        //De momento para ir rapido, buscamos direccion en array de scanline
        //screen_addr_table

        //dir -=16384;  //TODO: quiza en vez de hacer una resta, hacer un AND para quitar ese bit. Quiza incluso quitar todos los bits 15,14,13
        //asi quitaria offsets 32768, 16384 y 8192

        dir &=(65535-32768-16384-8192);


        int indice=0;
        int x,y;
        for (y=0;y<192;y++) {
                for (x=0;x<32;x++) {

                        if (dir==screen_addr_table[indice]) {
                                *xdest=x*8;
                                *ydest=y;
                                return;
                        }

                        indice++;
                }
        }

}


//Convierte una pantalla de tipo spectrum (con sus particulares direcciones) a un sprite
void util_convert_scr_sprite(z80_byte *origen,z80_byte *destino)
{
	int x,y;
	z80_byte *linea_origen;

	for (y=0;y<192;y++) {
		int offset_origen=screen_addr_table[y*32] & 8191;
		linea_origen=&origen[offset_origen];
		for (x=0;x<32;x++) {
			*destino=*linea_origen;
			destino++;
			linea_origen++;
		}
	}
}

//Devolver valor sin signo
int util_get_absolute(int valor)
{
        if (valor<0) valor=-valor;

        return valor;
}


//Devolver signo de valor
int util_get_sign(int valor)
{
	if (valor<0) return -1;

	return +1;
}

//Rutina para extraer TAP pero tambien para convertir a TZX
//Si tzxfile !=NULL, lo convierte a tzx, en vez de expandir
int util_extract_tap(char *filename,char *tempdir,char *tzxfile)
{

	
	//tapefile
	if (util_compare_file_extension(filename,"tap")!=0) {
		debug_printf(VERBOSE_ERR,"Tape expander not supported for this tape type");
		return 1;
	}

	//Leemos cinta en memoria
	int total_mem=get_file_size(filename);

	z80_byte *taperead;



        FILE *ptr_tapebrowser;
        ptr_tapebrowser=fopen(filename,"rb");

        if (!ptr_tapebrowser) {
		debug_printf(VERBOSE_ERR,"Unable to open tape");
		return 1; 
	}

	taperead=malloc(total_mem);
	if (taperead==NULL) cpu_panic("Error allocating memory for tape browser");

	z80_byte *puntero_lectura;
	puntero_lectura=taperead;

        //Abrir fichero tzxfile si conviene convertir
        FILE *ptr_tzxfile;
        if (tzxfile!=NULL) {
                ptr_tzxfile=fopen(tzxfile,"wb");

                if (!ptr_tzxfile) {
                                debug_printf (VERBOSE_ERR,"Can not open %s",tzxfile);
                                return 1;
                }

                tape_write_tzx_header_ptr(ptr_tzxfile);
        }        


        int leidos=fread(taperead,1,total_mem,ptr_tapebrowser);

	if (leidos==0) {
                debug_printf(VERBOSE_ERR,"Error reading tape");
		free(taperead);
                return 1;
        }


        fclose(ptr_tapebrowser);

	char buffer_texto[32*4]; //4 lineas mas que suficiente

	int longitud_bloque;



	int filenumber=0;

	int previo_longitud_segun_cabecera=-1; //Almacena longitud de bloque justo anterior
	z80_byte previo_flag=255; //Almacena flag de bloque justo anterior 
	z80_byte previo_tipo_bloque=255; //Almacena previo tipo bloque anterior (0, program, 3 bytes etc)

	while(total_mem>0) {
		z80_byte *copia_puntero=puntero_lectura;
		longitud_bloque=util_tape_tap_get_info(puntero_lectura,buffer_texto);
		total_mem-=longitud_bloque;
		puntero_lectura +=longitud_bloque;
		//debug_printf (VERBOSE_DEBUG,"Tape browser. Block: %s",buffer_texto);


     //printf ("nombre: %s c1: %d\n",buffer_nombre,buffer_nombre[0]);

		char buffer_temp_file[PATH_MAX];
		int longitud_final=longitud_bloque-2-2; //Saltar los dos de cabecera, el de flag y el crc

		z80_byte tipo_bloque=255;

		//Si bloque de flag 0 y longitud 17 o longitud 34 (sped)
		z80_byte flag=copia_puntero[2];

		//printf ("flag %d previo_flag %d previolong %d longitud_final %d\n",flag,previo_flag,previo_longitud_segun_cabecera,longitud_final);

		int longitud_segun_cabecera=-1;

		if (flag==0 && (longitud_final==17 || longitud_final==34) ) {
			if (tzxfile==NULL) sprintf (buffer_temp_file,"%s/%02d-header-%s",tempdir,filenumber,buffer_texto);

			tipo_bloque=copia_puntero[3]; //0, program, 3 bytes etc

			//printf ("%s : tipo %d\n",buffer_temp_file,tipo_bloque);

			//Longitud segun cabecera
			longitud_segun_cabecera=value_8_to_16(copia_puntero[15],copia_puntero[14]);
		
		}
		else {
			char extension_agregar[10];
			extension_agregar[0]=0; //Por defecto

			//Si bloque de flag 255, ver si corresponde al bloque anterior de flag 0	
			if (flag==255 && previo_flag==0 && previo_longitud_segun_cabecera==longitud_final) {
				//Corresponde. Agregar extensiones bas o scr segun el caso
				if (previo_tipo_bloque==0) {
					//Basic
					strcpy(extension_agregar,".bas");
				}

				if (previo_tipo_bloque==3 && longitud_final==6912) {
					//Screen
                                        strcpy(extension_agregar,".scr");
                                }
			}

			if (tzxfile==NULL) sprintf (buffer_temp_file,"%s/%02d-data-%d%s",tempdir,filenumber,longitud_final,extension_agregar);
		}


                if (tzxfile==NULL) {
		        //Generar bloque con datos, saltando los dos de cabecera y el flag
		        util_save_file(copia_puntero+3,longitud_final,buffer_temp_file);
                }

                //Convertir a tzx
                else {
                        //Generar bloque con datos. TZX ID 10h, word pausa de 500ms, longitud
                        z80_byte buffer_tzx[5];
                        buffer_tzx[0]=0x10;
                        buffer_tzx[1]=244;
                        buffer_tzx[2]=1;

                        fwrite(buffer_tzx,1,3,ptr_tzxfile);

                        //Meter datos tal cual de tap: longitud, flag, datos, crc
                        fwrite(copia_puntero,1,longitud_bloque,ptr_tzxfile);


                }                

		filenumber++;

		previo_flag=flag;
		previo_longitud_segun_cabecera=longitud_segun_cabecera;
		previo_tipo_bloque=tipo_bloque;
	}


	free(taperead);

        if (tzxfile!=NULL) fclose(ptr_tzxfile);        

	return 0;

}


//Rutina para extraer TZX pero tambien para convertir a TAP
//Si tapfile !=NULL, lo convierte a tap, en vez de expandir
int util_extract_tzx(char *filename,char *tempdirectory,char *tapfile)
{

	
	//tapefile
	if (util_compare_file_extension(filename,"tzx")!=0) {
		debug_printf(VERBOSE_ERR,"Tape expander not supported for this tape type");
		return 1;
	}

	//Leemos cinta en memoria
	int total_mem=get_file_size(filename);

	z80_byte *taperead;



        FILE *ptr_tapebrowser;
        ptr_tapebrowser=fopen(filename,"rb");

        if (!ptr_tapebrowser) {
		debug_printf(VERBOSE_ERR,"Unable to open tape");
		return 1; 
	}

	taperead=malloc(total_mem);
	if (taperead==NULL) cpu_panic("Error allocating memory for tape browser/convert");

	z80_byte *puntero_lectura;
	puntero_lectura=taperead;

        //Abrir fichero tapfile si conviene convertir
        FILE *ptr_tapfile;
        if (tapfile!=NULL) {
                ptr_tapfile=fopen(tapfile,"wb");

                if (!ptr_tapfile) {
                                debug_printf (VERBOSE_ERR,"Can not open %s",tapfile);
                                return 1;
                }
        }


        int leidos=fread(taperead,1,total_mem,ptr_tapebrowser);

	if (leidos==0) {
                debug_printf(VERBOSE_ERR,"Error reading tape");
		free(taperead);
                return 1;
        }


        fclose(ptr_tapebrowser);

	char buffer_texto[32*4]; //4 lineas mas que suficiente

	int longitud_bloque;



	int filenumber=0;

	int previo_longitud_segun_cabecera=-1; //Almacena longitud de bloque justo anterior
	z80_byte previo_flag=255; //Almacena flag de bloque justo anterior 
	z80_byte previo_tipo_bloque=255; //Almacena previo tipo bloque anterior (0, program, 3 bytes etc)

	puntero_lectura +=10; //Saltar cabecera (version tzx etc)

	int salir=0;

		z80_byte *copia_puntero;

	while(total_mem>0 && !salir) {

		z80_byte tzx_id=*puntero_lectura;

		puntero_lectura++;

		switch (tzx_id) {

		case 0x10:
                case 0x11:

                //ID 10 - Standard Speed Data Block
                //ID 11 - Turbo Speed Data Block

                if (tzx_id==0x11) {
                        copia_puntero=puntero_lectura;
                        longitud_bloque=puntero_lectura[15]+256*puntero_lectura[16]+65536*puntero_lectura[17];
                        puntero_lectura+=18;
                }

                else {
		        puntero_lectura +=2;
		        total_mem -=2;
                        copia_puntero=puntero_lectura;
                        longitud_bloque=util_tape_tap_get_info(puntero_lectura,buffer_texto);
                }

		
		total_mem-=longitud_bloque;
		puntero_lectura +=longitud_bloque;
		//debug_printf (VERBOSE_DEBUG,"Tape browser. Block: %s",buffer_texto);


     //printf ("nombre: %s c1: %d\n",buffer_nombre,buffer_nombre[0]);

		char buffer_temp_file[PATH_MAX];
		int longitud_final;

                if (tzx_id==0x11) longitud_final=longitud_bloque-2;

                else longitud_final=longitud_bloque-2-2; //Saltar los dos de cabecera, el de flag y el crc

		z80_byte tipo_bloque=255;

		//Si bloque de flag 0 y longitud 17 o longitud 34 (sped)
		z80_byte flag=copia_puntero[2];

		//printf ("flag %d previo_flag %d previolong %d longitud_final %d\n",flag,previo_flag,previo_longitud_segun_cabecera,longitud_final);

		int longitud_segun_cabecera=-1;

		if (flag==0 && (longitud_final==17 || longitud_final==34) ) {
			if (tapfile==NULL) sprintf (buffer_temp_file,"%s/%02d-header-%s",tempdirectory,filenumber,buffer_texto);

			tipo_bloque=copia_puntero[3]; //0, program, 3 bytes etc

			//printf ("%s : tipo %d\n",buffer_temp_file,tipo_bloque);

			//Longitud segun cabecera
			longitud_segun_cabecera=value_8_to_16(copia_puntero[15],copia_puntero[14]);
		
		}
		else {
			char extension_agregar[10];
			extension_agregar[0]=0; //Por defecto

			//Si bloque de flag 255, ver si corresponde al bloque anterior de flag 0	
			if (flag==255 && previo_flag==0 && previo_longitud_segun_cabecera==longitud_final) {
				//Corresponde. Agregar extensiones bas o scr segun el caso
				if (previo_tipo_bloque==0) {
					//Basic
					strcpy(extension_agregar,".bas");
				}

				if (previo_tipo_bloque==3 && longitud_final==6912) {
					//Screen
                                        strcpy(extension_agregar,".scr");
                                }
			}

			if (tapfile==NULL) sprintf (buffer_temp_file,"%s/%02d-data-%d%s",tempdirectory,filenumber,longitud_final,extension_agregar);
		}


                //Si expandir
                if (tapfile==NULL) {
		        //Generar bloque con datos, saltando los dos de cabecera y el flag                
		        util_save_file(copia_puntero+3,longitud_final,buffer_temp_file);
                }

                //Convertir a tap
                else {
                        //Generar bloque con datos
                        //Meter longitud, flag
                        z80_byte buffer_tap[3];
                        z80_int longitud_cabecera_tap=longitud_final+2;
                        buffer_tap[0]=value_16_to_8l(longitud_cabecera_tap);
                        buffer_tap[1]=value_16_to_8h(longitud_cabecera_tap);
                        buffer_tap[2]=flag;
                        fwrite(buffer_tap,1,3,ptr_tapfile);


                        //Meter datos
                        fwrite(copia_puntero+3,1,longitud_final,ptr_tapfile);

                        //Agregar CRC
                        z80_byte byte_crc=*(copia_puntero+3+longitud_final);

                        buffer_tap[0]=byte_crc;
                        fwrite(buffer_tap,1,1,ptr_tapfile);

                }

		filenumber++;

		previo_flag=flag;
		previo_longitud_segun_cabecera=longitud_segun_cabecera;
		previo_tipo_bloque=tipo_bloque;

		break;


		case 0x20:


                                //sprintf(buffer_texto,"ID 20 - Pause");
                                //indice_buffer +=util_add_string_newline(&texto_browser[indice_buffer],buffer_texto);


                                puntero_lectura+=2;
				total_mem -=2;



                        break;

                        case 0x30:
                                //sprintf(buffer_texto,"ID 30 Text description:");
                                //indice_buffer +=util_add_string_newline(&texto_browser[indice_buffer],buffer_texto);

                                longitud_bloque=*puntero_lectura;
                                //printf ("puntero: %d longitud: %d\n",puntero,longitud_bloque);
                                //util_binary_to_ascii(&tzx_file_mem[puntero+1],buffer_bloque,longitud_bloque,longitud_bloque);
                                //indice_buffer +=util_add_string_newline(&texto_browser[indice_buffer],buffer_bloque);

                                puntero_lectura+=1;
                                puntero_lectura+=longitud_bloque;
                                //printf ("puntero: %d\n",puntero);

				total_mem -=(longitud_bloque+1);
                        break;

                        case 0x31:
                                //sprintf(buffer_texto,"ID 31 Message block:");
                                //indice_buffer +=util_add_string_newline(&texto_browser[indice_buffer],buffer_texto);

                                longitud_bloque=puntero_lectura[1];
                                //util_binary_to_ascii(&tzx_file_mem[puntero+2],buffer_bloque,longitud_bloque,longitud_bloque);
                                //indice_buffer +=util_add_string_newline(&texto_browser[indice_buffer],buffer_bloque);

                                puntero_lectura+=2;
                                puntero_lectura+=longitud_bloque;
	
				total_mem -=(longitud_bloque+2);
                        break;

                        case 0x32:
                                //sprintf(buffer_texto,"ID 32 Archive info:");
                                //indice_buffer +=util_add_string_newline(&texto_browser[indice_buffer],buffer_texto);

                                longitud_bloque=puntero_lectura[0]+256*puntero_lectura[1];
                                puntero_lectura+=2;


                                z80_byte nstrings=*puntero_lectura;
				puntero_lectura++;

				total_mem -=3;

				//char buffer_text_id[256];
                                //char buffer_text_text[256];

                                for (;nstrings;nstrings--) {
                                        //z80_byte text_type=*puntero_lectura;
					puntero_lectura++;
                                        z80_byte longitud_texto=*puntero_lectura;
					puntero_lectura++;

					total_mem -=2;

                                        //tape_tzx_get_archive_info(text_type,buffer_text_id);
                                        //util_binary_to_ascii(&tzx_file_mem[puntero],buffer_text_text,longitud_texto,longitud_texto);
                                        //sprintf (buffer_texto,"%s: %s",buffer_text_id,buffer_text_text);

                                        //indice_buffer +=util_add_string_newline(&texto_browser[indice_buffer],buffer_texto);

                                        puntero_lectura +=longitud_texto;

					total_mem -=longitud_texto;
                                }

                        break;

		default:
                        debug_printf(VERBOSE_DEBUG,"Unhandled TZX id %02XH. Stopping",tzx_id);
			salir=1;
		break;

	   }
	}


	free(taperead);

        if (tapfile!=NULL) fclose(ptr_tapfile);

	return 0;

}




//Realmente lo que hacemos es copiar el .p en .baszx81 con un offset , saltando datos iniciales para situarnos en el programa basic
int util_extract_p(char *filename,char *tempdir)
{

	
	//tapefile
	if (util_compare_file_extension(filename,"p")!=0) {
		debug_printf(VERBOSE_ERR,"Expander not supported for this file type");
		return 1;
	}


	//Leemos archivo en memoria
	int total_mem=get_file_size(filename);

	z80_byte *taperead;



        FILE *ptr_tapebrowser;
        ptr_tapebrowser=fopen(filename,"rb");

        if (!ptr_tapebrowser) {
		debug_printf(VERBOSE_ERR,"Unable to open file");
		return 1; 
	}

	taperead=malloc(total_mem);
	if (taperead==NULL) cpu_panic("Error allocating memory for expander");


        int leidos=fread(taperead,1,total_mem,ptr_tapebrowser);

	if (leidos==0) {
                debug_printf(VERBOSE_ERR,"Error reading tape");
		free(taperead);
                return 1;
        }


        fclose(ptr_tapebrowser);



        char buffer_temp_file[PATH_MAX];
        sprintf (buffer_temp_file,"%s/basic-data.baszx81",tempdir);

        int offset=116;
        //116 = 16509-0x4009

        int longitud_final=total_mem-offset;

        util_save_file(&taperead[offset],longitud_final,buffer_temp_file);
        

        free(taperead);

	return 0;

}


//Realmente lo que hacemos es copiar el .o en .baszx80 con un offset , saltando datos iniciales para situarnos en el programa basic
int util_extract_o(char *filename,char *tempdir)
{

	
	//tapefile
	if (util_compare_file_extension(filename,"o")!=0) {
		debug_printf(VERBOSE_ERR,"Expander not supported for this file type");
		return 1;
	}


	//Leemos archivo en memoria
	int total_mem=get_file_size(filename);

	z80_byte *taperead;



        FILE *ptr_tapebrowser;
        ptr_tapebrowser=fopen(filename,"rb");

        if (!ptr_tapebrowser) {
		debug_printf(VERBOSE_ERR,"Unable to open file");
		return 1; 
	}

	taperead=malloc(total_mem);
	if (taperead==NULL) cpu_panic("Error allocating memory for expander");


        int leidos=fread(taperead,1,total_mem,ptr_tapebrowser);

	if (leidos==0) {
                debug_printf(VERBOSE_ERR,"Error reading tape");
		free(taperead);
                return 1;
        }


        fclose(ptr_tapebrowser);



        char buffer_temp_file[PATH_MAX];
        sprintf (buffer_temp_file,"%s/basic-data.baszx80",tempdir);

        int offset=40;
        //40 = 16424-16384

        int longitud_final=total_mem-offset;

        util_save_file(&taperead[offset],longitud_final,buffer_temp_file);
        

        free(taperead);

	return 0;

}







int util_extract_trd(char *filename,char *tempdir)
{

	//tapefile
	if (util_compare_file_extension(filename,"trd")!=0) {
		debug_printf(VERBOSE_ERR,"Expander not supported for this file type");
		return 1;
	}

	//Leemos trd en memoria
	int bytes_to_load=get_file_size(filename);


	int tamanyo_trd_entry=16;


	z80_byte *trd_file_memory;
	trd_file_memory=malloc(bytes_to_load);
	if (trd_file_memory==NULL) {
		debug_printf(VERBOSE_ERR,"Unable to assign memory");
		return 0;
	}
	
	//Leemos cabecera archivo trd
        FILE *ptr_file_trd_browser;
        ptr_file_trd_browser=fopen(filename,"rb");

        if (!ptr_file_trd_browser) {
		debug_printf(VERBOSE_ERR,"Unable to open file");
		free(trd_file_memory);
		return 0;
	}


        int leidos=fread(trd_file_memory,1,bytes_to_load,ptr_file_trd_browser);

	if (leidos==0) {
                debug_printf(VERBOSE_ERR,"Error reading file");
                return 0;
        }


        fclose(ptr_file_trd_browser);

     


	char buffer_texto[64]; //2 lineas, por si acaso




	int start_track_8=256*8;


        z80_int files_on_disk=trd_file_memory[start_track_8+228];


        z80_int deleted_files_on_disk=trd_file_memory[start_track_8+244];




	int puntero,i;

	puntero=0;

        char buffer_temp_file[PATH_MAX];

        //TODO: Total de archivos incluye los borrados??
	for (i=0;i<files_on_disk+deleted_files_on_disk;i++) {
		menu_file_mmc_browser_show_file(&trd_file_memory[puntero],buffer_texto,1,9);
		if (buffer_texto[0]!='?') {

        		z80_byte start_sector=trd_file_memory[puntero+14];
	        	z80_byte start_track=trd_file_memory[puntero+15];
                        z80_int longitud_final=trd_file_memory[puntero+11]+256*trd_file_memory[puntero+12];
	        	debug_printf (VERBOSE_DEBUG,"File %s starts at track %d sector %d size %d",buffer_texto,start_track,start_sector,longitud_final);

                        //calcular offset
                        /*
                        TR-DOS disk specs v1.0 - last revised on 9-28-1997

- Max disk sides are 2
- Max logical tracks per side are 80
- Logical sectors per track are 16
- Sector dimension is 256 bytes
- Root directory is 8 sectors long starting from track 0, sector 1
- Max root entries are 128
- Root entry dimension is 16 bytes
- Logical sector 8 (9th physical) holds disc info
- Logical sectors from 0 to 15 are unused
- Files are *NOT* fragmented

                        */
                        int offset=16*256*start_track+256*start_sector;

                        //grabar archivo
                        sprintf (buffer_temp_file,"%s/%s",tempdir,buffer_texto);
		
           
                        util_save_file(&trd_file_memory[offset],longitud_final,buffer_temp_file);
		}


		puntero +=tamanyo_trd_entry;	



	}
	

	free(trd_file_memory);

        return 0;

}


int util_extract_dsk(char *filename,char *tempdir)  {


	int tamanyo_dsk_entry=32;

	int max_entradas_dsk=16;

	//Asignamos para 16 entradas
	//int bytes_to_load=tamanyo_dsk_entry*max_entradas_dsk;

	//Max 300kb
	int bytes_to_load=300000;  //temp. 4096

	z80_byte *dsk_file_memory;
	dsk_file_memory=malloc(bytes_to_load);
	if (dsk_file_memory==NULL) {
		debug_printf(VERBOSE_ERR,"Unable to assign memory");
		return 0;
	}
	
	//Leemos archivo dsk
        FILE *ptr_file_dsk_browser;
        ptr_file_dsk_browser=fopen(filename,"rb");

        if (!ptr_file_dsk_browser) {
		debug_printf(VERBOSE_ERR,"Unable to open file");
		free(dsk_file_memory);
		return 0;
	}


        int leidos=fread(dsk_file_memory,1,bytes_to_load,ptr_file_dsk_browser);

	if (leidos==0) {
                debug_printf(VERBOSE_ERR,"Error reading file");
                return 0;
        }


        fclose(ptr_file_dsk_browser);


        


	char buffer_texto[64]; //2 lineas, por si acaso

	//int longitud_bloque;

	//int longitud_texto;

	//char texto_browser[MAX_TEXTO_BROWSER];
	//int indice_buffer=0;

	


 	//sprintf(buffer_texto,"DSK disk image");
	//indice_buffer +=util_add_string_newline(&texto_browser[indice_buffer],buffer_texto);


/*
00000000  45 58 54 45 4e 44 45 44  20 43 50 43 20 44 53 4b  |EXTENDED CPC DSK|
00000010  20 46 69 6c 65 0d 0a 44  69 73 6b 2d 49 6e 66 6f  | File..Disk-Info|
00000020  0d 0a 43 50 44 52 65 61  64 20 76 33 2e 32 34 00  |..CPDRead v3.24.|
00000030  2d 01 00 00 13 13 13 13  13 13 13 13 13 13 13 13  |-...............|
00000040  13 13 13 13 13 13 13 13  13 13 13 13 13 13 13 13  |................|
00000050  13 13 13 13 13 13 13 13  13 13 13 13 00 00 00 00  |................|
00000060  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
*
00000100  54 72 61 63 6b 2d 49 6e  66 6f 0d 0a 00 00 00 00  |Track-Info......|
00000110  00 00 00 00 02 09 4e e5  00 00 c1 02 00 00 00 02  |......N.........|
00000120  00 00 c6 02 00 00 00 02  00 00 c2 02 00 00 00 02  |................|
00000130  00 00 c7 02 00 00 00 02  00 00 c3 02 00 00 00 02  |................|
00000140  00 00 c8 02 00 00 00 02  00 00 c4 02 00 00 00 02  |................|
00000150  00 00 c9 02 00 00 00 02  00 00 c5 02 00 00 00 02  |................|
00000160  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
*
00000200  00 43 4f 4d 50 49 4c 45  52 c2 49 4e 00 00 00 80  |.COMPILER.IN....|
00000210  02 03 04 05 06 07 08 09  0a 0b 0c 0d 0e 0f 10 11  |................|
00000220  00 43 4f 4d 50 49 4c 45  52 c2 49 4e 01 00 00 59  |.COMPILER.IN...Y|
00000230  12 13 14 15 16 17 18 19  1a 1b 1c 1d 00 00 00 00  |................|
00000240  00 4b 49 54 31 32 38 4c  44 c2 49 4e 00 00 00 03  |.KIT128LD.IN....|
00000250  1e 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
00000260  00 4b 49 54 31 32 38 20  20 c2 49 4e 00 00 00 80  |.KIT128  .IN....|
00000270  1f 20 21 22 23 24 25 26  27 28 29 2a 2b 2c 2d 2e  |. !"#$%&'()*+,-.|
00000280  00 4b 49 54 31 32 38 20  20 c2 49 4e 01 00 00 59  |.KIT128  .IN...Y|
*/

	//La extension es de 1 byte


/*
 	sprintf(buffer_texto,"Disk information:");
	indice_buffer +=util_add_string_newline(&texto_browser[indice_buffer],buffer_texto);

	util_binary_to_ascii(&dsk_file_memory[0], buffer_texto, 34, 34);
	indice_buffer +=util_add_string_newline(&texto_browser[indice_buffer],buffer_texto);


 	sprintf(buffer_texto,"\nCreator:");
	indice_buffer +=util_add_string_newline(&texto_browser[indice_buffer],buffer_texto);
	util_binary_to_ascii(&dsk_file_memory[0x22], buffer_texto, 14, 14);

	sprintf(buffer_texto,"\nTracks: %d",dsk_file_memory[0x30]);
	indice_buffer +=util_add_string_newline(&texto_browser[indice_buffer],buffer_texto);

	sprintf(buffer_texto,"Sides: %d",dsk_file_memory[0x31]);
	indice_buffer +=util_add_string_newline(&texto_browser[indice_buffer],buffer_texto);	


        sprintf(buffer_texto,"\nFirst PLUS3 entries:");
        indice_buffer +=util_add_string_newline(&texto_browser[indice_buffer],buffer_texto);
*/


	int puntero,i;
	//puntero=0x201;

	puntero=menu_dsk_get_start_filesystem(dsk_file_memory,bytes_to_load);
/*
en teoria , el directorio empieza en pista 0 sector 0, aunque esta info dice otra cosa:

                TRACK 0          TRACK 1             TRACK 2

SPECTRUM +3    Reserved          Directory             -
                               (sectors 0-3)


Me encuentro con algunos discos en que empiezan en pista 1 y otros en pista 0 ??

*/

/*
TODO. supuestamente entradas del directorio pueden ocupar 4 sectores. Actualmente solo hago 1
*/

	if (puntero==-1) {
		//printf ("Filesystem track/sector 0/0 not found. Guessing it\n");
		//no encontrado. probar con lo habitual
		puntero=0x200;
	}
	//else {
		//Si contiene e5 en el nombre, nos vamos a pista 1
		if (dsk_file_memory[puntero+1]==0xe5) {
			//printf ("Filesystem doesnt seem to be at track 0. Trying with track 1\n");
                        int total_pistas=bytes_to_load/4864;

                        puntero=menu_dsk_getoff_track_sector(dsk_file_memory,total_pistas,1,0);

			//printf ("puntero: %d\n",puntero);

			if (puntero==-1) {
		                //printf ("Filesystem track/sector 1/0 not found. Guessing it\n");
		                //no encontrado. probar con lo habitual
	                	puntero=0x200;
			}
			else 	{
                                //printf ("Filesystem found at offset %XH\n",puntero);
                        }
		}
		else {
                        //printf ("Filesystem found at offset %XH\n",puntero);
                }
	//}
	
	puntero++; //Saltar el primer byte en la entrada de filesystem

/*
TODO
Pista 1 suele empezar en offset 1400H, aunque me encuentro con algunos discos, como el http://www.worldofspectrum.org/infoseekid.cgi?id=0011456 (4 Top Games),
en que empieza en 1300H. Porque??
*/


	//z80_byte buffer_temp[80000];
	z80_byte *buffer_temp;
	buffer_temp=malloc(80000);
	

	for (i=0;i<max_entradas_dsk;i++) {

                z80_byte file_is_deleted=dsk_file_memory[puntero-1];
		menu_file_mmc_browser_show_file(&dsk_file_memory[puntero],buffer_texto,1,11);

		if (buffer_texto[0]!='?') {
                if (file_is_deleted==0xE5) debug_printf (VERBOSE_DEBUG,"File %s is deleted. Skipping",buffer_texto);
                else {
			//indice_buffer +=util_add_string_newline(&texto_browser[indice_buffer],buffer_texto);

			debug_printf (VERBOSE_DEBUG,"File %s",buffer_texto);

			z80_byte continuation_marker=dsk_file_memory[puntero+12-1]; //-1 porque empezamos el puntero en primera posicion

			//Averiguar inicio de los datos
			//Es un poco mas complejo ya que hay que localizar cada sector donde esta ubicado
			int total_bloques=1;
			int bloque;

			bloque=dsk_file_memory[puntero+15];

			//Este bloque indica el primer bloque de 1k del archivo. Esta ubicado en el principio de cada entrada de archivo+16
			//(aqui hay 15 porque ya empezamos desplazados en 1 - 0x201)
			//Luego se pueden guardar hasta 16 bloques en esa entrada
			//Si el archivo ocupa mas de 16kb, se genera otra entrada con mismo nombre de archivo, con los siguientes bloques
			//Si ocupa mas de 32 kb, otra entrada mas, etc
			//Notas: archivo de 16 kb exactos, genera dos entradas de archivo, incluso ocupando un bloque del siguiente,
			//dado que al principio esta la cabecera de plus3dos
			//Desde la cabecera de plus3dos a los datos hay 0x80 bytes

			z80_int longitud_real_archivo=0;

			int destino_en_buffer_temp=0;

			do {
			
				int offset1,offset2;
				menu_dsk_getoff_block(dsk_file_memory,bytes_to_load,bloque,&offset1,&offset2);

				//Sacar longitud real, de cabecera plus3dos. Solo el primer sector contiene cabecera plus3dos y la primera entrada del archivo,
				//por tanto el primer sector contiene 512-128=384 datos, mientras que los siguientes,
				//contienen 512 bytes de datos. El sector final puede contener 512 bytes o menos
				if (total_bloques==1 && continuation_marker==0) {
					int offset_a_longitud=offset1+16;
					longitud_real_archivo=dsk_file_memory[offset_a_longitud]+256*dsk_file_memory[offset_a_longitud+1];
					debug_printf (VERBOSE_DEBUG,"Real length file %s read from PLUS3DOS header: %d",buffer_texto,longitud_real_archivo);

					memcpy(buffer_temp,&dsk_file_memory[offset1+128],512-128);
					destino_en_buffer_temp=destino_en_buffer_temp + (512-128);

					//Siguiente sector
					memcpy(&buffer_temp[destino_en_buffer_temp],&dsk_file_memory[offset2],512);
					//printf ("Escribiendo sector 2\n");

					destino_en_buffer_temp +=512;


				}

				else {

					//Los dos sectores
					memcpy(&buffer_temp[destino_en_buffer_temp],&dsk_file_memory[offset1],512);
					destino_en_buffer_temp +=512;

					memcpy(&buffer_temp[destino_en_buffer_temp],&dsk_file_memory[offset2],512);
					destino_en_buffer_temp +=512;
				}

			
				//printf ("b:%02XH of:%XH %XH  ",bloque,offset1,offset2);
				//Cada offset es un sector de 512 bytes

				total_bloques++;
				bloque=dsk_file_memory[puntero+15-1+total_bloques];



			} while (bloque!=0 && total_bloques<=16);

			//Grabar archivo
			char buffer_nombre_destino[PATH_MAX];
			sprintf (buffer_nombre_destino,"%s/%s",tempdir,buffer_texto);

			//Ver si es primer entrada de archivo (con lo que sobreescribimos) o si es segunda y siguientes, hacer append
/*

Byte 12
    _______________            Continuation marker.  Each directory entry
   | | | | | | | | |           provides space for 16 data blocks
   | | | |*| | | | |           identifying where data is stored on the
   | | | | | | | | |           disk.  Files exceeding 16K use additional
   |_|_|_|_|_|_|_|_|           entries to record data blocks.  For
                               files up to 16K and for the initial entry
   of a longer file this byte value is 00.  For the fist continuation
   it is 01 for the second 02 and so on.
*/

			int longitud_en_bloques=destino_en_buffer_temp; //(total_bloques-1)*1024;


			if (continuation_marker==0) {
                                int longitud_final=longitud_en_bloques;

                                //En este caso, se ha grabado el archivo inicial y se sabe la longitud real segun cabecera plus3dos
                                //si resulta que la longitud en bloques a guardar ahora es mayor que la cabecera, reducir
                                if (longitud_final>longitud_real_archivo) longitud_final=longitud_real_archivo;
				debug_printf (VERBOSE_DEBUG,"File entry is the first. Saving %d bytes on file",longitud_final);
				util_save_file(buffer_temp,longitud_final,buffer_nombre_destino);
			}
			
			else {
				debug_printf (VERBOSE_DEBUG,"File entry is not the first. Adding %d bytes to the file",longitud_en_bloques);
				util_file_append(buffer_nombre_destino,buffer_temp,longitud_en_bloques);
                                //TODO: al guardar entradas de archivo diferentes a la primera, 
                                //se agregan siempre longitudes en bloques de 1kb
                                //por lo que el archivo acabará ocupando mas de lo que deberia
			}


			debug_printf (VERBOSE_DEBUG,"Saving file %s",buffer_nombre_destino);

		}
                }

		puntero +=tamanyo_dsk_entry;	


		/*
		Como saber, de cada archivo, donde está ubicado en disco, y su longitud?
		Ejemplo:

00000200  00 4c 20 20 20 20 20 20  20 a0 a0 20 00 00 00 03  |.L       .. ....|
00000210  02 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
00000220  00 44 49 53 4b 20 20 20  20 a0 20 20 00 00 00 02  |.DISK    .  ....|
00000230  03 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
00000240  00 50 41 4e 54 20 20 20  20 42 41 4b 00 00 00 38  |.PANT    BAK...8|
00000250  04 05 06 07 08 09 0a 00  00 00 00 00 00 00 00 00  |................|
00000260  00 50 41 4e 54 20 20 20  20 20 20 20 00 00 00 38  |.PANT       ...8|
00000270  0b 0c 0d 0e 0f 10 11 00  00 00 00 00 00 00 00 00  |................|
00000280  00 50 41 4e 54 32 20 20  20 20 20 20 00 00 00 38  |.PANT2      ...8|
00000290  12 13 14 15 16 17 18 00  00 00 00 00 00 00 00 00  |................|
000002a0  00 50 41 4e 54 32 20 20  20 42 41 4b 00 00 00 38  |.PANT2   BAK...8|
000002b0  19 1a 1b 1c 1d 1e 1f 00  00 00 00 00 00 00 00 00  |................|
000002c0  00 50 41 4e 54 33 20 20  20 42 41 4b 00 00 00 38  |.PANT3   BAK...8|
000002d0  20 21 22 23 24 25 26 00  00 00 00 00 00 00 00 00  | !"#$%&.........|
000002e0  00 50 41 4e 54 33 20 20  20 20 20 20 00 00 00 38  |.PANT3      ...8|
000002f0  27 28 29 2a 2b 2c 2d 00  00 00 00 00 00 00 00 00  |'()*+,-.........|
00000300  00 42 41 53 49 43 20 20  20 20 20 20 00 00 00 03  |.BASIC      ....|
00000310  2e 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
00000320  e5 e5 e5 e5 e5 e5 e5 e5  e5 e5 e5 e5 e5 e5 e5 e5  |................|

Ese PANT es un code 16384,6913

Este es el DISK?:
00000800  50 4c 55 53 33 44 4f 53  1a 01 00 b3 00 00 00 00  |PLUS3DOS........|
00000810  33 00 0a 00 33 00 00 00  00 00 00 00 00 00 00 00  |3...3...........|
00000820  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
*
00000870  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 9b  |................|
00000880  00 0a 2f 00 e7 c3 a7 3a  f4 32 33 36 39 33 0e 00  |../....:.23693..|
00000890  00 8d 5c 00 2c c3 a7 3a  fd b0 22 32 33 39 39 39  |..\.,..:.."23999|
000008a0  22 3a ef 22 6c 22 af 3a  f9 c0 b0 22 32 34 30 30  |":."l".:..."2400|
000008b0  30 22 0d 00 00 00 00 00  00 00 00 00 00 00 00 00  |0"..............|


Este el PANT:

00001000  50 4c 55 53 33 44 4f 53  1a 01 00 81 1b 00 00 03  |PLUS3DOS........|
00001010  01 1b 00 40 00 80 00 00  00 00 00 00 00 00 00 00  |...@............|
00001020  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
*
00001070  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 f3  |................|
00001080  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
*
00001180  00 00 00 18 00 7c 7e 42  00 00 00 00 00 00 00 00  |.....|~B........|


Fijarse en la linea:
00001010  01 1b 00 40

1b01H=6913
4000H=16384

Aqui supuestamente esta PANT2:
(19968=4e00h)
00004e00  50 4c 55 53 33 44 4f 53  1a 01 00 82 1b 00 00 03  |PLUS3DOS........|
00004e10  02 1b 00 40 00 80 00 00  00 00 00 00 00 00 00 00  |...@............|
00004e20  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
*
00004e70  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 f5  |................|
00004e80  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|

PANT2 en la tabla de asignacion:
00000280  00 50 41 4e 54 32 20 20  20 20 20 20 00 00 00 38  |.PANT2      ...8|
00000290  12 13 14 15 16 17 18 00  00 00 00 00 00 00 00 00  |................|

Ese 12h indica de alguna manera que empieza en 4e00h
19968-512=19456
19456/1024=19 =18+1=12H+1


PANT en la tabla de asignacion:
00000260  00 50 41 4e 54 20 20 20  20 20 20 20 00 00 00 38  |.PANT       ...8|
00000270  0b 0c 0d 0e 0f 10 11 00  00 00 00 00 00 00 00 00  |................|



Este "Basic" es:
00000300  00 42 41 53 49 43 20 20  20 20 20 20 00 00 00 03  |.BASIC      ....|
00000310  2e 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|

2e+1=2f
2fh*1024=48128=0xBC00

BC00H+512=0xBE00

Aunque el basic esta en:

0000c800  50 4c 55 53 33 44 4f 53  1a 01 00 18 01 00 00 00  |PLUS3DOS........|
0000c810  98 00 00 80 98 00 00 00  00 00 00 00 00 00 00 00  |................|
0000c820  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
*
0000c870  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 41  |...............A|
0000c880  00 01 0e 00 ea 68 6f 6c  61 20 71 75 65 20 74 61  |.....hola que ta|
0000c890  6c 0d 00 02 02 00 f0 0d  00 03 21 00 f8 22 61 3a  |l.........!.."a:|
0000c8a0  70 61 6e 74 22 af 31 36  33 38 34 0e 00 00 00 40  |pant".16384....@|

c800h/1024=32h


		*/
	}
	


	//texto_browser[indice_buffer]=0;


	//menu_generic_message_tooltip("DSK file browser", 0, 0, 1, NULL, "%s", texto_browser);


	free(dsk_file_memory);

	return 0;

}



int util_extract_z88_card(char *filename,char *tempdir)  
{
      
        long int bytes_to_load=get_file_size(filename);

        z80_byte *flash_file_memory;
        flash_file_memory=malloc(bytes_to_load);
        if (flash_file_memory==NULL) {
                debug_printf(VERBOSE_ERR,"Unable to assign memory");
                return 1;
        }
        
        //Leemos archivo 
        FILE *ptr_file_flash_browser;
        ptr_file_flash_browser=fopen(filename,"rb");

        if (!ptr_file_flash_browser) {
                debug_printf(VERBOSE_ERR,"Unable to open file");
                free(flash_file_memory);
                return 1;
        }


        int leidos=fread(flash_file_memory,1,bytes_to_load,ptr_file_flash_browser);

        if (leidos==0) {
                debug_printf(VERBOSE_ERR,"Error reading file");
                return 1;
        }


        fclose(ptr_file_flash_browser);

        //El segundo byte tiene que ser 0 (archivo borrado) o '/'
        if (flash_file_memory[1]!=0 && flash_file_memory[1]!='/') {
                //No es archivo epr, eprom o flash con archivos. Salir
                debug_printf (VERBOSE_INFO,"This is not a Z88 File Card");
        	free(flash_file_memory);
        	return 1;
        }


        z88_eprom_flash_file file;


        z80_byte *dir;

        dir=flash_file_memory;

        char buffer_nombre[Z88_MAX_CARD_FILENAME+1];

        int retorno;
 

        do {
                z80_byte *dir_current;
                dir_current=dir;

                retorno=z88_eprom_new_ptr_flash_find_next(&dir,&file);
                if (retorno) {
                        z88_eprom_flash_get_file_name(&file,buffer_nombre);

			if (buffer_nombre[0]=='.') buffer_nombre[0]='D'; //archivo borrado
                 
                        z80_long_int tamanyo=file.size[0]+(file.size[1]*256)+(file.size[2]*65536)+(file.size[3]*16777216);
                        debug_printf (VERBOSE_INFO,"Name: %s size: %d",buffer_nombre,tamanyo);

                        //Datos empieza en dir+(byte longitud nombre)+longitud nombre+4(de tamanyo archivo)
                        z80_byte *data_ptr=dir_current+1+file.namelength+4;

                        //Si nombre empieza por /, evitar primer byte
                        int index_nombre=0;
                        if (buffer_nombre[0]=='/') index_nombre++;


                        //Grabar archivo
			char buffer_nombre_destino[PATH_MAX];
			sprintf (buffer_nombre_destino,"%s/%s",tempdir,&buffer_nombre[index_nombre]);

			util_save_file(data_ptr,tamanyo,buffer_nombre_destino);

                }

        } while (retorno!=0);


        free(flash_file_memory);


        return 0;
}



//Archivos de basic z88 acaban con 3 bytes: 00 FF FF
int file_is_z88_basic(char *filename)
{
        long int bytes_to_load=get_file_size(filename);

        z80_byte *flash_file_memory;
        flash_file_memory=malloc(bytes_to_load);
        if (flash_file_memory==NULL) {
                debug_printf(VERBOSE_ERR,"Unable to assign memory");
                return 0;
        }
        
        //Leemos archivo 
        FILE *ptr_file_flash_browser;
        ptr_file_flash_browser=fopen(filename,"rb");

        if (!ptr_file_flash_browser) {
                debug_printf(VERBOSE_ERR,"Unable to open file");
                free(flash_file_memory);
                return 0;
        }


        int leidos=fread(flash_file_memory,1,bytes_to_load,ptr_file_flash_browser);

        if (leidos==0) {
                debug_printf(VERBOSE_ERR,"Error reading file");
                return 0;
        }


        fclose(ptr_file_flash_browser);

        int esbasic=0;

        if (bytes_to_load>3) {
           if (flash_file_memory[bytes_to_load-3]==0x00 && 
                flash_file_memory[bytes_to_load-2]==0xFF &&      
                flash_file_memory[bytes_to_load-1]==0xFF) {

                debug_printf (VERBOSE_INFO,"File is probably Z88 Basic");

                esbasic=1;
           }
        }

        free(flash_file_memory);    

        return esbasic;
}



void util_save_game_config(char *filename)
{
  //Sobreescribimos archivos con settings actuales
  debug_printf (VERBOSE_INFO,"Writing configuration file");

  //Agregamos contenido
  char config_settings[20000]; //Esto es mas que suficiente

  char buffer_temp[MAX_CONFIG_SETTING]; //Para cadenas temporales
  //char buffer_temp2[MAX_CONFIG_SETTING]; //Para cadenas temporales

  int indice_string=0;

  int i;


  //Al menos generamos una linea en blanco inicial
                                              ADD_STRING_CONFIG,";Autogenerated autoconfig file");



  //Y empezamos a meter opciones

  get_machine_config_name_by_number(buffer_temp,current_machine_type);
  if (buffer_temp[0]!=0) {
                                              ADD_STRING_CONFIG,"--machine %s",buffer_temp);
  }

  if (frameskip)                              ADD_STRING_CONFIG,"--frameskip %d",frameskip);


  if (border_enabled.v==0)                    ADD_STRING_CONFIG,"--disableborder");


//Estas siguientes solo si es ZX80/81
  if (MACHINE_IS_ZX8081) {
        if (zx8081_get_standard_ram()!=16)          ADD_STRING_CONFIG,"--zx8081mem %d",zx8081_get_standard_ram());
        if (zx8081_vsync_sound.v)                   ADD_STRING_CONFIG,"--zx8081vsyncsound");
        if (ram_in_8192.v)                          ADD_STRING_CONFIG,"--zx8081ram8K2000");
        if (ram_in_32768.v)                         ADD_STRING_CONFIG,"--zx8081ram16K8000");
        if (ram_in_49152.v)                         ADD_STRING_CONFIG,"--zx8081ram16KC000");  
        if (wrx_present.v)                          ADD_STRING_CONFIG,"--wrx");
        if (chroma81.v)                             ADD_STRING_CONFIG,"--chroma81");
                                                    ADD_STRING_CONFIG,"--vsync-minimum-length %d",minimo_duracion_vsync); 
        if (video_zx8081_estabilizador_imagen.v==0) ADD_STRING_CONFIG,"--no-horiz-stabilization"); 
        if (video_zx8081_lnctr_adjust.v)            ADD_STRING_CONFIG,"--enablezx8081lnctradjust");        
  }


  if (rainbow_enabled.v)                      ADD_STRING_CONFIG,"--realvideo");
  if (video_interlaced_mode.v)                ADD_STRING_CONFIG,"--enableinterlaced");

  


//Estas solo si es Spectrum
  if (MACHINE_IS_SPECTRUM) {
        if (snow_effect_enabled.v)                  ADD_STRING_CONFIG,"--snoweffect");    
        if (ulaplus_presente.v)                     ADD_STRING_CONFIG,"--enableulaplus");
        if (spectra_enabled.v)                      ADD_STRING_CONFIG,"--enablespectra");  
        if (timex_video_emulation.v)                ADD_STRING_CONFIG,"--enabletimexvideo");
  }


  if (gigascreen_enabled.v)                   ADD_STRING_CONFIG,"--enablegigascreen");  

/*
--redefinekey src dest
--joystickkeyev evt key
*/

					ADD_STRING_CONFIG,"--clearredefinekey");
					ADD_STRING_CONFIG,"--cleareventlist");


						ADD_STRING_CONFIG,"--joystickemulated \"%s\"",joystick_texto[joystick_emulation]);


  //real joystick buttons to events
  for (i=0;i<MAX_EVENTS_JOYSTICK;i++) {
        if (realjoystick_events_array[i].asignado.v) {
                char texto_button[20];
                int button_type;
                button_type=realjoystick_events_array[i].button_type;

                util_write_config_aux_realjoystick(button_type, realjoystick_events_array[i].button, texto_button);

                ADD_STRING_CONFIG,"--joystickevent %s %s",texto_button,realjoystick_event_names[i]);
        }
  }


  //real joystick buttons to keys
  for (i=0;i<MAX_KEYS_JOYSTICK;i++) {
        if (realjoystick_keys_array[i].asignado.v) {
                char texto_button[20];
                int button_type;
                z80_byte caracter;
                caracter=realjoystick_keys_array[i].caracter;
                button_type=realjoystick_keys_array[i].button_type;

                //Si hay evento en vez de numero de boton, meter --joystickkeyev
                
                //printf ("Buscando evento para boton %d tipo %d\n",realjoystick_keys_array[i].button,button_type);
                int buscar_evento_index=realjoystick_buscar_evento_en_tabla(realjoystick_keys_array[i].button,button_type);
                if (buscar_evento_index>=0) {
                        //printf ("Encontrado evento %d para boton %d tipo %d\n",buscar_evento_index,realjoystick_keys_array[i].button,button_type);
                    ADD_STRING_CONFIG,"--joystickkeyev %s %d",realjoystick_event_names[buscar_evento_index],caracter);    
                }

                else {
                        util_write_config_aux_realjoystick(button_type, realjoystick_keys_array[i].button, texto_button);
                        ADD_STRING_CONFIG,"--joystickkeybt %s %d",texto_button,caracter);
                }
        }
  }


  //text osd keyboard
  for (i=0;i<osd_adv_kbd_defined;i++) {
          //Truco para poder poner " en el texto. Con barra invertida
          if (!strcmp(osd_adv_kbd_list[i],"\"")) ADD_STRING_CONFIG,"--text-keyboard-add \\");
        else ADD_STRING_CONFIG,"--text-keyboard-add \"%s\"",osd_adv_kbd_list[i]);
  }

        ADD_STRING_CONFIG,"--text-keyboard-length %d",adventure_keyboard_key_length);



         FILE *ptr_configfile;

     ptr_configfile=fopen(filename,"wb");
     if (!ptr_configfile) {
                        debug_printf(VERBOSE_ERR,"Cannot write configuration file %s",filename);
                        return;
      }

    fwrite(config_settings, 1, strlen(config_settings), ptr_configfile);


      fclose(ptr_configfile);


}


void util_add_text_adventure_kdb(char *texto)
{
	if (osd_adv_kbd_defined==MAX_OSD_ADV_KEYB_WORDS) {
		debug_printf (VERBOSE_ERR,"Maximum keyboard text entries reached");
		return;
	}

	if (strlen(texto)>MAX_OSD_ADV_KEYB_TEXT_LENGTH-1) {
		debug_printf (VERBOSE_ERR,"String %s too long to add to the keyboard text entries (max: %d)",texto,MAX_OSD_ADV_KEYB_TEXT_LENGTH-1);
		return;
	}



//OSD teclado aventura
/*
//numero maximo de entradas
#define MAX_OSD_ADV_KEYB_WORDS 40
//longitud maximo de cada entrada
#define MAX_OSD_ADV_KEYB_TEXT_LENGTH 20
*/


//3 entradas definidas de ejemplo
//int osd_adv_kbd_defined=100;
//char osd_adv_kbd_list[MAX_OSD_ADV_KEYB_WORDS][MAX_OSD_ADV_KEYB_TEXT_LENGTH]={
        //Truco para poder poner " en la configuracion, mientras no tenga un parser que me permita escapar,
        //es meter la barra invertida
        if (!strcmp(texto,"\\")) strcpy(osd_adv_kbd_list[osd_adv_kbd_defined++],"\"");

	else strcpy(osd_adv_kbd_list[osd_adv_kbd_defined++],texto);

}

void util_clear_text_adventure_kdb(void)
{
	osd_adv_kbd_defined=0;
}

int util_find_right_most_space(char *texto)
{
        int i=strlen(texto)-1;

        for (;i>=0;i--) {
                if (texto[i]!=' ') return i;
        }

        return i;
}

void util_clear_final_spaces(char *orig,char *destination)
{
        //Quitar espacios al final del texto
        //Primero localizar primer espacio de la derecha

        int indice=0;

        int espacio_derecha=util_find_right_most_space(orig);

        if (espacio_derecha>=0) {
            for (indice=0;indice<=espacio_derecha && orig[indice];indice++) {
                    destination[indice]=orig[indice];
            }    
        }

        destination[indice]=0;
}


//Usado en unpaws, ungac etc para agregar letra de hotkey automaticamente
#define TOTAL_UNPAWSGAC_HOTKEYS 26
int util_unpawsgac_hotkeys[TOTAL_UNPAWSGAC_HOTKEYS]; //De la A a la Z

void util_init_unpawsgac_hotkeys(void)
{
        int i;

        for (i=0;i<TOTAL_UNPAWSGAC_HOTKEYS;i++) {
             util_unpawsgac_hotkeys[i]=0;   
        }
}


//util_add_text_adventure_kdb(buffer_palabra);
void util_unpawsgac_add_word_kb(char *palabra)
{

        //Buffer de maximo tamanyo y agregando dos ~~ al principio
        char buffer_palabra_destino[MAX_OSD_ADV_KEYB_TEXT_LENGTH+2];
        //De momento sin hotkey
        sprintf (buffer_palabra_destino,"%s",palabra);        

        //Agregamos la palabra metiendo hotkey si conviene
        char inicial=letra_minuscula(*palabra);

        if (inicial>='a' && inicial<'z') {
                int indice=inicial-'a';

                if (util_unpawsgac_hotkeys[indice]==0) {
                        //Meterla con hotkey
                        util_unpawsgac_hotkeys[indice]=1;

                        sprintf (buffer_palabra_destino,"~~%s",palabra);
                }
        }

        util_add_text_adventure_kdb(buffer_palabra_destino);
}

char *quillversions_strings[]={
        "PAW",
        "Quill.A",
        "Unkown version 2",
        "Quill.C"
};

int util_unpaws_detect_version(z80_int *p_mainattr)
{
        int quillversion=0;

        z80_int MainTop=peek_word_no_time(65533);
        z80_int MainAttr=MainTop+311;

        if   (MainTop<=(65535-321) 
   && (MainTop>=(16384-311))
   && (peek_byte_no_time(MainAttr) == 16)
   && (peek_byte_no_time(MainAttr+2) == 17)
   && (peek_byte_no_time(MainAttr+4) == 18)
   && (peek_byte_no_time(MainAttr+6) == 19)
   && (peek_byte_no_time(MainAttr+8) == 20)
   && (peek_byte_no_time(MainAttr+10) == 21) 
        ) {
                //debug_printf (VERBOSE_DEBUG,"PAW signature found");
        }

  else {
       MainTop=26931;
      MainAttr=MainTop+977;
      if  (  (peek_byte_no_time(MainAttr) == 16)
        && (peek_byte_no_time(MainAttr+2) == 17)
        && (peek_byte_no_time(MainAttr+4) == 18)
        && (peek_byte_no_time(MainAttr+6) == 19)
        && (peek_byte_no_time(MainAttr+8) == 20)
        && (peek_byte_no_time(MainAttr+10) == 21)
      ) {
          //debug_printf (VERBOSE_DEBUG,"Quill.A signature found");
          quillversion=1;
      }

      else {
              MainTop=27356;
           MainAttr=MainTop+169;
           if (    (peek_byte_no_time(MainAttr) == 16)
             && (peek_byte_no_time(MainAttr+2) == 17)
             && (peek_byte_no_time(MainAttr+4) == 18)
             && (peek_byte_no_time(MainAttr+6) == 19)
             && (peek_byte_no_time(MainAttr+8) == 20)
             && (peek_byte_no_time(MainAttr+10) == 21)
           ) {
               //debug_printf (VERBOSE_DEBUG,"Quill.C signature found");
               quillversion=3;
           }

           else {
                   quillversion=-1;
           }
     
      }
  }

  if (quillversion>=0) debug_printf (VERBOSE_DEBUG,"%s signature found",quillversions_strings[quillversion]);

  *p_mainattr=MainAttr;
  return quillversion;

/*
QuillVersion:=0;

 MainTop:=DPeek(65533);
 MainAttr:=MainTop+311;
 IF   (MainTop<=(65535-321))
   and (MainTop>=(16384-311))
   and (Peek(MainAttr) = 16)
   and (Peek(MainAttr+2) = 17)
   and (Peek(MainAttr+4) = 18)
   and (Peek(MainAttr+6) = 19)
   and (Peek(MainAttr+8) = 20)
   and (Peek(MainAttr+10) = 21)
 THEN
     Writeln('PAW signature found.')
 ELSE BEGIN
      MainTop:=26931;
      MainAttr:=MainTop+977;
      IF    (Peek(MainAttr) = 16)
        and (Peek(MainAttr+2) = 17)
        and (Peek(MainAttr+4) = 18)
        and (Peek(MainAttr+6) = 19)
        and (Peek(MainAttr+8) = 20)
        and (Peek(MainAttr+10) = 21)
      THEN BEGIN
          Writeln('Quill.A signature found.');
          QuillVersion:=1;
      END
      ELSE BEGIN
           MainTop:=27356;
           MainAttr:=MainTop+169;
           IF    (Peek(MainAttr) = 16)
             and (Peek(MainAttr+2) = 17)
             and (Peek(MainAttr+4) = 18)
             and (Peek(MainAttr+6) = 19)
             and (Peek(MainAttr+8) = 20)
             and (Peek(MainAttr+10) = 21)
           THEN BEGIN
               Writeln('Quill.C signature found.');
               QuillVersion:=3;
           END
           ELSE
               Error(3);
      END
 END;

*/

}


/*
CONST tVocs:ARRAY [0..6] OF String=(
'Verb',
'Adverb',
'Noun',
'Adjective',
'Preposition',
'Conjunction',
'Pronoun');
*/
  char *unpaws_tvocs[]={
"Verb",
"Adverb",
"Noun",
"Adjective",
"Preposition",
"Conjunction",
"Pronoun"        
  };

int util_paws_dump_vocabulary(int *p_quillversion)
{


        /*

        Extracted from https://github.com/Utodev/unPAWs
UnPAWS takes snapshot files (.SNA, .SP, .Z80) of  Spectrum  games written  with PAWS or Quill system (version A or C), and  produces  text listings of the game database.

        if QuillVersion=0 then
  begin
     vocptr := DPeek(65509);
     OffVoc := DPeek(65509);

     WHILE (vocptr < 65509) AND  (Peek(vocptr)<>0) DO
     BEGIN
      WriteLn(FOut,char(PeekNeg(vocptr)),char(PeekNeg(vocptr+1)),
             char(PeekNeg(vocptr+2)),char(PeekNeg(vocptr+3)),char(PeekNeg(vocptr+4)),
             Peek(vocptr+5):5,' ',type_voc(Peek(vocptr+6)));
      VocPtr := VocPtr + 7 ;
     END;
  end
  else { Quill Vocabulary}
  begin
     if QuillVersion=1 then vocptr := DPeek(MainAttr+29)
     else vocptr := DPeek(MainAttr+32);
     OffVoc := vocptr;

     WHILE (vocptr < 65530) AND  (Peek(vocptr)<>0) DO
     BEGIN
      WriteLn(FOut,char(PeekNeg(vocptr)),char(PeekNeg(vocptr+1)),
             char(PeekNeg(vocptr+2)),char(PeekNeg(vocptr+3)),Peek(vocptr+4):5);
      VocPtr := VocPtr + 5 ;
     END;
  end;
        */

#define  MAXTVOC 6
/*
FUNCTION Type_Voc(C:Integer):String;
BEGIN
 IF (C>=0) AND (C<=MAXTVOC) then Type_Voc:=TVocs[c]
                            else Type_Voc:='RESERVED';
END;
*/





  int total_palabras=0;

  //Precargo palabras (solo 1 sinonimo de cada, la mas corta) antes en un array
  //tipo, indice
  char lista_palabras[MAXTVOC][256][6];

  //Inicializarlas vacias
  int i,j;
  for (i=0;i<MAXTVOC;i++) {
        for (j=0;j<256;j++) lista_palabras[i][j][0]=0;
  }

  int quillversion;

  z80_int mainattr;

  quillversion=util_unpaws_detect_version(&mainattr);

  if (quillversion<0) {
          debug_printf (VERBOSE_DEBUG,"It does not seem to be a Quill/PAW game");
          *p_quillversion=-1;
          return 0;
  }
  
  util_clear_text_adventure_kdb();

  z80_int vocptr;
  z80_int limite_voc;

  if (quillversion==0) {
          vocptr=peek_word_no_time(65509);
          limite_voc=65509;
  }

  else {
          if (quillversion==1) vocptr=peek_word_no_time(mainattr+29);
          else vocptr=peek_word_no_time(mainattr+32);
          limite_voc=65530;
  }



  while (vocptr < limite_voc && peek_byte_no_time(vocptr)!=0) {
          char palabra[6];
          z80_byte indice_palabra;
          z80_byte tipo_palabra;

          if (quillversion==0) {

                for (j=0;j<5;j++) palabra[j]=peek_byte_no_time(vocptr+j)^255;

                indice_palabra=peek_byte_no_time(vocptr+j);
                palabra[j]=0;
                tipo_palabra=peek_byte_no_time(vocptr+j+1);

          }

          else {
                for (j=0;j<4;j++) palabra[j]=peek_byte_no_time(vocptr+j)^255;

                indice_palabra=peek_byte_no_time(vocptr+j);
                palabra[j]=0;
                tipo_palabra=0; //no hay tipos. lo dejamos forzado                  
          }

          char buf_tipo_palabra[30];

          int reservado=0;

          if (tipo_palabra>=0 && tipo_palabra<=MAXTVOC) strcpy (buf_tipo_palabra,unpaws_tvocs[tipo_palabra]);
          else {
                  strcpy(buf_tipo_palabra,"RESERVED");
                  reservado=1;
          }

          if (quillversion==0) debug_printf (VERBOSE_DEBUG,"unPAWs dump. Vocabulary word: %s Index: %d Type: %s",palabra,indice_palabra,buf_tipo_palabra);
          else debug_printf (VERBOSE_DEBUG,"unPAWs dump. Vocabulary word: %s Index: %d",palabra,indice_palabra);

          if (!reservado) {
                  //Meter en array. Quitar antes espacios del final
                  char palabra_sin_espacios[6];
                  util_clear_final_spaces(palabra,palabra_sin_espacios);

                  int insertar=1;

                //Ver si ya habia un sinonimo, y en ese caso, ver si la de ahora es mas corta
                  if (lista_palabras[tipo_palabra][indice_palabra][0]!=0) {
                        int longitud_almacenada=strlen(lista_palabras[tipo_palabra][indice_palabra]);
                        int longitud_nueva=strlen(palabra_sin_espacios);

                        if (longitud_nueva>longitud_almacenada) insertar=0;
                  }

                  if (insertar) {
                        strcpy(lista_palabras[tipo_palabra][indice_palabra],palabra_sin_espacios);
                        debug_printf (VERBOSE_DEBUG,"Adding word %s to array list",palabra_sin_espacios);
                        total_palabras++;

                  }

                  else {
                      debug_printf (VERBOSE_DEBUG,"Not adding word %s to array list as it is a longer synonim of %s",
                      palabra_sin_espacios,lista_palabras[tipo_palabra][indice_palabra]);    
                  }

                  
          }

          if (quillversion==0) vocptr+=7;
          else vocptr+=5;
  }
  /*
            if (!reservado) {
                  //Meter a teclado de palabras. Quitar antes espacios del final
                  char palabra_sin_espacios[7];
                  util_clear_final_spaces(palabra,palabra_sin_espacios);
                  debug_printf (VERBOSE_DEBUG,"Adding word %s to OSD Adventure text keyboard",palabra_sin_espacios);
                  util_add_text_adventure_kdb(palabra_sin_espacios);

                  total_palabras++;
          }
        */

  //Y ahora agregamos la lista total del array
  if (quillversion==0) debug_printf (VERBOSE_DEBUG,"Adding words to OSD Adventure text keyboard");
  for (i=0;i<MAXTVOC;i++) {
          debug_printf (VERBOSE_DEBUG,"Adding words type %s",unpaws_tvocs[i]);
        for (j=0;j<256;j++) {
                  if (lista_palabras[i][j][0]!=0) {
                          debug_printf (VERBOSE_DEBUG,"Adding word %s to OSD Adventure text keyboard",lista_palabras[i][j]);
                        util_unpawsgac_add_word_kb(lista_palabras[i][j]);   
                }
        }
  }

  *p_quillversion=quillversion;
  return total_palabras;


}

z80_int readtokenised(z80_int puntero)
{
   z80_byte low, high;

   low=peek_byte_no_time(puntero++);
   high=peek_byte_no_time(puntero++);

   return ((high & 0x7f) << 8) + low;
}

#define MAX_DICT_GAC_ENTRIES 5000
#define MAX_DICT_GAC_STRING_LENGTH 30

int util_gac_palabras_agregadas;

int util_gac_get_offset_dictionary(int index)
{
        return index*(MAX_DICT_GAC_STRING_LENGTH+1);        
}

void util_gac_put_string_dictionary(int index,z80_byte *memoria,char *string)
{

        char string_shown[256];

        //menu_tape_settings_trunc_name(string,string_shown,MAX_DICT_GAC_STRING_LENGTH+1);       
        //int longitud_texto=strlen(string);
        strcpy(string_shown,string);
        string_shown[MAX_DICT_GAC_STRING_LENGTH]=0;
        int offset=util_gac_get_offset_dictionary(index);

        strcpy((char *)&memoria[offset],string_shown);
}

void util_gac_get_string_dictionary(int index,z80_byte *memoria,char *string)
{
        int offset=util_gac_get_offset_dictionary(index);
        if (index>MAX_DICT_GAC_ENTRIES) strcpy(string,"");

        else strcpy(string,(char *)&memoria[offset]);
}

void util_gac_readobjects(z80_int puntero,z80_int endptr,z80_byte *mem_diccionario)
{
        //z80_byte count,temp; 
        z80_int copia_puntero;

        z80_byte object,tamanyo,weight,scrap;

        int start;

      do {
      copia_puntero=puntero;
      object=peek_byte_no_time(puntero++);
      tamanyo=peek_byte_no_time(puntero++);
      weight=peek_byte_no_time(puntero++);
      start=peek_byte_no_time(puntero++);
      scrap=peek_byte_no_time(puntero++);
      start+=scrap<<8;
      tamanyo-=3;
      //puntero+=5;
      if (object!=0 && tamanyo!=0)
      {

         //len=0;
         z80_int dictentry=readtokenised(puntero);
         if ((dictentry&0xC000)==0xC000) {
                 debug_printf (VERBOSE_DEBUG,"Ignore. Is a puntuation word");
         }
         else {
                char buffer_palabra[MAX_DICT_GAC_STRING_LENGTH+1];
                //printf("token %d\n",dictentry);
                util_gac_get_string_dictionary(dictentry,mem_diccionario,buffer_palabra);
         
                debug_printf (VERBOSE_DEBUG,"Dictionary entry %d word: %s",dictentry,buffer_palabra); 

                if (strlen(buffer_palabra)) {
                        debug_printf (VERBOSE_DEBUG,"Adding word %s to OSD Adventure text keyboard",buffer_palabra);
                        util_unpawsgac_add_word_kb(buffer_palabra);
                        util_gac_palabras_agregadas++;
                }
         
                //strcat(objects[current]->description,readstring(infile, size));
         }
         puntero+=tamanyo;
         puntero+=3;
         //current++;
      }
      // move up to next object
      puntero=copia_puntero+tamanyo+5;
      //fseek(infile,fileptr+size+5,SEEK_SET);          

  } while (puntero<endptr);
}

void util_gac_readwords(z80_int puntero,z80_int endptr,z80_byte *mem_diccionario)
{
        z80_byte count,temp;
       temp=1;
       z80_int dictentry;

       do {

      count=peek_byte_no_time(puntero++);
      if (count==0)
      {
         temp=peek_byte_no_time(puntero);
      }
      if (count!=0 && temp!=0)
      {
         dictentry=readtokenised(puntero);
         char buffer_palabra[256];
         util_gac_get_string_dictionary(dictentry,mem_diccionario,buffer_palabra);
         debug_printf (VERBOSE_DEBUG,"Dictionary entry %d word: %s",dictentry,buffer_palabra);
         puntero+=2;

                if (strlen(buffer_palabra)) {
                        debug_printf (VERBOSE_DEBUG,"Adding word %s to OSD Adventure text keyboard",buffer_palabra);
                        util_unpawsgac_add_word_kb(buffer_palabra);
                        util_gac_palabras_agregadas++;
                }

         //strncpy(words[current]->word,dictionary[dictentry],60);
         //words[current]->number=count;
         //current++;
      }


       } while (puntero<endptr && count!=0 && temp!=0);
}

char *gacversions_strings[]={
        "GAC version 0",
};

int util_gac_detect_version(void)
{

        int version=0;

        //en 67cf se encuentra esta cadena
        char *signature="You have run out of memory";

        char read_signature[100];

        //Leemos la firma de la memoria
        int longitud_firma=strlen(signature);

        z80_int puntero=0x67cf;

        int i;
        for (i=0;i<longitud_firma;i++) {
                read_signature[i]=peek_byte_no_time(puntero+i);
        }

        read_signature[i]=0;
        if (strcmp(read_signature,signature)) version=-1;

        return version;

}


/* based on grackle tool
http://ifarchive.jmac.org/indexes/if-archiveXsolutionsXtools.html
Grackle 0.2 Alpha, by David Lodge. A dumper and player for adventures written using the Graphic Adventure Creator. At the moment it supports games from the Spectrum (in .sna format) and the Amstrad CPC (as extracted files with the AMSDOS header from a disk image)
*/
int util_gac_dump_dictonary(int *p_gacversion)
{

        int gacversion; //Realmente no se si hay mas de una version

        gacversion=util_gac_detect_version();

  if (gacversion<0) {
          debug_printf (VERBOSE_DEBUG,"It does not seem to be a GAC game");
          *p_gacversion=-1;
          return 0;
  }        

        util_clear_text_adventure_kdb();
        util_gac_palabras_agregadas=0;

        //Asignar memoria para el diccionario. 
        z80_byte *diccionario_array;

        diccionario_array=malloc(MAX_DICT_GAC_ENTRIES*(MAX_DICT_GAC_STRING_LENGTH+1));

        if (diccionario_array==NULL) cpu_panic("Can not allocate memory");

        //Array para el diccionario. Palabras de mas de 30 caracteres los ignoramos

        //char diccionario_array[MAX_DICT_GAC_ENTRIES][MAX_DICT_GAC_STRING_LENGTH+1];

        //Inicializar a ""
        int i;
        for (i=0;i<MAX_DICT_GAC_ENTRIES;i++) util_gac_put_string_dictionary(i,diccionario_array,"");

        z80_int spec_start=0xA51F;
        z80_int room_data=0xA54D;

        //Vamos primero a hacer dump del dicccionario
        z80_int dictptr=peek_word_no_time(spec_start+9*2); //Saltar los 9 word de delante


        z80_int nounptr=peek_word_no_time(spec_start);
        z80_int adverbptr=peek_word_no_time(spec_start+1*2);
        z80_int objectptr=peek_word_no_time(spec_start+2*2);
        z80_int roomptr=peek_word_no_time(spec_start+3*2);

        z80_int endptr=peek_word_no_time(spec_start+10*2); 

        z80_int verbptr=room_data+2;

        debug_printf (VERBOSE_DEBUG,"Dictionary start: %04XH",dictptr);

        z80_byte longitud_palabra;

        z80_int puntero=dictptr;
        int indice=0;

        do {
                longitud_palabra=peek_byte_no_time(puntero++);
                if (longitud_palabra>0) {
                        char palabra[256];
                        int i;
                        for (i=0;i<longitud_palabra;i++) {
                                z80_byte caracter_leido=peek_byte_no_time(puntero++) & 127;
                                if (caracter_leido<32) caracter_leido=32;
                                palabra[i]=caracter_leido & 127;
                        }

                        palabra[i]=0;

                        debug_printf (VERBOSE_DEBUG,"Dictonary word index %d: %s (length: %d)",indice,palabra,longitud_palabra);
                        if (longitud_palabra<=MAX_DICT_GAC_STRING_LENGTH) {
                                //strcpy(diccionario_array[indice],palabra);
                                util_gac_put_string_dictionary(indice,diccionario_array,palabra);
                        }
                        indice++;
                }
        } while (longitud_palabra!=0 && puntero<endptr);

       debug_printf (VERBOSE_DEBUG,"Dumping verbs. Start at %04XH",verbptr);
       util_gac_readwords(verbptr,nounptr,diccionario_array);       

       debug_printf (VERBOSE_DEBUG,"Dumping nouns. Start at %04XH",nounptr);
       util_gac_readwords(nounptr,adverbptr,diccionario_array);

       debug_printf (VERBOSE_DEBUG,"Dumping adverbs. Start at %04XH",adverbptr);
       util_gac_readwords(adverbptr,objectptr,diccionario_array);

       debug_printf (VERBOSE_DEBUG,"Dumping objects. Start at %04XH",objectptr);
       util_gac_readobjects(objectptr,roomptr,diccionario_array);

   /*


   printf("Reading adverbs: %x\n",header->adverbptr);
   header->adverbs=readwords(infile,header,adverbs,header->adverbptr,header->objectptr);       */

       /*puntero=nounptr;
       z80_byte count,temp;
       temp=1;
       z80_int dictentry;

       do {

      count=peek_byte_no_time(puntero++);
      if (count==0)
      {
         temp=peek_byte_no_time(puntero);
      }
      if (count!=0 && temp!=0)
      {
         dictentry=readtokenised(puntero);
         printf ("nombre token %d palabra: %s\n",dictentry,diccionario_array[dictentry]);
         puntero+=2;
         //strncpy(words[current]->word,dictionary[dictentry],60);
         //words[current]->number=count;
         //current++;
      }


       } while (puntero<endptr && count!=0 && temp!=0);*/


       // free(mem_diccionario);

       free(diccionario_array);

        *p_gacversion=gacversion;
        return util_gac_palabras_agregadas;
}


//Retorna 0 si ok. -1 si error
int util_unpawsetc_dump_words(char *mensaje)
{

        util_init_unpawsgac_hotkeys();

        int version;

	int palabras=util_paws_dump_vocabulary(&version);        

	//Es Paws?
	if (version>=0) {
                sprintf(mensaje,"OK. %s signature found. %d words added",
			quillversions_strings[version],palabras);
	}

	else {
		//No es paws. Probar con GAC
		palabras=util_gac_dump_dictonary(&version);
		if (version>=0) {
			sprintf(mensaje,"OK. %s signature found. %d words added",
				gacversions_strings[version],palabras);
		}	

		else {
			//Ni paws ni gac
			sprintf(mensaje,"It does not seem to be a Quill/PAW/GAC game");
		}
	}

        return version;
}

int util_is_digit(char c)
{
        if (c>='0' && c<='9') return 1;
        return 0;
}
