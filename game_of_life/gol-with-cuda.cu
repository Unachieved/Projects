#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdbool.h>
#include <cuda.h>
#include <cuda_runtime.h>

extern "C" unsigned int *g_resultData;

// Current state of world.
extern "C" unsigned int *g_data;

// Current width of world.
extern "C" size_t g_worldWidth;

/// Current height of world.
extern "C" size_t g_worldHeight;

/// Current data length (product of width and height)
extern "C" size_t g_dataLength;

static inline void gol_initAllZeros( size_t worldWidth, size_t worldHeight )
{
    // calloc inits
    cudaMallocManaged(&g_data, (g_dataLength+(2*worldWidth))*sizeof(unsigned int));
    cudaMallocManaged(&g_resultData, (g_dataLength+(2*worldWidth))*sizeof(unsigned int));
    //set inits to 0
    for(size_t i = 0; i < g_dataLength; i++)
    {
        g_data[i+g_worldWidth] = 0;
        g_resultData[i+g_worldWidth] = 0;
    }
}

static inline void gol_initAllOnes( size_t worldWidth, size_t worldHeight )
{
    int i;

    cudaMallocManaged(&g_data, (g_dataLength+(2*worldWidth))*sizeof(unsigned int));
    cudaMallocManaged(&g_resultData, (g_dataLength+(2*worldWidth))*sizeof(unsigned int));

    // set all rows of world to true
    for( i = 0; i < g_dataLength; i++)
    {
        g_data[i+g_worldWidth] = 1;
        g_resultData[i+g_worldWidth]=0;
    }
    
    
}

static inline void gol_initOnesInMiddle( size_t worldWidth, size_t worldHeight )
{
    int i;
    
    cudaMallocManaged(&g_data, (g_dataLength+(2*worldWidth))*sizeof(unsigned int));
    cudaMallocManaged(&g_resultData, (g_dataLength+(2*worldWidth))*sizeof(unsigned int));
	
	
	for(i =0; i<10;i++){
		g_data[g_dataLength+127+i]=1;
	}
    
}

static inline void gol_initOnesAtCorners( size_t worldWidth, size_t worldHeight, int my_rank, int max_rank)
{
    cudaMallocManaged(&g_data, (g_dataLength+(2*worldWidth))*sizeof(unsigned int));
    cudaMallocManaged(&g_resultData, (g_dataLength+(2*worldWidth))*sizeof(unsigned int));

    if(my_rank==0){
		g_data[worldWidth] = 1; // upper left
    	g_data[(worldWidth*2)-1]=1; // upper right
	}
    if(my_rank == max_rank){
		
	    g_data[g_dataLength]=1; // lower left
	    g_data[g_dataLength + worldWidth-1]=1; // lower right
	}
    
    
}

static inline void gol_initSpinnerAtCorner( size_t worldWidth, size_t worldHeight, int my_rank)
{

    cudaMallocManaged(&g_data, (g_dataLength+(2*worldWidth))*sizeof(unsigned int));
    cudaMallocManaged(&g_resultData, (g_dataLength+(2*worldWidth))*sizeof(unsigned int));

	if(my_rank==0){
    	g_data[worldWidth] = 1; // upper left
    	g_data[1+worldWidth] = 1; // upper left +1
    	g_data[(worldWidth*2)-1]=1; // upper right
	}
    
}

static inline void gol_initMaster( unsigned int pattern, size_t worldWidth, size_t worldHeight, int myrank, int size)
{
	
    g_worldWidth = worldWidth;
    //g_worldHeight = worldHeight*size;
	g_worldHeight = worldHeight;
    g_dataLength = g_worldWidth * g_worldHeight;
	
	
	int cE, cudaDeviceCount;
	if((cE = cudaGetDeviceCount( &cudaDeviceCount)) != cudaSuccess ){
		printf(" Unable to determine cuda device count, error is %d, count is %d\n",
			cE, cudaDeviceCount );
		exit(-1);
	}
	if((cE = cudaSetDevice( myrank % cudaDeviceCount )) != cudaSuccess ){
		printf(" Unable to have rank %d set to cuda device %d, error is %d \n",
			myrank, (myrank % cudaDeviceCount), cE);
		exit(-1);
	}
	
    switch(pattern)
    {
    case 0:
    gol_initAllZeros( worldWidth, worldHeight );
    break;
    
    case 1:
    gol_initAllOnes( worldWidth, worldHeight );
    break;
    
    case 2:
    gol_initOnesInMiddle( worldWidth, worldHeight );
    break;
    
    case 3:
    gol_initOnesAtCorners( worldWidth, worldHeight, myrank, size-1);
    break;

    case 4:
    gol_initSpinnerAtCorner( worldWidth, worldHeight, myrank);
    break;

    default:
    printf("Pattern %u has not been implemented \n", pattern);
    exit(-1);
    }
}

extern "C" void initMaster( unsigned int pattern, size_t worldWidth, size_t worldHeight, int myrank, int size, int my_rank){
	gol_initMaster(pattern, worldWidth, worldHeight, myrank, size);
}

__device__ unsigned int gol_countAliveCells(const unsigned int* data,
                       size_t x0,
                       size_t x1,
                       size_t x2,
                       size_t y0,
                       size_t y1,
                       size_t y2)
{
    int counter = 0;
    // You write this function - it should return the number of alive cell for data[x1+y1]
    // There are 8 neighbors - see the assignment description for more details.
    
    //This increments counter based on the value of the 8 neighbors of the current element
    // if the neighbor is alive counter in incremented by 1
    counter+= (data[y0 + x0]+data[y0 + x1]+data[y0 + x2]);
    counter+= (data[y1 + x0]+data[y1 + x2]);
    counter+= (data[y2 + x0]+data[y2 + x1]+data[y2 + x2]);
    
    // the status of the node follow a tic is returns bellow according the specification
    // conditions
    if(data[x1+y1]==1){
        if(counter >=2 && counter <=3) return 1;
    }
    else
        if(counter==3) return 1;
    
    return 0;
}

__global__ void gol_kernel(const unsigned int * d_data, unsigned int worldWidth,
                           unsigned int worldHeight, unsigned int * d_resultData, int my_rank){
    unsigned int index = blockIdx.x * blockDim.x + threadIdx.x;
    unsigned int stride = blockDim.x * gridDim.x;
    size_t dataLength = worldWidth * worldHeight;

    size_t y0,y1,y2;
    size_t x0,x1,x2;
    for(unsigned int i = index; i<worldHeight*worldWidth; i+=stride){
        
        x1 = index % worldWidth;
        y1 = index - x1;
        x0 = (x1 + worldWidth - 1) % worldWidth;
        x2 = (x1 + 1) % worldWidth;
        y0 = (y1 + dataLength - worldWidth) % dataLength;
        y2 = (y1 + worldWidth) % dataLength;
		
		unsigned int tmp = gol_countAliveCells(d_data, x0, x1, x2, y0, y1, y2);
        d_resultData[y1+x1] = tmp;
		
    }
}

extern "C" void kernelCall(int numBlocks, ushort threadsCount, const unsigned int *d_data, size_t worldWidth, size_t worldHeight, unsigned int** d_resultData, int my_rank){
	
	gol_kernel<<<numBlocks, threadsCount>>>(d_data, worldWidth, worldHeight, *d_resultData, my_rank);
	//cudaDeviceSynchronize();
}


