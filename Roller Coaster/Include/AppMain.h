#ifndef APPMAIN_H
#define APPMAIN_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include <QtWidgets/QFileDialog>
#include "ui_AppMain.h"
#include <QtOpenGL/qgl.h>
#include "TrainView.h"  

class CTrack;

class AppMain : public QMainWindow
{
	Q_OBJECT

public:
	AppMain(QWidget *parent = 0);
	~AppMain();

	void ToggleMenuBar();
	void ToggleToolBar();
	void ToggleStatusBar();
	void TogglePanel();

	static AppMain *getInstance();
	static AppMain *Instance;

public:
	// call this method when things change
	void damageMe();

	// this moves the train forward on the track - its up to you to do this
	// correctly. it gets called from the idle callback loop
	// it should handle forward and backwards
	void advanceTrain(float dir = 1);


public:
	// keep track of the stuff in the world
	CTrack				m_Track;

	// the widgets that make up the Window
	TrainView*			trainview;

	//Terrain
	//Terrain				terrain;
	//Square				square;

	bool canpan;
	bool isHover;
	int translateAxis = 0;
	int speedVal = 50;

private:
	void UpdateCameraState( int index );
	void UpdateCurveState( int index );
	void UpdateTrackState( int index );
	void UpdateVelocityState( int index );
	void rollx( float dir );
	void rollz( float dir );
	Ui::AppMainClass ui;

	private slots:
		void LoadTrackPath();
		void SaveTrackPath();
		void ExitApp();

		void ChangeCameraType( QString type );
		void ChangeCamToWorld();
		void ChangeCamToTop();
		void ChangeCamToTrain();

		void ChangeCurveType( QString type );
		void ChangeCurveToLinear();
		void ChangeCurveToCardinal();
		void ChangeCurveToCubic();

		void ChangeTrackType( QString type );
		void ChangeTrackToLine();
		void ChangeTrackToTrack();
		void ChangeTrackToRoad();

		void ChangeTerrainDesert();
		void ChangeTerrainCobble();
		void ChangeTerrainStump();
		void ChangeTerrainBeitun();

		void ChangeTunnelOn();
		void ChangeTunnelOff();

		void ChangeWaveSine();
		void ChangeWaveHeight();

		void SwitchPlayAndPause();
		void ChangeSpeedOfTrain( int val );

		void AddControlPoint();
		void DeleteControlPoint();

		void ChangeTranslate(QString type);

		void RotateControlPointAddX();
		void RotateControlPointSubX();
		void RotateControlPointAddZ();
		void RotateControlPointSubZ();

protected:
	bool eventFilter(QObject *watched, QEvent *e); 
};

#endif // APPMAIN_H
