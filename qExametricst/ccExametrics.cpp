
//##########################################################################
//#                                                                        #
//#                       CLOUDCOMPARE PLUGIN: qExametrics                 #
//#                                                                        #
//##########################################################################

#include "ccExametrics.h"
#include "ccExametricsDialog.h"

//Qt
#include <QtGui>
#include<QTime>

/* Default constructor */
ccExametrics::ccExametrics( QObject *parent )
        : QObject( parent )
        , ccStdPluginInterface( ":/CC/plugin/qExametricst/info.json" )
        , m_action( nullptr )
{
}
/* Deconstructor */
ccExametrics::~ccExametrics()
{
    if (m_dlg)
        delete m_dlg;
}
//This method should enable or disable each plugin action
//depending on the currently selected entities ('selectedEntities').
//For example: if none of the selected entities is a cloud, and your
//plugin deals only with clouds, call 'm_action->setEnabled(false)'
void ccExametrics::onNewSelection(const ccHObject::Container& selectedEntities)
{
    //m_action->setEnabled(m_app && m_app->dbRootObject() && m_app->dbRootObject()->getChildrenNumber() != 0);

    /*Enable EXAMETREICS Plugin only when you select a point cloud */
    m_action->setEnabled(selectedEntities.size()==1 && selectedEntities[0]->isA(CC_TYPES::POINT_CLOUD));

    if (!m_dlg)
    {
        return; //not initialized yet - ignore callback
    }
}

//This method returns all 'actions' of your plugin.
//It will be called only once, when plugin is loaded.
void ccExametrics::getActions(QActionGroup& group)
{
    //default action (if it has not been already created, it's the moment to do it)
    if (!m_action)
    {
        //here we use the default plugin name, description and icon,
        //but each action can have its own!
        m_action = new QAction(getName(),this);
        m_action->setToolTip(getDescription());
        m_action->setIcon(getIcon());
        //connect appropriate signal
        connect(m_action, SIGNAL(triggered()), this, SLOT(doAction()));
    }

    group.addAction(m_action);


    this->logger = new ExaLog(m_app);

    logger->logInfo("Plugin succesfully initialized!");
}

//This is an example of an action's slot called when the corresponding action
//is triggered (i.e. the corresponding icon or menu entry is clicked in CC's
//main interface). You can access to most of CC components (database,
//3D views, console, etc.) via the 'm_app' attribute (ccMainAppInterface
//object).
void ccExametrics::doAction()
{
    //m_app should have already been initialized by CC when plugin is loaded!
    //(--> pure internal check)
    assert(m_app);
    if (!m_app)
        return;

    /*** HERE STARTS THE ACTION ***/

    //check valid window
    if (!m_app->getActiveGLWindow())
    {
        logger->logError("Could not find valid 3D window.");
        return;
    }

    //bind GUI events
    m_dlg = new ccExametricsDialog((QWidget*)m_app->getMainWindow());

    // initialize parameters widgets with the object in db tree
    this->rootLasFile = m_app->dbRootObject()->getChild(0);
    // for random values
    srand (time(NULL));
    initializeParameterWidgets();

    //general
    ccExametricsDialog::connect(m_dlg->computeButton, SIGNAL(clicked()), this, SLOT(onCompute()));
    ccExametricsDialog::connect(m_dlg->closeButton, SIGNAL(clicked()), this, SLOT(onClose()));
    //parameter changed
    ccExametricsDialog::connect(m_dlg->spbX, SIGNAL(valueChanged(double)), this, SLOT(onXwidthSpbChanged(double)));
    ccExametricsDialog::connect(m_dlg->spbY, SIGNAL(valueChanged(double)), this, SLOT(onYwidthSpbChanged(double)));
    //POSITION PARAMETER
    ccExametricsDialog::connect(m_dlg->spbPx, SIGNAL(valueChanged(double)), this, SLOT(onPxSpbChanged(double)));
    ccExametricsDialog::connect(m_dlg->spbPy, SIGNAL(valueChanged(double)), this, SLOT(onPySpbChanged(double)));
    ccExametricsDialog::connect(m_dlg->spbPz, SIGNAL(valueChanged(double)), this, SLOT(onPzSpbChanged(double)));
    //Tolerance Parameter
    ccExametricsDialog::connect(m_dlg->toleranceSpb, SIGNAL(valueChanged(double)), this, SLOT(onToleranceSpbChanged(double)));
    // initialize polylines etc
    initializeDrawSettings();

    m_dlg->linkWith(m_app->getActiveGLWindow());

    //start GUI
    m_app->registerOverlayDialog(m_dlg, Qt::TopRightCorner);
    m_dlg->start();
}

/* Called when the plugin is being stopped */
void ccExametrics::stop()
{
    //remove overlay GUI
    if (m_dlg)
    {
        m_dlg->stop(true);
        m_app->unregisterOverlayDialog(m_dlg);
    }

    //redraw
    if (m_app->getActiveGLWindow())
    {
        m_app->getActiveGLWindow()->redraw(true, false);
    }


    m_dlg = nullptr;


    ccStdPluginInterface::stop();
}

/* Return the plugin icon */


/* Slot on Compute button click */
void ccExametrics::onCompute()
{

/* crop and extract data*/
            ccPointCloud* cloudx = static_cast<ccPointCloud*>(this->rootLasFile->getChild(0));
            ccGenericPointCloud* boxCloud = this->box->getAssociatedCloud();
            ccBBox bbox = boxCloud->getBB_recursive();
            this->box->refreshBB();
            CCLib::ReferenceCloud* selection =cloudx->crop(bbox,true);
    if (!selection)
		{
			//process failed!
			ccLog::Warning(QString("[Crop] Failed to crop cloud '%1'!").arg(cloudx->getName()));
        }
        if (selection->size() == 0)
		{
			//no points fall inside selection!
			ccLog::Warning(QString("[Crop] No point of the cloud falls Inside the input box!"));
			delete selection;
			selection = 0;

		}
		else{
		//crop
            ccPointCloud* croppedEnt = cloudx->partialClone(selection);
            delete selection;
            selection = 0;
            croppedEnt->setDisplay(m_app->getActiveGLWindow());
            this->m_exametricsGroup->addChild(croppedEnt);

/* Save the new .las file*/
            QString tmpFileName = this->rootLasFile->getName();
            int parenthesis = tmpFileName.indexOf('(');
            QString path = tmpFileName.right(tmpFileName.length() - parenthesis).remove('(').remove(')');
            //std::time_t t = std::time(0);
             QTime t= 	t.currentTime();
             QString time= t.toString();
            Utils::saveCloudToLasFile(ccHObjectCaster::ToGenericPointCloud(croppedEnt), path +QString("/ExtractedCloudAt") + time+QString(".las"), m_dlg);

            }
/* end of crop*/


        }

/* Slot on dialog closed */
void ccExametrics::onClose()
{
    stop();
    // also removes childs of this->m_exametricsGroup
    m_app->removeFromDB(this->m_exametricsGroup, true);
}

/* Initialize plan parameters at random values with min and max limits */
void ccExametrics::initializeParameterWidgets()
{
    logger->logInfo("Initializing parameters widgets with informations from \"" + this->rootLasFile->getName() + "\"");

    ccBBox box = this->rootLasFile->getBB_recursive();
    CCVector3 minCorner = box.minCorner();
    CCVector3 maxCorner = box.maxCorner();

    float xBox =2, yBox = 2, zBox = 5;
    xBox = maxCorner.x - minCorner.x;
    yBox = maxCorner.y - minCorner.y;
    zBox = maxCorner.z - minCorner.z;

    this->m_boxXWidth = xBox;
    this->m_boxYWidth = yBox;


    m_dlg->spbX->setMinimum(0.1);
    m_dlg->spbY->setMinimum(0.1);
    m_dlg->spbX->setValue(6);
    m_dlg->spbY->setValue(3);

    m_dlg->spbPx->setValue(2);
    m_dlg->spbPy->setValue(2);
    m_dlg->spbPz->setValue(5);

    // tolerancedisplay cloud
    m_dlg->toleranceSpb->setValue(TOLERANCE_INIT);
}

/* Initialize draw settings for normalized vector, point and plan display */
void ccExametrics::initializeDrawSettings()
{
    // Create directory "Exametrics" in DB tree
    this->m_exametricsGroup = new ccHObject("Exametrics");
    m_app->addToDB(this->m_exametricsGroup, false, true, false, false);
  // this->m_extract = new ccHObject("extracted cloud");
    //m_app->addToDB(this->m_extract, false, true, false, false);

    if(DEBUG_BOX_POINTS || DEBUG_PLAN)
    {
        if(DEBUG_BOX_POINTS && DEBUG_PLAN)
        {
            logger->logInfo("Debug enabled for box points and positionning plan.");
        }
        else if(DEBUG_BOX_POINTS)
        {
            logger->logInfo("Debug enabled for box points.");
        }
        else
        {
            logger->logInfo("Debug enabled for positionning plan.");
        }
    }


    if(DEBUG_BOX_POINTS)
    {
        tmpPointCloudList = new QList<ccPointCloud*>();
        tmpPointList = new QList<cc2DLabel*>();

        for(int i = 0; i < N_point; i++)
        {
            tmpPointCloudList->append(new ccPointCloud("Cloud Point " + QString::number(i)));
            tmpPointCloudList->at(i)->reserve(1);
            tmpPointCloudList->at(i)->addPoint(CCVector3(0,0,0));

            tmpPointList->append(new cc2DLabel("Point " + QString::number(i)));
            tmpPointList->at(i)->setVisible(true);
            tmpPointList->at(i)->setDisplay(m_app->getActiveGLWindow());
            tmpPointList->at(i)->addPoint(tmpPointCloudList->at(i), tmpPointCloudList->at(i)->size() - 1);

            this->m_exametricsGroup->addChild(tmpPointList->at(i));
        }
    }


    /* Box */
    this->updateBox();

    /* Update Graphics*/
    this->canUpdateGraphics = true;
}


void ccExametrics::updateBox()
{
    //return;
    if(this->box)
    {
        this->m_exametricsGroup->removeChild(this->box);
    }
    // dimensions définissant la box
    CCVector3 dimensions = CCVector3(this->getXwidth(), this->getYwidth(), this->getTolerance());
    CCVector3 translation = CCVector3(this->getPx(),this->getPy(),this->getPz());
    //get the plane transformation to apply it for the box
    ccGLMatrix matBox;
    matBox.setTranslation(translation);
    this->box = new ccBox(dimensions, &matBox,"box");
    if(this->box)
    {
        box->showNormals(true);
        box->setColor(ccColor::yellow);
        box->showColors(true);
        box->enableStippling(true);
        box->getOwnBB(true);
        box->refreshBB();
        box->refreshDisplay(true);
        box->refreshDisplay_recursive(true);
        box->getBB_recursive(true);
        box->getDisplayBB_recursive(true);
		box->setVisible(true);
		box->setDisplay(m_app->getActiveGLWindow());
		this->m_exametricsGroup->addChild(this->box);

    }
}

/* Called when Xwidth spinbox changed value */
void ccExametrics::onXwidthSpbChanged(double value)
{
    onParameterChanged(m_dlg->spbX, value);

    if(!this->canUpdateGraphics)
        return;

    this->updateBox();
}
/* Called when Xwidth spinbox changed value */
void ccExametrics::onYwidthSpbChanged(double value)
{
    onParameterChanged(m_dlg->spbY, value);

    if(!this->canUpdateGraphics)
        return;

    this->updateBox();
}
/* Called when tolerance spinbox changed value */
void ccExametrics::onToleranceSpbChanged(double value)
{
    onParameterChanged(m_dlg->toleranceSpb, value);

    if(!this->canUpdateGraphics)
        return;

    this->updateBox();
}

void ccExametrics::onPxSpbChanged(double value)
{
    onParameterChanged(m_dlg->spbPx, value);

    if(!this->canUpdateGraphics)
        return;

    this->updateBox();
}

void ccExametrics::onPySpbChanged(double value)
{
    onParameterChanged(m_dlg->spbPy, value);

    if(!this->canUpdateGraphics)
        return;

    this->updateBox();
}


void ccExametrics::onPzSpbChanged(double value)
{
    onParameterChanged(m_dlg->spbPz, value);

    if(!this->canUpdateGraphics)
        return;

    this->updateBox();
}
/* Called when a parameters of the plan is changing */
void ccExametrics::onParameterChanged(QWidget* w, double value)
{

}

double ccExametrics::getPx()
{
    if(!m_dlg) return 0 ;
    return m_dlg->spbPx->value();
}

double ccExametrics::getPy()
{
    if(!m_dlg) return 0 ;
    return m_dlg->spbPy->value();
}

double ccExametrics::getPz()
{
    if(!m_dlg) return 0 ;
    return m_dlg->spbPz->value();
}

double ccExametrics::getTolerance()
{
    if(!m_dlg) return 0 ;
    return m_dlg->toleranceSpb->value();
}


double ccExametrics::getXwidth()
{
    if(!m_dlg) return 0 ;
    return m_dlg->spbX->value();
}


double ccExametrics::getYwidth()
{
    if(!m_dlg) return 0 ;
    return m_dlg->spbY->value();
}

