#pragma once

#include <QOpenGLShaderProgram>

#include <glm.h>
#include "Camera.h"

/**
    */
class GLBasicVolumeShader : public QOpenGLShaderProgram
{

public:
    /**
    * Constructor.
    *
    * @param parent the parent of this object.
    */
    explicit GLBasicVolumeShader();

    /// Destructor.
    ~GLBasicVolumeShader();

    /// Enable vertex coordinates.
    void
    enableCoords();

    /// Disable vertex coordinates.
    void
    disableCoords();

    /**
    * Set vertex coordinates from array.
    *
    * @param offset data offset if using a buffer object otherwise
    * the coordinate values.
    * @param tupleSize the tuple size of the data.
    * @param stride the stride of the data.
    */
    void
    setCoords(const GLfloat *offset = 0,
            int            tupleSize = 2,
            int            stride = 0);

    /**
    * Set vertex coordinates from buffer object.
    *
    * @param coords the coordinate values; null if using a buffer object.
    * @param offset the offset into the coords buffer.
    * @param tupleSize the tuple size of the data.
    * @param stride the stride of the data.
    */
    void
    setCoords(GLuint  coords,
            const GLfloat  *offset = 0,
            int             tupleSize = 2,
            int             stride = 0);


    /**
    * Set the texture to render.
    *
    * @param texunit the texture unit to use.
    */
    void
    setTexture(int texunit);

    /**
    * Set minimum limits for linear contrast.
    *
    * @param min the RGB channel limits.
    */
    void
    setMin(float min);

    /**
    * Set maximum limits for linear contrast.
    *
    * @param max the RGB channel limits.
    */
    void
    setMax(float max);

    /**
    * Set correction multipliers to normalise pixel intensity.
    *
    * Use to correct the pixel value limits to the storage size
    * limits, for example when using data with 12 bits per
    * sample with a 16-bit storage type it will require
    * multiplying by 2^(16-12) = 2^4 = 16.  To leave
    * uncorrected, e.g. for float and complex types, and
    * integer types where the bits per sample is the same as
    * the storage size, set to 1.0.
    *
    * @param corr the RGB channel correction multipliers.
    */
    void
    setCorrection(const glm::vec3& corr);
          
    /**
    * Set the LUT to use.
    *
    * @param texunit the texture unit to use.
    */
    void
    setLUT(int texunit);

	void setTransformUniforms(const Camera& camera, const glm::mat4& modelMatrix);
	void setShadingUniforms();

	float dataRangeMin;
	float dataRangeMax;
	float GAMMA_MIN;
	float GAMMA_MAX;
	float GAMMA_SCALE;
	float BRIGHTNESS;
	float DENSITY;
	float maskAlpha;
	int BREAK_STEPS;
	glm::vec3 AABB_CLIP_MIN;
	glm::vec3 AABB_CLIP_MAX;

private:
    /// The vertex shader.
    QOpenGLShader *vshader;
    /// The fragment shader.
    QOpenGLShader *fshader;

    /// Vertex coordinates attribute.
    int attr_coords;
	int
		uModelViewMatrix,
		uProjectionMatrix,
		uDataRangeMin,
		uDataRangeMax,
		uBreakSteps,
		uAABBClipMin,
		uAABBClipMax,
		uInverseModelMatrix,
		uCameraPosition,
		uResolution,
		uGammaMin,
		uGammaMax,
		uGammaScale,
		uBrightness,
		uDensity,
		uMaskAlpha,
		uTextureAtlas,
		uTextureAtlasMask;
};

