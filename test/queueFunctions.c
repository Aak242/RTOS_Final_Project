#include "constants.h"
#include "queueFunctions.h"

void Q_Init(Q_T * q) 
{
	unsigned int i;
	
	for (i=0; i<Q_SIZE; i++)
		q->Data[i] = 0; // to simplify our lives when debugging
	
	q->Head = 0;
	q->Tail = 0;
	q->Size = 0;
}

int Q_Empty(Q_T * q) 
{
	return q->Size == 0;
}

int Q_Full(Q_T * q) 
{
	return q->Size == Q_SIZE;
}

int Q_Enqueue(Q_T * q, unsigned char d) 
{
	// What if queue is full?
	if (!Q_Full(q)) 
	{
		q->Data[q->Tail++] = d;
		q->Tail %= Q_SIZE;
		q->Size++;
		return 1; // success
	} 
	
	else
		return 0; // failure
}

uint8_t Q_Dequeue(Q_T * q) 
{
	// Must check to see if queue is empty before dequeueing
	uint8_t t = 0;
	
	if (!Q_Empty(q)) 
	{
		t = q->Data[q->Head];
		q->Data[q->Head++] = 0; // to simplify debugging
		q->Head %= Q_SIZE;
		q->Size--;
	}
	
	return t;
}