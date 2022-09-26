#include "WAD.h"

Wad* Wad::loadWad(const string& path)
{
	//Vars
	string header;
	string tempStr;
	char buffer[12];
	char elemString[9];
	//Make dynamic Wad obj
	Wad* newObj = new Wad;
	int level = 0;
	char* placeHolder;
	int offSet;
	int fileLength;
	string fileName;
	int currentOffset;
	vector<Node::Element> allElems;

	//Reading a file from https://www.tutorialspoint.com/read-data-from-a-text-file-using-cplusplus
	//Using read https://www.tutorialspoint.com/reading-and-writing-binary-file-in-c-cplusplus
	ifstream file(path, std::ios::binary);
	//If open read
	if (file.is_open())
	{
	//Reading header
		//Get Magic
		file.read(buffer, 4);
		char stringArray[5];
		memcpy(stringArray, buffer, 4);
		stringArray[4] = '\0';
		newObj->magic = string(stringArray);

		//Get Number of Descriptors
		file.read((char*)&newObj->descriptNum, sizeof(int));

		//Get Descriptor Offset
		file.read((char*)&newObj->descriptOffset, sizeof(int));

		//Reading descriptors
		file.seekg(newObj->descriptOffset, std::ios::beg);
		for (int i = 0; i < newObj->descriptNum; i++)
		{

			//Offset
			file.read((char*)&offSet, sizeof(int));

			//Length
			file.read((char*)&fileLength, sizeof(int));

			//Name
			file.read(elemString, 8);
			elemString[8] = '\0';
			fileName = string(elemString);

			allElems.push_back(Node::Element(offSet, fileLength, fileName));
		}
		newObj->fileSys = nullptr;
		newObj->fileSys = newObj->fileSys->addNode(newObj->fileSys, 0, 0, "/");
		for (unsigned int i = 0; i < newObj->descriptNum; i++)
		{
			//Put stuff into tree
			//newObj->fileSys = newObj->fileSys->addNode(newObj->fileSys, &newObj->newObj->allElems[i]);
			if (allElems[i].isLump == false)
			{
				//Namespace pairs
				if (allElems[i].name.find("_START") != string::npos)
				{
					offSet = allElems[i].name.find("_START");
					tempStr = allElems[i].name.substr(0, offSet);
					newObj->fileSys = newObj->fileSys->addNode(newObj->fileSys, allElems[i].offset, allElems[i].length, tempStr);
					level++;
					continue;
				}
				else if (allElems[i].name.find("_END") != string::npos)
				{
					level--;
					newObj->fileSys = newObj->fileSys->parent;
				}
				//Marker Elements
				else
				{
					newObj->fileSys = newObj->fileSys->addNode(newObj->fileSys, allElems[i].offset, allElems[i].length, allElems[i].name);
					level++;
					for (unsigned int j = 0; j < 10; j++) //Change to iterate back cause it might not always be a lump
					{
						//Lumps;
						i++;
						newObj->fileSys = newObj->fileSys->addNode(newObj->fileSys, allElems[i].offset, allElems[i].length, allElems[i].name);
						file.seekg(allElems[i].offset, ios::beg);
						placeHolder = new char[allElems[i].length + 1];
						file.read(placeHolder, allElems[i].length);
						placeHolder[allElems[i].length] = '\0';
						newObj->fileSys->data.content = placeHolder;
						delete[] placeHolder;
						newObj->fileSys = newObj->fileSys->parent;
					}
					level--;
					newObj->fileSys = newObj->fileSys->parent;
				}
			}
			else
			{
				//Lumps
				newObj->fileSys = newObj->fileSys->addNode(newObj->fileSys, allElems[i].offset, allElems[i].length, allElems[i].name);
				//Go to offset
				file.seekg(offSet, ios::beg);
				//Make char array
				placeHolder = new char[allElems[i].length + 1];
				//Read file
				file.read(placeHolder, allElems[i].length);
				//Add null terminator
				placeHolder[allElems[i].length] = '\0';
				//Place in object and go back to parent directory
				newObj->fileSys->data.content = string(placeHolder);
				newObj->fileSys = newObj->fileSys->parent;
				//Deallocate memory
				delete[] placeHolder;
			}
		}
	}
	return newObj;
}

string Wad::getMagic()
{
	return magic;
}

bool Wad::isContent(const string& path)
{
	int index;
	int stringStart;
	string pathSub;
	Node* curr = fileSys;
	bool isPath;
	bool lastSub = false;
	
	stringStart = 1;
	if (path.find("/") != string::npos)
	{
		isPath = true;
	}
	else
	{
		return false;
	}
	while (isPath)
	{
		isPath = false; 
		if (path.find("/", stringStart) == string::npos)
		{
			lastSub = true;
			pathSub = path.substr(stringStart, path.size() - stringStart);
		}
		else
		{
			index = path.find("/", stringStart);
			pathSub = path.substr(stringStart, index - stringStart);
		}
		if (curr->data.name == pathSub)
		{
			stringStart = index + 1;
			isPath = true;
			if (!lastSub)
			{
				continue;
			}
			else
			{
				break;
			}
		}
		else
		{
			for (int i = 0; i < curr->children.size(); i++)
			{
				if (pathSub == curr->children[i]->data.name)
				{
					curr = curr->children[i];
					isPath = true;
					break;
				}
			}
			if (!isPath)
			{
				return false;
			}
			else if (!lastSub)
			{
				stringStart = index + 1;
				continue;
			}
			else
			{
				break;
			}
		}
	}
	if (curr->data.isLump && isPath)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Wad::isDirectory(const string& path)
{
	int index;
	int stringStart;
	string pathSub;
	Node* curr = fileSys;
	bool isPath;
	bool lastSub = false;

	stringStart = 1;
	if (path.find("/") != string::npos)
	{
		isPath = true;
		if (path.size() == 1)
		{
			return true;
		}
	}
	else
	{
		return false;
	}
	while (isPath)
	{
		isPath = false;
		if (path.find("/", stringStart) == string::npos && stringStart != path.size())
		{
			lastSub = true;
			pathSub = path.substr(stringStart, path.size() - stringStart);
		}
		else
		{
			index = path.find("/", stringStart);
			pathSub = path.substr(stringStart, index - stringStart);
			if (index == path.size() - 1)
			{
				lastSub = true;
			}
		}
		if (curr->data.name == pathSub)
		{
			stringStart = index + 1;
			isPath = true;
			if (!lastSub)
			{
				continue;
			}
			else
			{
				break;
			}
		}
		else
		{
			for (int i = 0; i < curr->children.size(); i++)
			{
				if (pathSub == curr->children[i]->data.name)
				{
					curr = curr->children[i];
					isPath = true;
					break;
				}
			}
			if (!isPath)
			{
				return false;
			}
			else if (!lastSub)
			{
				stringStart = index + 1;
				continue;
			}
			else
			{
				break;
			}
		}
	}
	if (isPath && !curr->data.isLump)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int Wad::getSize(const string& path)
{
	int index;
	int startString = 1;
	bool foundIt = false;
	string strName;
	Node* curr = fileSys;

	if (isContent(path))
	{
		while (path.find("/", startString) != string::npos)
		{
			index = path.find("/", startString);
			strName = path.substr(startString, index - startString);
			startString = index + 1;
			if (curr->data.name == strName)
			{
				if (path.find("/", startString) == string::npos)
				{
					strName = path.substr(startString, path.size() - startString);
					for (unsigned int i = 0; i < curr->children.size(); i++)
					{
						if (curr->children[i]->data.name == strName)
							return curr->children[i]->data.length;
					}
				}
			}
			else
			{
				for (int i = 0; i < curr->children.size(); i++)
				{
					if (curr->children[i]->data.name == strName)
					{
						curr = curr->children[i];
						break;
					}
				}
			}
		}
		strName = path.substr(startString, path.size() - startString);
		for (int i = 0; i < curr->children.size(); i++)
		{
			if (curr->children[i]->data.name == strName)
			{
				return curr->children[i]->data.length;
			}
		}
	}
	else
	{
		return -1;
	}
}

int Wad::getContents(const string& path, char* buffer, int length, int offset)
{
	int index;
	int startString = 1;
	bool foundIt = false;
	string strName;
	Node* curr = fileSys;
	string subStr;

	if (isContent(path))
	{
		while (path.find("/", startString) != string::npos)
		{
			index = path.find("/", startString);
			strName = path.substr(startString, index - startString);
			startString = index + 1;
			if (curr->data.name == strName)
			{
				if (path.find("/", startString) == string::npos)
				{
					strName = path.substr(startString, path.size() - startString);
					for (unsigned int i = 0; i < curr->children.size(); i++)
					{
						if (curr->children[i]->data.name == strName)
						{
							curr = curr->children[i];
							if (length >= curr->children[i]->data.length)
							{
								buffer = &curr->data.content[0];
								return curr->data.length;
							}
							else
							{
								subStr = curr->data.content.substr(offset, length);
								buffer = &subStr[0];
								return length;
							}
						}
					}
				}
			}
			else
			{
				for (int i = 0; i < curr->children.size(); i++)
				{
					if (curr->children[i]->data.name == strName)
					{
						curr = curr->children[i];
					}
				}
			}
		}
	}
	else
	{
		return -1;
	}
}

int Wad::getDirectory(const string& path, vector<string>* directory)
{
	int startStr = 1;
	Node* curr = fileSys;
	int index;
	string pathName;
	string childName;

	if (isDirectory(path))
	{
		if (path.size() == 1)
		{
			for (int i = 0; i < curr->children.size(); i++)
			{
				childName = curr->children[i]->data.name;
				directory->push_back(childName);
			}
			return curr->children.size();
		}
		//Do stuff
		while (path.find("/", startStr) != string::npos)
		{
			index = path.find("/", startStr);
			pathName = path.substr(startStr, index - startStr);
			startStr = index + 1;
			if (pathName == curr->data.name)
			{
				if (path.find("/", startStr) == path.size() - 1 || path.find("/", startStr) == string::npos) //If next is last directory
				{
					pathName = path.substr(startStr, path.size() - startStr - 1);
					for (int i = 0; i < curr->children.size(); i++)
					{
						if (pathName == curr->children[i]->data.name)
						{
							curr = curr->children[i];
							for (int j = 0; j < curr->children.size(); j++)
							{
								childName = curr->children[i]->data.name;
								directory->push_back(childName);
							}
							return curr->children.size();
						}
					}
				}
			}
			else //If path is in children
			{
				if (path.find("/", startStr) == string::npos) //If next is last directory
				{
					for (int i = 0; i < curr->children.size(); i++)
					{
						if (pathName == curr->children[i]->data.name)
						{
							curr = curr->children[i];
							for (int j = 0; j < curr->children.size(); j++)
							{
								childName = curr->children[j]->data.name;
								directory->push_back(childName);
							}
							return curr->children.size();
						}
					}
				}
				else
				{
					for (int i = 0; i < curr->children.size(); i++)
					{
						if (pathName == curr->children[i]->data.name)
						{
							curr = curr->children[i];
							break;
						}
					}
				}
			}
		}
	}
	else
	{
		//It's not a directory so fails
		return -1;
	}
}