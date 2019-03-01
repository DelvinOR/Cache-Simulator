#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<ctype.h>
//we are implementing a hash table as a cache
//initially the cache is empty
//all cache lines are empty (validkey = 0 and tag = 0)
//memory size is 2 pow 48
//so address are 48 bits long

//we have to create a queue for fifo policy
struct node;
struct node{
  unsigned long long int data;
  struct node*next;
};
struct Queue;
struct Queue{//queue points to the front and end of queue
  struct node*front;
  struct node*end;
  int size;
};
void enqueue(struct Queue*, unsigned long long int);
void enqueue(struct Queue*q, unsigned long long int data){
  struct node*newnode = (struct node*)malloc(sizeof(struct node));
  newnode->data = data;
  newnode->next = NULL;
  if(q->front == NULL){//the queue is empty
    q->front = newnode;
    q->end = newnode;
    q->size++;
    return;
  }else{//we have to add it to the end
    q->end->next = newnode;
    q->end = newnode;
    q->size++;
  }
  return;
}
struct node*dequeue(struct Queue*);//returns the front node
struct node*dequeue(struct Queue*q){
  struct node*temp = q->front;
  if(temp == NULL){
    //queue is empty so return null;
    return NULL;
  }
  q->front = temp->next;
  temp->next = NULL;
  if(q->front == NULL){
    //queue is now empty so we have to update the end of the queue so that the front is not saved by end pointer
    q->end = NULL;
  }
  q->size--;
  return temp;
}//you can free in the actual removal of the dequeue
//have to see if I have to check if the string is initially an int
struct block;
struct block{
  unsigned long long int tag;
  int validkey;
};
struct set;
struct set{
  struct block**blocks;
  struct Queue*q;//for fifo

};
int isPowerOfTwo(int);
int isPowerOfTwo(int num){
  return (num != 0)&&((num &(num-1)) == 0);
}

int main(int argc, char**argv){
  if(argc != 6){//change to 6
    printf("error\n");
    return 0;
  }
  char cacheinput[60];
  strcpy(cacheinput, argv[1]);
  for(int i = 0; i<strlen(cacheinput);i++){
    if(isalpha(cacheinput[i]) != 0){
      printf("error\n");
      return 0;
    }
  }
  int cachesize  = atoi(argv[1]);
  if( isPowerOfTwo(cachesize) == 0){
    printf("error\n");
  return 0;
  }
  char direct[15];
  strcpy(direct, "direct");
  char assoc[15];
  strcpy(assoc, "assoc");
  char assoccolon[15];
  strcpy(assoccolon, "assoc:");
  int directq = 0;
  int assocq = 0;
  int assoccolonq = 0;
  int len = strlen(argv[2]);
  char *associativity;
  associativity = (char*)malloc((1+len)*sizeof(char));
  strcpy(associativity, argv[2]);
  if(strcmp(direct, associativity) == 0){//associativity is direct
    directq = 1;
  }else if( strcmp(assoc, associativity) == 0){//associativity is assoc
    assocq = 1;
  }else{
    if(strncmp(assoccolon, associativity, 6) == 0){//associativity is assoc:
      assoccolonq = 1;
    }
  }
  if(directq == 0 && assocq == 0 && assoccolonq == 0){
    printf("error\n");
    return 0;
  }
  int A;
  if(directq == 1){
    A = 1;
  }else if(assocq == 1){
    A = 0;
  }else{//there is a value in the associativity
    int c = 0;
    for(int i = 6; i<strlen(associativity); i++){
      c++;
    }
    char*AN;
    AN = (char*)malloc((c + 1)*sizeof(char));
    int j = 0;
    for(int i = 6; i<strlen(associativity); i++){
      AN[j] = associativity[i];
      j++;
    }
    AN[c] = '\0';
    A = atoi(AN);
    if( isPowerOfTwo(A) == 0){
      printf("error\n");
      return 0;
    }
  }
  //now we have the cache size and we have the associativity value which we can distinguish with one bigset
  //and checking if equal to 1
  char fifo[15];
  strcpy(fifo, "fifo");
  char lru[15];
  strcpy(lru, "lru");
  int policylen = strlen(argv[3]);
  char*policy;
  policy = (char*)malloc((policylen + 1)*sizeof(char));
  strcpy(policy, argv[3]);
  int fifoq = 0;
  int lruq = 0;
  if(strcmp(policy, fifo) == 0){
    fifoq = 1;
  }else{
    if(strcmp(policy, lru) == 0){
      lruq = 1;//for this I am planning on using a different type of cache from setlru
    }
  }
  if(fifoq == 0 && lruq == 0){
    printf("error\n");
    return 0;
  }
  char blocksizeinput[60];
  strcpy(blocksizeinput, argv[4]);
  for(int i= 0; i<strlen(blocksizeinput);i++){
    if(isalpha(blocksizeinput[i]) != 0){
      printf("error\n");
      return 0;
    }
  }
  int blocksize = atoi(argv[4]);
  if(isPowerOfTwo(blocksize) == 0){
    printf("error\n");
    return 0;
  }
  FILE*fp;
  fp = fopen(argv[5], "r");
  if(fp == NULL){
    printf("error\n");
    return 0;
  }
  if(assocq ==1){
    A = cachesize/blocksize;
  }
  int sets = (cachesize/A)/blocksize;
  struct set**cache;
  if(sets == 1){//it is a fully associative mapped cache
    cache = (struct set**)malloc(sizeof(struct set*));
    cache[0] = (struct set*)malloc(sizeof(struct set));
  }else{
    cache = (struct set**)malloc(sets*sizeof(struct set*));
    for(int i = 0; i<sets; i++){
      cache[i] = (struct set*)malloc(sizeof(struct set));
    }
  }//sets are made
  for(int i = 0; i<sets; i++){
    cache[i]->blocks = (struct block**)malloc(A*sizeof(struct block*));
    for(int j = 0; j<A;j++){
      cache[i]->blocks[j] = (struct block*)malloc(sizeof(struct block));
    }
  }
  //cache is created and memory is allocated
  //and the tags and valid key's are set to 0
  for(int i = 0; i<sets; i++){
    cache[i]->q = (struct Queue*)malloc(sizeof(struct Queue));
    cache[i]->q->front = NULL;
    cache[i]->q->end = NULL;
    cache[i]->q->size = 0;
  }//queue for each set is initialize
  int blockbits = log2(blocksize);
  int indexbits = log2(sets);
  unsigned long long int p  = (unsigned long long int)pow(2,indexbits);
  int reads = 0;
  int writes = 0;
  int hits = 0;
  int misses = 0;
  //have go create strings for the first string delimeter character and hex number
  printf("no-prefetch\n");
  char aa[34];
  while(fgets(aa,34,fp) != NULL){
    if(aa[0] == '#'){
     break;
    }
    int be = strcspn(aa,"\n");
    char a[be+1];
    strncpy(a, aa, be);
    a[be] = '\0';
    int posofcolon;
    for(int i = 0; i<strlen(a);i++){
      if(a[i] == ':'){//found pos of colon
	posofcolon = i;
	break;
      }
    }
    char transaction = a[posofcolon+2];
    char memoryaddress[15];//correct
    int pos = 0;
    for(int i = posofcolon+4; i<strlen(a);i++){
      memoryaddress[pos] = a[i];
      pos++;
    }
    memoryaddress[pos] = '\0';
    unsigned long long int memaddress = (unsigned long long int)strtoll(memoryaddress, NULL, 0);
    unsigned long long int index = (memaddress>>blockbits)%p;
    unsigned long long int tag = (memaddress>>(blockbits+ indexbits));
    // printf("%s: index is %llu and tag is %llu\n", memoryaddress, index, tag);
    int hitormiss = 0;//0 means miss 1 means hit
    for(int i = 0;i<A; i++){
      if(cache[index]->blocks[i]->validkey == 1){
	if(cache[index]->blocks[i]->tag == tag){
	  hitormiss = 1;
	  break;
	}
      }
    }
    if(hitormiss == 0){
      misses++;
      if(transaction == 'R'){
	reads++;
      }
      if(transaction == 'W'){//it is a write miss so we have to account for the extra memory read
	reads++;
	writes++;
      }
      //we have to add it to the cache and also check that there is space
      int added = 0;//1 means added
      for(int i = 0; i<A; i++){
	if(cache[index]->blocks[i]->validkey == 0){//we add here
	  cache[index]->blocks[i]->validkey = 1;
	  cache[index]->blocks[i]->tag = tag;
	  added = 1;
	  enqueue(cache[index]->q,tag);
	  break;
	}
      }
      if(added ==0){//the queue has something so we must take out the front of the queue and find that tag in the cache and add to cache
	struct node*n = dequeue(cache[index]->q);
	unsigned long long int tagtofind = n->data;
	for(int i = 0;i<A; i++){
	  if(cache[index]->blocks[i]->tag == tagtofind){//update tag
	    cache[index]->blocks[i]->tag = tag;
	    enqueue(cache[index]->q, tag);
	    break;
	  }
	}
      }
    }else{
      if(hitormiss == 1){
	hits++;
	if(transaction == 'W'){
	  writes++;
	}
	if(lruq == 1){//no fifo so we have to update the q
	  struct node*pp = dequeue(cache[index]->q);
	  if(pp->data == tag){//just add it to the end of the q
	    enqueue(cache[index]->q,tag);
	  }else{//we have to do the update of lru
	    enqueue(cache[index]->q, pp->data);
	    int s = cache[index]->q->size -1;//enquees needed to go back to normal
	    int found = 0;
	    while(found == 0){
	      pp = dequeue(cache[index]->q);
	      if(pp->data == tag){
		s--;
		found = 1;
		break;
	      }else{
		enqueue(cache[index]->q, pp->data);
		s--;
	      }
	    }
	    for(int i = 0; i<s; i++){
	      struct node*tp = dequeue(cache[index]->q);
	      enqueue(cache[index]->q,tp->data);
	    }
	    enqueue(cache[index]->q, tag);
	  }
	}
      }
    }
  }
  printf("Memory reads: %d\n", reads);
  printf("Memory writes: %d\n", writes);
  printf("Cache hits: %d\n", hits);
  printf("Cache misses: %d\n", misses);
  printf("with-prefetch\n");
  rewind(fp);
  reads = 0;
  writes = 0;
  hits = 0;
  misses = 0;
  struct set**cache2;
  if(sets == 1){//it is a fully associative mapped cache
    cache2 = (struct set**)malloc(sizeof(struct set*));
    cache2[0] = (struct set*)malloc(sizeof(struct set));
  }else{
    cache2 = (struct set**)malloc(sets*sizeof(struct set*));
    for(int i = 0; i<sets; i++){
      cache2[i] = (struct set*)malloc(sizeof(struct set));
    }
  }//sets are made
  for(int i = 0; i<sets; i++){
    cache2[i]->blocks = (struct block**)malloc(A*sizeof(struct block*));
    for(int j = 0; j<A;j++){
      cache2[i]->blocks[j] = (struct block*)malloc(sizeof(struct block));
    }
  }
  for(int i = 0; i<sets; i++){
    cache2[i]->q = (struct Queue*)malloc(sizeof(struct Queue));
    cache2[i]->q->front = NULL;
    cache2[i]->q->end = NULL;
    cache[i]->q->size =0;
  }//queue for each set is initialize
  while(fgets(aa,34,fp) != NULL){

    if(aa[0] == '#'){
     break;
    }
    int be = strcspn(aa,"\n");
    char a[be+1];
    strncpy(a, aa, be);
    a[be] = '\0';
    int posofcolon;
    for(int i = 0; i<strlen(a);i++){
      if(a[i] == ':'){//found pos of colon
	posofcolon = i;
	break;
      }
    }
    char transaction = a[posofcolon+2];
    char memoryaddress[15];//correct
    int pos = 0;
    for(int i = posofcolon+4; i<strlen(a);i++){
      memoryaddress[pos] = a[i];
      pos++;
    }
    memoryaddress[pos] = '\0';
    unsigned long long int memaddress = (unsigned long long int)strtoll(memoryaddress, NULL, 0);
    unsigned long long int index = (memaddress>>blockbits)%p;
    unsigned long long int tag = (memaddress>>(blockbits+ indexbits));
    // printf("%s: index is %llu and tag is %llu\n", memoryaddress, index, tag);
    int hitormiss = 0;//0 means miss 1 means hit
    for(int i = 0;i<A; i++){
      if(cache2[index]->blocks[i]->validkey == 1){
	if(cache2[index]->blocks[i]->tag == tag){
	  hitormiss = 1;
	  break;
	}
      }
    }
    if(hitormiss == 0){
      misses++;
      if(transaction == 'R'){
	reads++;
      }
      if(transaction == 'W'){//it is a write miss so we have to account for the extra memory read
	reads++;
	writes++;
      }
      //we have to add it to the cache and also check that there is space
      int added = 0;//1 means added
      for(int i = 0; i<A; i++){
	if(cache2[index]->blocks[i]->validkey == 0){//we add here
	  cache2[index]->blocks[i]->validkey = 1;
	  cache2[index]->blocks[i]->tag = tag;
	  added = 1;
	  enqueue(cache2[index]->q,tag);
	  break;
	}
      }
      if(added ==0){//the queue has something so we must take out the front of the queue and find that tag in the cache and add to cache
	struct node*n = dequeue(cache2[index]->q);
	unsigned long long int tagtofind = n->data;
	for(int i = 0;i<A; i++){
	  if(cache2[index]->blocks[i]->tag == tagtofind){//update tag
	    cache2[index]->blocks[i]->tag = tag;
	    enqueue(cache2[index]->q, tag);
	    break;
	  }
	} 
      }
      //now we have to implement the pre fetcher
      //we have to get the  address plus block size
      memaddress = memaddress + blocksize;//ASK PROFESSOR ABOUT THIS
      index = (memaddress>>blockbits)%p;
      tag = (memaddress>>(blockbits+ indexbits));
      //ALSO ASK IF WE HAVE TO UPDATE HIT AND MISS IN THIS ADDITION OF PREFETCHER
      //I'm thinking no for now
      //now our memaddress is the next adjacent block
      int presentincache = 0;// 1 if true
      for(int i = 0; i<A; i++){
	if(cache2[index]->blocks[i]->validkey == 1){
	  if(cache2[index]->blocks[i]->tag == tag){
	    presentincache = 1;
	    break;
	  }
	}
      }
      if(presentincache ==0){//now we do a memory read
	//misses++;
	reads++;
	int nadded = 0;
	for(int i = 0; i<A; i++){
	  if(cache2[index]->blocks[i]->validkey == 0){//add here
	    cache2[index]->blocks[i]->validkey = 1;
	    cache2[index]->blocks[i]->tag = tag;
	    nadded = 1;
	    enqueue(cache2[index]->q, tag);
	    break;
	  }
	}
	if(nadded == 0){//fifo comes into play
	struct node*tem = dequeue(cache2[index]->q);
	unsigned long long int tagtofind = tem->data;
	 for(int i = 0; i<A; i++){
	   if(cache2[index]->blocks[i]->tag == tagtofind){
	     cache2[index]->blocks[i]->tag = tag;
	     enqueue(cache2[index]->q,tag);
	     break;
	   }
	 }
       }
      }//else{
      //	hits++;
      // }
    }else{
      if(hitormiss == 1){
	hits++;
	if(transaction == 'W'){
	  writes++;
	}
	if(lruq == 1){//no fifo so we have to update the q
	  struct node*pp = dequeue(cache2[index]->q);
	  if(pp->data == tag){//just add it to the end of the q
	    enqueue(cache2[index]->q,tag);
	  }else{//we have to do the update of lru
	    enqueue(cache2[index]->q, pp->data);
	    int s = cache2[index]->q->size -1;//enquees needed to go back to normal
	    int found = 0;
	    while(found == 0){
	      pp = dequeue(cache2[index]->q);
	      if(pp->data == tag){
		s--;
		found = 1;
		break;
	      }else{
		enqueue(cache2[index]->q, pp->data);
		s--;
	      }
	    }
	    for(int i = 0; i<s; i++){
	      struct node*tp = dequeue(cache2[index]->q);
	      enqueue(cache2[index]->q,tp->data);
	    }
	    enqueue(cache2[index]->q, tag);
	  }
	}
      }
    }
  }
  fclose(fp);
  printf("Memory reads: %d\n", reads);
  printf("Memory writes: %d\n", writes);
  printf("Cache hits: %d\n", hits);
  printf("Cache misses: %d\n", misses);
  
  
  return 0;
}
