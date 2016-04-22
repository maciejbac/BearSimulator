
#include "stdafx.h"
#include "texture_loader.h"
#include "FreeImagePlus.h"
#include <wincodec.h>
#include <iostream>


using namespace std;

HRESULT initCOM(void) {

	return CoInitialize(NULL);
}


void shutdownCOM(void) {

	CoUninitialize();
}



#ifdef __CG_USE_WINDOWS_IMAGING_COMPONENT___

#pragma region WIC texture loader

static IWICImagingFactory			*wicFactory = NULL;


template <class T>
inline void SafeRelease(T **comInterface) {

	if (*comInterface) {

        (*comInterface)->Release();
        *comInterface = NULL;
    }
}


HRESULT getWICFormatConverter(IWICFormatConverter **formatConverter) {
	
	if (!formatConverter || !wicFactory)
		return E_FAIL;
	else
		return wicFactory->CreateFormatConverter(formatConverter);
}



HRESULT loadWICBitmap(LPCWSTR path, IWICBitmap **bitmap) {

	if (!bitmap || !wicFactory)
		return E_FAIL;

	IWICBitmapDecoder *bitmapDecoder = NULL;
	IWICBitmapFrameDecode *imageFrame = NULL;
	IWICFormatConverter *formatConverter = NULL;
	
	*bitmap = NULL;

	HRESULT hr = wicFactory->CreateDecoderFromFilename(path, NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &bitmapDecoder);
	
	
	UINT numFrames = 0;

	if (SUCCEEDED(hr)) {

		hr = bitmapDecoder->GetFrameCount(&numFrames);
	}

	if (SUCCEEDED(hr) && numFrames>0) {

		hr = bitmapDecoder->GetFrame(0, &imageFrame);
	}

	if (SUCCEEDED(hr)) {

		hr = wicFactory->CreateFormatConverter(&formatConverter);
	}

	WICPixelFormatGUID pixelFormat;

	if (SUCCEEDED(hr)) {

		hr = imageFrame->GetPixelFormat(&pixelFormat);
	}

	BOOL canConvert = FALSE;

	if (SUCCEEDED(hr)) {

		hr = formatConverter->CanConvert(pixelFormat, GUID_WICPixelFormat32bppPBGRA, &canConvert);
	}

	if (SUCCEEDED(hr) && canConvert==TRUE) {
		
		hr = formatConverter->Initialize(
		imageFrame,						
		GUID_WICPixelFormat32bppPBGRA,	
		WICBitmapDitherTypeNone,		
		NULL,							
		0.f,							
		WICBitmapPaletteTypeCustom		
		);
	}

	if (SUCCEEDED(hr)) {

		hr = wicFactory->CreateBitmapFromSource(formatConverter, WICBitmapCacheOnDemand, bitmap);
	}


	SafeRelease(&formatConverter);
	SafeRelease(&imageFrame);
	SafeRelease(&bitmapDecoder);

	return hr;
}


GLuint wicLoadTexture(const std::wstring& filename) {

	HRESULT		hr;

	if (!wicFactory) {

		hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory));

		if (!SUCCEEDED(hr))
			return 0;
	}


	IWICBitmap			*textureBitmap = NULL;
	IWICBitmapLock		*lock = NULL;
	GLuint				newTexture = 0;

	hr = loadWICBitmap(filename.c_str(), &textureBitmap);

	UINT w = 0, h = 0;

	if (SUCCEEDED(hr))
		hr = textureBitmap->GetSize(&w, &h);

	WICRect rect = {0, 0, w, h};

	if (SUCCEEDED(hr))
		hr = textureBitmap->Lock(&rect, WICBitmapLockRead, &lock);

	UINT bufferSize = 0;
	BYTE *buffer = NULL;

	if (SUCCEEDED(hr))
		hr = lock->GetDataPointer(&bufferSize, &buffer);

	if (SUCCEEDED(hr)) {
		
		glGenTextures(1, &newTexture);
		glBindTexture(GL_TEXTURE_2D, newTexture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, buffer);
	}

	SafeRelease(&lock);
	SafeRelease(&textureBitmap);


	// Setup default texture properties
	if (newTexture) {

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	return newTexture;
}

#pragma endregion

#endif


#pragma region FreeImagePlus texture loader

GLuint fiLoadTexture(const char *filename) {

	BOOL				fiOkay = FALSE;
	GLuint				newTexture = 0;
	fipImage			I;

	fiOkay = I.load(filename);

	if (!fiOkay) {

		cout << "FreeImagePlus: Cannot open image file.\n";
		return 0;
	}

	fiOkay = I.flipVertical();
	fiOkay = I.convertTo24Bits();

	if (!fiOkay) {

		cout << "FreeImagePlus: Conversion to 24 bits successful.\n";
		return 0;
	}

	auto w = I.getWidth();
	auto h = I.getHeight();

	BYTE *buffer = I.accessPixels();

	if (!buffer) {

		cout << "FreeImagePlus: Cannot access bitmap data.\n";
		return 0;
	}

	glGenTextures(1, &newTexture);
	glBindTexture(GL_TEXTURE_2D, newTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, buffer);

	if (newTexture) {

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	return newTexture;
}

#pragma endregion

