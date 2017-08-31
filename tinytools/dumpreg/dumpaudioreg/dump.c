#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <getopt.h>
#include "dump.h"
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <ctype.h>
#include <fcntl.h> 

#define DESCRIPTION ""
#define VERSION     "alpha 0.1.0"
#define AUTHOR      "shenpengru@xiaomi.com"

void help(int version)
{
    if(version){
        printf("\tDescription : %s\n", DESCRIPTION);
        printf("\tVersion     : %s\n", VERSION );
        printf("\tAuthor      : %s\n", AUTHOR );
        printf("\n\t==========================================\n\n");
    }
    else{
        printf("\n\tUsage:\n" );
        printf("\t\tgetuevent --filter block\n");
    }
}

  
/* 
 * 将字符转换为数值 
 * */  
int c2i(char ch)  
{  
        // 如果是数字，则用数字的ASCII码减去48, 如果ch = '2' ,则 '2' - 48 = 2  
        if(isdigit(ch))  
                return ch - 48;  
  
        // 如果是字母，但不是A~F,a~f则返回  
        if( ch < 'A' || (ch > 'F' && ch < 'a') || ch > 'z' )  
                return -1;  
  
        // 如果是大写字母，则用数字的ASCII码减去55, 如果ch = 'A' ,则 'A' - 55 = 10  
        // 如果是小写字母，则用数字的ASCII码减去87, 如果ch = 'a' ,则 'a' - 87 = 10  
        if(isalpha(ch))  
                return isupper(ch) ? ch - 55 : ch - 87;  
  
        return -1;  
}  
  
/* 
 * 功能：将十六进制字符串转换为整型(int)数值 
 * */  
int hex2dec(char *hex)  
{  
        int len;  
        int num = 0;  
        int temp;  
        int bits;  
        int i;  
          
        // 此例中 hex = "1de" 长度为3, hex是main函数传递的  
        len = strlen(hex);  
  
        for (i=0, temp=0; i<len; i++, temp=0)  
        {  
                // 第一次：i=0, *(hex + i) = *(hex + 0) = '1', 即temp = 1  
                // 第二次：i=1, *(hex + i) = *(hex + 1) = 'd', 即temp = 13  
                // 第三次：i=2, *(hex + i) = *(hex + 2) = 'd', 即temp = 14  
                temp = c2i( *(hex + i) );  
                // 总共3位，一个16进制位用 4 bit保存  
                // 第一次：'1'为最高位，所以temp左移 (len - i -1) * 4 = 2 * 4 = 8 位  
                // 第二次：'d'为次高位，所以temp左移 (len - i -1) * 4 = 1 * 4 = 4 位  
                // 第三次：'e'为最低位，所以temp左移 (len - i -1) * 4 = 0 * 4 = 0 位  
                bits = (len - i - 1) * 4;  
                temp = temp << bits;  
  
                // 此处也可以用 num += temp;进行累加  
                num = num | temp;  
        }  
  
        // 返回结果  
        return num;  
}  

int dump_reg(int type, int index)
{
	int fd = 0;
	int base = 0;
	char cmd[20];
	char rd[100];
	int i;

	switch (type) {
	case eTypeIn:
		base = AUDIO_IN_MODULE_REG_BASE;
		break;
	case eTypeOut:
		base = AUDIO_OUT_MODULE_REG_BASE;
		break;
	case eTypePdm:
		base = AUDIO_PDM_MODULE_REG_BASE;
		break;
	default:
		base = AUDIO_IN_MODULE_REG_BASE;
		break;
	}

	sprintf(cmd, "0x%08x", REAL_ADDR(base, index));

	fd = open(AML_REG_DBG_CTRL, O_RDWR);
	if (fd < 0) {
		return -1;
	}

	write (fd, cmd, 10);

	read (fd, rd, 100);

#if 0
	for (i=0; i<100; i++) {
		if (!strcmp("\r", &rd[i])) {
			strcpy(&rd[i], '\0');
			break;
		}
	}
#else
	char *tmp = NULL;
	if (tmp = strstr(rd, "\n"))
		*tmp = '\0';
#endif

	printf("\t[base: 0x%08x] [idx: 0x%02x] %s\n", base, index, rd);

	return 0;
}

int main(int argc, char* argv[])
{
    int  opt;
    int  option_index = 0;
    char   *optstring = ":h:";
	int          type = 0;
	int           idx = 0;

    static struct option long_options[] = {
        {   "type", required_argument, NULL, 't'},
        {  "index", required_argument, NULL, 'i'},
        {   "help", no_argument,       NULL, 'h'},
        {"version", no_argument,       NULL, 'v'},
        {0, 0, 0, 0}
    };

    while( (opt = getopt_long(argc, argv, optstring, long_options, &option_index)) != -1 ){
        switch( opt ){
            case 't':
				if (!strncmp(optarg, TYPE_AUDIO_IN, 2)) {
					type = eTypeIn;
				} else if (!strncmp(optarg, TYPE_AUDIO_OUT, 3)) {
					type = eTypeOut;
				} else if (!strncmp(optarg, TYPE_AUDIO_PDM, 3)) {
					type = eTypePdm;
				} else {
					type = eTypeMax;
                	printf("The type is unsupport %s\n", optarg);
					exit(1);
				}
                break;
			case 'i':
				if (!strncmp(optarg, "0x", 2) || !strncmp(optarg, "0X", 2)) {
					idx = hex2dec(optarg+2);
				} else {
					idx = atoi(optarg);
				}
				break;
            case 'h':
                exit(0);
                break;
            case 'v':
                exit(0);
            case '?':
            default:
                printf("\n\tERROR: Unknown options!\n");
                exit(1);
                break;
        }
    }

	//printf("type: %d index: 0x%04x\n", type, idx);

	switch (type) {
	case eTypeIn:
		if ( idx < 0 || idx > 0xc1 ) {
			printf("wrong idx\n");
			exit(1);
		}
		break;
	case eTypeOut:
		if ( idx < 0 || idx > 0xab ) {
			printf("wrong idx\n");
			exit(1);
		}
		break;
	case eTypePdm:
		if ( idx < 0x40 || idx > 0x46 ) {
			printf("wrong idx\n");
			exit(1);
		}
		break;
	default:
		printf("wrong type\n");
		exit(1);
		break;
	}

	dump_reg(type, idx);

	return 0;
}
