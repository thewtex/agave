#pragma once

#include "Shader.cuh"
#include "RayMarching.cuh"

DEV CColorXyz EstimateDirectLight(CScene* pScene, const cudaVolume& volumedata, const CVolumeShader::EType& Type, const float& Density, CLight& Light, CLightingSample& LS, const Vec3f& Wo, const Vec3f& Pe, const Vec3f& N, CRNG& RNG)
{
	CColorXyz Ld = SPEC_BLACK, Li = SPEC_BLACK, F = SPEC_BLACK;
	
	CVolumeShader Shader(Type, N, Wo, GetDiffuse(Density).ToXYZ(), GetSpecular(Density).ToXYZ(), 2.5f/*pScene->m_IOR*/, GetRoughness(Density));
	
	CRay Rl; 

	float LightPdf = 1.0f, ShaderPdf = 1.0f;
	
	Vec3f Wi, P, Pl;

 	Li = Light.SampleL(Pe, Rl, LightPdf, LS);
	
	CLight* pLight = NULL;

	Wi = -Rl.m_D; 

	F = Shader.F(Wo, Wi); 

	ShaderPdf = Shader.Pdf(Wo, Wi);

	if (!Li.IsBlack() && ShaderPdf > 0.0f && LightPdf > 0.0f && !FreePathRM(Rl, RNG, volumedata))
	{
		const float WeightMIS = PowerHeuristic(1.0f, LightPdf, 1.0f, ShaderPdf);
		
		if (Type == CVolumeShader::Brdf)
			Ld += F * Li * AbsDot(Wi, N) * WeightMIS / LightPdf;

		if (Type == CVolumeShader::Phase)
			Ld += F * Li * WeightMIS / LightPdf;
	}

	F = Shader.SampleF(Wo, Wi, ShaderPdf, LS.m_BsdfSample);

	if (!F.IsBlack() && ShaderPdf > 0.0f)
	{
		if (NearestLight(pScene, CRay(Pe, Wi, 0.0f), Li, Pl, pLight, &LightPdf))
		{
			LightPdf = pLight->Pdf(Pe, Wi);

			if ((LightPdf > 0.0f) &&
				!Li.IsBlack()) {
				CRay rr(Pl, Normalize(Pe - Pl), 0.0f, (Pe - Pl).Length());
				if (!FreePathRM(rr, RNG, volumedata))
				{
					const float WeightMIS = PowerHeuristic(1.0f, ShaderPdf, 1.0f, LightPdf);

					if (Type == CVolumeShader::Brdf)
						Ld += F * Li * AbsDot(Wi, N) * WeightMIS / ShaderPdf;

					if (Type == CVolumeShader::Phase)
						Ld += F * Li * WeightMIS / ShaderPdf;
				}

			}
		}
	}

	return Ld;
}

DEV CColorXyz UniformSampleOneLight(CScene* pScene, const cudaVolume& volumedata, const CVolumeShader::EType& Type, const float& Density, const Vec3f& Wo, const Vec3f& Pe, const Vec3f& N, CRNG& RNG, const bool& Brdf)
{
	const int NumLights = pScene->m_Lighting.m_NoLights;

 	if (NumLights == 0)
 		return SPEC_BLACK;

	CLightingSample LS;

	LS.LargeStep(RNG);

	const int WhichLight = (int)floorf(LS.m_LightNum * (float)NumLights);

	CLight& Light = pScene->m_Lighting.m_Lights[WhichLight];

	return (float)NumLights * EstimateDirectLight(pScene, volumedata, Type, Density, Light, LS, Wo, Pe, N, RNG);
}