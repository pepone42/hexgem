module hexgem;
//#define HAVE_GETTIMEOFDAY 1

enum R43 = 0;
enum R169 = 1;

struct CONF
{
	int scale;
	int ratio;
	int fullboard;
};
__gshared CONF conf;
