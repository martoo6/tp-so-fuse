#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "fat.h"

fat_boot_sector_t read_boot_sector(char *image)
{
	fat_boot_sector_t data;
	FILE *fp;

	printf("Opening file %s\n", image);
	if ((fp = fopen(image, "r")) == NULL)
		printf("error: %s", strerror(errno));

	printf("Reading Boot Sector from file %s\n\n", image);
	fread(&data, sizeof(fat_boot_sector_t), 1, fp);

	fclose(fp);
    return data;
}

void print_boot_sector(fat_boot_sector_t fat_bpb)
{
	int i;

	printf("Boot Sector:\n");
	printf("============\n");

	printf("JMP opcodes: %.2X %.2X %.2X\n", fat_bpb.jump[0], fat_bpb.jump[1], fat_bpb.jump[2]);
	printf("OEM Name: %8s\n", fat_bpb.oem_name);
	printf("Bytes per sector: %d\n", fat_bpb.bytes_per_sector);
	printf("Sectors per cluster: %d\n", fat_bpb.sectors_per_cluster);
	printf("reserved sectors: %d\n", fat_bpb.reserved_sectors);
	printf("FATs on volume: %d\n", fat_bpb.number_of_fats);
	printf("root directory entries: %d\n", fat_bpb.number_root_entries);
	printf("Sectors in volume: %d\n", fat_bpb.total_sectors);
	printf("Media descriptor type: %d\n", fat_bpb.media_descriptor);
	printf("Sectors per FAT: %d\n", fat_bpb.sectors_per_fat);
	printf("Sectors per Track: %d\n", fat_bpb.sectors_per_track);
	printf("heads: %d\n", fat_bpb.heads);
	printf("hidden sectors: %d\n", fat_bpb.hidden_sectors);
	printf("Huge sectors in volume: %d\n", fat_bpb.huge_total_sectors);
    printf("Drive number: %d\n", fat_bpb.drive_number);
    printf("Signature: %X\n", fat_bpb.signature);
    printf("Volume ID: %d\n", fat_bpb.volume_id);
    printf("Volume Label: ");
    for (i=0;i<11;i++)
            printf("%c", fat_bpb.volume_label[i]);
    printf("\n");
	printf("Fat Version: %d\n", fat_bpb.fat_version);
    printf("Root Cluster: %d\n", fat_bpb.root_cluster);
	printf("FAT Type: ");
    for (i=0;i<8;i++)
    	printf("%c", fat_bpb.fat_type_label[i]);
	printf("\n");
}

int main(int argc, char *argv[])
{
	print_boot_sector(read_boot_sector(argv[argc-1]));

	return 0;
}


