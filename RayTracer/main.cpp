#include "Scene.h"
#include <fstream>


#include <SDL.h>
#undef main

typedef int LONG;
typedef unsigned int DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;

#pragma pack(1)


typedef struct _WinNtBitmapHeader
{
	DWORD Size;            /* Size of this header in bytes */
	LONG  Width;           /* Image width in pixels */
	LONG  Height;          /* Image height in pixels */
	WORD  Planes;          /* Number of color planes */
	WORD  BitsPerPixel;    /* Number of bits per pixel */
	DWORD Compression;     /* Compression methods used */
	DWORD SizeOfBitmap;    /* Size of bitmap in bytes */
	LONG  HorzResolution;  /* Horizontal resolution in pixels per meter */
	LONG  VertResolution;  /* Vertical resolution in pixels per meter */
	DWORD ColorsUsed;      /* Number of colors in the image */
	DWORD ColorsImportant; /* Minimum number of important colors */
} WINNTBITMAPHEADER;

struct BitmapFileHeader {
	WORD magic; // always 0x42 0x4D
	DWORD size; // size of BMP file(bytes)
	WORD reserved;
	WORD reserved2;
	DWORD pixel_array_offset;
};
struct Pixel {
	Pixel() : r(0), g(0), b(0) {}
	Pixel(BYTE r0, BYTE g0, BYTE b0) : r(r0), g(g0), b(b0) {}
	BYTE r,g,b;
};

void SaveBitmap(Pixel* data, int width, int height) {
	std::ofstream file("out.bmp");
	if(!file.is_open()){
		printf("ERROR\n");
	}
	int pixel_array_size = 3 * width * height;

	BitmapFileHeader header;
	header.magic = 0x4D42;
	header.size = sizeof(BitmapFileHeader) + sizeof(WINNTBITMAPHEADER) + pixel_array_size;
	header.reserved = 0;
	header.reserved2 = 0;
	header.pixel_array_offset = sizeof(BitmapFileHeader) + sizeof(WINNTBITMAPHEADER);
	file.write((char*)&header, sizeof(BitmapFileHeader));


	WINNTBITMAPHEADER bitmap;
	bitmap.Size = sizeof(WINNTBITMAPHEADER);
	bitmap.Width = width;
	bitmap.Height = height;
	bitmap.Planes = 1;
	bitmap.BitsPerPixel = 24;
	bitmap.Compression = 0;
	bitmap.SizeOfBitmap = 0;
	bitmap.HorzResolution = 0;
	bitmap.VertResolution = 0;
	bitmap.ColorsUsed = 0;
	bitmap.ColorsImportant = 0;
	file.write((char*)&bitmap, sizeof(WINNTBITMAPHEADER));

	for(int y = 0; y < height; ++y) {
		for(int x = 0; x < width; ++x) {
			file.write((char*)&data[y * width + x], 3);
		}
	}

	file.close();
}

Uint32 get_pixel32( SDL_Surface *surface, int x, int y ) {
	//Convert the pixels to 32 bit
	Uint32 *pixels = (Uint32 *)surface->pixels; 
	//Get the requested pixel
	return pixels[ ( y * surface->w ) + x ];
} 
void put_pixel32( SDL_Surface *surface, int x, int y, Uint32 pixel ) {
	//Convert the pixels to 32 bit 
	Uint32 *pixels = (Uint32 *)surface->pixels;
	//Set the pixel
	pixels[ ( y * surface->w ) + x ] = pixel;
}





#define WIDTH 512
#define HEIGHT 512
class Camera {
public:
	float near;
	float width;
	float height;

	Pixel img_data[HEIGHT][WIDTH];

	glm::vec3 pos;
	glm::vec3 dir;

	void RenderScene(Scene& scene) {
		int xw = WIDTH / 2;
		int yh = HEIGHT / 2;
		glm::vec3 target = this->pos + this->dir * this->near;
		float multip = 1;
		for(int y = 0; y < HEIGHT; ++y) {
			for(int x = 0; x < WIDTH; ++x) {
				glm::vec3 ptarget = target + glm::vec3((x - xw) * multip, (y - yh) * multip, 0);
				glm::vec3 val = scene.TracePixel(this->pos, glm::normalize(ptarget - this->pos));
				ClampVec(val);
				Pixel p;
				p.r = glm::clamp((int)(val.x * 255.0f), 0, 255);
				p.g = glm::clamp((int)(val.y * 255.0f), 0, 255);
				p.b = glm::clamp((int)(val.z * 255.0f), 0, 255);
				this->img_data[y][x] = p;
			}
		}
	}
};


void DisplayWithSDL(Pixel* pixels, int width, int height) {
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* win = SDL_CreateWindow("Sample", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);
	SDL_Surface* surf = SDL_GetWindowSurface(win);

	SDL_LockSurface(surf);
	for(int y = 0; y < height; ++y) {
		for(int x = 0; x < width; ++x) {
			put_pixel32(surf, x, y, (pixels[y * width + x].r << 16) |
									(pixels[y * width + x].g << 8) |
									(pixels[y * width + x].b));
		}
	}
	SDL_UnlockSurface(surf);

	while(true) {
		SDL_Event e;
		SDL_PollEvent(&e);
		if(e.type == SDL_QUIT)
			break;

		SDL_UpdateWindowSurface( win );
	}
	SDL_Quit();
}



int main(int argc, char* argv[]) {
	Scene scene;
	Sphere sph(glm::vec3(0,0,0), 1.f);
	sph.color = glm::vec3(1.0, 1.0, 1.0);
	sph.emit = glm::vec3(0,0,0.82);

	Sphere sph2(glm::vec3(-3, 0, 0), 1);
	sph2.color = glm::vec3(0,0,1);
	sph2.emit = glm::vec3(1,0,0);

	Sphere sph3(glm::vec3(-2, -2, 0), 0.6);
	sph3.color = glm::vec3(1,1,1);
	sph3.emit = glm::vec3(0,1,0);

	Sphere sph4(glm::vec3(1.6, -2, -0.2), 0.3);
	sph4.color = glm::vec3(1,1,1);
	sph4.emit = glm::vec3(0.2,0.3,0.4);


	Sphere sph5(glm::vec3(0.8, 0.8, -2), 0.4);
	sph5.color = glm::vec3(1,1,1);
	sph5.emit = glm::vec3(0.2,0.3,0.4);

	scene.spheres.push_back(Sphere(sph));
	scene.spheres.push_back(Sphere(sph2));
	scene.spheres.push_back(Sphere(sph3));
	scene.spheres.push_back(Sphere(sph4));
	scene.spheres.push_back(Sphere(sph5));

	scene.directional = glm::normalize(glm::vec3(-1, -2, 2));
	scene.dir_color = glm::vec3(0.4f, 0.4f, 0.4f);
	

	Camera cam;
	cam.near = 280;
	cam.pos = glm::vec3(0.0f, 0, -3.f);
	cam.dir = glm::vec3(0.f, 0.f, 1.f);


	cam.RenderScene(scene);

	SaveBitmap(*cam.img_data, WIDTH, HEIGHT);
	DisplayWithSDL(*cam.img_data, WIDTH, HEIGHT);
	return 0;
}