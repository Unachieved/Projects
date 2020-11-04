#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdbool.h>
#include <mpi.h>


unsigned int *g_resultData=NULL;

// Current state of world.
unsigned int *g_data=NULL;

// Current width of world.
size_t g_worldWidth=0;

/// Current height of world.
size_t g_worldHeight=0;

/// Current data length (product of width and height)
size_t g_dataLength=0;

//extern int cudaDeviceSynchronize();

extern void initMaster( unsigned int pattern, size_t worldWidth, size_t worldHeight, int myrank, int size);
extern void kernelCall(int numBlocks, ushort threadsCount, unsigned int *d_data, size_t worldWidth, size_t worldHeight, unsigned int** d_resultData, int my_rank);
extern int cudaDeviceSynchronize();
extern void cudaFree();

//, int my_rank, int tot

//outputs world to external file in the proper order
static inline void gol_printWorld(FILE * fp, int my_rank, int comm_sz)
{
	MPI_Request request;
	MPI_Status status;
	
	int ierr;
	int i, j;
	if(my_rank==0){
		//output my cell first
	    for( i = 0; i < g_worldHeight; i++)
	    {
	    	fprintf(fp, "Row %2d: ", i);
	    	for( j = 0; j < g_worldWidth; j++){
	        	fprintf(fp, "%u ", (unsigned int)g_data[((i+1)*g_worldWidth) + j]);
				printf("%d", g_data[((i+1)*g_worldWidth) + j]);
	    	}
	    	fprintf(fp, "\n");
	    }
		
		unsigned int * rec_cells = (unsigned int *)calloc(g_dataLength, sizeof(unsigned int));
		//recieve and output the cells in order
		for(i=1;i<comm_sz;i++){
			MPI_Irecv(rec_cells, g_dataLength, MPI_UNSIGNED, i, MPI_ANY_TAG, MPI_COMM_WORLD, &request);
			ierr=MPI_Wait(&request, &status);
			fprintf(fp, "1 iteration\n");
		    for( int k = 0; k < g_worldHeight; k++)
		    {
		    	fprintf(fp, "Row %2lu: ", k+(i*g_worldHeight));
		    	for( j = 0; j < g_worldWidth; j++){
		        	fprintf(fp, "%u ", (unsigned int)rec_cells[(k*g_worldWidth) + j]);
		    	}
		    	fprintf(fp, "\n");
		    }
		}
		
		fprintf(fp, "\n\n");
	}
	else{
		//get and send the cells to rank 0 for output
		unsigned int * all_cells = (unsigned int *)calloc(g_dataLength, sizeof(unsigned int));
		for(i = 0; i<g_dataLength;i++){
			all_cells[i] = g_data[i+g_worldWidth];
		}
		MPI_Isend(all_cells, g_dataLength, MPI_UNSIGNED, 0, 0, MPI_COMM_WORLD, &request);
		ierr=MPI_Wait(&request, &status);
		
	}
}

static inline void gol_swap( unsigned int **pA, unsigned int **pB)
{
    //swap the pointers of pA and pB.
    // temp assigned to hold value
    unsigned int * temp = *pA;
    *pA = *pB;
    *pB = temp;
}

//launches the kernel, passing in the proper informationf for kernel setup. The iterations
//are conducted in this function
bool kernelLaunch(unsigned int** d_data, unsigned int** d_resultData,
                      size_t worldWidth, size_t worldHeight, size_t iterationsCount, ushort threadsCount, const int my_rank, const int comm_sz){
    int numBlocks = ((g_dataLength+(2*worldWidth)) + threadsCount -1 ) / threadsCount;
	
	MPI_Status status;
	size_t i;
	
	int ierr;
	MPI_Request request_send = MPI_REQUEST_NULL;
	MPI_Request request_send2 = MPI_REQUEST_NULL;
	MPI_Request request_recv = MPI_REQUEST_NULL;
	MPI_Request request_recv2 = MPI_REQUEST_NULL;
	
	for(size_t i = 0; i<iterationsCount; i++){
		 
		 if(comm_sz>1){
			 unsigned int * data_last = (unsigned int *)calloc(g_worldWidth, sizeof(unsigned int));
			 unsigned int * data_first = (unsigned int *)calloc(g_worldWidth, sizeof(unsigned int));
			 unsigned int * ghost_before = (unsigned int *)calloc(g_worldWidth, sizeof(unsigned int));
			 unsigned int * ghost_after = (unsigned int *)calloc(g_worldWidth, sizeof(unsigned int));
			 size_t j;
			 
			 //populate the  local ghost row to send using MPI
			 for(j=0; j < g_worldWidth; ++j){
				 //Last row in current rank
				 data_last[j] = (*d_data)[ g_dataLength +j ];
				 //First row in current rank
				 data_first[j] = (*d_data)[j+worldWidth];
			 }
			 
			 
			 //send the current ranks first and last true rows
			 MPI_Isend(data_last, g_worldWidth, MPI_UNSIGNED, (my_rank+1)%comm_sz, 0, MPI_COMM_WORLD, &request_send);
			 if((my_rank-1) >=0)
			 	 MPI_Isend(data_first, g_worldWidth, MPI_UNSIGNED, (my_rank-1)%comm_sz, 0, MPI_COMM_WORLD, &request_send2);
			 else
			 	 MPI_Isend(data_first, g_worldWidth, MPI_UNSIGNED, comm_sz-1, 0, MPI_COMM_WORLD, &request_send2);
			 
			 
			 
			 //recieved the last row of the previous rank and the first row of the next rank --- to populate this ranks ghost cells
			 if((my_rank -1) >=0)
				  MPI_Irecv(ghost_before, g_worldWidth, MPI_UNSIGNED, (my_rank-1)%comm_sz, MPI_ANY_TAG, MPI_COMM_WORLD, &request_recv);
			 else
			 	MPI_Irecv(ghost_before, g_worldWidth, MPI_UNSIGNED, comm_sz-1, MPI_ANY_TAG, MPI_COMM_WORLD, &request_recv);
			 MPI_Irecv(ghost_after, g_worldWidth, MPI_UNSIGNED, (my_rank+1)%comm_sz, MPI_ANY_TAG, MPI_COMM_WORLD, &request_recv2);
			 
			 //The following are waits to make sure that the data was sent and recieved as expected
			 ierr=MPI_Wait(&request_recv, &status);
			 ierr=MPI_Wait(&request_recv2, &status);
			 ierr=MPI_Wait(&request_send, &status);
			 ierr=MPI_Wait(&request_send2, &status);
			 
			 //place the recieved ghost rows in their proper positions in the local rank
			for(j=0; j < g_worldWidth; ++j){
				unsigned int tmp = ghost_after[j];
				 (*d_data)[ j ] = ghost_before[j]; 
				 (*d_data)[g_dataLength + worldWidth + j ] = tmp;
			}
			
			
			free(data_last);
			free(data_first);
			free(ghost_before);
			free(ghost_after);
		}
		
		//calls kernel with height+2 to account for the ghost rows --- they are not important in output but
		//this has to be accounted for in order to properly update the local grid
 		kernelCall(numBlocks, threadsCount, *d_data, worldWidth, worldHeight+2, d_resultData, my_rank);
 		gol_swap(d_resultData, d_data);
	 }
	cudaDeviceSynchronize();
	
    return true;
}

int main(int argc, char *argv[])
{
	setvbuf( stdout, NULL, _IONBF, 0 );
	
    unsigned int pattern = 0;
    unsigned int localWorldSize = 0;
    unsigned int itterations = 0;
    unsigned int thrdsCount = 0;
    unsigned int printFlag = 0;
	unsigned int totWorldSize = 0;
	int comm_sz, my_rank;
	
	FILE * fp;
	fp = fopen("outfile.txt", "w+");

    if( argc != 6 ){
        printf("GOL requires 5 arguments:\n\tpattern number,\n\tsq size of the world,\n\tthe number of itterations\n\t,the threadcount,\n\toutput flag\n ex1: ./gol 4 64 2 2 0 --- output off\nex2: ./gol 4 64 2 2 1 --- output on\n");
        exit(-1);
    }

    pattern = atoi(argv[1]);
    localWorldSize = atoi(argv[2]);
    itterations = atoi(argv[3]);
    thrdsCount = atoi(argv[4]);
    printFlag = atoi(argv[5]);
	
    
    if(printFlag)
        fprintf(fp, "This is the Game of Life running in Parallel on GPU.\n");
	

	// Initialize the MPI
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

	initMaster(pattern, localWorldSize, localWorldSize, my_rank,comm_sz);

	double t_start;
	if(my_rank==0){
		t_start = MPI_Wtime();
	}
	
	kernelLaunch(&g_data, &g_resultData, localWorldSize, localWorldSize, itterations, thrdsCount, my_rank, comm_sz);
	MPI_Barrier(MPI_COMM_WORLD);
	
	double t_end;
	if(my_rank==0){
		double t_end = (MPI_Wtime() - t_start) * 1000;
		printf("Execution time:\t%.2lf msecs\n",t_end);
		fprintf(fp,"Execution time:\t%.2lf msecs\n",t_end);
	}
	
	
    if(printFlag){
        fprintf(fp, "######################### FINAL WORLD IS ###############################\n");
        gol_printWorld(fp, my_rank, comm_sz);
		fclose(fp);
    }
    
    cudaFree(g_data);
    cudaFree(g_resultData);
	
	MPI_Finalize();
	
	//printf("debug: 1, rank: %d\n", my_rank);
    
    return 0;
}
