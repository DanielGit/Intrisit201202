//[PROPERTY]===========================================[PROPERTY]
//            *****   电子词典平台GUI  *****
//        --------------------------------------
//       	            GUI头文件
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明		
//  V0.1    2005-8      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#include <kernel/kernel.h>

#ifndef _WIN_TREE_H
#define _WIN_TREE_H

typedef struct _TREE
{
	struct _TREE *Parent;
	struct _TREE *Child;
	struct _TREE *SiblingNext;
	struct _TREE *SiblingPrev;
}TREE;
typedef TREE *PTREE;

#define TreeParent(tree)		((tree)->Parent)
#define TreeChild(tree)			((tree)->Child)
#define TreeChildEmpty(tree)	(((tree)->Child) == NULL)

#define TreeSiblingInit(tree)	((tree)->SiblingNext = (tree)->SiblingPrev = (tree))
#define TreeSiblingPrev(tree)	((tree)->SiblingPrev)
#define TreeSiblingNext(tree)	((tree)->SiblingNext)
#define TreeSiblingEmpty(tree)	((((tree)->SiblingNext) == (tree)) && (((tree)->SiblingPrev) == (tree)))
#define TreeSiblingHead(tree)	((tree)->Parent->Child)

#define TreeEntry(tree, type, member) \
    ((type*)((DWORD)(tree) - (DWORD)(&((type*)0)->member)))

void TreeInit(PTREE tree);

void TreeSiblingMoveToTail(PTREE node);
void TreeSiblingMoveToHead(PTREE node);
void TreeSiblingMoveForward(PTREE node);
void TreeSiblingMoveBackward(PTREE node);
void TreeSiblingRemove(PTREE node);
void TreeSiblingInsertBehind(PTREE node, PTREE insert);
void TreeSiblingInsertAhead(PTREE node, PTREE insert);

PTREE TreeSiblingTail(PTREE tree);
PTREE TreeChildFirst(PTREE tree);
PTREE TreeChildLast(PTREE tree);

void TreeChildInsertFirst(PTREE tree, PTREE child);
void TreeChildInsertLast(PTREE tree, PTREE child);

void TreeRemove(PTREE tree);

#endif // _WIN_TREE_H

