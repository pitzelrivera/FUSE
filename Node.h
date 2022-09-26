#pragma once
#include <vector>
#include <string>
#include <iostream>
using namespace std;

//Idea for dynamic N-ary tree https://www.geeksforgeeks.org/generic-treesn-array-trees/
struct Node
{
	struct Element
	{
		bool isLump;
		int offset;
		int length;
		string name;
		string content;
		Element();
		Element(int offSet, int fileLength, string fileName);
	};
	Node* parent;
	vector<Node*> children;
	Element data;
	Node();
	~Node();
	Node* addNode(Node* nodeParent, int offSet, int fileLength, string fileName);
	Node* searchNode(Node* nodeParent, Element* searchTerm);
	//Node* deleteNode(Node* toDelete);
};