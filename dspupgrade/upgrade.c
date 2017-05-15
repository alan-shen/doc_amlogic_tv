#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <getopt.h>

#define DEBUG (0)
#define DBG if(DEBUG)printf

#define bool int
#define false 0
#define true 1

#define DSP_CARD_INDEX 0
#define VERSION_OFFSET 0x1007c
#define DSP_READ_COMMAND_STR "DSP Read Command"
#define DSP_FW_VERSION_INDEX_BASE 0xefc00048
#define FLASH_KERNEL_MODULE_NAME "gida_spidev"
#define FLASH_DEV_NODE "/dev/gida_spidev0.1"

void *module_dt = NULL;
void *fw_dt     = NULL; 
int  module_sz  = 0;
int  fw_sz      = 0;

int spi_flash_fd;

#if 0
char timestamp[256];
char* get_timestamp(void)
{
	time_t now;
	struct tm *timenow;
	char strtemp[255];
  
	time(&now);
	timenow = localtime(&now);
	sprintf(timestamp, "%s", asctime(timenow));
	return timestamp;
}

#define debug_info(fmt, args...) do { \
		char log[256]; \
		char time_s[26]; \
		strcpy(time_s, get_timestamp()); \
		time_s[25] = '\0'; \
		sprintf(log, fmt, ##args); \
		printf("[%s] %s", time_s, log); \
} while (0)
#endif

void correct_version(uint32_t *version)
{
    uint32_t older_ver = *version;
    /* for example:
     * 1.0.8  will be 0x01000800
     * 1.0.10 will be 0x01000100
     * the right number for 1.0.8 was 0x01000008
     * TODO: maybe we can get more accurate number
     */
    *version &= 0xffff0000;
    *version |= (older_ver >> 8) & 0xff;
}

int get_dsp_version(uint32_t *version)
{
    int ret = 0;
    bool need_to_correct = false;
    char temp_version[8] = { '\0', };

    if (version == NULL) {
        printf("Invalid input param\n");
        ret = -1;
        goto exit;
    }
	spi_flash_fd = open(FLASH_DEV_NODE, O_RDWR);
	if( spi_flash_fd < 0 ){
		printf("Open the %s error\n", FLASH_DEV_NODE);
		goto exit;
	}

    /* version info was ascii code:
     * for example 1.0.8:
     * 0x31 0x2e 0x30 0x2e  (1 . 0 .)
     * 0x38 0x20 0x20 0x20  (8 0 0 0)
     */
    if (lseek(spi_flash_fd, VERSION_OFFSET, SEEK_SET) == VERSION_OFFSET) {
        if (read(spi_flash_fd, temp_version, 8) == 8) {
            *version |= ((temp_version[0] & 0xff) - 0x30) << 24;
            *version |= ((temp_version[2] & 0xff) - 0x30) << 16;
            *version |= ((temp_version[4] & 0xff) - 0x30) << 8;
            if ((temp_version[5] & 0xff) == 0x20) {
                *version |= 0;
                need_to_correct = true;
            } else
                *version |= (temp_version[5] & 0xff) - 0x30;

            if (need_to_correct)
                correct_version(version);
        } else
            printf("read flash failed\n");
    } else
        printf("lseek flash failed\n");

exit:
	if (spi_flash_fd)
		close(spi_flash_fd);
   return ret;
}

int get_fw_version(uint32_t *fw_version)
{
    int ret = 0;
    bool need_to_correct = false;
    char *temp_pos;
    /* fw_version was in offset 0x7c of flash.bin
     * but we combine egg.bin with flash.bin,
     * and flash.bin was in 0x10000 of final binary,
     * so we need to get version from offset 0x1007c in final binary.
     * --------------------------------------------
     * version info was ascii code:
     * for example 1.0.8:
     * 0x31 0x2e 0x30 0x2e  (1 . 0 .)
     * 0x38 0x20 0x20 0x20  (8 0 0 0)
     */
    temp_pos = (char *)fw_dt;
    temp_pos = temp_pos + 0x1007c;
    *fw_version |= ((*temp_pos       & 0xff) - 0x30) << 24;
    *fw_version |= ((*(temp_pos + 2) & 0xff) - 0x30) << 16;
    *fw_version |= ((*(temp_pos + 4) & 0xff) - 0x30) << 8;

    if ((*(temp_pos + 5) & 0xff) == 0x20) {
        *fw_version |= 0;
        need_to_correct = true;
    } else
        *fw_version |= ((*(temp_pos + 5) & 0xff) - 0x30);

    if (need_to_correct)
        correct_version(fw_version);
exit:
    return ret;
}

int load_firmware_upgrade(char *path)
{
	int ret = 0;

	fw_dt = load_file(path, &fw_sz);
	if (fw_dt == NULL) {
		printf("load fw fail! %s\n", path);
		ret = -1;
	}

	return ret;
}

int install_kernel_module(char *path)
{
	int ret = 0;
	char options[1] = {'\0'};
	int retry = 10;

	module_dt = load_file(path, &module_sz);
	if (!module_dt) {
		printf("load module failed\n");
		ret = -1;
		goto exit;
	}

    ret = init_module(module_dt, module_sz, options);
    if (ret < 0) {
        printf("init module failed\n");
		ret = -1;
        goto exit;
    }

	do{
		usleep(500000);
		if (access(FLASH_DEV_NODE, F_OK) == 0)
			break;
	}while(retry--);

exit:
	if (module_dt)
		free(module_dt);
	return ret;
}

int uninstall_kernel_module(char *path)
{
	int ret = 0;
	int retry = 10;

	do{
		ret = delete_module(FLASH_KERNEL_MODULE_NAME, O_NONBLOCK | O_EXCL);
		if (ret < 0 && errno == EAGAIN)
			usleep(500000);
		else
			break;
	}while(retry--);

	return ret;
}

int main( int argc, char** argv )
{
	int ret = 0;
	int i,j;

	char image[50];
	char module[50] = "./gida_spidev.ko";

	uint32_t fw_version = 0;
	uint32_t dsp_version = 0;

	int time_start = 0;
	int time_end   = 0;

	printf("================================================================\n");
	printf("===================    Upgrade DSP Firmware    =================\n");
	printf("================================================================\n");

	/* check argument first */
	printf("check input argument...\n");
	if (argc != 2){
		printf("\n\tERROR: Too few argruments!\n");
		printf("\n\t - %s spi_vxxx.bin\n", argv[0]);
		exit(1);
	}

	printf("check if the spi flash driver(ko) is exist...\n");
	if (access(module, F_OK) != 0) {
		/* chk spi flash driver */
		printf("Can't find the %s, exit!\n", module);
		exit(1);
	}

	printf("check if the firmware image which will be use is exist...\n");
	strcpy(image, argv[1]);
	if (access(image, F_OK) != 0) {
		/* chk firmware */
		printf("Can't find the %s, exit!\n", image);
		exit(1);
	}

	/* 1, insmod spi flash driver */
	printf("install spi flash driver(%s)...\n", module);
	ret = install_kernel_module(module);
	if (ret != 0) {
		printf("insmod ko fail!\n");
		ret = -1;
		goto ko_fail;
	}

	/* 2, read upgrade image */
	printf("load update firmware(%s)...\n", image);
	ret = load_firmware_upgrade(image);
	if (ret != 0) {
		printf("load firmware image fail!\n");
		ret = -1;
		goto fail;
	}

	/* 3, check version */
	get_dsp_version(&dsp_version);
	get_fw_version(&fw_version);

	printf("\t----------------------------\n");
	printf("\t-> get dsp version: %d.%d.%d <-\n",
		(dsp_version>>24)&0xFF,
		(dsp_version>>16)&0xFF,
		(dsp_version)&0xFF
		);
	printf("\t-> get fw  version: %d.%d.%d <-\n",
		(fw_version>>24)&0xFF,
		(fw_version>>16)&0xFF,
		(fw_version)&0xFF
		);
	printf("\t----------------------------\n");

	/* 4, write the fw to SPI Flash */
	printf("open spi flash device point(%s)...\n", FLASH_DEV_NODE);
	spi_flash_fd = open(FLASH_DEV_NODE, O_RDWR);
	if( spi_flash_fd < 0 ){
		printf("Open the %s error\n", FLASH_DEV_NODE);
		goto fail;
	}

#if 0
	#define SIZE_PER_LINE (64)
	for(i=0; i<fw_sz/SIZE_PER_LINE; i++) {
		for(j=0; j<SIZE_PER_LINE;) {
			printf("%08x ", ((int*) fw_dt)[i*SIZE_PER_LINE+j]);
			j += 4;
		}
		printf("\n");
	}
#else
	printf("start to upgrade the firmware...\n");
	time_start = time((time_t*)NULL);
    ret = write(spi_flash_fd, fw_dt, fw_sz);
    if (ret != fw_sz) {
        printf("Update flash failed\n");
        ret = -1;
    }
	time_end = time((time_t*)NULL);
	sleep(1);
#endif

exit:
	if (spi_flash_fd)
		close(spi_flash_fd);
	if (fw_dt)
		free(fw_dt);
	uninstall_kernel_module(module);
	printf("===================== spend %4d seconds =======================\n", (time_end-time_start));
	printf("===================== upgrade success ==========================\n");
	return 0;

fail:
	uninstall_kernel_module(module);
ko_fail:
	printf("=================== upgrade fail ===============================\n");
	return -1;
}

