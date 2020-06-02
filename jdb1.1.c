#include "btree.c"

int main(int ac, char const *av[])
{
	char line[128], cmd[128], key[16], val[16];
	struct data_ent *data_ptr;
	NODE *root;
	root = creatNode(1); // single-node-tree is leaf
	root->parent = root;

	printf("*************** Welcome to JinDB ***************\n");


	while(1){
		memset(line, 0, 64);
		printf("> ");
		fgets(line, 128, stdin);
		line[strlen(line)-1] = '\0';
		if (line[0]=='\0')
			continue;
		sscanf(line, "%s %s %s", cmd, key, val);
		if (!strcmp(cmd, "insert")){ // insert 0 7
			data_ptr = malloc(sizeof(struct data_ent));
			data_ptr->id = atoi(key);
			data_ptr->val = atoi(val);
			printf("*****Before insert: \n");
			printTree(root);
			insertKey(root, atoi(key), data_ptr);
			printf("*****After insert: \n");
			printTree(root);
		}
		if (!strcmp(cmd, "select")) { // select 0
			printAll(root, atoi(key));
		}
	}
	return 0;
}


// int main(int ac, char const *av[])
// {
// 	// NODE *root;
// 	root = creatNode(1); // single-node-tree is leaf
// 	root->parent = root;

// 	for (int i = 0; i < NDATA; ++i){
// 		dataset[i].id = i;
// 		dataset[i].val = i * 3 + 1; // whatever...
// 	}

// 	int tmp = NDATA;
// 	for (int i = 0; i < tmp; ++i){
// printf("*************Insert key %d*************\n", i);
// printf("*****Before insert: \n");
// printTree(root);
// 		insertKey(root, i, &dataset[i]);
// printf("*****After insert: \n");
// printTree(root);
// 	}
	
// 	for (int i = 0; i < tmp; ++i){
// 		printAll(root, i);
// 	}
	
// 	return 0;
// }