/*
 * image.c
 *
 *  Created on: May 7, 2014
 *      Author: palotasb
 */

#include "glcd.h"
#include "image_font.h"
#include "image.h"

Image const Image_Empty = { .size.x = 0, .size.y = 0, .getPixel =
        Image_Empty_zeroFunction, .getPixelByte = Image_Empty_zeroFunction };

/**
 * Calculates the buffer offset for a pixel in an ImageData structure with a
 * specified size.
 *
 * @param imageData: the structure
 * @param pixelCoord: the coordinate
 * @return: the buffer index
 */
inline uint32_t ImageData_GetBufferIndexFromPixelCoord(ImageData* imageData,
        Coord pixelCoord) {
    return pixelCoord.y / 8 * imageData->size.x + pixelCoord.x;
}

/**
 * Returns the bit mask for a specific coordinate in an image.
 *
 * @param imageData: the image
 * @param pixelCoord: the coordinate
 * @return: the bit mask
 */
inline uint8_t ImageData_BufferBitMaskFromPixelCoord(Coord pixelCoord) {
    return 0x01 << (pixelCoord.y % 8);
}

/**
 * Returns the bit index for a specific coordinate in an image.
 * @param imageData: the image
 * @param pixelCoord: the coordinate
 * @return: the bit index (0-7)
 */
inline uint8_t ImageData_BufferBitIndexFromPixelCoord(Coord pixelCoord) {
    return pixelCoord.y % 8;
}

/**
 * Creates a Coord structure
 * @param x: x coordinate or width
 * @param y: y coordinate or height
 * @return: the Coord struct
 */
inline Coord coord(int16_t x, int16_t y) {
    Coord c = { .x = x, .y = y };
    return c;
}

/**
 * Initializes an ImageData structure.
 *
 * @param imageData: the structure.
 * @param size: the size of the image in pixels
 * @param data: pointer to an array where each bit can represent a pixel
 */
void ImageData_Init(ImageData* imageData, Coord size, uint8_t* data) {
    imageData->size = size;
    imageData->data = data;
    imageData->getPixel = ImageData_GetPixel;
    imageData->getPixelByte = ImageData_GetByte;
}

/**
 * Sets a pixel of an ImageData structure to a specific value.
 *
 * @param imageData: the structure containing the image data and the pixel to set.
 * @param pixelCoord: the coordinate of the pixel.
 * @param data: value of 0 or 1.
 */
void ImageData_SetPixel(ImageData* imageData, Coord pixelCoord, uint8_t data) {
    if (imageData->size.x < pixelCoord.x || imageData->size.y < pixelCoord.y
            || pixelCoord.x < 0 || pixelCoord.y < 0)
        return;
    if (data) {
        imageData->data[ImageData_GetBufferIndexFromPixelCoord(imageData,
                pixelCoord)] |= ImageData_BufferBitMaskFromPixelCoord(
                pixelCoord);
    } else {
        imageData->data[ImageData_GetBufferIndexFromPixelCoord(imageData,
                pixelCoord)] &= ~ImageData_BufferBitMaskFromPixelCoord(
                pixelCoord);
    }
}

/**
 * Returns the pixel value of an ImageData structure at a specified coordinate.
 *
 * @param imageData: the object
 * @param pixelCoord: the coordinate
 * @return: 0 or 1, the pixel value
 */
uint8_t ImageData_GetPixel(ImageData* imageData, Coord pixelCoord) {
    if (imageData->size.x < pixelCoord.x || imageData->size.y < pixelCoord.y
            || pixelCoord.x < 0 || pixelCoord.y < 0)
        return 0;
    return imageData->data[ImageData_GetBufferIndexFromPixelCoord(imageData,
            pixelCoord)] & ImageData_BufferBitMaskFromPixelCoord(pixelCoord) ?
            1 : 0;
}

/**
 *  Sets a byte of pixels (1-by-8 size) the value specified.
 *
 * @param imageData: the image whose data is to be set.
 * @param byteCoord: the pixel coordinate of the byte
 * @param data: the data
 */
void ImageData_SetByte(ImageData* imageData, Coord byteCoord, uint8_t data) {
    if (imageData->size.x < byteCoord.x || imageData->size.y < byteCoord.y
            || byteCoord.x < 0 || byteCoord.y < 0)
        return;
    uint8_t bit0index = ImageData_BufferBitIndexFromPixelCoord(byteCoord);
    uint32_t bufferIndex = ImageData_GetBufferIndexFromPixelCoord(imageData,
            byteCoord);
    imageData->data[bufferIndex] &= 0xff >> (8 - bit0index);
    imageData->data[bufferIndex] |= data << bit0index;

    if (bit0index) {
        // If the bit index is not zero, then the 8 bits are from two different buffers.

        // Ignore second buffer if out of range.
        if (imageData->size.y < byteCoord.y + 7)
            return;

        bufferIndex = ImageData_GetBufferIndexFromPixelCoord(imageData,
                coord(byteCoord.x, byteCoord.y + 7));

        imageData->data[bufferIndex] &= 0xff << bit0index;
        imageData->data[bufferIndex] |= data >> (8 - bit0index);
    }
}

/**
 * Returns a byte of pixels (1-by-8 size) as one byte starting at the specified coordinate.
 *
 * @param imageData: the image
 * @param byteCoord: the pixel coordinate.
 * @return
 */
uint8_t ImageData_GetByte(ImageData* imageData, Coord byteCoord) {
    if (imageData->size.x < byteCoord.x || imageData->size.y < byteCoord.y
            || byteCoord.x < 0 || byteCoord.y < 0)
        return 0;
    uint8_t ret = imageData->data[ImageData_GetBufferIndexFromPixelCoord(
            imageData, byteCoord)];
    uint8_t bit0index = ImageData_BufferBitIndexFromPixelCoord(byteCoord);
    if (bit0index) {
        // If the bit index is not zero, then the 8 bits are from two different buffers.

        // 1. shift lower bytes from the first buffer into place
        ret >>= bit0index;

        // If the second buffer would be out of range, this makes it like 0
        if (imageData->size.y < byteCoord.y + 7)
            return ret;

        // 2. get the second 8 bytes from the second buffer
        // 3. shift into place by shifting them  up (8 - bit0index)
        // 4. clear the lower bit0index number of bits (& 0xff>>bit0index)
        // 5. combine the lower and upper parts by OR-ing them
        ret |= (imageData->data[ImageData_GetBufferIndexFromPixelCoord(
                imageData, coord(byteCoord.x, byteCoord.y + 7))]
                << (8 - bit0index)) & (0xff >> bit0index);
    }
    return ret;
}

/**
 * Initializes a virtual image.
 *
 * @param virtualImage: the struct to initialize
 * @param size: the size of the virtual image canvas
 * @param imageA: the first image (by default: background) composing the virtual image
 * @param imageB: the second image (by default: overlay) composing the virtual image
 * @param offsetA: the offset of the first image from the (0,0) point of the virtual image
 * @param offsetB: the offset of the second image from the (0,0) point of the virtual image
 */
void VirtualImage_Init(VirtualImage* virtualImage, Coord size, Image* imageA,
        Image* imageB, Coord offsetA, Coord offsetB) {
    virtualImage->combinator = VIC_JUST_B;
    virtualImage->func = 0;
    virtualImage->getPixel = VirtualImage_GetPixel;
    virtualImage->getPixelByte = VirtualImage_GetByte;
    virtualImage->imageA = imageA;
    virtualImage->imageB = imageB;
    virtualImage->offsetA = offsetA;
    virtualImage->offsetB = offsetB;
    virtualImage->size = size;
}

/**
 * Returns the pixel value of the virtual image at a specified coordinate.
 *
 * @param virtualImage: the image
 * @param pixelCoord: the pixel coordinate
 * @return: the pixel value, 0 or 1;
 */
uint8_t VirtualImage_GetPixel(Image* virtualImage, Coord pixelCoord) {
    VirtualImage* vi = (VirtualImage*) virtualImage;
    Coord ca = pixelCoord, cb = pixelCoord;
    uint8_t a, b, res, combinator;
    ca.x += vi->offsetA.x;
    ca.y += vi->offsetA.y;
    cb.x += vi->offsetB.x;
    cb.y += vi->offsetB.y;

    a = vi->imageA->getPixel(vi->imageA, ca);
    if (vi->combinator & VIC_NEGATE_A)
        a = ~a;
    b = vi->imageB->getPixel(vi->imageB, cb);
    if (vi->combinator & VIC_NEGATE_B)
        b = ~b;

    combinator = vi->combinator & 0xf0;
    switch (combinator) {
    case VIC_AND:
        res = a & b;
        break;
    case VIC_OR:
        res = a | b;
        break;
    case VIC_XOR:
        res = a ^ b;
        break;
    case VIC_EQUAL:
        res = ~(a ^ b);
        break;
    case VIC_JUST_A:
        if (0 <= ca.x && ca.x < vi->imageA->size.x && 0 <= ca.y
                && ca.y < vi->imageA->size.y)
            res = a;
        else
            res = b;
        break;
    case VIC_JUST_B:
        if (0 <= cb.x && cb.x < vi->imageB->size.x && 0 <= cb.y
                && cb.y < vi->imageB->size.y)
            res = b;
        else
            res = a;
        break;
    case VIC_CUSTOM_FUNC:
        if (vi->func)
            res = vi->func(vi, pixelCoord);
        break;
    default:
        res = 0;
    }
    if (vi->combinator & VIC_NEGATE_RESULT)
        res = ~res;

    // The lowest bit carries the value, the rest is just overhead.
    return res & 0x01;
}

/**
 * Returns a byte of pixels (1-by-8 size) as one byte starting at the specified coordinate.
 *
 * @param virtualImage: the virtual image
 * @param pixelCoord: the pixel coordinate
 * @return: the byte
 */
uint8_t VirtualImage_GetByte(Image* virtualImage, Coord pixelCoord) {
    VirtualImage* vi = (VirtualImage*) virtualImage;
    Coord ca = pixelCoord, cb = pixelCoord;
    uint8_t a, b, res, combinator;
    ca.x += vi->offsetA.x;
    ca.y += vi->offsetA.y;
    cb.x += vi->offsetB.x;
    cb.y += vi->offsetB.y;

    a = vi->imageA->getPixelByte(vi->imageA, ca);
    if (vi->combinator & VIC_NEGATE_A)
        a = ~a;
    b = vi->imageB->getPixelByte(vi->imageB, cb);
    if (vi->combinator & VIC_NEGATE_B)
        b = ~b;

    combinator = vi->combinator & 0xf0;
    switch (combinator) {
    case VIC_AND:
        res = a & b;
        break;
    case VIC_OR:
        res = a | b;
        break;
    case VIC_XOR:
        res = a ^ b;
        break;
    case VIC_EQUAL:
        res = ~(a ^ b);
        break;
    case VIC_JUST_A:
        if (0 <= ca.x && ca.x < vi->imageA->size.x && 0 <= ca.y
                && ca.y < vi->imageA->size.y)
            res = a;
        else
            res = b;
        break;
    case VIC_JUST_B:
        if (0 <= cb.x && cb.x < vi->imageB->size.x && 0 <= cb.y
                && cb.y < vi->imageB->size.y)
            res = b;
        else
            res = a;
        break;
    case VIC_CUSTOM_FUNC:
        if (vi->func)
            res = vi->func(vi, pixelCoord);
        break;
    default:
        res = 0;
    }
    if (vi->combinator & VIC_NEGATE_RESULT)
        res = ~res;

    return res;
}

/**
 * Helper function for using an empty image.
 *
 * @param i
 * @param c
 * @return
 */
inline uint8_t Image_Empty_zeroFunction(Image* i, Coord c) {
    return 0;
}

/**
 * Displays an image on the 128x64 LCD.
 *
 * @param image: the image to display
 * @param xy1: the upper left coordinate of the pixels to display
 * @param xy2: the lower right coordinate of the pixels to display
 */
void Image_DisplayOnLCD(Image* image, Coord xy1, Coord xy2) {
    uint8_t x, y;
    for (y = xy1.y / 8; y <= xy2.y / 8; y++)
        for (x = xy1.x; x <= xy2.x; x++) {
            GLCD_Write_Block(image->getPixelByte(image, coord(x, y * 8)), y, x);
        }
}
