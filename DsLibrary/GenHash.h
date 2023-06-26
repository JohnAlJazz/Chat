#ifndef __GEN_HASH_H__
#define __GEN_HASH_H__

#include <stddef.h> /*size_t*/

#define FOUND 1
#define NOT_FOUND 0

typedef void (*PrintKey)(void*);

typedef size_t (*HashFunction)(void* _data);

typedef int(*EqualityFunction)(void* _firstData, void* _secondData);

typedef  void (*KeyDestroy)(void* _data);

typedef struct Hash Hash;

typedef enum HashSet_Result
 {

	SET_SUCCESS,

	SET_UNINITIALIZED,
	/** Uninitialized HasSeterror*/
	SET_KEY_DUPLICATE,
	/** Duplicate key error */
	SET_DATA_UNINITIALIZED,
	/** Uninitialized data error*/
	SET_KEY_NOT_FOUND,
	/** Key not found */

	SET_OVERFLOW
	/**No more space in HashSe */ 

}HashSet_Result;




Hash* HashCreate(size_t _size, HashFunction _HashFunc, EqualityFunction _equalityFunc);

void HashDestroy(Hash** _hash, KeyDestroy _keydestroy);

HashSet_Result HashInsert(Hash* _hash, void* _data);

HashSet_Result HashRemove(Hash* _hash, void* _key, void** _data);

int HashIsFound(const Hash* _hash, void* _key);

size_t HashNumOfItems(const Hash* _hash);

size_t HashCapacity(const Hash* _hash);

double HashAverageRehashes(const Hash* _hash);

size_t HashMaxReHash(const Hash* _hash);

void HashPrint(const Hash* _hash, PrintKey _print);







#endif /*__GEN_HASH_H__*/

