#include "GfMeshLoader.h"

//  Public functions
int GfMeshLoader::load(std::string file)
{
	std::ifstream meshFile(file);
	if (!meshFile.good())
	{
		std::cerr << "ERROR : loading mesh : " << file << "\ncould not open file" << std::endl;
		return -1;
	}

	glm::vec3 vector3;
	glm::vec2 vector2;
	unsigned short attributesFlags = none;

	int errorCode = 0;
	unsigned int lineNumber = 0;
	std::string line;
	while (!meshFile.eof())
	{
		std::getline(meshFile, line);
		lineNumber++;

		if (line.size() == 0) continue;				//	empty line
		else if (line.find("#") == 0) continue;		//	comment line
		std::stringstream lineStream(line);			//	pack stream in string
		removeWhiteSpaceAndComment(line);			// remove white space
		
		if (line.find("vn") != std::string::npos)		//	vertex normal
		{
			attributesFlags |= hasNormals;
			lineStream.ignore(2);
			lineStream >> vector3.x;
			lineStream >> vector3.y;
			lineStream >> vector3.z;
			normales.insert(normales.end(), vector3);
		}
		else if (line.find("vt") != std::string::npos)	//	vertex texture coordinates
		{
			attributesFlags |= hasTextures;
			lineStream.ignore(2);
			lineStream >> vector2.x;
			lineStream >> vector2.y;
			texture.insert(texture.end(), vector2);
		}
		else if (line.find("vc") != std::string::npos)  //	vertex color
		{
			attributesFlags |= hasColors;
			lineStream.ignore(2);
			lineStream >> vector3.x;
			lineStream >> vector3.y;
			lineStream >> vector3.z;
			color.insert(color.end(), vector3);
		}
		else if (line.find("vw") != std::string::npos)	//	vertex weight
		{
			attributesFlags |= hasWeights;
		}
		else if (line.find('v') != std::string::npos)	//	vertex position
		{
			attributesFlags |= hasVertices;
			lineStream.ignore(1);
			lineStream >> vector3.x;
			lineStream >> vector3.y;
			lineStream >> vector3.z;
			vertices.insert(vertices.end(), vector3);
		}
		else if (line.find("f") != std::string::npos)	//	faces index
		{
			lineStream.ignore(2);
			int validLine = 0;
			std::vector<unsigned int> index;

			for (int i = 0; i < 3; i++)
			{
				std::string vertexIndexList;
				std::getline(lineStream, vertexIndexList, ' ');
				std::stringstream lineStream2(vertexIndexList);

				for (int j = 0; j < rankCount; j++)
				{
					vertexIndexList.clear();
					std::getline(lineStream2, vertexIndexList, '/');
					if (lineStream2.eof() && j!=4 && !validLine)
						validLine = 1;
					else if(attributesFlags&(1<<j))
						index.push_back(atoi(vertexIndexList.c_str()));
				}
			}

			//	analyse parsing result
			if (validLine == 0)		//	no parsing error in line
				faces.insert(faces.end(), index.begin(), index.end());
			else
			{
				if (errorCode == 0)	//	first parsing error : print error header
					std::cerr << "ERROR : loading mesh : " << file << "\nparsing error at lines : " << std::endl;
				std::cerr << "    line " << lineNumber << " : ";
				switch (validLine)
				{
					case 1:  std::cerr << "wrong number of argument per vertex" << std::endl;	break;
					default: std::cerr << "unknown error ! BSOD required !" << std::endl;		break;
				}
				errorCode = -2;
			}
		}
		else std::cerr << "useless line ? : " << lineNumber << std::endl;
	}
	if (errorCode)	//	parsing finish : print error log signature
		std::cerr << "refer to the documentation for help" << std::endl;

	std::cout << "file " << (errorCode ? "loaded but errors occur (see error log for details)":"succesfully loaded !!") << std::endl;
	std::cout << "  vertice position : " << vertices.size() << std::endl;
	std::cout << "  texture coord : " << texture.size() << std::endl;
	std::cout << "  vertice normals : " << normales.size() << std::endl;
	std::cout << "  vertice weight : " << weight.size() << std::endl;
	std::cout << "  vertice color : " << color.size() << std::endl;
	std::cout << "  faces count : " << faces.size() << std::endl;
	return errorCode;
}

int GfMeshLoader::pack(
	std::vector<glm::vec3>* _vertices,
	std::vector<glm::vec3>* _normales,
	std::vector<glm::vec3>* _color,
	std::vector<glm::vec2>* _texture,
	std::vector<glm::vec2>* _weight,
	std::vector<unsigned int>* _faces)
{
	return 0;
}
//

//	Private functions
void GfMeshLoader::removeWhiteSpaceAndComment(std::string& input)
{
	for (std::string::iterator it = input.begin(); it != input.end(); it++)
	{
		if (isspace(*it))							//	curent char is a whitespace char
		{
			*it = ' ';								//	replace it by a space
			if (*std::prev(it) == *it)				//	previous is also a space
				it = input.erase(std::prev(it));	//	erase previous char
		}
		else if (*it == '#')						//	erase all comment (the rest of the string)
			it = std::prev(input.erase(it,input.end()));
	}
}
//
