#pragma once

#include <QMainWindow>
#include "ui_QVpj.h"

#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkGenericOpenGLRenderWindow.h>

class QVpj : public QMainWindow
{
	Q_OBJECT

public:
	QVpj(QWidget *parent = nullptr);
	~QVpj();

private:
	Ui::QVpjClass ui;

	vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow;
	vtkSmartPointer<vtkRenderer> renderer;

	void loadDICOMVolume();
};
