#include "Stable.h"

#include "AppearanceSettingsWidget.h"
#include "TransferFunction.h"
#include "RenderThread.h"
#include "Scene.h"

QAppearanceSettingsWidget::QAppearanceSettingsWidget(QWidget* pParent, QTransferFunction* tran, CScene* scene) :
	QGroupBox(pParent),
	m_MainLayout(),
	m_DensityScaleSlider(),
	m_DensityScaleSpinner(),
	m_RendererType(),
	m_ShadingType(),
	m_GradientFactorLabel(),
	m_GradientFactorSlider(),
	m_GradientFactorSpinner(),
	m_StepSizePrimaryRaySlider(),
	m_StepSizePrimaryRaySpinner(),
	m_StepSizeSecondaryRaySlider(),
	m_StepSizeSecondaryRaySpinner(),
	m_DiffuseColorButton(),
	m_SpecularColorButton(),
	m_EmissiveColorButton(),
	_transferFunction(tran)
{
	setLayout(&m_MainLayout);

	m_MainLayout.addWidget(new QLabel("Renderer"), 1, 0);
	m_RendererType.addItem("OpenGL simple", 0);
	m_RendererType.addItem("CUDA full", 1);
	m_RendererType.setCurrentIndex(1);
	m_MainLayout.addWidget(&m_RendererType, 1, 1, 1, 2);

	m_MainLayout.addWidget(new QLabel("Density Scale"), 2, 0);

	m_DensityScaleSlider.setOrientation(Qt::Horizontal);
	m_DensityScaleSlider.setRange(0.001, 100.0);
	m_DensityScaleSlider.setValue(scene->m_DensityScale);
	m_MainLayout.addWidget(&m_DensityScaleSlider, 2, 1);

	m_DensityScaleSpinner.setRange(0.001, 100.0);
	m_DensityScaleSpinner.setDecimals(3);
	m_DensityScaleSpinner.setValue(scene->m_DensityScale);
	m_MainLayout.addWidget(&m_DensityScaleSpinner, 2, 2);

	m_MainLayout.addWidget(new QLabel("Shading Type"), 3, 0);

	m_ShadingType.addItem("BRDF Only", 0);
	m_ShadingType.addItem("Phase Function Only", 1);
	m_ShadingType.addItem("Hybrid", 2);
	m_MainLayout.addWidget(&m_ShadingType, 3, 1, 1, 2);

	m_GradientFactorLabel.setText("Gradient Factor");
	m_MainLayout.addWidget(&m_GradientFactorLabel, 4, 0);
	
	m_GradientFactorSlider.setRange(0.001, 100.0);
	m_GradientFactorSlider.setValue(scene->m_GradientFactor);

	m_MainLayout.addWidget(&m_GradientFactorSlider, 4, 1);

	m_GradientFactorSpinner.setRange(0.001, 100.0);
	m_GradientFactorSpinner.setDecimals(3);
	m_GradientFactorSpinner.setValue(scene->m_GradientFactor);

	m_MainLayout.addWidget(&m_GradientFactorSpinner, 4, 2);

	QObject::connect(&m_DensityScaleSlider, SIGNAL(valueChanged(double)), &m_DensityScaleSpinner, SLOT(setValue(double)));
	QObject::connect(&m_DensityScaleSpinner, SIGNAL(valueChanged(double)), &m_DensityScaleSlider, SLOT(setValue(double)));
	QObject::connect(&m_DensityScaleSlider, SIGNAL(valueChanged(double)), this, SLOT(OnSetDensityScale(double)));

	QObject::connect(&m_GradientFactorSlider, SIGNAL(valueChanged(double)), &m_GradientFactorSpinner, SLOT(setValue(double)));
	QObject::connect(&m_GradientFactorSpinner, SIGNAL(valueChanged(double)), &m_GradientFactorSlider, SLOT(setValue(double)));
	QObject::connect(&m_GradientFactorSlider, SIGNAL(valueChanged(double)), this, SLOT(OnSetGradientFactor(double)));

	m_MainLayout.addWidget(new QLabel("Primary Step Size"), 5, 0);

	m_StepSizePrimaryRaySlider.setRange(1.0, 10.0);

	m_MainLayout.addWidget(&m_StepSizePrimaryRaySlider, 5, 1);

	m_StepSizePrimaryRaySpinner.setRange(1.0, 10.0);
	m_StepSizePrimaryRaySpinner.setDecimals(2);

	m_MainLayout.addWidget(&m_StepSizePrimaryRaySpinner, 5, 2);

	QObject::connect(&m_StepSizePrimaryRaySlider, SIGNAL(valueChanged(double)), &m_StepSizePrimaryRaySpinner, SLOT(setValue(double)));
	QObject::connect(&m_StepSizePrimaryRaySpinner, SIGNAL(valueChanged(double)), &m_StepSizePrimaryRaySlider, SLOT(setValue(double)));
	QObject::connect(&m_StepSizePrimaryRaySlider, SIGNAL(valueChanged(double)), this, SLOT(OnSetStepSizePrimaryRay(double)));

	m_MainLayout.addWidget(new QLabel("Secondary Step Size"), 6, 0);

	m_StepSizeSecondaryRaySlider.setRange(1.0, 10.0);

	m_MainLayout.addWidget(&m_StepSizeSecondaryRaySlider, 6, 1);

	m_StepSizeSecondaryRaySpinner.setRange(1.0, 10.0);
	m_StepSizeSecondaryRaySpinner.setDecimals(2);

	m_MainLayout.addWidget(&m_StepSizeSecondaryRaySpinner, 6, 2);

	QObject::connect(&m_StepSizeSecondaryRaySlider, SIGNAL(valueChanged(double)), &m_StepSizeSecondaryRaySpinner, SLOT(setValue(double)));
	QObject::connect(&m_StepSizeSecondaryRaySpinner, SIGNAL(valueChanged(double)), &m_StepSizeSecondaryRaySlider, SLOT(setValue(double)));
	QObject::connect(&m_StepSizeSecondaryRaySlider, SIGNAL(valueChanged(double)), this, SLOT(OnSetStepSizeSecondaryRay(double)));


	m_DiffuseColorButton.SetColor(QColor::fromRgbF(scene->m_DiffuseColor[0], scene->m_DiffuseColor[1], scene->m_DiffuseColor[2]), true);
	m_MainLayout.addWidget(new QLabel("DiffuseColor"), 7, 0);
	m_MainLayout.addWidget(&m_DiffuseColorButton, 7, 2);
	QObject::connect(&m_DiffuseColorButton, SIGNAL(currentColorChanged(const QColor&)), this, SLOT(OnDiffuseColorChanged(const QColor&)));
	m_SpecularColorButton.SetColor(QColor::fromRgbF(scene->m_SpecularColor[0], scene->m_SpecularColor[1], scene->m_SpecularColor[2]), true);
	m_MainLayout.addWidget(new QLabel("SpecularColor"), 8, 0);
	m_MainLayout.addWidget(&m_SpecularColorButton, 8, 2);
	QObject::connect(&m_SpecularColorButton, SIGNAL(currentColorChanged(const QColor&)), this, SLOT(OnSpecularColorChanged(const QColor&)));
	m_EmissiveColorButton.SetColor(QColor::fromRgbF(scene->m_EmissiveColor[0], scene->m_EmissiveColor[1], scene->m_EmissiveColor[2]), true);
	m_MainLayout.addWidget(new QLabel("EmissiveColor"), 9, 0);
	m_MainLayout.addWidget(&m_EmissiveColorButton, 9, 2);
	QObject::connect(&m_EmissiveColorButton, SIGNAL(currentColorChanged(const QColor&)), this, SLOT(OnEmissiveColorChanged(const QColor&)));

	m_MainLayout.addWidget(new QLabel("Window"), 10, 0);
	m_WindowSlider.setRange(0.001, 1.0);
	m_WindowSlider.setValue(_transferFunction->GetWindow());
	m_MainLayout.addWidget(&m_WindowSlider, 10, 1);

	m_MainLayout.addWidget(new QLabel("Level"), 11, 0);
	m_LevelSlider.setRange(0.001, 1.0);
	m_LevelSlider.setValue(_transferFunction->GetLevel());
	m_MainLayout.addWidget(&m_LevelSlider, 11, 1);

	QObject::connect(&m_WindowSlider, SIGNAL(valueChanged(double)), this, SLOT(OnSetWindow(double)));
	QObject::connect(&m_LevelSlider, SIGNAL(valueChanged(double)), this, SLOT(OnSetLevel(double)));


	QObject::connect(&m_RendererType, SIGNAL(currentIndexChanged(int)), this, SLOT(OnSetRendererType(int)));
	QObject::connect(&m_ShadingType, SIGNAL(currentIndexChanged(int)), this, SLOT(OnSetShadingType(int)));
	//QObject::connect(&gStatus, SIGNAL(RenderBegin()), this, SLOT(OnRenderBegin()));
	
	QObject::connect(_transferFunction, SIGNAL(Changed()), this, SLOT(OnTransferFunctionChanged()));

}

void QAppearanceSettingsWidget::OnRenderBegin(void)
{
	m_DensityScaleSlider.setValue(_transferFunction->GetDensityScale());
	m_ShadingType.setCurrentIndex(_transferFunction->GetShadingType());
	m_GradientFactorSlider.setValue(_transferFunction->scene()->m_GradientFactor);

	m_StepSizePrimaryRaySlider.setValue(_transferFunction->scene()->m_StepSizeFactor, true);
	m_StepSizePrimaryRaySpinner.setValue(_transferFunction->scene()->m_StepSizeFactor, true);
	m_StepSizeSecondaryRaySlider.setValue(_transferFunction->scene()->m_StepSizeFactorShadow, true);
	m_StepSizeSecondaryRaySpinner.setValue(_transferFunction->scene()->m_StepSizeFactorShadow, true);
}

void QAppearanceSettingsWidget::OnSetDensityScale(double DensityScale)
{
	_transferFunction->SetDensityScale(DensityScale);
}

void QAppearanceSettingsWidget::OnSetShadingType(int Index)
{
	_transferFunction->SetShadingType(Index);
	m_GradientFactorLabel.setEnabled(Index == 2);
	m_GradientFactorSlider.setEnabled(Index == 2);
	m_GradientFactorSpinner.setEnabled(Index == 2);
}

void QAppearanceSettingsWidget::OnSetRendererType(int Index)
{
	_transferFunction->SetRendererType(Index);
}

void QAppearanceSettingsWidget::OnSetGradientFactor(double GradientFactor)
{
	_transferFunction->SetGradientFactor(GradientFactor);
}
void QAppearanceSettingsWidget::OnSetWindow(double window)
{
	_transferFunction->SetWindow(window);
}
void QAppearanceSettingsWidget::OnSetLevel(double level)
{
	_transferFunction->SetLevel(level);
}

void QAppearanceSettingsWidget::OnSetStepSizePrimaryRay(const double& StepSizePrimaryRay)
{
	_transferFunction->scene()->m_StepSizeFactor = (float)StepSizePrimaryRay;
	_transferFunction->scene()->m_DirtyFlags.SetFlag(RenderParamsDirty);
}

void QAppearanceSettingsWidget::OnSetStepSizeSecondaryRay(const double& StepSizeSecondaryRay)
{
	_transferFunction->scene()->m_StepSizeFactorShadow = (float)StepSizeSecondaryRay;
	_transferFunction->scene()->m_DirtyFlags.SetFlag(RenderParamsDirty);
}

void QAppearanceSettingsWidget::OnDiffuseColorChanged(const QColor& color)
{
	qreal rgba[4];
	color.getRgbF(&rgba[0], &rgba[1], &rgba[2], &rgba[3]);
	_transferFunction->scene()->m_DiffuseColor[0] = rgba[0];
	_transferFunction->scene()->m_DiffuseColor[1] = rgba[1];
	_transferFunction->scene()->m_DiffuseColor[2] = rgba[2];
	_transferFunction->scene()->m_DiffuseColor[3] = rgba[3];
	_transferFunction->scene()->m_DirtyFlags.SetFlag(RenderParamsDirty);
}
void QAppearanceSettingsWidget::OnSpecularColorChanged(const QColor& color)
{
	qreal rgba[4];
	color.getRgbF(&rgba[0], &rgba[1], &rgba[2], &rgba[3]);
	_transferFunction->scene()->m_SpecularColor[0] = rgba[0];
	_transferFunction->scene()->m_SpecularColor[1] = rgba[1];
	_transferFunction->scene()->m_SpecularColor[2] = rgba[2];
	_transferFunction->scene()->m_SpecularColor[3] = rgba[3];
	_transferFunction->scene()->m_DirtyFlags.SetFlag(RenderParamsDirty);
}
void QAppearanceSettingsWidget::OnEmissiveColorChanged(const QColor& color)
{
	qreal rgba[4];
	color.getRgbF(&rgba[0], &rgba[1], &rgba[2], &rgba[3]);
	_transferFunction->scene()->m_EmissiveColor[0] = rgba[0];
	_transferFunction->scene()->m_EmissiveColor[1] = rgba[1];
	_transferFunction->scene()->m_EmissiveColor[2] = rgba[2];
	_transferFunction->scene()->m_EmissiveColor[3] = rgba[3];
	_transferFunction->scene()->m_DirtyFlags.SetFlag(RenderParamsDirty);
}

void QAppearanceSettingsWidget::OnTransferFunctionChanged(void)
{
	m_DensityScaleSlider.setValue(_transferFunction->GetDensityScale(), true);
	m_DensityScaleSpinner.setValue(_transferFunction->GetDensityScale(), true);
	m_ShadingType.setCurrentIndex(_transferFunction->GetShadingType());
	m_GradientFactorSlider.setValue(_transferFunction->GetGradientFactor(), true);
	m_GradientFactorSpinner.setValue(_transferFunction->GetGradientFactor(), true);
}