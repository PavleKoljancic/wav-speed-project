#include "DECENTRALIZED_BUFFER.h"
#include <string.h>
#include <stdlib.h>
DECENTRALIZED_BUFFER CREATE_DB(int mainBufferCapacity, int subBufferCapacity)
{
    DECENTRALIZED_BUFFER dcb;
    dcb.buffer = calloc(mainBufferCapacity, sizeof(char *));
    dcb.buffer[0] = calloc(subBufferCapacity, sizeof(char));
    dcb.mainBufferCapacity = mainBufferCapacity;
    dcb.subBufferCapacity = subBufferCapacity;
    dcb.writePosition = 0;
    dcb.numOfSubBuffers = 1;
    return dcb;
}

void ADD_SUB_BUFFER(DECENTRALIZED_BUFFER *dcb)
{
    if ((*dcb).numOfSubBuffers < (*dcb).mainBufferCapacity)
    {
        (*dcb).buffer[(*dcb).numOfSubBuffers] = calloc((*dcb).subBufferCapacity, sizeof(char));
        (*dcb).numOfSubBuffers++;
    }
}
int WRITE_TO_DCB(DECENTRALIZED_BUFFER *dcb, char *data, int DataSizeInBytes)
{
    int mPos = (*dcb).writePosition / (*dcb).subBufferCapacity;

    if (mPos < (*dcb).mainBufferCapacity)
    {
        if (mPos >= (*dcb).numOfSubBuffers)
            ADD_SUB_BUFFER(dcb);
        int sPos = (*dcb).writePosition % (*dcb).subBufferCapacity;
        memcpy(&(*dcb).buffer[mPos][sPos], data, DataSizeInBytes);
        (*dcb).writePosition+=DataSizeInBytes;
        return 1;
    }
    return 0;
}

void FREE_DB(DECENTRALIZED_BUFFER dcb)
{   
    for(int i =0;i <dcb.numOfSubBuffers;i++)
        free(dcb.buffer[i]);
    free(dcb.buffer);
}
