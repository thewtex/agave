#pragma once

#include "Aperture.h"
#include "Controls.h"

#include <QtWidgets/QGroupBox>

class QCamera;

class QApertureWidget : public QGroupBox
{
    Q_OBJECT

public:
    QApertureWidget(QWidget* pParent = NULL, QCamera* cam = nullptr);

public slots:
	void SetAperture(const double& Aperture);
	void OnApertureChanged(const QAperture& Aperture);

private:
	QGridLayout		m_GridLayout;
	QDoubleSlider	m_SizeSlider;
	QDoubleSpinner	m_SizeSpinner;

	QCamera* _camera;
};