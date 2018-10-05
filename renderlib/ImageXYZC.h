#pragma once

#include "Histogram.h"

#include "glm.h"

#include <QThread>

#include <inttypes.h>
#include <vector>

struct Channelu16
{
	Channelu16(uint32_t x, uint32_t y, uint32_t z, uint16_t* ptr);
	~Channelu16();

	uint32_t _x, _y, _z;

	uint16_t* _ptr;
	uint16_t _min;
	uint16_t _max;

	uint16_t* _gradientMagnitudePtr;
	//uint16_t _gradientMagnitudeMin;
	//uint16_t _gradientMagnitudeMax;

	Histogram _histogram;
	float* _lut;

	uint16_t* generateGradientMagnitudeVolume(float scalex, float scaley, float scalez);

	void generate_windowLevel(float window, float level) { delete[] _lut;  _lut = _histogram.generate_windowLevel(window, level); _window = window; _level = level; }
	void generate_auto2(float& window, float& level) { delete[] _lut;  _lut = _histogram.generate_auto2(window, level); _window = window; _level = level; }
	void generate_auto(float& window, float& level) { delete[] _lut;  _lut = _histogram.generate_auto(window, level); _window = window; _level = level; }
	void generate_bestFit(float& window, float& level) { delete[] _lut;  _lut = _histogram.generate_bestFit(window, level); _window = window; _level = level; }
	void generate_chimerax() { delete[] _lut;  _lut = _histogram.initialize_thresholds(); }
	void generate_equalized() { delete[] _lut;  _lut = _histogram.generate_equalized(); }

	void debugprint();

	QString _name;

	// convenience.  may not be accurate if LUT input is generalized.
	float _window, _level;
};

class ImageXYZC
{
public:
	ImageXYZC(uint32_t x, uint32_t y, uint32_t z, uint32_t c, uint32_t bpp, uint8_t* data=nullptr, float sx=1.0, float sy=1.0, float sz=1.0);
	virtual ~ImageXYZC();

	void setPhysicalSize(float x, float y, float z);

	uint32_t sizeX() const;
	uint32_t sizeY() const;
	uint32_t sizeZ() const;
	uint32_t maxPixelDimension() const;
	float physicalSizeX() const;
	float physicalSizeY() const;
	float physicalSizeZ() const;

	glm::vec3 getDimensions() const;

	uint32_t sizeC() const;

	uint32_t sizeOfElement() const;
	size_t sizeOfPlane() const;
	size_t sizeOfChannel() const;
	size_t size() const;

	uint8_t* ptr(uint32_t channel=0, uint32_t z=0) const;
	Channelu16* channel(uint32_t channel) const;

	// if channel color is 0, then channel will not contribute.
	// allocates memory for outRGBVolume and outGradientVolume
	void fuse(const std::vector<glm::vec3>& colorsPerChannel, uint8_t** outRGBVolume, uint16_t** outGradientVolume) const;

	void setChannelNames(std::vector<QString>& channelNames);
private:
	uint32_t _x, _y, _z, _c, _bpp;
	uint8_t* _data;
	float _scaleX, _scaleY, _scaleZ;
	std::vector<Channelu16*> _channels;
};


class FuseWorkerThread : public QThread
{
	Q_OBJECT
public:
	// count is how many elements to walk for input and output.
	FuseWorkerThread(size_t thread_idx, size_t nthreads, uint8_t* outptr, const ImageXYZC* img, const std::vector<glm::vec3>& colors);
	void run() override;
private:
	size_t _thread_idx;
	size_t _nthreads;
	uint8_t* _outptr;

	// read only!
	const ImageXYZC* _img;
	const std::vector<glm::vec3>& _channelColors;
signals:
	void resultReady(size_t threadidx);
};
