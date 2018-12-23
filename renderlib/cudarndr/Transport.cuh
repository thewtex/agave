#pragma once

#include "RayMarching.cuh"
#include "Shader.cuh"

DEV CColorXyz
EstimateDirectLight(const CudaLighting& lighting,
                    const cudaVolume& volumedata,
                    const CVolumeShader::EType& Type,
                    float Density,
                    int ch,
                    const CudaLight& Light,
                    CLightingSample& LS,
                    const float3& Wo,
                    const float3& Pe,
                    const float3& N,
                    CRNG& RNG)
{
  CColorXyz Ld = SPEC_BLACK, Li = SPEC_BLACK, F = SPEC_BLACK;

  // CColorRgbHdr diffuse = GetBlendedDiffuse(volumedata, Density);
  CColorRgbHdr diffuse = GetDiffuseN(Density, volumedata, ch);
  // CColorRgbHdr specular = GetBlendedSpecular(volumedata, Density);
  CColorRgbHdr specular = GetSpecularN(Density, volumedata, ch);
  // float roughness = GetBlendedRoughness(volumedata, Density);
  float roughness = GetRoughnessN(Density, volumedata, ch);

  CVolumeShader Shader(Type, N, Wo, diffuse.ToXYZ(), specular.ToXYZ(), 2.5f, roughness);

  CRay Rl;

  float LightPdf = 1.0f, ShaderPdf = 1.0f;

  float3 Wi, Pl;

  Li = Light.SampleL(Pe, Rl, LightPdf, LS);

  const CudaLight* pLight = NULL;

  Wi = -Rl.m_D;

  F = Shader.F(Wo, Wi);

  ShaderPdf = Shader.Pdf(Wo, Wi);

  if (!Li.IsBlack() && ShaderPdf > 0.0f && LightPdf > 0.0f && !FreePathRM(Rl, RNG, volumedata)) {
    const float WeightMIS = PowerHeuristic(1.0f, LightPdf, 1.0f, ShaderPdf);

    if (Type == CVolumeShader::Brdf)
      Ld += F * Li * AbsDot(Wi, N) * WeightMIS / LightPdf;

    if (Type == CVolumeShader::Phase)
      Ld += F * Li * WeightMIS / LightPdf;
  }

  F = Shader.SampleF(Wo, Wi, ShaderPdf, LS.m_BsdfSample);

  if (!F.IsBlack() && ShaderPdf > 0.0f) {
    int n = NearestLight(lighting, CRay(Pe, Wi, 0.0f), Li, Pl, &LightPdf);
    if (n > -1) {
      pLight = &gLighting.m_Lights[n];
      LightPdf = pLight->Pdf(Pe, Wi);

      if ((LightPdf > 0.0f) && !Li.IsBlack()) {
        CRay rr(Pl, normalize(Pe - Pl), 0.0f, Length(Pe - Pl));
        if (!FreePathRM(rr, RNG, volumedata)) {
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

DEV CColorXyz
UniformSampleOneLight(const CudaLighting& lighting,
                      const cudaVolume& volumedata,
                      const CVolumeShader::EType& Type,
                      float Density,
                      int ch,
                      const float3& Wo,
                      const float3& Pe,
                      const float3& N,
                      CRNG& RNG,
                      const bool& Brdf)
{
  const int NumLights = lighting.m_NoLights;

  if (NumLights == 0)
    return SPEC_BLACK;

  CLightingSample LS;
  // select a random light, a random 2d sample on light, and a random 2d sample on brdf
  LS.LargeStep(RNG);

  const int WhichLight = (int)floorf(LS.m_LightNum * (float)NumLights);

  const CudaLight& Light = lighting.m_Lights[WhichLight];

  return (float)NumLights * EstimateDirectLight(lighting, volumedata, Type, Density, ch, Light, LS, Wo, Pe, N, RNG);
}
