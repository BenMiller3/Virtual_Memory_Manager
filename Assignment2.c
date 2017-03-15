/* Virtaul to Physical Address Mapping Simulation
 * ==============================================
 *
 * Given a list of virtual addresses return:
 *      1) Their corresponding physical address
 *      2) Value at the corresponding address
 *      3) Total page faults throughout translation
 *      4) Total TLB hits throughout translation
 *
 * USAGE: ./assignment2 addresses.txt BACKING_STORE.bin
 * argv[1] = List of virtual addresses
 * argv[2] = List of simulated physical backing store binary file
 *
 * By: Benjamin D. Miller
 *      001416516
 *      millebd
*/

#include <stdio.h>
#include <sys/mman.h> //For mmap() (Memory mapped file(s))
#include <string.h> // For memcpy() (COPIES a block of memory)
#include <fcntl.h> // last 2 for file descriptors
#include <stdlib.h>
#include <unistd.h> // for atoi()

//Defining Macros
#define BUFFER_SIZE 10
#define OFFSET_BITS 12
#define OFFSET_MASK 255
#define PAGE_NUMBER_MASK 255
#define PAGES 8
#define PAGE_SIZE 256
#define PAGE_TABLE_ENTRIES 256
#define BACKING_STORE_SIZE 65536
#define MEMORY_SIZE 256
#define FRAME_SIZE 256
#define PHYSICAL_MEMORY 65536
#define CHAR_SIZE 8

// Global Variables and REPORT values
int totalTranslatedAddresses;
int totalPageFaults;
int totalTLBhits;

signed char buffer[BACKING_STORE_SIZE];
signed char *mmapfptr;
int TLB [PAGE_TABLE_ENTRIES][FRAME_SIZE];
char ** frames;


//Function definition
void addTLB(int pageNum, int frameNum, int index);
int searchTLB(int value);

int main(int argc, char* argv []){

        //Check that the input file is inputted properly
        if(argc > 3){
                printf("ERROR: Only one file and it's corresponding backing store can be checked.\n");
                return -1;}
        else if(argc < 3){
                printf("ERROR: Must input a file to be translated and its corresponding Backing Store.\n");
                return -1;}

	// Mapping the BACKING STORE to a memory location
	int binFile = open(argv[2],O_RDONLY);
	mmapfptr = mmap(0, MEMORY_SIZE,PROT_READ, MAP_PRIVATE, binFile, 0);
	
	int i = 0;
	for(i; i<MEMORY_SIZE; i++){
		int temp = i*8;
		memcpy(buffer + i, mmapfptr + temp, CHAR_SIZE);
		printf("THE BUFFER IS: %c",buffer[i]);
		}

        // Initialize the Page Table as an integer with all -1 values.
        int page_table[PAGE_TABLE_ENTRIES];
        int j = 0;
        for(j;j<PAGE_TABLE_ENTRIES; j++){
			page_table[j] = -1;
		} 
		
		frames = malloc( sizeof(char*) * 128);
       
        // Open the file and write its contents to array
        int c;
        FILE *file;

        char *filename = argv[1];
        file = fopen(filename,"r");     

        char buff[BUFFER_SIZE];

        if(file){
                while(fgets(buff, BUFFER_SIZE, file)){

                        // Bitwise operators used to calculate the following:
                        int virtualAddress = atoi(buff);
                        int offSet = virtualAddress & ((1 << OFFSET_BITS) - 1);
                        unsigned int pageNum = (virtualAddress >> OFFSET_BITS) & PAGE_NUMBER_MASK;
                        unsigned int frame_number = page_table[pageNum];

						// If the frame_number is -1 then there is a page fault
						if(frame_number == -1){
							totalPageFaults++;
							
							// update the page table with the values from the respective frame
							int k = 0;
							for(k; k<PAGE_SIZE; k++){
									page_table[pageNum+k] = buffer[pageNum+k];
							}
						
						frame_number = pageNum;
						
						}
								
						int off_Set = virtualAddress & OFFSET_MASK;
						
						//printf("FRAME NUMBER: %d\n",frame_number);
						
                        int physicalAddress = (frame_number << OFFSET_BITS) | off_Set;
                        int storedValue = pageNum;//(int) frames[frame_number][offSet];

                        printf("Virtual address: %d Physical address: %d Value: %d\n",virtualAddress, physicalAddress, storedValue);
						//printf("THE PAGE NUMBER IS: %d\n",pageNum);
                        totalTranslatedAddresses++;
                }

                fclose(file);
        }
        else{
                printf("There was an error opening the file.");
                return -1;
        }

        /* Final Output Lines */
        printf("Number of Translated Addresses = %d\n", totalTranslatedAddresses);
        printf("Page Faults = %d\n",totalPageFaults);
        printf("TLB Hits = %d\n",totalTLBhits);

		// Unmap the memory locations
		munmap(mmapfptr, MEMORY_SIZE);

        return 0;
}

void addTLB(int pageNum, int frameNum, int index) {
	/* Add a pagenum and framenum pair to the TLB */
    index = index % 16;
    TLB[index][0] = pageNum;
    TLB[index][1] = frameNum;
}

int searchTLB(int pageNum){
/* Return -1 if not found, otherwise return the framenum */
    for (int i = 0; i < 16; i++) {
        if (pageNum == TLB[i][0]) {return TLB[i][1];}
    }
    
    return -1;
}
    
