#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <getopt.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>

#define DESCRIPTION ""
#define VERSION     "0.0.1"
#define AUTHOR      "shenpengru@xiaomi.com"

void help(int version)
{
	if(version){
		printf("\tDescription : %s\n", DESCRIPTION);
		printf("\tVersion     : %s\n", VERSION );
		printf("\tAuthor      : %s\n", AUTHOR );
	} else {
		printf("\n\tUsage:\n" );
		printf("\t\t\n");
	}
}

int c2i(char ch)
{
	if(isdigit(ch))
		return ch - 48;

	if( ch < 'A' || (ch > 'F' && ch < 'a') || ch > 'z' )
		return -1;

	if(isalpha(ch))
		return isupper(ch) ? ch - 55 : ch - 87;

	return -1;
}

int hex2dec(char *hex)
{
	int len;
	int num = 0;
	int temp;
	int bits;
	int i;

	len = strlen(hex);

	for (i=0, temp=0; i<len; i++, temp=0) {
		temp = c2i( *(hex + i) );
		bits = (len - i - 1) * 4;
		temp = temp << bits;
		num = num | temp;
	}

	return num;
}

static unsigned int _trans_n_dot_m_format(
		int factor_n, int factor_m,
		int max,      int min,
		float value)
{
	unsigned int reg_val = 0;
	int integer_part = (int)value;
	float decimal_part = integer_part > value? (integer_part - value):(value - integer_part);
 
	///< interger transform
	if (integer_part > max) {
		integer_part = max;
	}
	if (integer_part < min) {
		integer_part = min;
	}
	reg_val = reg_val | (integer_part << factor_m);
	//ALOGE("%s: integer_part=%d, decimal_part=%f, reg 0x%x\n", __FUNCTION__, integer_part, decimal_part, reg_val);
	 
	///< decimal_part transform
	int count=0;
	int tmp_integer=0;
	float tmp_decimal = decimal_part;
	for (count = 0;count <= (factor_m-1) ;count++ )
	{
		tmp_decimal = tmp_decimal * 2;
		tmp_integer = (int)tmp_decimal;
		tmp_decimal = tmp_decimal - tmp_integer;
		reg_val |= (tmp_integer & 0x1) << ((factor_m-1)-count);
	}
	//ALOGE("%s: final integer_part=%d, decimal_part=%f, reg 0x%x\n", __FUNCTION__, integer_part, decimal_part, reg_val);
 
	return reg_val;
}
 
static unsigned int volume_to_dspreg(float volume)
{
	/* 5.27 format */
	/* range: [-16, 15] */
	return _trans_n_dot_m_format(5, 27, 15, -16, volume);
}

int main(int argc, char* argv[])
{
	int  opt;
	int  option_index = 0;
	char   *optstring = ":h:";
	int          type = 0;
	int           idx = 0;
	int           val = 0;

#if 1
	val = _trans_n_dot_m_format(15, 17, 0, 25, 10.83);
	printf("10.83 -> 0x%08x\n", val);
	val = _trans_n_dot_m_format(15, 17, 0, 25, 10.35);
	printf("10.35 -> 0x%08x\n", val);
	val = _trans_n_dot_m_format(15, 17, 0, 25,  7.37);
	printf(" 7.37 -> 0x%08x\n", val);
#else
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
#if 0
			if (!strncmp(optarg, TYPE_AUDIO_IN, 2)) {
				type = eTypeIn;
			} else if (!strncmp(optarg, TYPE_AUDIO_OUT, 3)) {
				type = eTypeOut;
			} else if (!strncmp(optarg, TYPE_AUDIO_PDM, 3)) {
				type = eTypePdm;
			} else {
				type = eTypeMax;
				printf("The type is unsupport %s\n", optarg);
				help(0);
				exit(1);
			}
#endif
			break;
		case 'i':
#if 0
			if (!strncmp(optarg, "0x", 2) || !strncmp(optarg, "0X", 2)) {
				idx = hex2dec(optarg+2);
			} else {
				idx = atoi(optarg);
			}
#endif
			break;
		case 'h':
			help(0);
			exit(0);
			break;
		case 'v':
			help(1);
			exit(0);
		case '?':
		default:
			printf("\n\tERROR: Unknown options!\n");
			help(0);
			exit(1);
			break;
		}
	}
#endif

	return 0;
}
