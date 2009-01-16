#include <string.h>
#include <strings.h>

#include "mapper.h"

char mapper_list[NUMBER_OF_MAPPERS][MAX_MAPPER_NAME_SIZE];

void initialize_mapper_list() {

	bzero(mapper_list,NUMBER_OF_MAPPERS);

	strcpy(mapper_list[0 ],"No mapper");
	strcpy(mapper_list[1 ],"Nintendo MMC1");
	strcpy(mapper_list[2 ],"CNROM switch");
	strcpy(mapper_list[3 ],"UNROM switch");
	strcpy(mapper_list[4 ],"Nintendo MMC3");
	strcpy(mapper_list[5 ],"Nintendo MMC5");
	strcpy(mapper_list[6 ],"FFE F4xxx");
	strcpy(mapper_list[7 ],"AOROM switch");
	strcpy(mapper_list[8 ],"FFE F3xxx");
	strcpy(mapper_list[9 ],"Nintendo MMC2");
	strcpy(mapper_list[10],"Nintendo MMC4");
	strcpy(mapper_list[11],"ColorDreams chip");
	strcpy(mapper_list[12],"FFE F6xxx");
	strcpy(mapper_list[13],"CPROM switch");
	strcpy(mapper_list[15],"100-in-1 switch");
	strcpy(mapper_list[16],"Bandai chip");
	strcpy(mapper_list[17],"FFE F8xxx");
	strcpy(mapper_list[18],"Jaleco SS8806 chip");
	strcpy(mapper_list[19],"Namcot 106 chip");
	strcpy(mapper_list[20],"Nintendo DiskSystem");
	strcpy(mapper_list[21],"Konami VRC4a");
	strcpy(mapper_list[22],"Konami VRC2a");
	strcpy(mapper_list[23],"Konami VRC2a");
	strcpy(mapper_list[24],"Konami VRC6");
	strcpy(mapper_list[25],"Konami VRC4b");
	strcpy(mapper_list[32],"Irem G-101 chip");
	strcpy(mapper_list[33],"Taito TC0190/TC0350");
	strcpy(mapper_list[34],"Nina-1 board");
	strcpy(mapper_list[64],"Tengen RAMBO-1 chip");
	strcpy(mapper_list[65],"Irem H-3001 chip");
	strcpy(mapper_list[66],"GNROM switch");
	strcpy(mapper_list[67],"SunSoft3 chip");
	strcpy(mapper_list[68],"SunSoft4 chip");
	strcpy(mapper_list[69],"SunSoft5 FME-7 chip");
	strcpy(mapper_list[71],"Camerica chip");
	strcpy(mapper_list[78],"Irem 74HC161/32-based");
	strcpy(mapper_list[79],"AVE Nina-3 board");
	strcpy(mapper_list[81],"AVE Nina-6 board");
	strcpy(mapper_list[91],"Pirate HK-SF3 chip");
}
