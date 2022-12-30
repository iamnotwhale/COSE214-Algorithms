#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

#include "node.h"
#include "heap.h"

// 허프만 트리를 순회하며 허프만 코드를 생성하여 codes에 저장
// leaf 노드에서만 코드를 생성
// strdup 함수 사용
// make_huffman_code 함수에서 호출
static void traverse_tree( tNode *root, char *code, int depth, char *codes[]);

// 새로운 노드를 생성
// 좌/우 subtree가 NULL(0)이고 문자(data)와 빈도값(freq)이 저장됨
// make_huffman_tree 함수에서 호출
// return value : 노드의 포인터
static tNode *newNode(unsigned char data, int freq);

////////////////////////////////////////////////////////////////////////////////
// 허프만 코드를 화면에 출력
void print_huffman_code( char *codes[])
{
	int i;
	
	for (i = 0; i < 256; i++)
	{
		printf( "%d\t%s\n", i, codes[i]);
	}
}

////////////////////////////////////////////////////////////////////////////////
// 허프만 트리로부터 허프만 코드를 생성
// traverse_tree 함수 호출
void make_huffman_code( tNode *root, char *codes[])
{
	char code[256];
	
	traverse_tree( root, code, 0, codes);
}

// 허프만 트리를 순회하며 허프만 코드를 생성하여 codes에 저장
// leaf 노드에서만 코드를 생성
// strdup 함수 사용
// make_huffman_code 함수에서 호출
static void traverse_tree( tNode *root, char *code, int depth, char *codes[]) {
	if (root!=NULL){
		if (root->left==NULL&&root->right==NULL){
			code[depth] = '\0';
			codes[root->data] = strdup(code); // hint: strdup
		}
		else{
			if (root->left!=NULL){
			code[depth] = '0';
			traverse_tree(root->left, code, depth+1, codes);
			}
			if (root->right!=NULL){
			code[depth] = '1';
			traverse_tree(root->right, code, depth+1, codes);
			}
		}
		
	}
}

// 새로운 노드를 생성
// 좌/우 subtree가 NULL(0)이고 문자(data)와 빈도값(freq)이 저장됨
// make_huffman_tree 함수에서 호출
// return value : 노드의 포인터
static tNode *newNode(unsigned char data, int freq) {
	tNode *new_node = (tNode*) malloc(sizeof(tNode));
	new_node->data = data;
	new_node->freq = freq;
	new_node->left = NULL;
	new_node->right = NULL;

	return new_node;
}

// 파일에 속한 각 문자(바이트)의 빈도 저장
// return value : 파일에서 읽은 바이트 수
int read_chars( FILE *fp, int ch_freq[]) {
	int byte = 0;
	int c;
	while (1) {
		c = fgetc(fp);
		if (feof(fp)) {
			break;
		}
		(ch_freq[c])++;
		byte++;
	}
	return byte;
}

// 파일로부터 문자별 빈도(256개)를 읽어서 ch_freq에 저장
void get_char_freq( FILE *fp, int ch_freq[]) {
	//fread(ch_freq, sizeof(int), 256, fp); // 한줄
	for (int i=0; i<256; i++) {
		int num, freq;
		fscanf(fp, "%d %d", &num, &freq);
		ch_freq[num] = freq;
	}
}

// 허프만 코드에 대한 메모리 해제
void free_huffman_code( char *codes[]) {
	for (int i=255; i>=0; i--) {
		free(codes[i]);
	}
}

// 허프만 트리를 생성
// 1-> capacity 256 짜리 빈(empty) 힙 생성 // HEAP *heap = heapCreate( 256);
// 2-> 개별 알파벳에 대한 노드 생성
// 3-> 힙에 삽입 (minheap 구성) // heapInsert( heap, node);
// 4-> 2개의 최소값을 갖는 트리 추출 // tNode *left = heapDelete( heap); tNode *right = heapDelete( heap);
// 5-> 두 트리를 결합 후 새 노드에 추가
// 6-> 새 트리를 힙에 삽입
// 7-> 힙에 한개의 노드가 남을 때까지 반복
// return value: 트리의 root 노드의 포인터
tNode *make_huffman_tree( int ch_freq[]) {
	HEAP *heap = heapCreate( 256);

	for (int i=0; i<256; i++) {
		tNode *node = newNode(i, ch_freq[i]);
		heapInsert(heap, node);
 	}

	while (heap->last > 0) {
		tNode *left = heapDelete( heap);
		tNode *right = heapDelete( heap);

		tNode *combineNode = newNode(0, left->freq+right->freq);
		combineNode->left = left;
		combineNode->right = right;

		heapInsert(heap, combineNode);
	}

	tNode *root = heap->heapArr[0];

	heapDestroy(heap);
	
	return root;
}

// 허프만 트리 메모리 해제
void destroyTree( tNode *root) {
	if (root->left != NULL) {
		destroyTree(root->left);
	}
	if (root->right != NULL) {
		destroyTree(root->right);
	}

	free(root);
}

// 입력 텍스트 파일(infp)을 허프만 코드를 이용하여 출력 파일(outfp)로 인코딩
// return value : 인코딩된 텍스트의 바이트 수 (파일 크기와는 다름)
int encoding( char *codes[], int ch_freq[], FILE *infp, FILE *outfp) {
	fwrite(ch_freq, sizeof(int), 256, outfp);
	unsigned char c;
	int cntBytes=0, cntBits=0, cnt=0;
	unsigned char bit =0; //인코딩된 비트 넣을 변수 
	while (1){
		c = fgetc(infp);
		if (feof(infp)) break;
		for (int i=0; codes[c][i] != '\0'; i++){
			//bit추가~~
			// 비트연산자
			bit = bit << 1;
			//bit = bit+codes[c][i];
			bit = codes[c][i]=='1' ? bit+1 : bit;
			cnt++; cntBits++;
			if (cnt==8){
				//
				fputc(bit, outfp);
				cnt=0;
				// bit 초기화
				bit = 0;
				cntBytes++;
			}
		}
	}
	if (bit!=0){
		bit = bit << (8-cnt);
		fputc(bit, outfp);
		cntBytes++;
		
	}
	fprintf(outfp, "%c%c%c%c", cntBits, cntBits>>8, cntBits>>16, cntBits>>24 );
	return cntBytes;
}

// 입력 파일(infp)을 허프만 트리를 이용하여 텍스트 파일(outfp)로 디코딩
void decoding( tNode *root, FILE *infp, FILE *outfp) {
	int bits;
	int index = 0;
	unsigned char bit;
	fseek(infp, -4, SEEK_END);
	fread(&bits, sizeof(int), 1, infp);

	fseek(infp, 256 * sizeof(int), SEEK_SET);

	int *bitArr = (int*) malloc(bits*sizeof(int));
	
	int n = bits/8;
	int num = bits%8;
	
	for (int i=0; i<n; i++) {
		bit = fgetc(infp);
		for (int j=0; j<8; j++) {
			bitArr[index] = ((bool)(bit&(0b10000000))== true) ? 1 : 0;
			bit = bit<<1;
			index++;
		}
	}
	if (num > 0) {
		bit = fgetc(infp);
		for (int j=0; j<num; j++) {
			bitArr[index] = ((bool)(bit&(0b10000000))==true) ? 1 : 0;
			bit = bit<<1;
			index++;
		}
	}

	tNode *tmp = root;

	for (int i=0; i<bits; i++){
		if (bitArr[i]==0){
			tmp = tmp->left;
		}
		else if (bitArr[i]==1){
			tmp = tmp->right;
		}
		
		if (tmp->data != 0){
			fputc(tmp->data, outfp);
			tmp = root;
		}
	}

	free(bitArr);
}