/*
 * fat.h
 *
 *  Created on: Sep 12, 2011
 *      Author: fernando
 */

#ifndef FAT_H_
#define FAT_H_

typedef struct fat_boot_sector {
  u_char	jump[3];			// 00 03 Jump
  u_char	oem_name[8];		// 03 08 nombre OEM
  u_short	bytes_per_sector;	// 11 02 bytes por sector
  u_char	sectors_per_cluster;// 13 01 sectores por cluster
  u_short	reserved_sectors;	// 14 02 sectores reservados
  u_char	number_of_fats;		// 16 01 cantidad de FATs
  u_short	number_root_entries;// 17 02 Entradas maximas en directorio raiz. FAT32 -> 0
  u_short	total_sectors;		// 19 02 Sectores totales, si es cero mirar 'huge_total_sectors'
  u_char	media_descriptor;	// 21 01 Descriptor de medio
  u_short	sectors_per_fat;	// 22 02 Sectores por fat
  u_short	sectors_per_track;	// 24 02 Sectores por pista
  u_short	heads;				// 26 02 Numero de caras
  u_int		hidden_sectors;		// 28 04 Sectores ocultos
  u_int		huge_total_sectors;	// 32 04 Longitud total de sectores, si es mayor a 65535

  //Exclusivos FAT32
  u_int		huge_sectors_per_fat;// 36 04 sectores por FAT (sectors_per_fat>65536)
  u_short	flags;				// 40 02 Banderas
  u_short	fat_version;		// 42 02 Version FAT
  u_int		root_cluster;		// 44 04 Numero de cluster del directorio raiz
  u_short	fsinfo_cluster;		// 48 02 Numero de sector del structura "File system information"
  u_short	backup_cluster;		// 50 02 Numero de sector donde se encuentra el backup del Boot sector
  u_char	reserved[12];		// 52 12 Reservado
  u_char	drive_number;		// 64 01 Numero de unidad
  u_char	winnt_flags;		// 65 01 Banderas WINNT
  u_char	signature;			// 66 01 Firma extendida
  u_int		volume_id;			// 67 04 numero de serie de la particion
  u_char	volume_label[11];	// 71 11 Etiqueta del volumen
  u_char	fat_type_label[8];	// 82 08 Nombre de FAT
}__attribute__((packed)) fat_boot_sector_t;

#endif /* FAT_H_ */
