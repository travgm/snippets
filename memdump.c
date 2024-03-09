/* 20130814
 * 20250308
 *  memdump.c
 *  Written by Travis Montoya 
 *  This program was written for analysing the heap of various binaries and exploring
 *  data in memory.
 *
 *  (C) Copyright 2013-2024 Travis Montoya
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

unsigned int heap_structure_size = 0;

struct heap
{
  void *saddress;    /* Start address */
	void *address;     /* End address */
	int size;          /* Size */
};

char *ltostr(char *str, long l) 
{
  	memset(str, 0, sizeof(long) + 1);
  	memcpy(str, &l, sizeof(long));
  	return str;
}

/* Returns back a word  of data at void *addr */
long peekdata(void *addr, int pid) 
{
  	return ptrace(PTRACE_PEEKDATA, pid, addr, NULL);
}

int dump_heap(struct heap **heap_dump, int pid_id)
{
	char dump_file[30];
	char *dump_data = (char*)malloc(100);
	FILE *df = NULL;
	int hs = 0;
	int bytes = 0;
	
	bzero(dump_data,100);
  ptrace(PTRACE_ATTACH, pid_id, NULL, NULL);

	sprintf(dump_file, "%d.dump", pid_id);
	df = fopen(dump_file,"a+");
	if(df == NULL) {
		printf("* Error creating file %s, quitting.\n", dump_file);
		ptrace(PTRACE_DETACH, pid_id, NULL, NULL);
		return -1;
	}

	printf("--------------------------------------------------------------\n");
  printf("* Dumping memory to %s this can take awhile.\n", dump_file);
	while(hs < heap_structure_size) 
  {
    /* Dump the entire heap locations to the file specified */
		while(heap_dump[hs]->saddress <= heap_dump[hs]->address) 
    {
			ltostr(dump_data,peekdata(heap_dump[hs]->saddress,pid_id));
			bytes += fwrite(dump_data,1,sizeof(dump_data),df);
			heap_dump[hs]->saddress++;
		}

		printf("* Wrote %d bytes.\n", bytes);
		bytes = 0;
		hs++;
	}

	fclose(df);
	ptrace(PTRACE_DETACH, pid_id, NULL, NULL);
	printf("* Finished.\n");	
	return 0;
}

/*
 * Finds the heap start/end addreses of a process (*nix specific)
*/
struct heap** find_heap_values(int pid_id)
{
	struct heap **heaps = (struct heap**)malloc(sizeof(struct heap**));
	int heap_size = 0;
	char *end, *heap_loc;
	char pid_map[30], data[1024];
	FILE *map;
	
	sprintf(pid_map,"/proc/%d/maps",pid_id);
	map = fopen(pid_map,"r");
	if(map == NULL) 
  {
		printf("* Error, unable to read process [%d] file map.\n",pid_id);
		exit(1);
	}
 
  /* Iterate through the map and find all heap locations */
	printf("* Attempting to find process [%d] heap location(s).\n",pid_id);
	while(fgets(data, 1024, map) != NULL) 
  {
		if(strstr(data, "[heap]") != NULL) 
                {
      /* Allocate memory for structure, and search for address seperator */
			heaps[heap_size] = (struct heap*)malloc(sizeof(struct heap*));
			heap_loc = strtok(data,"-");
			if((strcmp(heap_loc,"")) == 0) 
      {
				printf("* Error finding heap addresses.\n");
				exit(1);
			}
                        
      /* get start of heap */
			heaps[heap_size]->saddress = (void *)strtol(heap_loc,&end,16);
			printf("* [heap] start:0x%s ",heap_loc);
			
      /* end of the heap */
			heap_loc = strtok(NULL,"- ");
			printf("end:0x%s ",heap_loc);
			
      /* get the size based from our start/stop locations */
			heaps[heap_size]->address = (void *)strtol(heap_loc,&end,16);
			heaps[heap_size]->size = (int)(heaps[heap_size]->address - heaps[heap_size]->saddress);
			
			printf("size=%d\n", heaps[heap_size]->size*8);
			heap_size++;
			heaps = realloc(heaps, heap_size*sizeof(struct heap*));
		}
	}

	fclose(map);
	if(heap_size == 0) 
  {
		return NULL;
	} else {
		heap_structure_size = heap_size;
	}
	return heaps;
}


/*
 * Main program execution
*/
int main(int argc, char *argv[])
{
	struct heap **heap_dump = NULL;
	int i,proc_pid = 0;

  printf("memory dumper version 1.0.1 2013-11-12\n");
  printf("(C) Copyright 2013-2024 Travis Montoya\n");
  printf("--------------------------------------------------------------\n");

	if(argc != 2) 
  {
		printf("usage %s [PID]\n",argv[0]);
    printf("\nNote: Program needs to be ran as root!\n");
		exit(1);
	} else {
    if(geteuid() != 0)
    {
      printf("\nError: Program needs to be ran as root!\n");
      exit(1);
    }
		proc_pid = atoi(argv[1]);
	}

	heap_dump = find_heap_values(proc_pid);
	if(heap_dump != NULL) 
  {
		dump_heap(heap_dump,proc_pid);
	} else {
		printf("* Unknown error occured.\n");
	}

	for(i=0; i<heap_structure_size; i++) 
  {
    free(heap_dump[i]);
  }
  free(heap_dump);

	return 0;
}	
