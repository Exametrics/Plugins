//##########################################################################
//#                                                                        #
//#                       CLOUDCOMPARE PLUGIN: qExametrics                 #
//#																									                       #
//##########################################################################

    #ifndef Q_EXAMETRICS_PLUGIN_HEADER
    #define Q_EXAMETRICS_PLUGIN_HEADER

    //qCC
    #include "ccStdPluginInterface.h"
    #include <DgmOctree.h>
    #include <ccOctree.h>
    #include <ccOctreeProxy.h>
    #include <QThread>
    #include <QMovie>
    #include <QList>
    #include <QProcess>
    #include <QObject>
// #include "ccStdPluginInterface.h"
    #include <QDoubleSpinBox>
    #include <ccPointCloud.h>
    #include<ccGenericPointCloud.h>
    #include <ccPolyline.h>
    #include <cc2DLabel.h>
    #include <ccPlane.h>
    #include <ccBox.h>
    #include <ccClipBox.h>
    #include <unistd.h>
    #include <iostream>
    #include <iomanip>
    #include "utils.h"
    #include "ccExaLog.h"
    #include "ccExaWorker.h"
    #include<ccCropTool.h>
    /* Init DEFINES */

    // Percentage distance of vector point from point A
    #define COEF_INIT 50
    // Box tolerance
    #define TOLERANCE_INIT 1
    /* Debug DEFINES */
    // Plan is used to position box
    #define DEBUG_PLAN false
    // Box points
    #define DEBUG_BOX_POINTS false


Q_DECLARE_METATYPE(ccOctree::Shared);


class ccExametricsDialog;
class ccExametrics : public QObject, public ccStdPluginInterface
{
	Q_OBJECT
	Q_INTERFACES(ccStdPluginInterface)
	Q_PLUGIN_METADATA(IID "cccorp.cloudcompare.plugin.qExametricst" FILE "info.json")
public:

	//! Default constructor
	explicit ccExametrics(QObject* parent = nullptr);

	//desconstructor
	 ~ccExametrics();

// 	//inherited from ccPluginInterface
// 	virtual QString getName() const override { return "Exametrics"; }
// 	virtual QString getDescription() const override { return "Exametrics plugin (plan intersection view)"; }
// 	virtual QIcon getIcon() const override;

	virtual void stop() override;

	//inherited from ccStdPluginInterface
	void onNewSelection(const ccHObject::Container& selectedEntities) override;
	virtual void getActions(QActionGroup& group) override;
	ExaLog* logger = nullptr;

private:

    /* Members */

    ccHObject* rootLasFile = nullptr;

    // Cloud limits
	double m_boxXWidth = 0;
	double m_boxYWidth = 0;

	// Vector point
	CCVector3d m_vectorPoint = CCVector3d(0,0,0);

	// DB Tree Exametrics group
	ccHObject* m_exametricsGroup = nullptr;
	ccHObject* m_extract = nullptr;


	/* Display purpose variables */
	bool canUpdateGraphics = false;

	// normalized vector cloud (display purpose)
	ccPointCloud* normalizedVectorCloud = nullptr;
	// normalized vector polyline
	ccPolyline* normalizedVectorPoly = nullptr;

	// vector point cloud (display purpose)
	ccPointCloud* vectorPointCloud = nullptr;
	// vector point 2dlabel
	cc2DLabel* vectorPoint2DLabel = nullptr;

	// plan cloud (display purpose)
	ccPointCloud* planCloud = nullptr;

/* */
	//new cloud to test the cell center
	ccPointCloud* newCloud = nullptr;
    cc2DLabel* centrePoint2DLabel = nullptr;
/* */


	// plan
	ccPlane* pPlane = nullptr;
    //box
	ccBox* box = nullptr;




    QList<ccPointCloud*>* tmpPointCloudList = nullptr;
    QList<cc2DLabel*>* tmpPointList = nullptr;
    const int N_point = 8;

	// state
	bool planeIsInDBTree = false;
	/* Initialization methods */

	// spb limits and initial values
	void initializeParameterWidgets();
	// draw vectors and plans basic settings
	void initializeDrawSettings();


	/* On events methods */

	void onNormalizedVectorChanged();
	void onVectorPointChanged(int coef);
	void onParameterChanged(QWidget* w, double value);

    /* Display methods */

	// Update box display
	void updateBox();


	/* Getters */

	double getTolerance();
    double getXwidth();
	double getYwidth();
	double getPx();
	double getPy();
	double getPz();
	/* Other methods*/

	void setGifLoading(bool enabled);

signals:
    void operatePythonWorker(QStringList, ExaLog*);
    void operateOctreeWorker(ccOctree::Shared octree, double tolerance, ExaLog*);


protected slots:

	void doAction();

	//**************
	//GUI actions:
	//**************
	//general
	void onCompute();
	void onClose();
	//plan parameters changed
	void onXwidthSpbChanged(double value);
	void onYwidthSpbChanged(double value);
	void onToleranceSpbChanged(double value);

    void onPxSpbChanged(double value);
    void onPySpbChanged(double value);
    void onPzSpbChanged(double value);


protected:

	//link to application windows
	//useless?
	QMainWindow* m_main_window = nullptr;

	//ccExametrics UI
	ccExametricsDialog* m_dlg = nullptr;

	//icon action
	QAction* m_action;


};

//extern void computeInThread(QStringList arguments, ExaLog* logger);

#endif
