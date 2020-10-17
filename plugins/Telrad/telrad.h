#ifndef TELRAD_H
#define TELRAD_H
//#include "GridLinesMgr.hpp"
//#include "LabelMgr.hpp"
//#include "ConstellationMgr.hpp"
//#include "MilkyWay.hpp"
//#include "StelActionMgr.hpp"
#include "StelApp.hpp"
#include "StelCore.hpp"
//#include "StelFileMgr.hpp"
//#include "StelLocaleMgr.hpp"
//#include "StelMainView.hpp"
//#include "StelModuleMgr.hpp"
//#include "StelMovementMgr.hpp"
//#include "StelObjectMgr.hpp"
#include "StelPainter.hpp"
//#include "StelProjector.hpp"
//#include "StelTextureMgr.hpp"
//#include "StelTranslator.hpp"
//#include "SolarSystem.hpp"
//#include "StelUtils.hpp"
//#include "LandscapeMgr.hpp"

#include <QDebug>
#include <QDir>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPixmap>
#include <QSignalMapper>

#include <cmath>
#include <QFont>
#include <QSettings>

class Telrad
{
public:
    Telrad();
    void TelradShow();
};

#endif // TELRAD_H
