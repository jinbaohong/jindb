#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NWAY 4

typedef struct kp {
	void *ptr;
	int key;
} KP;

typedef struct node {
	KP arr[NWAY];
	// struct node *lptr;
	int isLeaf; // 1 for leaf, 0 for non-leaf
	int key_cnt; // used key
	int ptr_cnt; // used ptr
	struct node *parent;
} NODE;

struct data_ent {
	int id;
	int val;
};


NODE *root;
#define NDATA 32
struct data_ent dataset[NDATA];


NODE *creatNode(int isLeaf)
{
	NODE *res = calloc(sizeof(NODE), 1);
	res->isLeaf = isLeaf;
	return res;
}

NODE *findLeaf(NODE *root, int key, int *idx)
{
	NODE *C = root;
	while (!C->isLeaf){
		for (int i = 0; i < C->key_cnt; ++i){
			if (C->arr[i].key > key){
				C = C->arr[i].ptr;
				break;
			} else if (C->arr[i].key == key){
				C = C->arr[i+1].ptr;
				break;
			} else if (C->arr[i].key < key && i == C->key_cnt - 1){
				C = C->arr[i+1].ptr;
				break;
			}
		}
	}
	/* C is a leafNode */
	for (int i = 0; i < C->key_cnt; ++i)
		if (C->arr[i].key == key){
			*idx = i;
			return C;
		}
	// If run to here, means no such key in this tree.
	*idx = -1;
	return C;
}

int printAll(NODE *root, int key)
{
	NODE *L;
	int idx, done;

	done = 0;
	L = findLeaf(root, key, &idx);
	if (idx == -1){
		printf("Error: Can't find key: %d\n", key);
		return -1;
	}

	printf("Got key: %d, it's value is %d\n",
			key, ((struct data_ent*)L->arr[idx].ptr)->val);

}

int printNode(NODE *node)
{
	printf("key_cnt=%d, ptr_cnt=%d\n", node->key_cnt, node->ptr_cnt);
	for (int i = 0; i < NWAY; ++i){
		printf("| %2p | %d ", node->arr[i].ptr, node->arr[i].key);
	}
	printf("|\n");
}

int printTree(NODE *node)
{
	printNode(node);
	if (!node->isLeaf)
		for (int i = 0; i < NWAY; ++i)
			if (node->arr[i].ptr)
				printNode(node->arr[i].ptr);		
}

int insertKeyInLeaf(KP arr[], int key, void *ptr)
{
	for (int i = NWAY-2; i >= 0; i--)
		if (arr[i].ptr){
			if (arr[i].key < key){
				arr[i+1].key = key;
				arr[i+1].ptr = ptr;
				return 0;
			} else {
				arr[i+1].key = arr[i].key;
				arr[i+1].ptr = arr[i].ptr;
			}
		}
	arr[0].key = key;
	arr[0].ptr = ptr;
	return 0;
}



int swapKp(KP *a, KP *b)
{
	KP tmp = *a;
	*a = *b;
	*b = tmp;
}



int insertKeyInNonLeaf(KP arr[], int up, int K, NODE *L_new)
{
	for (int i = up; i >= 0; i--){
		if (arr[i].key < K){
			arr[i+1].key = K;
			arr[i+2].ptr = L_new;
			return 0;
		} else {
			arr[i+1].key = arr[i].key;
			arr[i+2].ptr = arr[i+1].ptr;
		}
	}
	arr[0].key = K;
	arr[1].ptr = L_new;
	return 0;
}

int insertKeyInParent(NODE *L, int K, NODE *L_new)
{
	KP T[NWAY+1];
	NODE *P_new;
	int K_new;

	if (L->parent == L) { // L is root
printf("*****Root splitted!\n");
		root = creatNode(0);
		root->arr[0].ptr = L;
		root->arr[0].key = K;
		root->arr[1].ptr = L_new;
		root->key_cnt = 1;
		root->ptr_cnt = 2;
		L_new->parent = L->parent = root->parent = root;
		return 0;
	}
	// L is not root
	NODE *P = L->parent;
	L_new->parent = P;
	if (P->ptr_cnt < NWAY){ // has space to insert (K, L_new)
printf("*****Parent is not full, but still need insert key %d into:\n", K);
printNode(P);
		insertKeyInNonLeaf(P->arr, P->ptr_cnt-2, K, L_new);
		P->ptr_cnt++;
		P->key_cnt++;
	} else { // No space to insert (K, L_new) ---> split
printf("*****Parent is full, need insert key %d , and split:\n", K);
printNode(P);
		P_new = creatNode(0);
		// Sort all entries at a new space T
		memcpy(T, P, NWAY*sizeof(KP));
		insertKeyInNonLeaf(T, NWAY-2, K, L_new);

		// Erase P's entries
		memset(P->arr, 0, NWAY*sizeof(KP));
printf("*****Erase:\n");
printNode(P);
		 // Copy first half
		memcpy(P->arr, T, (NWAY/2)*sizeof(KP)+sizeof(void*));
		P->key_cnt = NWAY/2;
		P->ptr_cnt = NWAY/2+1;
		for (int i = 0; i < P->ptr_cnt; ++i)
			((NODE*)P->arr[i].ptr)->parent = P;
printf("*****Copy first half:\n");
printNode(P);
		// Copy last half
		memcpy(P_new->arr, T+NWAY/2+1, ((NWAY+1)/2)*sizeof(KP));
		P_new->key_cnt = (NWAY-1)/2;
		P_new->ptr_cnt = (NWAY-1)/2+1;
		for (int i = 0; i < P_new->ptr_cnt; ++i)
			((NODE*)P_new->arr[i].ptr)->parent = P_new;
printf("*****Copy last half:\n");
printNode(P_new);
		K_new = T[NWAY/2].key;
printf("*****K_new=%d\n", K_new);
		insertKeyInParent(P, K_new, P_new); // K will be a new key-val entry in parent
	}

}

int insertKey(NODE *root, int key, void *ptr)
{
	int idx, K;
	NODE *L, *L_new;
	KP T[NWAY];

	L = findLeaf(root, key, &idx);
	if (L->key_cnt < NWAY-1) { // Don't need to split
printf("*****leaf is not full\n");

		insertKeyInLeaf(L->arr, key, ptr);
		L->key_cnt++;
		L->ptr_cnt++;
	}
	else { // L is already full, need to split
printf("*****leaf is full, need to update:\n");
printNode(L);
		L_new = creatNode(1);
		memcpy(T, L, (NWAY-1)*sizeof(KP));
		insertKeyInLeaf(T, key, ptr);

		L_new->arr[NWAY-1].ptr = L->arr[NWAY-1].ptr;
		L->arr[NWAY-1].ptr = L_new;

		memset(L->arr, 0, (NWAY-1)*sizeof(KP));
printf("*****Erase:\n");
printNode(L);
		// Copy first half
		memcpy(L->arr, T, ((NWAY+1)/2)*sizeof(KP));
		L->key_cnt = (NWAY+1)/2;
		L->ptr_cnt = (NWAY+1)/2;
printf("*****Copy first half:\n");
printNode(L);
		// Copy last half
		memcpy(L_new->arr, T+((NWAY+1)/2), (NWAY/2)*sizeof(KP));
		L_new->key_cnt = NWAY/2;
		L_new->ptr_cnt = NWAY/2;
printf("*****Copy last half:\n");
printNode(L_new);
		K = L_new->arr[0].key;
		insertKeyInParent(L, K, L_new); // K will be a new key-val entry in parent
	}
}

