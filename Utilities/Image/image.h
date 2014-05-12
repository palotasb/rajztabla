/*
 * image.h
 *
 *  Created on: May 7, 2014
 *      Author: palotasb
 */

#ifndef IMAGE_H_
#define IMAGE_H_

typedef struct Coord {
    int16_t x;
    int16_t y;
} Coord;

typedef struct Image Image;
struct Image {
    Coord size;
    /**
     * Returns the value of a pixel at the specified coordinate.
     *
     * @param image: the image
     * @param coord: the coordinate
     * @return 0 or 1, the value of the pixel
     */
    uint8_t (*getPixel)(Image *, Coord);
    /**
     * Returns a byte of pixels (1-by-8 size) as one byte starting at the specified coordinate.
     *
     * @param image: the image.
     * @param coord: the pixel coordinate of the first pixel.
     * @return the value of the pixels in a byte
     */
    uint8_t (*getPixelByte)(Image * image, Coord coord);
};

typedef struct ImageData ImageData;
struct ImageData {
    Coord size;
    uint8_t (*getPixel)(ImageData *, Coord);
    uint8_t (*getPixelByte)(ImageData *, Coord);
    uint8_t * data;
};

typedef struct VirtualImage VirtualImage;
struct VirtualImage {
    Coord size;
    uint8_t (*getPixel)(Image *, Coord);
    uint8_t (*getPixelByte)(Image *, Coord);
    Image * imageA;
    Image * imageB;
    Coord offsetA;
    Coord offsetB;
    enum Combinator {
        VIC_NEGATE_A = 0x01,
        VIC_NEGATE_B = 0x02,
        VIC_NEGATE_RESULT = 0x04,
        VIC_AND = 0x10,
        VIC_OR = 0x20,
        VIC_XOR = 0x30,
        VIC_EQUAL = 0x40,
        VIC_NAND = 0x50,
        VIC_NOR = 0x60,
        VIC_JUST_A = 0x70,
        VIC_JUST_B = 0x80,
        VIC_CUSTOM_FUNC = 0xff
    } combinator;
    uint8_t (*func)(VirtualImage*, Coord coord);
};

uint8_t Image_Empty_zeroFunction(Image*, Coord);

Image const Image_Empty;

Coord coord(int16_t x, int16_t y);

uint32_t ImageData_GetBufferIndexFromPixelCoord(ImageData* imageData,
        Coord pixelCoord);
uint8_t ImageData_BufferBitMaskFromPixelCoord(Coord pixelCoord);
uint8_t ImageData_BufferIndexMaskFromPixelCoord(Coord pixelCoord);

void ImageData_Init(ImageData* imageData, Coord size, uint8_t* data);
void ImageData_SetPixel(ImageData* imageData, Coord pixelCoord, uint8_t data);
uint8_t ImageData_GetPixel(ImageData* imageData, Coord pixelCoord);
void ImageData_SetByte(ImageData* imageData, Coord byteCoord, uint8_t data);
uint8_t ImageData_GetByte(ImageData* imageData, Coord byteCoord);

void VirtualImage_Init(VirtualImage* virtualImage, Coord size, Image* imageA,
        Image* imageB, Coord offsetA, Coord offsetB);
uint8_t VirtualImage_GetPixel(Image* virtualImage, Coord pixelCoord);
uint8_t VirtualImage_GetByte(Image* virtualImage, Coord byteCoord);

void Image_DisplayOnLCD(Image* image, Coord xy1, Coord xy2);

#endif /* IMAGE_H_ */
