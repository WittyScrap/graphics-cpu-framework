#include "MD2Loader.h"

// File reading
#include <iostream>
#include <fstream>

using namespace std;

// BYTE added in case Windows.h is not included.
typedef unsigned char BYTE;

// Magic number for MD2 files  "IDP2" or 844121161
const int MD2_IDENT = (('2' << 24) + ('P' << 16) + ('D' << 8) + 'I');

// MS2 version
const int MD2_VERSION = 8;

struct Md2Header
{
	int indent;               // The magic number used to identify the file.
	int version;              // The file version number (must be 8).
	int skinWidth;            // The width in pixels of our image.
	int skinHeight;           // The height in pixels of our image.
	int frameSize;            // The size in bytes the frames are.
	int numSkins;             // The number of skins associated with the model.
	int numVertices;		  // The number of vertices.
	int numTexCoords;		  // The number of texture coordinates.
	int numTriangles;		  // The number of faces (polygons).
	int numGlCommands;        // The number of gl commands.
	int numFrames;			  // The number of animated frames.
	int offsetSkins;		  // The offset in the file for the skin data.
	int offsetTexCoords;	  // The offset in the file for the texture data.
	int offsetTriangles;	  // The offset in the file for the face data.
	int offsetFrames;		  // The offset in the file for the frames data.
	int offsetGlCommands;	  // The offset in the file for the gl commands data.
	int offsetEnd;            // The end of the file offset.
};

struct Md2Triangle
{
	short vertexIndex[3];   // Vertex indices of the triangle
	short uvIndex[3];       // Texture coordinate indices 
};

struct Md2Vertex
{
	BYTE v[3];                // Compressed vertex (x, y, z) coordinates
	BYTE lightNormalIndex;    // Index to a normal vector for the lighting
};

// Texture co-ordinates
struct Md2TextureCoord
{
	short textureCoord[2];
};


struct Md2Frame
{
	float       scale[3];       // Scale values
	float       translate[3];   // Translation vector
	char        name[16];       // Frame name
	Md2Vertex   verts[1];       // First vertex of this frame
};

struct PcxHeader
{
	BYTE  ID;
	BYTE  Version;
	BYTE  Encoding;
	BYTE  BitsPerPixel;
	short XMin;
	short YMin;
	short XMax;
	short YMax;
	short HRes;
	short VRes;
	BYTE  ClrMap[16 * 3];
	BYTE  Reserved;
	BYTE  NumPlanes;
	short BytesPerLine;
	short Pal;
	BYTE  Filler[58];
};


MD2Loader::MD2Loader()
{
}

MD2Loader::~MD2Loader()
{
}

bool LoadPCX(const char* textureFilename, Texture& texture, const Md2Header* md2Header)
{
	std::ifstream   file;

	BYTE* paletteIndices = texture.GetPaletteIndices();
	COLORREF* palette = texture.GetPalette();

	// Try to open file
	file.open(textureFilename, std::ios::in | std::ios::binary);
	if (file.fail())
	{
		return false;
	}
	// Read PCX header
	PcxHeader header;
	file.read(reinterpret_cast<char*>(&header), sizeof(PcxHeader));

	// Verify that this is a valid PCX file

	// We only handle those with 256 colour palette
	if ((header.Version != 5) || (header.BitsPerPixel != 8) ||
		(header.Encoding != 1) || (header.NumPlanes != 1) ||
		(md2Header && (header.BytesPerLine != md2Header->skinWidth)))
	{
		// This is not valid supported PCX
		file.close();
		return false;
	}

	//	Check dimensions

	int xSize = header.XMax - header.XMin + 1;
	int ySize = header.YMax - header.YMin + 1;
	int size = xSize * ySize;

	// Check that this matches our MD2 expected texture
	// Note. valid size is <= because uses RLE (so potentially smaller)
	if (md2Header && (size > (md2Header->skinHeight * md2Header->skinWidth)))
	{
		// Doesn't match expected MD2 skin size
		file.close();
		return false;
	}

	// Reading file data

	BYTE processByte, colourByte;
	int count = 0;
	while (count < size)
	{
		file.read(reinterpret_cast<char*>(&processByte), 1);

		// Run length encoding - test if byte is an RLE byte
		if ((processByte & 192) == 192)
		{
			// Extract number of times repeated byte
			processByte &= 63;
			file.read(reinterpret_cast<char*>(&colourByte), 1);
			for (int index = 0; index < processByte; ++index)
			{
				// repeatedly write colour 
				paletteIndices[count] = colourByte;
				++count;
			}
		}
		else
		{
			// Byte is the colour
			paletteIndices[count] = processByte;
			++count;
		}
	}

	bool returnValue = false;

	// read palette data...
	file.seekg(-769, std::ios::end);	// This offset from end of file
	file.read(reinterpret_cast<char*>(&processByte), 1);
	if (processByte == 12)
	{
		BYTE rawPalette[768];
		file.read(reinterpret_cast<char*>(&rawPalette), 768);

		// Build palette
		for (int palIndex = 0; palIndex < 256; ++palIndex)
		{
			palette[palIndex] = RGB(rawPalette[palIndex * 3],
				rawPalette[(palIndex * 3) + 1],
				rawPalette[(palIndex * 3) + 2]);
		}
		returnValue = true;
	}

	file.close();
	return returnValue;
}

// Load model from file.

bool MD2Loader::LoadModel(const char* md2Filename, const char* textureFilename, Mesh& model, AddPolygon addPolygon, AddVertex addVertex, AddTextureUV addTextureUV)
{
	ifstream   file;
	Md2Header header;
	bool bHasTexture = false;

	// Try to open MD2 file
	file.open(md2Filename, ios::in | ios::binary);
	if (file.fail())
	{
		return false;
	}
	// Read file header
	file.read(reinterpret_cast<char*>(&header), sizeof(Md2Header));

	// Verify that this is a MD2 file (check for the magic number and version number)
	if ((header.indent != MD2_IDENT) && (header.version != MD2_VERSION))
	{
		// This is not a MD2 model
		file.close();
		return false;
	}

	// Allocate the memory we need
	Md2Triangle* triangles = new Md2Triangle[header.numTriangles];
	// We are only interested in the first frame 
	BYTE* frameBuffer = new BYTE[header.frameSize];
	Md2Frame* frame = reinterpret_cast<Md2Frame*>(frameBuffer);
	Md2TextureCoord* textureCoords = new Md2TextureCoord[header.numTexCoords];

	// Read polygon data...
	file.seekg(header.offsetTriangles, ios::beg);
	file.read(reinterpret_cast<char*>(triangles), sizeof(Md2Triangle)* header.numTriangles);

	// Read frame data...
	file.seekg(header.offsetFrames, ios::beg);
	file.read(reinterpret_cast<char*>(frame), header.frameSize);

	// Read texture coordinate data
	file.seekg(header.offsetTexCoords, std::ios::beg);
	file.read(reinterpret_cast<char*>(textureCoords), sizeof(Md2TextureCoord)* header.numTexCoords);

	// Close the file 
	file.close();

	// Attempt to load any texture
	if (textureFilename)
	{
		model.GetTexture().SetTextureSize(header.skinWidth, header.skinHeight);
		bHasTexture = LoadPCX(textureFilename, model.GetTexture(), &header);
	}

	// Polygon array initialization
	for (int i = 0; i < header.numTriangles; ++i)
	{
		// Call supplied member function to add a new polygon to the list
		std::invoke(addPolygon, model,
			triangles[i].vertexIndex[0], triangles[i].vertexIndex[1], triangles[i].vertexIndex[2],
			triangles[i].uvIndex[0], triangles[i].uvIndex[1], triangles[i].uvIndex[2]);
	}

	// Vertex array initialization
	for (int i = 0; i < header.numVertices; ++i)
	{
		// The following are the expressions needed to access each of the co-ordinates.
		// 
		// X co-ordinate:   frame->verts[i].v[0] * frame->scale[0] + frame->translate[0]
		// Y co-ordinate:   frame->verts[i].v[2] * frame->scale[2] + frame->translate[2]
		// Z co-ordinate:   frame->verts[i].v[1] * frame->scale[1] + frame->translate[1]
		//
		// NOTE: We have to swap Y and Z over because Z is up in MD2 and we have Y as up-axis
		std::invoke(addVertex, model,
			static_cast<float>((frame->verts[i].v[0] * frame->scale[0]) + frame->translate[0]),
			static_cast<float>((frame->verts[i].v[2] * frame->scale[2]) + frame->translate[2]),
			static_cast<float>((frame->verts[i].v[1] * frame->scale[1]) + frame->translate[1]));
	}
	// Texture coordinates initialisation
	if (bHasTexture)
	{
		for (int i = 0; i < header.numTexCoords; i++)
		{
			std::invoke(addTextureUV, model, textureCoords[i].textureCoord[0], textureCoords[i].textureCoord[1]);
		}
	}
	// Free dynamically allocated memory
	delete[] triangles; // NOTE: this is 'array' delete. Must be sure to use this
	triangles = 0;

	delete[] frameBuffer;
	frameBuffer = 0;
	frame = 0;

	delete[] textureCoords;
	textureCoords = 0;

	return true;
}