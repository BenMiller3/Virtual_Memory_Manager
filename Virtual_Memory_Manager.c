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

// Global Variables REPORT values
int totalTranslatedAddresses;
int totalPageFaults;
int totalTLBhits;

//Defining Macros
#define BUFFER_SIZE 10
#define OFFSET_BITS 8
#define OFFSET_MASK 255
#define PAGE_NUMBER_MASK 255
#define PAGES 8
#define PAGE_SIZE 256
#define PAGE_TABLE_ENTRIES 256
#define BACKING_STORE_SIZE 65536
#define MEMORY_SIZE 8

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
	mmap(0,BACKING_STORE_SIZE,PROT_WRITE,MAP_SHARED, binFile, OFFSET_MASK);

        // Initialize the Page Table as an integer with all -1 values.
        int page_table[PAGE_TABLE_ENTRIES];
        int i = 0;
        for(i;i<PAGE_TABLE_ENTRIES; i++){page_table[i] = -1;} 
       
        // Open the file and write its contents to array
        int c;
        FILE *file;

        char *filename = argv[1];
        file = fopen(filename,"r");     

        // TODO: TLB = [PAGE_NUMBER][FRAME_NUMBER]
        char buff[BUFFER_SIZE];

        if(file){
                while(fgets(buff, BUFFER_SIZE, file)){

                        // Bitwise operators used to calculate the following:
                        int virtualAddress = atoi(buff);
                        int offSet = virtualAddress & ((1 << OFFSET_BITS) - 1);
                        unsigned int pageNum = (virtualAddress >> OFFSET_BITS) & PAGE_NUMBER_MASK;
                        unsigned int frame_number = page_table[pageNum];

                        int physicalAddress = (frame_number << OFFSET_BITS) | offSet;
                        int storedValue = 10;

                        printf("Virtual address: %d Physical address: %d Value: %d\n",virtualAddress, physicalAddress, storedValue);
			printf("THE PAGE NUMBER IS: %d\n",pageNum);
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

        return 0;
}
