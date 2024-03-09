#include "stdafx.h"
#include "TxtLoder.h"

TxtLoder::TxtLoder() :Models(nullptr)
{
}

TxtLoder::~TxtLoder()
{
	if (Models != nullptr)
	{
		delete Models;
		Models = nullptr;
	}
}

bool TxtLoder::LoadModel(const wchar_t* fileName)
{
	std::ifstream fin;
	fin.open(fileName);
	
	wchar_t currentDir[MAX_PATH];
	GetCurrentDirectoryW(MAX_PATH, currentDir);


	if (!fin.is_open()&&fin.fail())
	{
		return false;
	}
	
	//get���� ������ geline���� ������ ����ߴµ� get���� �ϳ��� ����
	wchar_t input = 0;
	fin.seekg(sizeof("Vertex Count:"), std::ios::cur);
	fin >> vertexCount;//���ؽ� ī����
	Models = new Vertex[vertexCount]; //������ŭ �Ҵ�
	
	char word[10] = {};
	while (!fin.eof()) //������
	{
		if (strcmp(word ,"Data:\0")==0) //Ư���ܾ ã�Ҵٸ� 
			break;
		fin >> word; //�ܾ�ã�����ؼ� �б�
	}

	for (int i = 0; i < vertexCount; ++i)
	{
		fin >> Models[i].position.x >> Models[i].position.y >> Models[i].position.z;
		fin >> Models[i].color.x >> Models[i].color.y >> Models[i].color.z;
		Models[i].color.w = 1.0f;
	}


	fin.close();

	return true;
}

TxtLoder::Vertex* TxtLoder::GetModel()
{
	return Models;
}

int TxtLoder::GetModelSize()
{
	return sizeof(Vertex)* vertexCount;
}

int TxtLoder::GetModelFaceCount()
{
	return vertexCount;
}



