#ifndef mapper_h
#define mapper_h

#define NUMBER_OF_MAPPERS 92
#define MAX_MAPPER_NAME_SIZE 100

/* Mapper list from http://fms.komkon.org/EMUL8/NES.html */
extern char mapper_list[NUMBER_OF_MAPPERS][MAX_MAPPER_NAME_SIZE];

/** Initialize the mapper_list with their respective names */
void initialize_mapper_list();

#endif /* mapper_h */
