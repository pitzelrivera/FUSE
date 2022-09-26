#include "Node.h"

Node::Element::Element()
{}

Node::Element::Element(int offSet, int fileLength, string fileName)
{
	offset = offSet;
	length = fileLength;
	name = fileName;
	if (length == 0)
	{
		isLump = false;
	}
	else
	{
		isLump = true;
	}
	content = "";
}

Node::Node()
{
	parent = nullptr;
	data = Element();
}

Node::~Node()
{}

Node* Node::addNode(Node* nodeParent, int offSet, int fileLength, string fileName)
{
	Node* newNode = new Node;
	Node::Element newData = Node::Element(offSet, fileLength, fileName);
	newNode->data = newData;
	newNode->parent = nodeParent;
	if (nodeParent != nullptr)
	{
		nodeParent->children.push_back(newNode);
	}
	return newNode;
}

Node* Node::searchNode(Node* nodeParent, Element* searchTerm)
{
	for (unsigned int i = 0; i < nodeParent->children.size(); i++)
	{
		if (nodeParent->children[i]->data.name == searchTerm->name)
		{
			return nodeParent->children[i];
		}
	}
	return nodeParent;
}
