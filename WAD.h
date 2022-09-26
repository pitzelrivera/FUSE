#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <list>
#include "Node.h"
using std::string;
using std::vector;
using std::ifstream;
using std::map;
using std::list;
using std::ios;

class Wad
{
	string magic;
	int descriptNum;
	int descriptOffset;
	Node* fileSys = nullptr;
public:
	static Wad* loadWad(const string& path);
	string getMagic();
	bool isContent(const string& path);
	bool isDirectory(const string& path);
	int getSize(const string& path);
	int getContents(const string& path, char* buffer, int length, int offset = 0);
	int getDirectory(const string& path, vector<string>* directory);
};