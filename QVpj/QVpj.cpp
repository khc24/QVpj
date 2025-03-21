#include "QVpj.h"

#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkDICOMImageReader.h>
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkVolumeProperty.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkVolume.h>
#include <vtkNamedColors.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

#include <QDebug>
#include <QFileDialog>

QVpj::QVpj(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	// 초기 렌더러 설정
	renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
	ui.widget->SetRenderWindow(renderWindow);

	renderer = vtkSmartPointer<vtkRenderer>::New();
	renderer->SetBackground(0.1, 0.2, 0.3); // 어두운 배경
	renderWindow->AddRenderer(renderer);

	// 버튼 클릭 시 DICOM 볼륨 로드
	connect(ui.pushButton, &QPushButton::clicked, this, [this]() {
		loadDICOMVolume();
	});
}

QVpj::~QVpj() {}

void QVpj::loadDICOMVolume()
{
	QString dir = QFileDialog::getExistingDirectory(this, "DICOM 폴더 선택");
	if (dir.isEmpty()) {
		qDebug() << "폴더 선택 안함";
		return;
	}

	// VTK DICOM 리더 설정
	auto reader = vtkSmartPointer<vtkDICOMImageReader>::New();
	QByteArray localPath = dir.toLocal8Bit();  // 한글 경로 호환
	reader->SetDirectoryName(localPath.constData());

	reader->Update();

	// 볼륨 매퍼 설정 (GPU 기반)
	auto volumeMapper = vtkSmartPointer<vtkGPUVolumeRayCastMapper>::New();
	volumeMapper->SetInputConnection(reader->GetOutputPort());

	// 컬러 매핑 (Grayscale)
	auto colorFunc = vtkSmartPointer<vtkColorTransferFunction>::New();
	colorFunc->AddRGBPoint(-1000, 0.0, 0.0, 0.0);   // Air
	colorFunc->AddRGBPoint(0, 1.0, 1.0, 1.0);       // Tissue
	colorFunc->AddRGBPoint(1000, 1.0, 1.0, 1.0);    // Bone

	// 불투명도 매핑
	auto opacityFunc = vtkSmartPointer<vtkPiecewiseFunction>::New();
	opacityFunc->AddPoint(-1000, 0.0);
	opacityFunc->AddPoint(0, 0.1);
	opacityFunc->AddPoint(1000, 0.9);

	auto volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
	volumeProperty->SetColor(colorFunc);
	volumeProperty->SetScalarOpacity(opacityFunc);
	volumeProperty->ShadeOn();
	volumeProperty->SetInterpolationTypeToLinear();

	// 볼륨 객체
	auto volume = vtkSmartPointer<vtkVolume>::New();
	volume->SetMapper(volumeMapper);
	volume->SetProperty(volumeProperty);

	// 기존 볼륨 제거하고 새로 추가
	renderer->RemoveAllViewProps();
	renderer->AddVolume(volume);
	renderer->ResetCamera();
	renderWindow->Render();
}
