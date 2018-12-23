#include "AppearanceSettingsWidget.h"
#include "RangeWidget.h"
#include "Section.h"
#include "TransferFunction.h"

#include "ImageXYZC.h"
#include "renderlib/AppScene.h"
#include "renderlib/Logging.h"
#include "renderlib/RenderSettings.h"

QAppearanceSettingsWidget::QAppearanceSettingsWidget(QWidget* pParent, QTransferFunction* tran, RenderSettings* rs)
  : QGroupBox(pParent)
  , m_MainLayout()
  , m_DensityScaleSlider()
  , m_RendererType()
  , m_ShadingType()
  , m_GradientFactorLabel()
  , m_GradientFactorSlider()
  , m_StepSizePrimaryRaySlider()
  , m_StepSizeSecondaryRaySlider()
  , m_transferFunction(tran)
  , m_scene(nullptr)
{
  setLayout(&m_MainLayout);

  m_MainLayout.addWidget(new QLabel("Renderer"), 1, 0);
  m_RendererType.addItem("OpenGL simple", 0);
  m_RendererType.addItem("CUDA full", 1);
  m_RendererType.setCurrentIndex(1);
  m_MainLayout.addWidget(&m_RendererType, 1, 1, 1, 2);

  m_MainLayout.addWidget(new QLabel("Scattering Density"), 2, 0);
  m_DensityScaleSlider.setRange(0.001, 100.0);
  m_DensityScaleSlider.setDecimals(3);
  m_DensityScaleSlider.setValue(rs->m_RenderSettings.m_DensityScale);
  m_MainLayout.addWidget(&m_DensityScaleSlider, 2, 1, 1, 2);

  m_MainLayout.addWidget(new QLabel("Shading Type"), 3, 0);

  m_ShadingType.addItem("BRDF Only", 0);
  m_ShadingType.addItem("Phase Function Only", 1);
  m_ShadingType.addItem("Mixed", 2);
  m_ShadingType.setCurrentIndex(rs->m_RenderSettings.m_ShadingType);
  m_MainLayout.addWidget(&m_ShadingType, 3, 1, 1, 2);

  m_GradientFactorLabel.setText("Shading Type Mixture");
  m_MainLayout.addWidget(&m_GradientFactorLabel, 4, 0);
  m_GradientFactorSlider.setRange(0.001, 100.0);
  m_GradientFactorSlider.setDecimals(3);
  m_GradientFactorSlider.setValue(rs->m_RenderSettings.m_GradientFactor);
  m_MainLayout.addWidget(&m_GradientFactorSlider, 4, 1, 1, 2);

  QObject::connect(&m_DensityScaleSlider, SIGNAL(valueChanged(double)), this, SLOT(OnSetDensityScale(double)));
  QObject::connect(&m_GradientFactorSlider, SIGNAL(valueChanged(double)), this, SLOT(OnSetGradientFactor(double)));

  m_MainLayout.addWidget(new QLabel("Primary Ray Step Size"), 5, 0);
  m_StepSizePrimaryRaySlider.setRange(0.1, 100.0);
  m_StepSizePrimaryRaySlider.setValue(rs->m_RenderSettings.m_StepSizeFactor);
  m_StepSizePrimaryRaySlider.setDecimals(3);
  m_MainLayout.addWidget(&m_StepSizePrimaryRaySlider, 5, 1, 1, 2);

  QObject::connect(
    &m_StepSizePrimaryRaySlider, SIGNAL(valueChanged(double)), this, SLOT(OnSetStepSizePrimaryRay(double)));

  m_MainLayout.addWidget(new QLabel("Secondary Ray Step Size"), 6, 0);

  m_StepSizeSecondaryRaySlider.setRange(0.1, 100.0);
  m_StepSizeSecondaryRaySlider.setValue(rs->m_RenderSettings.m_StepSizeFactorShadow);
  m_StepSizeSecondaryRaySlider.setDecimals(3);

  m_MainLayout.addWidget(&m_StepSizeSecondaryRaySlider, 6, 1, 1, 2);

  QObject::connect(
    &m_StepSizeSecondaryRaySlider, SIGNAL(valueChanged(double)), this, SLOT(OnSetStepSizeSecondaryRay(double)));

  m_scaleSection = new Section("Volume Scale", 0);
  auto* scaleSectionLayout = new QGridLayout();
  scaleSectionLayout->addWidget(new QLabel("X"), 0, 0);
  m_xscaleSpinner = new QDoubleSpinner();
  m_xscaleSpinner->setValue(1.0);
  scaleSectionLayout->addWidget(m_xscaleSpinner, 0, 1);
  QObject::connect(m_xscaleSpinner,
                   QOverload<double>::of(&QDoubleSpinner::valueChanged),
                   this,
                   &QAppearanceSettingsWidget::OnSetScaleX);
  scaleSectionLayout->addWidget(new QLabel("Y"), 1, 0);
  m_yscaleSpinner = new QDoubleSpinner();
  m_yscaleSpinner->setValue(1.0);
  scaleSectionLayout->addWidget(m_yscaleSpinner, 1, 1);
  QObject::connect(m_yscaleSpinner,
                   QOverload<double>::of(&QDoubleSpinner::valueChanged),
                   this,
                   &QAppearanceSettingsWidget::OnSetScaleY);
  scaleSectionLayout->addWidget(new QLabel("Z"), 2, 0);
  m_zscaleSpinner = new QDoubleSpinner();
  m_zscaleSpinner->setValue(1.0);
  scaleSectionLayout->addWidget(m_zscaleSpinner, 2, 1);
  QObject::connect(m_zscaleSpinner,
                   QOverload<double>::of(&QDoubleSpinner::valueChanged),
                   this,
                   &QAppearanceSettingsWidget::OnSetScaleZ);

  m_scaleSection->setContentLayout(*scaleSectionLayout);
  m_MainLayout.addWidget(m_scaleSection, 12, 0, 1, -1);

  m_clipRoiSection = new Section("ROI", 0);
  auto* roiSectionLayout = new QGridLayout();
  roiSectionLayout->addWidget(new QLabel("X"), 0, 0);
  m_roiX = new RangeWidget(Qt::Horizontal);
  m_roiX->setRange(0, 100);
  m_roiX->setFirstValue(0);
  m_roiX->setSecondValue(100);
  roiSectionLayout->addWidget(m_roiX, 0, 1);
  QObject::connect(m_roiX, &RangeWidget::firstValueChanged, this, &QAppearanceSettingsWidget::OnSetRoiXMin);
  QObject::connect(m_roiX, &RangeWidget::secondValueChanged, this, &QAppearanceSettingsWidget::OnSetRoiXMax);
  roiSectionLayout->addWidget(new QLabel("Y"), 1, 0);
  m_roiY = new RangeWidget(Qt::Horizontal);
  m_roiY->setRange(0, 100);
  m_roiY->setFirstValue(0);
  m_roiY->setSecondValue(100);
  roiSectionLayout->addWidget(m_roiY, 1, 1);
  QObject::connect(m_roiY, &RangeWidget::firstValueChanged, this, &QAppearanceSettingsWidget::OnSetRoiYMin);
  QObject::connect(m_roiY, &RangeWidget::secondValueChanged, this, &QAppearanceSettingsWidget::OnSetRoiYMax);
  roiSectionLayout->addWidget(new QLabel("Z"), 2, 0);
  m_roiZ = new RangeWidget(Qt::Horizontal);
  m_roiZ->setRange(0, 100);
  m_roiZ->setFirstValue(0);
  m_roiZ->setSecondValue(100);
  roiSectionLayout->addWidget(m_roiZ, 2, 1);
  QObject::connect(m_roiZ, &RangeWidget::firstValueChanged, this, &QAppearanceSettingsWidget::OnSetRoiZMin);
  QObject::connect(m_roiZ, &RangeWidget::secondValueChanged, this, &QAppearanceSettingsWidget::OnSetRoiZMax);

  m_clipRoiSection->setContentLayout(*roiSectionLayout);
  m_MainLayout.addWidget(m_clipRoiSection, 13, 0, 1, -1);

  Section* section = createLightingControls();
  m_MainLayout.addWidget(section, 14, 0, 1, -1);

  QObject::connect(&m_RendererType, SIGNAL(currentIndexChanged(int)), this, SLOT(OnSetRendererType(int)));
  QObject::connect(&m_ShadingType, SIGNAL(currentIndexChanged(int)), this, SLOT(OnSetShadingType(int)));
  // QObject::connect(&gStatus, SIGNAL(RenderBegin()), this, SLOT(OnRenderBegin()));

  QObject::connect(m_transferFunction, SIGNAL(Changed()), this, SLOT(OnTransferFunctionChanged()));
}

void QAppearanceSettingsWidget::OnSetAreaLightTheta(double value)
Section*
QAppearanceSettingsWidget::createLightingControls()
{
	if (!_scene) return;
	_scene->_lighting.m_Lights[1].m_Theta = value;
	_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(LightsDirty);
  Section* section = new Section("Lighting", 0);
  auto* sectionLayout = new QGridLayout();

  int row = 0;
  sectionLayout->addWidget(new QLabel("AreaLight Theta"), row, 0);
  m_lt0gui.m_thetaSlider = new QNumericSlider();
  m_lt0gui.m_thetaSlider->setRange(0.0, 3.14159265 * 2.0);
  m_lt0gui.m_thetaSlider->setValue(0.0);
  sectionLayout->addWidget(m_lt0gui.m_thetaSlider, row, 1, 1, 4);
  QObject::connect(
    m_lt0gui.m_thetaSlider, &QNumericSlider::valueChanged, this, &QAppearanceSettingsWidget::OnSetAreaLightTheta);

  row++;
  sectionLayout->addWidget(new QLabel("AreaLight Phi"), row, 0);
  m_lt0gui.m_phiSlider = new QNumericSlider();
  m_lt0gui.m_phiSlider->setRange(0.0, 3.14159265);
  m_lt0gui.m_phiSlider->setValue(0.0);
  sectionLayout->addWidget(m_lt0gui.m_phiSlider, row, 1, 1, 4);
  QObject::connect(
    m_lt0gui.m_phiSlider, &QNumericSlider::valueChanged, this, &QAppearanceSettingsWidget::OnSetAreaLightPhi);

  row++;
  sectionLayout->addWidget(new QLabel("AreaLight Size"), row, 0);
  m_lt0gui.m_sizeSlider = new QNumericSlider();
  m_lt0gui.m_sizeSlider->setRange(0.1, 5.0);
  m_lt0gui.m_sizeSlider->setValue(1.0);
  sectionLayout->addWidget(m_lt0gui.m_sizeSlider, row, 1, 1, 4);
  QObject::connect(
    m_lt0gui.m_sizeSlider, &QNumericSlider::valueChanged, this, &QAppearanceSettingsWidget::OnSetAreaLightSize);

  row++;
  sectionLayout->addWidget(new QLabel("AreaLight Distance"), row, 0);
  m_lt0gui.m_distSlider = new QNumericSlider();
  m_lt0gui.m_distSlider->setRange(0.1, 100.0);
  m_lt0gui.m_distSlider->setValue(10.0);
  sectionLayout->addWidget(m_lt0gui.m_distSlider, row, 1, 1, 4);
  QObject::connect(
    m_lt0gui.m_distSlider, &QNumericSlider::valueChanged, this, &QAppearanceSettingsWidget::OnSetAreaLightDistance);

  row++;
  sectionLayout->addWidget(new QLabel("AreaLight Intensity"), row, 0);
  m_lt0gui.m_intensitySlider = new QNumericSlider();
  m_lt0gui.m_intensitySlider->setRange(0.1, 1000.0);
  m_lt0gui.m_intensitySlider->setValue(100.0);
  sectionLayout->addWidget(m_lt0gui.m_intensitySlider, row, 1, 1, 3);
  m_lt0gui.m_areaLightColorButton = new QColorPushButton();
  sectionLayout->addWidget(m_lt0gui.m_areaLightColorButton, row, 4);
  QObject::connect(m_lt0gui.m_areaLightColorButton, &QColorPushButton::currentColorChanged, [this](const QColor& c) {
    this->OnSetAreaLightColor(this->m_lt0gui.m_intensitySlider->value(), c);
  });
  QObject::connect(m_lt0gui.m_intensitySlider, &QNumericSlider::valueChanged, [this](double v) {
    this->OnSetAreaLightColor(v, this->m_lt0gui.m_areaLightColorButton->GetColor());
  });

  row++;
  sectionLayout->addWidget(new QLabel("SkyLight Top"), row, 0);
  m_lt1gui.m_stintensitySlider = new QNumericSlider();
  m_lt1gui.m_stintensitySlider->setRange(0.1, 10.0);
  m_lt1gui.m_stintensitySlider->setValue(1.0);
  sectionLayout->addWidget(m_lt1gui.m_stintensitySlider, row, 1, 1, 3);
  m_lt1gui.m_stColorButton = new QColorPushButton();
  sectionLayout->addWidget(m_lt1gui.m_stColorButton, row, 4);
  QObject::connect(m_lt1gui.m_stColorButton, &QColorPushButton::currentColorChanged, [this](const QColor& c) {
    this->OnSetSkyLightTopColor(this->m_lt1gui.m_stintensitySlider->value(), c);
  });
  QObject::connect(m_lt1gui.m_stintensitySlider, &QNumericSlider::valueChanged, [this](double v) {
    this->OnSetSkyLightTopColor(v, this->m_lt1gui.m_stColorButton->GetColor());
  });

  row++;
  sectionLayout->addWidget(new QLabel("SkyLight Mid"), row, 0);
  m_lt1gui.m_smintensitySlider = new QNumericSlider();
  m_lt1gui.m_smintensitySlider->setRange(0.1, 10.0);
  m_lt1gui.m_smintensitySlider->setValue(1.0);
  sectionLayout->addWidget(m_lt1gui.m_smintensitySlider, row, 1, 1, 3);
  m_lt1gui.m_smColorButton = new QColorPushButton();
  sectionLayout->addWidget(m_lt1gui.m_smColorButton, row, 4);
  QObject::connect(m_lt1gui.m_smColorButton, &QColorPushButton::currentColorChanged, [this](const QColor& c) {
    this->OnSetSkyLightMidColor(this->m_lt1gui.m_smintensitySlider->value(), c);
  });
  QObject::connect(m_lt1gui.m_smintensitySlider, &QNumericSlider::valueChanged, [this](double v) {
    this->OnSetSkyLightMidColor(v, this->m_lt1gui.m_smColorButton->GetColor());
  });

  row++;
  sectionLayout->addWidget(new QLabel("SkyLight Bot"), row, 0);
  m_lt1gui.m_sbintensitySlider = new QNumericSlider();
  m_lt1gui.m_sbintensitySlider->setRange(0.1, 10.0);
  m_lt1gui.m_sbintensitySlider->setValue(1.0);
  sectionLayout->addWidget(m_lt1gui.m_sbintensitySlider, row, 1, 1, 3);
  m_lt1gui.m_sbColorButton = new QColorPushButton();
  sectionLayout->addWidget(m_lt1gui.m_sbColorButton, row, 4);
  QObject::connect(m_lt1gui.m_sbColorButton, &QColorPushButton::currentColorChanged, [this](const QColor& c) {
    this->OnSetSkyLightBotColor(this->m_lt1gui.m_sbintensitySlider->value(), c);
  });
  QObject::connect(m_lt1gui.m_sbintensitySlider, &QNumericSlider::valueChanged, [this](double v) {
    this->OnSetSkyLightBotColor(v, this->m_lt1gui.m_sbColorButton->GetColor());
  });

  section->setContentLayout(*sectionLayout);
  return section;
}
void QAppearanceSettingsWidget::OnSetAreaLightPhi(double value)

void
QAppearanceSettingsWidget::OnSetScaleX(double value)
{
	if (!_scene) return;
	_scene->_lighting.m_Lights[1].m_Phi = value;
	_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(LightsDirty);
  if (!m_scene)
    return;
  m_scene->m_volume->setPhysicalSize(value, m_scene->m_volume->physicalSizeY(), m_scene->m_volume->physicalSizeZ());
  m_scene->initBoundsFromImg(m_scene->m_volume);
  m_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(CameraDirty);
}
void QAppearanceSettingsWidget::OnSetAreaLightSize(double value)
void
QAppearanceSettingsWidget::OnSetScaleY(double value)
{
	if (!_scene) return;
	_scene->_lighting.m_Lights[1].m_Width = value;
	_scene->_lighting.m_Lights[1].m_Height = value;
	_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(LightsDirty);
  if (!m_scene)
    return;
  m_scene->m_volume->setPhysicalSize(m_scene->m_volume->physicalSizeX(), value, m_scene->m_volume->physicalSizeZ());
  m_scene->initBoundsFromImg(m_scene->m_volume);
  m_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(CameraDirty);
}
void QAppearanceSettingsWidget::OnSetAreaLightDistance(double value)
void
QAppearanceSettingsWidget::OnSetScaleZ(double value)
{
	if (!_scene) return;
	_scene->_lighting.m_Lights[1].m_Distance = value;
	_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(LightsDirty);
  if (!m_scene)
    return;
  m_scene->m_volume->setPhysicalSize(m_scene->m_volume->physicalSizeX(), m_scene->m_volume->physicalSizeY(), value);
  m_scene->initBoundsFromImg(m_scene->m_volume);
  m_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(CameraDirty);
}
void QAppearanceSettingsWidget::OnSetAreaLightColor(double intensity, const QColor& color)
void
QAppearanceSettingsWidget::OnSetRoiXMin(int value)
{
	if (!_scene) return;
	qreal rgba[4];
	color.getRgbF(&rgba[0], &rgba[1], &rgba[2], &rgba[3]);

	_scene->_lighting.m_Lights[1].m_Color = glm::vec3(intensity * rgba[0], intensity*rgba[1], intensity*rgba[2]);
	_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(LightsDirty);
  if (!m_scene)
    return;
  glm::vec3 v = m_scene->m_roi.GetMinP();
  v.x = (float)value / 100.0;
  m_scene->m_roi.SetMinP(v);
  m_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(RoiDirty);
}

void QAppearanceSettingsWidget::OnSetSkyLightTopColor(double intensity, const QColor& color)
void
QAppearanceSettingsWidget::OnSetRoiYMin(int value)
{
	if (!_scene) return;
	qreal rgba[4];
	color.getRgbF(&rgba[0], &rgba[1], &rgba[2], &rgba[3]);

	_scene->_lighting.m_Lights[0].m_ColorTop = glm::vec3(intensity * rgba[0], intensity*rgba[1], intensity*rgba[2]);
	_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(LightsDirty);
  if (!m_scene)
    return;
  glm::vec3 v = m_scene->m_roi.GetMinP();
  v.y = (float)value / 100.0;
  m_scene->m_roi.SetMinP(v);
  m_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(RoiDirty);
}
void QAppearanceSettingsWidget::OnSetSkyLightMidColor(double intensity, const QColor& color)
void
QAppearanceSettingsWidget::OnSetRoiZMin(int value)
{
	if (!_scene) return;
	qreal rgba[4];
	color.getRgbF(&rgba[0], &rgba[1], &rgba[2], &rgba[3]);
  if (!m_scene)
    return;
  glm::vec3 v = m_scene->m_roi.GetMinP();
  v.z = (float)value / 100.0;
  m_scene->m_roi.SetMinP(v);
  m_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(RoiDirty);
}
void
QAppearanceSettingsWidget::OnSetRoiXMax(int value)
{
  if (!m_scene)
    return;
  glm::vec3 v = m_scene->m_roi.GetMaxP();
  v.x = (float)value / 100.0;
  m_scene->m_roi.SetMaxP(v);
  m_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(RoiDirty);
}
void
QAppearanceSettingsWidget::OnSetRoiYMax(int value)
{
  if (!m_scene)
    return;
  glm::vec3 v = m_scene->m_roi.GetMaxP();
  v.y = (float)value / 100.0;
  m_scene->m_roi.SetMaxP(v);
  m_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(RoiDirty);
}
void
QAppearanceSettingsWidget::OnSetRoiZMax(int value)
{
  if (!m_scene)
    return;
  glm::vec3 v = m_scene->m_roi.GetMaxP();
  v.z = (float)value / 100.0;
  m_scene->m_roi.SetMaxP(v);
  m_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(RoiDirty);
}

void
QAppearanceSettingsWidget::OnSetAreaLightTheta(double value)
{
  if (!m_scene)
    return;
  m_scene->m_lighting.m_Lights[1].m_Theta = value;
  m_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(LightsDirty);
}
void
QAppearanceSettingsWidget::OnSetAreaLightPhi(double value)
{
  if (!m_scene)
    return;
  m_scene->m_lighting.m_Lights[1].m_Phi = value;
  m_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(LightsDirty);
}
void
QAppearanceSettingsWidget::OnSetAreaLightSize(double value)
{
  if (!m_scene)
    return;
  m_scene->m_lighting.m_Lights[1].m_Width = value;
  m_scene->m_lighting.m_Lights[1].m_Height = value;
  m_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(LightsDirty);
}
void
QAppearanceSettingsWidget::OnSetAreaLightDistance(double value)
{
  if (!m_scene)
    return;
  m_scene->m_lighting.m_Lights[1].m_Distance = value;
  m_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(LightsDirty);
}
void
QAppearanceSettingsWidget::OnSetAreaLightColor(double intensity, const QColor& color)
{
  if (!m_scene)
    return;
  qreal rgba[4];
  color.getRgbF(&rgba[0], &rgba[1], &rgba[2], &rgba[3]);

  m_scene->m_lighting.m_Lights[1].m_Color = glm::vec3(rgba[0], rgba[1], rgba[2]);
  m_scene->m_lighting.m_Lights[1].m_ColorIntensity = intensity;
  m_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(LightsDirty);
}

void
QAppearanceSettingsWidget::OnSetSkyLightTopColor(double intensity, const QColor& color)
{
  if (!m_scene)
    return;
  qreal rgba[4];
  color.getRgbF(&rgba[0], &rgba[1], &rgba[2], &rgba[3]);

  m_scene->m_lighting.m_Lights[0].m_ColorTop = glm::vec3(rgba[0], rgba[1], rgba[2]);
  m_scene->m_lighting.m_Lights[0].m_ColorTopIntensity = intensity;
  m_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(LightsDirty);
}
void
QAppearanceSettingsWidget::OnSetSkyLightMidColor(double intensity, const QColor& color)
{
  if (!m_scene)
    return;
  qreal rgba[4];
  color.getRgbF(&rgba[0], &rgba[1], &rgba[2], &rgba[3]);

  m_scene->m_lighting.m_Lights[0].m_ColorMiddle = glm::vec3(rgba[0], rgba[1], rgba[2]);
  m_scene->m_lighting.m_Lights[0].m_ColorMiddleIntensity = intensity;
  m_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(LightsDirty);
}
void
QAppearanceSettingsWidget::OnSetSkyLightBotColor(double intensity, const QColor& color)
{
  if (!m_scene)
    return;
  qreal rgba[4];
  color.getRgbF(&rgba[0], &rgba[1], &rgba[2], &rgba[3]);

  m_scene->m_lighting.m_Lights[0].m_ColorBottom = glm::vec3(rgba[0], rgba[1], rgba[2]);
  m_scene->m_lighting.m_Lights[0].m_ColorBottomIntensity = intensity;
  m_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(LightsDirty);
}

void
QAppearanceSettingsWidget::OnRenderBegin(void)
{
  m_DensityScaleSlider.setValue(m_transferFunction->GetDensityScale());
  m_ShadingType.setCurrentIndex(m_transferFunction->GetShadingType());
  m_GradientFactorSlider.setValue(m_transferFunction->renderSettings()->m_RenderSettings.m_GradientFactor);

  m_StepSizePrimaryRaySlider.setValue(m_transferFunction->renderSettings()->m_RenderSettings.m_StepSizeFactor, true);
  m_StepSizeSecondaryRaySlider.setValue(m_transferFunction->renderSettings()->m_RenderSettings.m_StepSizeFactorShadow,
                                        true);
}

void
QAppearanceSettingsWidget::OnSetDensityScale(double DensityScale)
{
  m_transferFunction->SetDensityScale(DensityScale);
}

void
QAppearanceSettingsWidget::OnSetShadingType(int Index)
{
  m_transferFunction->SetShadingType(Index);
  m_GradientFactorLabel.setEnabled(Index == 2);
  m_GradientFactorSlider.setEnabled(Index == 2);
}

void
QAppearanceSettingsWidget::OnSetRendererType(int Index)
{
  m_transferFunction->SetRendererType(Index);
}

void
QAppearanceSettingsWidget::OnSetGradientFactor(double GradientFactor)
{
  m_transferFunction->SetGradientFactor(GradientFactor);
}

void
QAppearanceSettingsWidget::OnSetStepSizePrimaryRay(const double& StepSizePrimaryRay)
{
  m_transferFunction->renderSettings()->m_RenderSettings.m_StepSizeFactor = (float)StepSizePrimaryRay;
  m_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(RenderParamsDirty);
}

void
QAppearanceSettingsWidget::OnSetStepSizeSecondaryRay(const double& StepSizeSecondaryRay)
{
  m_transferFunction->renderSettings()->m_RenderSettings.m_StepSizeFactorShadow = (float)StepSizeSecondaryRay;
  m_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(RenderParamsDirty);
}

void
QAppearanceSettingsWidget::OnTransferFunctionChanged(void)
{
  m_DensityScaleSlider.setValue(m_transferFunction->GetDensityScale(), true);
  m_ShadingType.setCurrentIndex(m_transferFunction->GetShadingType());
  m_GradientFactorSlider.setValue(m_transferFunction->GetGradientFactor(), true);
}

void
QAppearanceSettingsWidget::OnDiffuseColorChanged(int i, const QColor& color)
{
  if (!m_scene)
    return;
  qreal rgba[4];
  color.getRgbF(&rgba[0], &rgba[1], &rgba[2], &rgba[3]);
  m_scene->m_material.m_diffuse[i * 3 + 0] = rgba[0];
  m_scene->m_material.m_diffuse[i * 3 + 1] = rgba[1];
  m_scene->m_material.m_diffuse[i * 3 + 2] = rgba[2];
  m_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(RenderParamsDirty);
}

void
QAppearanceSettingsWidget::OnSpecularColorChanged(int i, const QColor& color)
{
  if (!m_scene)
    return;
  qreal rgba[4];
  color.getRgbF(&rgba[0], &rgba[1], &rgba[2], &rgba[3]);
  m_scene->m_material.m_specular[i * 3 + 0] = rgba[0];
  m_scene->m_material.m_specular[i * 3 + 1] = rgba[1];
  m_scene->m_material.m_specular[i * 3 + 2] = rgba[2];
  m_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(RenderParamsDirty);
}

void
QAppearanceSettingsWidget::OnEmissiveColorChanged(int i, const QColor& color)
{
  if (!m_scene)
    return;
  qreal rgba[4];
  color.getRgbF(&rgba[0], &rgba[1], &rgba[2], &rgba[3]);
  m_scene->m_material.m_emissive[i * 3 + 0] = rgba[0];
  m_scene->m_material.m_emissive[i * 3 + 1] = rgba[1];
  m_scene->m_material.m_emissive[i * 3 + 2] = rgba[2];
  m_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(RenderParamsDirty);
}
void
QAppearanceSettingsWidget::OnSetWindowLevel(int i, double window, double level)
{
  if (!m_scene)
    return;
  // LOG_DEBUG << "window/level: " << window << ", " << level;
  m_scene->m_volume->channel((uint32_t)i)->generate_windowLevel(window, level);

  m_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(TransferFunctionDirty);
}

void
QAppearanceSettingsWidget::OnOpacityChanged(int i, double opacity)
{
  if (!m_scene)
    return;
  // LOG_DEBUG << "window/level: " << window << ", " << level;
  //_scene->_volume->channel((uint32_t)i)->setOpacity(opacity);
  m_scene->m_material.m_opacity[i] = opacity;
  m_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(TransferFunctionDirty);
}

void
QAppearanceSettingsWidget::OnRoughnessChanged(int i, double roughness)
{
  if (!m_scene)
    return;
  m_scene->m_material.m_roughness[i] = roughness;
  m_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(TransferFunctionDirty);
}

void
QAppearanceSettingsWidget::OnChannelChecked(int i, bool is_checked)
{
  if (!m_scene)
    return;
  m_scene->m_material.m_enabled[i] = is_checked;
  m_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(VolumeDataDirty);
}

// split color into color and intensity.
inline void
normalizeColorForGui(const glm::vec3& incolor, QColor& outcolor, float& outintensity)
{
  // if any r,g,b is greater than 1, take max value as intensity, else intensity = 1
  float i = std::max(incolor.x, std::max(incolor.y, incolor.z));
  outintensity = (i > 1.0f) ? i : 1.0f;
  glm::vec3 voutcolor = incolor / i;
  outcolor = QColor::fromRgbF(voutcolor.x, voutcolor.y, voutcolor.z);
}

void
QAppearanceSettingsWidget::initLightingControls(Scene* scene)
{
  m_lt0gui.m_thetaSlider->setValue(scene->m_lighting.m_Lights[1].m_Theta);
  m_lt0gui.m_phiSlider->setValue(scene->m_lighting.m_Lights[1].m_Phi);
  m_lt0gui.m_sizeSlider->setValue(scene->m_lighting.m_Lights[1].m_Width);
  m_lt0gui.m_distSlider->setValue(scene->m_lighting.m_Lights[1].m_Distance);
  // split color into color and intensity.
  QColor c;
  float i;
  normalizeColorForGui(scene->m_lighting.m_Lights[1].m_Color, c, i);
  m_lt0gui.m_intensitySlider->setValue(i * scene->m_lighting.m_Lights[1].m_ColorIntensity);
  m_lt0gui.m_areaLightColorButton->SetColor(c);

  normalizeColorForGui(scene->m_lighting.m_Lights[0].m_ColorTop, c, i);
  m_lt1gui.m_stintensitySlider->setValue(i * scene->m_lighting.m_Lights[1].m_ColorTopIntensity);
  m_lt1gui.m_stColorButton->SetColor(c);
  normalizeColorForGui(scene->m_lighting.m_Lights[0].m_ColorMiddle, c, i);
  m_lt1gui.m_smintensitySlider->setValue(i * scene->m_lighting.m_Lights[1].m_ColorMiddleIntensity);
  m_lt1gui.m_smColorButton->SetColor(c);
  normalizeColorForGui(scene->m_lighting.m_Lights[0].m_ColorBottom, c, i);
  m_lt1gui.m_sbintensitySlider->setValue(i * scene->m_lighting.m_Lights[1].m_ColorBottomIntensity);
  m_lt1gui.m_sbColorButton->SetColor(c);
}

void
QAppearanceSettingsWidget::onNewImage(Scene* scene)
{
  // remove the previous per-channel ui
  for (auto s : m_channelSections) {
    delete s;
  }
  m_channelSections.clear();

  // I don't own this.
  m_scene = scene;

  if (!scene->m_volume) {
    return;
  }

  m_roiX->setFirstValue(m_scene->m_roi.GetMinP().x * 100.0);
  m_roiX->setSecondValue(m_scene->m_roi.GetMaxP().x * 100.0);
  m_roiY->setFirstValue(m_scene->m_roi.GetMinP().y * 100.0);
  m_roiY->setSecondValue(m_scene->m_roi.GetMaxP().y * 100.0);
  m_roiZ->setFirstValue(m_scene->m_roi.GetMinP().z * 100.0);
  m_roiZ->setSecondValue(m_scene->m_roi.GetMaxP().z * 100.0);

  m_xscaleSpinner->setValue(m_scene->m_volume->physicalSizeX());
  m_yscaleSpinner->setValue(m_scene->m_volume->physicalSizeY());
  m_zscaleSpinner->setValue(m_scene->m_volume->physicalSizeZ());

  initLightingControls(scene);

  for (uint32_t i = 0; i < scene->m_volume->sizeC(); ++i) {
    bool channelenabled = m_scene->m_material.m_enabled[i];

    Section* section = new Section(scene->m_volume->channel(i)->m_name, 0, channelenabled);

    auto* sectionLayout = new QGridLayout();

    int row = 0;
    sectionLayout->addWidget(new QLabel("Window"), row, 0);
    QNumericSlider* windowSlider = new QNumericSlider();
    windowSlider->setRange(0.001, 1.0);
    windowSlider->setValue(scene->m_volume->channel(i)->m_window, true);
    sectionLayout->addWidget(windowSlider, row, 1, 1, 2);

    row++;
    sectionLayout->addWidget(new QLabel("Level"), row, 0);
    QNumericSlider* levelSlider = new QNumericSlider();
    levelSlider->setRange(0.001, 1.0);
    levelSlider->setValue(scene->m_volume->channel(i)->m_level, true);
    sectionLayout->addWidget(levelSlider, row, 1, 1, 2);

    QObject::connect(windowSlider, &QNumericSlider::valueChanged, [i, this, levelSlider](double d) {
      this->OnSetWindowLevel(i, d, levelSlider->value());
    });
    QObject::connect(levelSlider, &QNumericSlider::valueChanged, [i, this, windowSlider](double d) {
      this->OnSetWindowLevel(i, windowSlider->value(), d);
    });
    // init
    // this->OnSetWindowLevel(i, init_window, init_level);
    row++;
    QPushButton* autoButton = new QPushButton("Auto");
    sectionLayout->addWidget(autoButton, row, 0);
    QObject::connect(autoButton, &QPushButton::clicked, [this, i, windowSlider, levelSlider]() {
      float w, l;
      this->m_scene->m_volume->channel((uint32_t)i)->generate_auto(w, l);
      // LOG_DEBUG << "Window/level: " << w << " , " << l;
      windowSlider->setValue(w, true);
      levelSlider->setValue(l, true);
      this->m_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(TransferFunctionDirty);
    });
    QPushButton* auto2Button = new QPushButton("Auto2");
    sectionLayout->addWidget(auto2Button, row, 1);
    QObject::connect(auto2Button, &QPushButton::clicked, [this, i, windowSlider, levelSlider]() {
      float w, l;
      this->m_scene->m_volume->channel((uint32_t)i)->generate_auto2(w, l);
      // LOG_DEBUG << "Window/level: " << w << " , " << l;
      windowSlider->setValue(w, true);
      levelSlider->setValue(l, true);
      this->m_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(TransferFunctionDirty);
    });
    QPushButton* bestfitButton = new QPushButton("BestFit");
    sectionLayout->addWidget(bestfitButton, row, 2);
    QObject::connect(bestfitButton, &QPushButton::clicked, [this, i, windowSlider, levelSlider]() {
      float w, l;
      this->m_scene->m_volume->channel((uint32_t)i)->generate_bestFit(w, l);
      windowSlider->setValue(w, true);
      levelSlider->setValue(l, true);
      // LOG_DEBUG << "Window/level: " << w << " , " << l;
      this->m_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(TransferFunctionDirty);
    });
    QPushButton* chimeraxButton = new QPushButton("ChimX");
    sectionLayout->addWidget(chimeraxButton, row, 3);
    QObject::connect(chimeraxButton, &QPushButton::clicked, [this, i]() {
      this->m_scene->m_volume->channel((uint32_t)i)->generate_chimerax();
      this->m_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(TransferFunctionDirty);
    });
    QPushButton* eqButton = new QPushButton("Eq");
    sectionLayout->addWidget(eqButton, row, 4);
    QObject::connect(eqButton, &QPushButton::clicked, [this, i]() {
      this->m_scene->m_volume->channel((uint32_t)i)->generate_equalized();
      this->m_transferFunction->renderSettings()->m_DirtyFlags.SetFlag(TransferFunctionDirty);
    });

    row++;
    sectionLayout->addWidget(new QLabel("Opacity"), row, 0);
    QNumericSlider* opacitySlider = new QNumericSlider();
    opacitySlider->setRange(0.0, 1.0);
    opacitySlider->setValue(scene->m_material.m_opacity[i], true);
    sectionLayout->addWidget(opacitySlider, row, 1, 1, 2);

    QObject::connect(
      opacitySlider, &QNumericSlider::valueChanged, [i, this](double d) { this->OnOpacityChanged(i, d); });
    // init
    this->OnOpacityChanged(i, scene->m_material.m_opacity[i]);

    row++;
    QColorPushButton* diffuseColorButton = new QColorPushButton();
    QColor cdiff = QColor::fromRgbF(scene->m_material.m_diffuse[i * 3 + 0],
                                    scene->m_material.m_diffuse[i * 3 + 1],
                                    scene->m_material.m_diffuse[i * 3 + 2]);
    diffuseColorButton->SetColor(cdiff, true);
    sectionLayout->addWidget(new QLabel("DiffuseColor"), row, 0);
    sectionLayout->addWidget(diffuseColorButton, row, 2);
    QObject::connect(diffuseColorButton, &QColorPushButton::currentColorChanged, [i, this](const QColor& c) {
      this->OnDiffuseColorChanged(i, c);
    });
    // init
    this->OnDiffuseColorChanged(i, cdiff);

    row++;
    QColorPushButton* specularColorButton = new QColorPushButton();
    QColor cspec = QColor::fromRgbF(scene->m_material.m_specular[i * 3 + 0],
                                    scene->m_material.m_specular[i * 3 + 1],
                                    scene->m_material.m_specular[i * 3 + 2]);
    specularColorButton->SetColor(cspec, true);
    sectionLayout->addWidget(new QLabel("SpecularColor"), row, 0);
    sectionLayout->addWidget(specularColorButton, row, 2);
    QObject::connect(specularColorButton, &QColorPushButton::currentColorChanged, [i, this](const QColor& c) {
      this->OnSpecularColorChanged(i, c);
    });
    // init
    this->OnSpecularColorChanged(i, cspec);

    row++;
    QColorPushButton* emissiveColorButton = new QColorPushButton();
    QColor cemis = QColor::fromRgbF(scene->m_material.m_emissive[i * 3 + 0],
                                    scene->m_material.m_emissive[i * 3 + 1],
                                    scene->m_material.m_emissive[i * 3 + 2]);
    emissiveColorButton->SetColor(cemis, true);
    sectionLayout->addWidget(new QLabel("EmissiveColor"), row, 0);
    sectionLayout->addWidget(emissiveColorButton, row, 2);
    QObject::connect(emissiveColorButton, &QColorPushButton::currentColorChanged, [i, this](const QColor& c) {
      this->OnEmissiveColorChanged(i, c);
    });
    // init
    this->OnEmissiveColorChanged(i, cemis);

    row++;
    sectionLayout->addWidget(new QLabel("Glossiness"), row, 0);
    QNumericSlider* roughnessSlider = new QNumericSlider();
    roughnessSlider->setRange(0.0, 100.0);
    roughnessSlider->setValue(scene->m_material.m_roughness[i]);
    sectionLayout->addWidget(roughnessSlider, row, 1, 1, 2);
    QObject::connect(
      roughnessSlider, &QNumericSlider::valueChanged, [i, this](double d) { this->OnRoughnessChanged(i, d); });
    this->OnRoughnessChanged(i, scene->m_material.m_roughness[i]);

    QObject::connect(section, &Section::checked, [i, this](bool is_checked) { this->OnChannelChecked(i, is_checked); });
    this->OnChannelChecked(i, channelenabled);

    section->setContentLayout(*sectionLayout);
    m_MainLayout.addWidget(section, 15 + i, 0, 1, -1);
    m_channelSections.push_back(section);
  }
}
