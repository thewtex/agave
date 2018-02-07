#pragma once

#include "Geometry.h"

//#include "MonteCarlo.cuh"
//#include "RNG.cuh"

#define DEF_FOCUS_TYPE					CenterScreen
#define DEF_FOCUS_SENSOR_POS_CANVAS		Vec2f(0.0f)
#define DEF_FOCUS_P						Vec3f(0.0f)
#define DEF_FOCUS_FOCAL_DISTANCE		100.0f
#define	DEF_FOCUS_T						0.0f
#define DEF_FOCUS_N						Vec3f(0.0f)
#define DEF_FOCUS_DOT_WN				0.0f

class CFocus
{
public:
	enum EType
	{
		CenterScreen,
		ScreenPoint,
		Probed,
		Manual
	};

	EType		m_Type;
	Vec2f		m_SensorPosCanvas;
	float		m_FocalDistance;
	float		m_T;
	Vec3f		m_P;
	Vec3f		m_N;
	float		m_DotWN;

	HO CFocus(void)
	{
		m_Type				= DEF_FOCUS_TYPE;
		m_SensorPosCanvas	= DEF_FOCUS_SENSOR_POS_CANVAS;
		m_FocalDistance		= DEF_FOCUS_FOCAL_DISTANCE;
		m_T					= DEF_FOCUS_T;
		m_P					= DEF_FOCUS_P;
		m_N					= DEF_FOCUS_N;
		m_DotWN				= DEF_FOCUS_DOT_WN;
	}

	HO CFocus& operator=(const CFocus& Other)
	{
		m_Type				= Other.m_Type;
		m_SensorPosCanvas	= Other.m_SensorPosCanvas;
		m_FocalDistance		= Other.m_FocalDistance;
		m_P					= Other.m_P;
		m_T					= Other.m_T;
		m_N					= Other.m_N;
		m_DotWN				= Other.m_DotWN;

		return *this;
	}
};

#define DEF_APERTURE_SIZE			0.0f
#define DEF_APERTURE_NO_BLADES		5
#define DEF_APERTURE_BIAS			BiasNone
#define DEF_APERTURE_ROTATION		0.0f

class CAperture
{
public:
	enum EBias
	{
		BiasCenter,
		BiasEdge,
		BiasNone
	};

	float			m_Size;
	int				m_NoBlades;
	EBias			m_Bias;
	float			m_Rotation;
	float			m_Data[MAX_BOKEH_DATA];

	HO CAperture(void)
	{
		m_Size		= DEF_APERTURE_SIZE;
		m_NoBlades	= DEF_APERTURE_NO_BLADES;
		m_Bias		= DEF_APERTURE_BIAS;
		m_Rotation	= DEF_APERTURE_ROTATION;

		for (int i = 0; i < MAX_BOKEH_DATA; i++)
			m_Data[i] = 0.0f;
	}

	CAperture& operator=(const CAperture& Other)
	{
		m_Size		= Other.m_Size;
		m_NoBlades	= Other.m_NoBlades;
		m_Bias		= Other.m_Bias;
		m_Rotation	= Other.m_Rotation;

		for (int i = 0; i < MAX_BOKEH_DATA; i++)
			m_Data[i] = Other.m_Data[i];

		return *this;
	}

	HO void Update(const float& FStop)
	{
		// Update bokeh
		int Ns = (int)m_NoBlades;

		if ((Ns >= 3) && (Ns <= 6))
		{
			float w = m_Rotation * PI_F / 180.0f, wi = (2.0f * PI_F) / (float)Ns;

			Ns = (Ns + 2) * 2;

			for (int i = 0; i < Ns && i < MAX_BOKEH_DATA; i += 2)
			{
				m_Data[i]		= cos(w);
				m_Data[i + 1]	= sin(w);
				w += wi;
			}
		}
	}
};

#define DEF_FILM_ISO 400.0f
#define DEF_FILM_EXPOSURE 0.5f
#define DEF_FILM_FSTOP 8.0f
#define DEF_FILM_GAMMA 2.2f

class CFilm
{
public:
	CResolution2D	m_Resolution;
	float			m_Screen[2][2];
	Vec2f			m_InvScreen;
	float			m_Iso;
	float			m_Exposure;
	int				m_ExposureIterations;
	float			m_FStop;
	float			m_Gamma;

	// ToDo: Add description
	HO CFilm(void)
	{
		m_Screen[0][0]	= 0.0f;
		m_Screen[0][1]	= 0.0f;
		m_Screen[1][0]	= 0.0f;
		m_Screen[1][1]	= 0.0f;
		m_InvScreen		= Vec2f(0.0f);
		m_Iso			= DEF_FILM_ISO;
		m_Exposure		= DEF_FILM_EXPOSURE;
		m_ExposureIterations = 1;
		m_FStop			= DEF_FILM_FSTOP;
		m_Gamma			= DEF_FILM_GAMMA;
	}

	CFilm& operator=(const CFilm& Other)
	{
		m_Resolution		= Other.m_Resolution;
		m_Screen[0][0]		= Other.m_Screen[0][0];
		m_Screen[0][1]		= Other.m_Screen[0][1];
		m_Screen[1][0]		= Other.m_Screen[1][0];
		m_Screen[1][1]		= Other.m_Screen[1][1];
		m_InvScreen			= Other.m_InvScreen;
		m_Iso				= Other.m_Iso;
		m_Exposure = Other.m_Exposure;
		m_ExposureIterations = Other.m_ExposureIterations;
		m_FStop				= Other.m_FStop;
		m_Gamma				= Other.m_Gamma;

		return *this;
	}

	HO void Update(const float& FovV, const float& Aperture)
	{
		float Scale = 0.0f;

		Scale = tanf(0.5f * (FovV * DEG_TO_RAD));

		m_Screen[0][0] = -Scale * m_Resolution.GetAspectRatio();
		m_Screen[0][1] = Scale * m_Resolution.GetAspectRatio();
		// the "0" Y pixel will be at +Scale.
		m_Screen[1][0] = Scale;
		m_Screen[1][1] = -Scale;

		// the amount to increment for each pixel
		m_InvScreen.x = (m_Screen[0][1] - m_Screen[0][0]) / m_Resolution.GetResX();
		m_InvScreen.y = (m_Screen[1][1] - m_Screen[1][0]) / m_Resolution.GetResY();

		m_Resolution.Update();
	}

	HO int GetWidth(void) const
	{
		return m_Resolution.GetResX();
	}

	HO int GetHeight(void) const
	{
		return m_Resolution.GetResY();
	}
};

#define FPS1 30.0f

//#define DEF_CAMERA_TYPE						Perspective
#define DEF_CAMERA_OPERATOR					CameraOperatorUndefined
#define DEF_CAMERA_VIEW_MODE				ViewModeBack
#define DEF_CAMERA_HITHER					0.01f
#define DEF_CAMERA_YON						20.0f
#define DEF_CAMERA_ENABLE_CLIPPING			true
#define DEF_CAMERA_GAMMA					2.2f
#define DEF_CAMERA_FIELD_OF_VIEW			55.0f
#define DEF_CAMERA_NUM_APERTURE_BLADES		4
#define DEF_CAMERA_APERTURE_BLADES_ANGLE	0.0f
#define DEF_CAMERA_ASPECT_RATIO				1.0f
//#define DEF_CAMERA_ZOOM_SPEED				1.0f
//#define DEF_CAMERA_ORBIT_SPEED				5.0f
//#define DEF_CAMERA_APERTURE_SPEED			0.25f
//#define DEF_CAMERA_FOCAL_DISTANCE_SPEED		10.0f

class CCamera 
{
public:
	CBoundingBox		m_SceneBoundingBox;
	float				m_Hither;
	float				m_Yon;
	bool				m_EnableClippingPlanes;
	Vec3f				m_From;
	Vec3f				m_Target;
	Vec3f				m_Up;
	float				m_FovV;
	float				m_AreaPixel;
	Vec3f 				m_N;
	Vec3f 				m_U;
	Vec3f 				m_V;
	CFilm				m_Film;
	CFocus				m_Focus;
	CAperture			m_Aperture;
	bool				m_Dirty;

	HO CCamera(void)
	{
		m_Hither				= DEF_CAMERA_HITHER;
		m_Yon					= DEF_CAMERA_YON;
		m_EnableClippingPlanes	= DEF_CAMERA_ENABLE_CLIPPING;
		m_From					= Vec3f(500.0f, 500.0f, 500.0f);
		m_Target				= Vec3f(0.0f, 0.0f, 0.0f);
		m_Up					= Vec3f(0.0f, 1.0f, 0.0f);
		m_FovV					= DEF_CAMERA_FIELD_OF_VIEW;
		m_N						= Vec3f(0.0f, 0.0f, 1.0f);
		m_U						= Vec3f(1.0f, 0.0f, 0.0f);
		m_V						= Vec3f(0.0f, 1.0f, 0.0f);
		m_Dirty					= true;
	}

	CCamera& operator=(const CCamera& Other)
	{
		m_SceneBoundingBox		= Other.m_SceneBoundingBox;
		m_Hither				= Other.m_Hither;
		m_Yon					= Other.m_Yon;
		m_EnableClippingPlanes	= Other.m_EnableClippingPlanes;
		m_From					= Other.m_From;
		m_Target				= Other.m_Target;
		m_Up					= Other.m_Up;
		m_FovV					= Other.m_FovV;
		m_AreaPixel				= Other.m_AreaPixel;
		m_N						= Other.m_N;
		m_U						= Other.m_U;
		m_V						= Other.m_V;
		m_Film					= Other.m_Film;
		m_Focus					= Other.m_Focus;
		m_Aperture				= Other.m_Aperture;
		m_Dirty					= Other.m_Dirty;

		return *this;
	}

	HO void Update(void)
	{
		// right handed coordinate system

		// "z" lookat direction
		m_N	= Normalize(m_Target - m_From);
		// camera left/right
		m_U	= Normalize(Cross(m_N, m_Up));
		// camera up/down
		m_V	= Normalize(Cross(m_U, m_N));

		m_Film.Update(m_FovV, m_Aperture.m_Size);

		m_AreaPixel = m_Film.m_Resolution.GetAspectRatio() / (m_Focus.m_FocalDistance * m_Focus.m_FocalDistance);

		m_Aperture.Update(m_Film.m_FStop);

		m_Film.Update(m_FovV, m_Aperture.m_Size);
	}

	HO void Zoom(float amount)
	{
		Vec3f reverseLoS = m_From - m_Target;

		if (amount > 0)
		{	
			reverseLoS.ScaleBy(1.1f);
		}
		else if (amount < 0)
		{	
			if (reverseLoS.Length() > 0.0005f)
			{ 
				reverseLoS.ScaleBy(0.9f);
			}
		}

		m_From = reverseLoS + m_Target;
	}

	// Pan operator
	HO void Pan(float UpUnits, float RightUnits)
	{
		Vec3f LoS = m_Target - m_From;

		Vec3f right		= Cross(LoS, m_Up);
		Vec3f orthogUp = Cross(right, LoS);

		right.Normalize();
		orthogUp.Normalize();

		const float Length = (m_Target - m_From).Length();

		const unsigned int WindowWidth	= m_Film.m_Resolution.GetResX();

		const float U = Length * (RightUnits / WindowWidth);
		const float V = Length * (UpUnits / WindowWidth);

		m_From		= m_From + right * U + m_Up * V;
		m_Target	= m_Target + right * U + m_Up * V;
	}

	HO void Orbit(float DownDegrees, float RightDegrees)
	{
		Vec3f ReverseLoS = m_From - m_Target;

		Vec3f right		= m_Up.Cross(ReverseLoS);
		Vec3f orthogUp	= ReverseLoS.Cross(right);
		Vec3f Up = Vec3f(0.0f, 1.0f, 0.0f);
		
		ReverseLoS.RotateAxis(right, DownDegrees);
		ReverseLoS.RotateAxis(Up, RightDegrees);
		m_Up.RotateAxis(right, DownDegrees);
		m_Up.RotateAxis(Up, RightDegrees);

		m_From = ReverseLoS + m_Target;
	}

	HO void SetViewMode(const EViewMode ViewMode)
	{
		if (ViewMode == ViewModeUser)
			return;

		m_Target	= m_SceneBoundingBox.GetCenter();
		m_Up		= Vec3f(0.0f, 1.0f, 0.0f);

		const float Distance = 1.5f;

		const float Length = Distance * m_SceneBoundingBox.GetMaxLength();

		m_From = m_Target;

		switch (ViewMode)
		{
		case ViewModeFront:							m_From.z -= Length;												break;
		case ViewModeBack:							m_From.z += Length;												break;
		case ViewModeLeft:							m_From.x += Length;												break;
		case ViewModeRight:							m_From.x -= -Length;											break;
		case ViewModeTop:							m_From.y += Length;		m_Up = Vec3f(0.0f, 0.0f, 1.0f);			break;
		case ViewModeBottom:						m_From.y -= -Length;	m_Up = Vec3f(0.0f, 0.0f, -1.0f);		break;
		case ViewModeIsometricFrontLeftTop:			m_From = Vec3f(Length, Length, -Length);						break;
		case ViewModeIsometricFrontRightTop:		m_From = m_Target + Vec3f(-Length, Length, -Length);			break;
		case ViewModeIsometricFrontLeftBottom:		m_From = m_Target + Vec3f(Length, -Length, -Length);			break;
		case ViewModeIsometricFrontRightBottom:		m_From = m_Target + Vec3f(-Length, -Length, -Length);			break;
		case ViewModeIsometricBackLeftTop:			m_From = m_Target + Vec3f(Length, Length, Length);				break;
		case ViewModeIsometricBackRightTop:			m_From = m_Target + Vec3f(-Length, Length, Length);				break;
		case ViewModeIsometricBackLeftBottom:		m_From = m_Target + Vec3f(Length, -Length, Length);				break;
		case ViewModeIsometricBackRightBottom:		m_From = m_Target + Vec3f(-Length, -Length, Length);			break;
		}

		Update();
	}

	float GetHorizontalFOV_radians() {
		// convert horz fov to vert fov
		// w/d = 2*tan(hfov/2)
		// h/d = 2*tan(vfov/2)
		float hfov = 2.0 * atan((float)m_Film.GetWidth()/(float)m_Film.GetHeight() * tan(m_FovV * 0.5 * DEG_TO_RAD));
		return hfov;
	}
	float GetVerticalFOV_radians() {
		return m_FovV * DEG_TO_RAD;
	}
};

