/*
 * Stellarium
 * Copyright (C) 2002 Fabien Chereau
 * Copyright (C) 2011 Alexander Wolf
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA  02110-1335, USA.
 */

// class used to manage groups of Nebulas

#include <algorithm>
#include <QDebug>
#include <QFile>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QRegExp>
#include <QDir>

#include "StelApp.hpp"
#include "NebulaMgr.hpp"
#include "Nebula.hpp"
#include "StelTexture.hpp"
#include "StelUtils.hpp"
#include "StelSkyDrawer.hpp"
#include "StelTranslator.hpp"
#include "StelTextureMgr.hpp"
#include "StelObjectMgr.hpp"
#include "StelLocaleMgr.hpp"
#include "StelSkyCultureMgr.hpp"
#include "StelFileMgr.hpp"
#include "StelModuleMgr.hpp"
#include "StelCore.hpp"
#include "StelSkyImageTile.hpp"
#include "StelPainter.hpp"
#include "RefractionExtinction.hpp"
#include "StelActionMgr.hpp"

void NebulaMgr::setLabelsColor(const Vec3f& c) {Nebula::labelColor = c;}
const Vec3f &NebulaMgr::getLabelsColor(void) const {return Nebula::labelColor;}
void NebulaMgr::setCirclesColor(const Vec3f& c) {Nebula::circleColor = c;}
const Vec3f &NebulaMgr::getCirclesColor(void) const {return Nebula::circleColor;}
void NebulaMgr::setCircleScale(float scale) {Nebula::circleScale = scale;}
float NebulaMgr::getCircleScale(void) const {return Nebula::circleScale;}


NebulaMgr::NebulaMgr(void) : nebGrid(200)
{
    setObjectName("NebulaMgr");
}

NebulaMgr::~NebulaMgr()
{
    Nebula::texCircle = StelTextureSP();
    Nebula::texCircleLarge = StelTextureSP();
    Nebula::texRegion = StelTextureSP();
    Nebula::texGalaxy = StelTextureSP();
    Nebula::texGalaxyLarge = StelTextureSP();
    Nebula::texOpenCluster = StelTextureSP();
    Nebula::texOpenClusterLarge = StelTextureSP();
    Nebula::texOpenClusterXLarge = StelTextureSP();
    Nebula::texGlobularCluster = StelTextureSP();
    Nebula::texGlobularClusterLarge = StelTextureSP();
    Nebula::texPlanetaryNebula = StelTextureSP();
    Nebula::texDiffuseNebula = StelTextureSP();
    Nebula::texDiffuseNebulaLarge = StelTextureSP();
    Nebula::texDiffuseNebulaXLarge = StelTextureSP();
    Nebula::texDarkNebula = StelTextureSP();
    Nebula::texDarkNebulaLarge = StelTextureSP();
    Nebula::texOpenClusterWithNebulosity = StelTextureSP();
    Nebula::texOpenClusterWithNebulosityLarge = StelTextureSP();
}

/*************************************************************************
 Reimplementation of the getCallOrder method
*************************************************************************/
double NebulaMgr::getCallOrder(StelModuleActionName actionName) const
{
    if (actionName==StelModule::ActionDraw)
        return StelApp::getInstance().getModuleMgr().getModule("MilkyWay")->getCallOrder(actionName)+10;
    return 0;
}

// read from stream
void NebulaMgr::init()
{
    // TODO: mechanism to specify which sets get loaded at start time.
    // candidate methods:
    // 1. config file option (list of sets to load at startup)
    // 2. load all
    // 3. flag in nebula_textures.fab (yuk)
    // 4. info.ini file in each set containing a "load at startup" item
    // For now (0.9.0), just load the default set


    loadNebulaSet("default");

    QSettings* conf = StelApp::getInstance().getSettings();
    Q_ASSERT(conf);

    nebulaFont.setPixelSize(StelApp::getInstance().getSettings()->value("gui/base_font_size", 13).toInt());
    Nebula::texCircle		= StelApp::getInstance().getTextureManager().createTexture(StelFileMgr::getInstallationDir()+"/textures/neb.png");
        // Load circle texture for large DSO
        Nebula::texCircleLarge	= StelApp::getInstance().getTextureManager().createTexture(StelFileMgr::getInstallationDir()+"/textures/neb_lrg.png");
        // Load dashed shape texture
        Nebula::texRegion		= StelApp::getInstance().getTextureManager().createTexture(StelFileMgr::getInstallationDir()+"/textures/neb_reg.png");
        // Load ellipse texture
        Nebula::texGalaxy		= StelApp::getInstance().getTextureManager().createTexture(StelFileMgr::getInstallationDir()+"/textures/neb_gal.png");
        // Load ellipse texture for large galaxies
        Nebula::texGalaxyLarge		= StelApp::getInstance().getTextureManager().createTexture(StelFileMgr::getInstallationDir()+"/textures/neb_gal_lrg.png");
        // Load open cluster marker texture
        Nebula::texOpenCluster		= StelApp::getInstance().getTextureManager().createTexture(StelFileMgr::getInstallationDir()+"/textures/neb_ocl.png");
        // Load open cluster marker texture for large objects
        Nebula::texOpenClusterLarge	= StelApp::getInstance().getTextureManager().createTexture(StelFileMgr::getInstallationDir()+"/textures/neb_ocl_lrg.png");
        // Load open cluster marker texture for extra large objects
        Nebula::texOpenClusterXLarge	= StelApp::getInstance().getTextureManager().createTexture(StelFileMgr::getInstallationDir()+"/textures/neb_ocl_xlrg.png");
        // Load globular cluster marker texture
        Nebula::texGlobularCluster	= StelApp::getInstance().getTextureManager().createTexture(StelFileMgr::getInstallationDir()+"/textures/neb_gcl.png");
        // Load globular cluster marker texture for large GCls
        Nebula::texGlobularClusterLarge	= StelApp::getInstance().getTextureManager().createTexture(StelFileMgr::getInstallationDir()+"/textures/neb_gcl_lrg.png");
        // Load planetary nebula marker texture
        Nebula::texPlanetaryNebula	= StelApp::getInstance().getTextureManager().createTexture(StelFileMgr::getInstallationDir()+"/textures/neb_pnb.png");
        // Load diffuse nebula marker texture
        Nebula::texDiffuseNebula	= StelApp::getInstance().getTextureManager().createTexture(StelFileMgr::getInstallationDir()+"/textures/neb_dif.png");
        // Load diffuse nebula marker texture for large DSO
        Nebula::texDiffuseNebulaLarge	= StelApp::getInstance().getTextureManager().createTexture(StelFileMgr::getInstallationDir()+"/textures/neb_dif_lrg.png");
        // Load diffuse nebula marker texture for extra large DSO
        Nebula::texDiffuseNebulaXLarge	= StelApp::getInstance().getTextureManager().createTexture(StelFileMgr::getInstallationDir()+"/textures/neb_dif_xlrg.png");
        // Load dark nebula marker texture
        Nebula::texDarkNebula		= StelApp::getInstance().getTextureManager().createTexture(StelFileMgr::getInstallationDir()+"/textures/neb_drk.png");
        // Load dark nebula marker texture for large DSO
        Nebula::texDarkNebulaLarge	= StelApp::getInstance().getTextureManager().createTexture(StelFileMgr::getInstallationDir()+"/textures/neb_drk_lrg.png");
        // Load Ocl/Nebula marker texture
        Nebula::texOpenClusterWithNebulosity = StelApp::getInstance().getTextureManager().createTexture(StelFileMgr::getInstallationDir()+"/textures/neb_ocln.png");
        // Load Ocl/Nebula marker texture for large objects
        Nebula::texOpenClusterWithNebulosityLarge = StelApp::getInstance().getTextureManager().createTexture(StelFileMgr::getInstallationDir()+"/textures/neb_ocln_lrg.png");
        // Load pointer texture
        texPointer = StelApp::getInstance().getTextureManager().createTexture(StelFileMgr::getInstallationDir()+"/textures/pointeur5.png");

    texPointer = StelApp::getInstance().getTextureManager().createTexture(StelFileMgr::getInstallationDir()+"/textures/pointeur5.png");   // Load pointer texture

    setFlagShow(conf->value("astro/flag_nebula",true).toBool());
    setFlagHints(conf->value("astro/flag_nebula_name",false).toBool());
    setHintsAmount(conf->value("astro/nebula_hints_amount", 3).toFloat());
    setLabelsAmount(conf->value("astro/nebula_labels_amount", 3).toFloat());
    setCircleScale(conf->value("astro/nebula_scale",1.0f).toFloat());

    updateI18n();

    StelApp *app = &StelApp::getInstance();
    connect(app, SIGNAL(languageChanged()), this, SLOT(updateI18n()));
    connect(app, SIGNAL(colorSchemeChanged(const QString&)), this, SLOT(setStelStyle(const QString&)));
    GETSTELMODULE(StelObjectMgr)->registerStelObjectMgr(this);

    addAction("actionShow_Nebulas", N_("Display Options"), N_("Deep-sky objects"), "flagHintDisplayed", "D", "N");
}

struct DrawNebulaFuncObject
{
    DrawNebulaFuncObject(float amaxMagHints, float amaxMagLabels, StelPainter* p, StelCore* aCore, bool acheckMaxMagHints) : maxMagHints(amaxMagHints), maxMagLabels(amaxMagLabels), sPainter(p), core(aCore), checkMaxMagHints(acheckMaxMagHints)
    {
        angularSizeLimit = 5.f/sPainter->getProjector()->getPixelPerRadAtCenter()*180.f/M_PI;
    }
    void operator()(StelRegionObject* obj)
    {
        Nebula* n = static_cast<Nebula*>(obj);
        StelSkyDrawer *drawer = core->getSkyDrawer();
        // filter out DSOs which are too dim to be seen (e.g. for bino observers)
        if ((drawer->getFlagNebulaMagnitudeLimit()) && (n->mag > drawer->getCustomNebulaMagnitudeLimit())) return;
        //silas
        if (n->angularSize>angularSizeLimit || n->angularSize == 0 || (checkMaxMagHints && n->mag <= maxMagHints))
        {
            float refmag_add=0; // value to adjust hints visibility threshold.
            sPainter->getProjector()->project(n->XYZ,n->XY);
            n->drawLabel(*sPainter, maxMagLabels-refmag_add);
            n->drawHints(*sPainter, maxMagHints -refmag_add);
        }
    }
    float maxMagHints;
    float maxMagLabels;
    StelPainter* sPainter;
    StelCore* core;
    float angularSizeLimit;
    bool checkMaxMagHints;
};

float NebulaMgr::computeMaxMagHint(const StelSkyDrawer* skyDrawer) const
{
    return skyDrawer->getLimitMagnitude()*1.2f-2.f+(hintsAmount *1.2f)-2.f;
}

// Draw all the Nebulae
void NebulaMgr::draw(StelCore* core)
{
    const StelProjectorP prj = core->getProjection(StelCore::FrameJ2000);
    StelPainter sPainter(prj);

    StelSkyDrawer* skyDrawer = core->getSkyDrawer();

    Nebula::hintsBrightness = hintsFader.getInterstate()*flagShow.getInterstate();

    sPainter.enableTexture2d(true);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    // Use a 1 degree margin
    const double margin = 1.*M_PI/180.*prj->getPixelPerRadAtCenter();
    const SphericalRegionP& p = prj->getViewportConvexPolygon(margin, margin);

    // Print all the nebulae of all the selected zones
    float maxMagHints  = computeMaxMagHint(skyDrawer);
    float maxMagLabels = skyDrawer->getLimitMagnitude()     -2.f+(labelsAmount*1.2f)-2.f;
    sPainter.setFont(nebulaFont);
    DrawNebulaFuncObject func(maxMagHints, maxMagLabels, &sPainter, core, hintsFader.getInterstate()>0.0001);
    nebGrid.processIntersectingPointInRegions(p.data(), func);

    if (GETSTELMODULE(StelObjectMgr)->getFlagSelectedObjectPointer())
        drawPointer(core, sPainter);
}

void NebulaMgr::drawPointer(const StelCore* core, StelPainter& sPainter)
{
    const StelProjectorP prj = core->getProjection(StelCore::FrameJ2000);

    const QList<StelObjectP> newSelected = GETSTELMODULE(StelObjectMgr)->getSelectedObject("Nebula");
    if (!newSelected.empty())
    {
        const StelObjectP obj = newSelected[0];
        Vec3d pos=obj->getJ2000EquatorialPos(core);

        // Compute 2D pos and return if outside screen
        if (!prj->projectInPlace(pos)) return;
        sPainter.setColor(0.4f,0.5f,0.8f);

        texPointer->bind();

        sPainter.enableTexture2d(true);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Normal transparency mode

        // Size on screen
        float size = obj->getAngularSize(core)*M_PI/180.*prj->getPixelPerRadAtCenter();

        size+=20.f + 10.f*std::sin(2.f * StelApp::getInstance().getTotalRunTime());
        sPainter.drawSprite2dMode(pos[0]-size/2, pos[1]-size/2, 10, 90);
        sPainter.drawSprite2dMode(pos[0]-size/2, pos[1]+size/2, 10, 0);
        sPainter.drawSprite2dMode(pos[0]+size/2, pos[1]+size/2, 10, -90);
        sPainter.drawSprite2dMode(pos[0]+size/2, pos[1]-size/2, 10, -180);
    }
}

void NebulaMgr::setStelStyle(const QString& section)
{
    // Load colors from config file
    QSettings* conf = StelApp::getInstance().getSettings();

    QString defaultColor = conf->value(section+"/default_color").toString();
    setLabelsColor(StelUtils::strToVec3f(conf->value(section+"/nebula_label_color", defaultColor).toString()));
    setCirclesColor(StelUtils::strToVec3f(conf->value(section+"/nebula_circle_color", defaultColor).toString()));
}


// Search by name
NebulaP NebulaMgr::search(const QString& name)
{
    QString uname = name.toUpper();

    foreach (const NebulaP& n, nebArray)
    {
        QString testName = n->getEnglishName().toUpper();
        if (testName==uname) return n;
    }

    // If no match found, try search by catalog reference
    static QRegExp catNumRx("^(M|NGC|IC|C)\\s*(\\d+)$");
    if (catNumRx.exactMatch(uname))
    {
        QString cat = catNumRx.capturedTexts().at(1);
        int num = catNumRx.capturedTexts().at(2).toInt();

        if (cat == "M") return searchM(num);
        if (cat == "NGC") return searchNGC(num);
        if (cat == "IC") return searchIC(num);
        if (cat == "C") return searchC(num);
    }



    return NebulaP();
}


/*load map and va ing*/
void NebulaMgr::checkdownObserve(){
    if (checked_show == 0){
        return;
    }
    else{
        QString record;

        QString mapfilePath = StelFileMgr::findFile("dsomap.txt");
        if(mapfilePath == ""){
            mapfilePath = "assets:/nebulae/default/dsomap.txt";
        }
        QFile mapfile(mapfilePath);
        if (mapfile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            while (!mapfile.atEnd())
            {
                record = QString::fromUtf8(mapfile.readLine());
                QRegExp rx("^(.*)\t(.*)$");
                if(rx.exactMatch(record)){
                    dsomap[rx.capturedTexts().at(1).trimmed().toUpper()]=rx.capturedTexts().at(2).trimmed().toUpper();
                    //qDebug()<<"dsomap"<<rx.capturedTexts().at(1).trimmed().toUpper()<<rx.capturedTexts().at(2).trimmed().toUpper();
                }
            }
        }
        else{
            qDebug()<<"dsomap not found";
        }


        QString ObservingfilePath = StelFileMgr::findFile("Observing.txt");
        QFile Observingfile(ObservingfilePath);
        if (Observingfile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            while (!Observingfile.atEnd())
            {
                record = QString::fromUtf8(Observingfile.readLine());
                record = record.replace(" ","").replace("\\n","").replace("\n","");
                if(dsomap[record]!=""){
                    listObserving.append(dsomap[record].toUpper());
                }
                else
                    listObserving.append(record.toUpper());
                //qDebug()<<"Observing"<<record<<dsomap[record];
            }
        }

        QString ObservationsfilePath = StelFileMgr::findFile("Observations.txt");
        QFile Observationsfile(ObservationsfilePath);
        if (Observationsfile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            while (!Observationsfile.atEnd())
            {
                record = QString::fromUtf8(Observationsfile.readLine());
                record = record.replace(" ","").replace("\\n","").replace("\n","");
                if(dsomap[record]!=""){
                    listObservations.append(dsomap[record].toUpper());
                }
                else
                    listObservations.append(record.toUpper());
                //listObservations.append(record);
                //qDebug()<<"Observations"<<record<<dsomap[record];
            }
        }

    }
}

bool NebulaMgr::judgeObserved(NebulaP e){

    //all
    //all-va
    //all-va+ing

    QString dsoid( "NGC"+QString::number(e->NGC_nb) );
    if(e->NGC_nb == 0){
        dsoid = "IC"+QString::number(e->IC_nb);
    }

    if (checked_show == 2){
        if (listObservations.contains(dsoid) && !listObserving.contains(dsoid)){
            //qDebug()<<"checked_show=2"<<dsoid;
            return false;
        }
        //qDebug()<<"checked_show=2"<<dsoid<<listObservations.contains(dsoid)<<listObserving.contains(dsoid);
        return true;
    }
    else if(checked_show == 1){
        if (listObservations.contains(dsoid)){
            //qDebug()<<"checked_show=1"<<dsoid<<listObservations.contains(dsoid)<<listObserving.contains(dsoid);
            return false;
        }
        //qDebug()<<"checked_show=1"<<dsoid<<listObservations.contains(dsoid);
        return true;
    }
    else {
        return true;
    }
}

void NebulaMgr::loadNebulaSet(const QString& setName)
{


    QSettings* conf = StelApp::getInstance().getSettings();
    Q_ASSERT(conf);

    //set ini
    checked_show = conf->value("nebula_range/checked_show",2).toInt();
    limit_mag_show = conf->value("nebula_range/limit_mag_show", 99).toFloat();
    flag_nomag_show = conf->value("nebula_range/flag_nomag_show", true).toBool();
    limit_size_show = conf->value("nebula_range/limit_size_show", 0).toFloat();
    flag_nosize_show = conf->value("nebula_range/flag_nosize_show", true).toBool();
    limit_surfbr_show = conf->value("nebula_range/limit_surfbr_show", 99).toFloat();
    flag_nosurfbr_show = conf->value("nebula_range/flag_nosurfbr_show", true).toBool();

    Messier_show = conf->value("nebula_catalog/Messier_show", true).toBool();
    Caldwell_show = conf->value("nebula_catalog/Caldwell_show", true).toBool();
    NGC_show = conf->value("nebula_catalog/NGC_show", true).toBool();
    IC_show = conf->value("nebula_catalog/IC_show", true).toBool();
    Other_show = conf->value("nebula_catalog/Other_show", true).toBool();

    Gxy_show = conf->value("nebula_type/Gxy_show", true).toBool();
    OCL_show = conf->value("nebula_type/OCL_show", true).toBool();
    GCL_show = conf->value("nebula_type/GCL_show", true).toBool();
    Nb_show = conf->value("nebula_type/Nb_show", true).toBool();
    PN_show = conf->value("nebula_type/PN_show", true).toBool();
    NotFound_show = conf->value("nebula_type/NotFound_show", false).toBool();
    SNR_show = conf->value("nebula_type/SNR_show", true).toBool();
    GxyP_show = conf->value("nebula_type/GxyP_show", true).toBool();
    Ass_show = conf->value("nebula_type/Ass_show", true).toBool();
    StarCloud_show = conf->value("nebula_type/StarCloud_show", true).toBool();
    Stars_show = conf->value("nebula_type/Stars_show", true).toBool();
    CarbonStar_show = conf->value("nebula_type/CarbonStar_show", true).toBool();
    Dup_show = conf->value("nebula_type/Dup_show", false).toBool();
    DarkNebula_show = conf->value("nebula_type/DarkNebula_show", true).toBool();
    Cluster_nebula_show = conf->value("nebula_type/Cluster_nebula_show", true).toBool();
    Gxy_Cluster_show = conf->value("nebula_type/Gxy_Cluster_show", true).toBool();
    IFN_show = conf->value("nebula_type/IFN_show", true).toBool();
    DoubleStar_show = conf->value("nebula_type/DoubleStar_show", true).toBool();
    VariableStar_show = conf->value("nebula_type/VariableStar_show", true).toBool();

    //set ini fin

    checkdownObserve();

    listmessier << 1952<< 7089<< 5272<< 6121<< 5904<< 6405<< 6475<< 6523<< 6333<< 6254<< 6705<< 6218<< 6205<< 6402<< 7078<< 6611<< 6618<< 6613<< 6273<< 6514<< 6531<< 6656<< 6494<< 12556<< 12566<< 6694<< 6853<< 6626<< 6913<< 7099<< 224<< 221<< 598<< 1039<< 2168<< 1960<< 2099<< 1912<< 7092<< 15968<< 2287<< 1976<< 1982<< 2632<< 15967<< 2437<< 2422<< 2548<< 4472<< 2323<< 5194<< 7654<< 5024<< 6715<< 6809<< 6779<< 6720<< 4579<< 4621<< 4649<< 4303<< 6266<< 5055<< 4826<< 3623<< 3627<< 2682<< 4590<< 6637<< 6681<< 6838<< 6981<< 6994<< 628<< 6864<< 650<< 1068<< 2068<< 1904<< 6093<< 3031<< 3034<< 5236<< 4374<< 4382<< 4406<< 4486<< 4501<< 4552<< 4569<< 4548<< 6341<< 2447<< 4736<< 3351<< 3368<< 3587<< 4192<< 4254<< 4321<< 5457<< 5866<< 581<< 4594<< 3379<< 4258<< 6171<< 3556<< 3992<< 205;

    listcaldwell << 188<< 40<< 4236<< 7023<< 8183<< 6543<< 2403<< 559<< 15969<< 663<< 7635<< 6946<< 457<< 869<< 6826<< 7243<< 147<< 185<< 12987<< 7000<< 4449<< 7662<< 891<< 1275<< 2419<< 4244<< 6888<< 752<< 5005<< 7331<< 8246<< 4631<< 6992<< 6960<< 4889<< 4559<< 6885<< 4565<< 2392<< 3626<< 15970<< 7006<< 7814<< 7479<< 5248<< 2261<< 6934<< 2775<< 2237<< 2244<< 9454<< 4697<< 3115<< 2506<< 7009<< 246<< 6822<< 2360<< 3242<< 4038<< 4039<< 247<< 7293<< 2362<< 253<< 5694<< 1097<< 6729<< 6302<< 300<< 2477<< 55<< 1851<< 3132<< 6124<< 6231<< 5128<< 6541<< 3201<< 5139<< 6352<< 6193<< 4945<< 5286<< 10232<< 6397<< 1261<< 5823<< 6087<< 2867<< 3532<< 3372<< 6752<< 4755<< 6025<< 2516<< 3766<< 4609<< 15971<< 10785<< 6744<< 10443<< 2070<< 362<< 4833<< 104<< 6101<< 4372<< 3195;

//    QString ngcPath = StelFileMgr::findFile("nebulae/" + setName + "/ngc2000.dat");
//    qDebug()<<ngcPath;
//    QString ngcNamesPath = StelFileMgr::findFile("nebulae/" + setName + "/ngc2000names.dat");

    QString nebula_data_file = conf->value("astro/nebula_data_file").toString();
    QString nebula_name_file = conf->value("astro/nebula_name_file").toString();


    QString ngcPath = StelFileMgr::findFile("nebulae/" + setName + "/"+"dso2020data.dat");
    QString ngcNamesPath = StelFileMgr::findFile("nebulae/" + setName + "/"+"dso2020names.dat");
    //QString ngcPath = StelFileMgr::findFile("ngc2000.dat");
    //QString ngcNamesPath = StelFileMgr::findFile("ngc2000names.dat");
    if (ngcPath.isEmpty() || ngcNamesPath.isEmpty())
    {
        qWarning() << "ERROR while loading nebula data set " << setName;
        return;
    }
    loadNGC(ngcPath);
    loadNGCNames(ngcNamesPath);
}

// Look for a nebulae by XYZ coords
NebulaP NebulaMgr::search(const Vec3d& apos)
{
    Vec3d pos = apos;
    pos.normalize();
    NebulaP plusProche;
    float anglePlusProche=0.;
    foreach (const NebulaP& n, nebArray)
    {
        if (n->XYZ*pos>anglePlusProche)
        {
            anglePlusProche=n->XYZ*pos;
            plusProche=n;
        }
    }
    if (anglePlusProche>0.999)
    {
        return plusProche;
    }
    else return NebulaP();
}


QList<StelObjectP> NebulaMgr::searchAround(const Vec3d& av, double limitFov, const StelCore*) const
{
    QList<StelObjectP> result;
    if (!getFlagShow())
        return result;

    Vec3d v(av);
    v.normalize();
    double cosLimFov = cos(limitFov * M_PI/180.);
    Vec3d equPos;
    foreach (const NebulaP& n, nebArray)
    {
        equPos = n->XYZ;
        equPos.normalize();
        if (equPos*v>=cosLimFov)
        {
            result.push_back(qSharedPointerCast<StelObject>(n));
        }
    }
    return result;
}

NebulaP NebulaMgr::searchM(unsigned int M)
{
    foreach (const NebulaP& n, nebArray)
        if (n->M_nb == M)
            return n;
    return NebulaP();
}

NebulaP NebulaMgr::searchNGC(unsigned int NGC)
{
    if (ngcIndex.contains(NGC))
        return ngcIndex[NGC];
    return NebulaP();
}

NebulaP NebulaMgr::searchIC(unsigned int IC)
{
    foreach (const NebulaP& n, nebArray)
        if (n->IC_nb == IC) return n;
    return NebulaP();
}

NebulaP NebulaMgr::searchC(unsigned int C)
{
    foreach (const NebulaP& n, nebArray)
        if (n->C_nb == C)
            return n;
    return NebulaP();
}


#if 0
// read from stream
bool NebulaMgr::loadNGCOld(const QString& catNGC)
{
    QFile in(catNGC);
    if (!in.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    int totalRecords=0;
    QString record;
    while (!in.atEnd())
    {
        in.readLine();
        ++totalRecords;
    }

    // rewind the file to the start
    in.seek(0);

    int currentLineNumber = 0;	// what input line we are on
    int currentRecordNumber = 0;	// what record number we are on
    int readOk = 0;			// how many records weree rad without problems
    while (!in.atEnd())
    {
        record = QString::fromUtf8(in.readLine());
        ++currentLineNumber;

        // skip comments
        if (record.startsWith("//") || record.startsWith("#"))
            continue;
        ++currentRecordNumber;

        // Create a new Nebula record
        NebulaP e = NebulaP(new Nebula);
        if (!e->readNGC((char*)record.toLocal8Bit().data())) // reading error
        {
            e.clear();
        }
        else
        {
            nebArray.append(e);
            nebGrid.insert(qSharedPointerCast<StelRegionObject>(e));
            if (e->NGC_nb!=0)
                ngcIndex.insert(e->NGC_nb, e);
            ++readOk;
        }
    }
    in.close();
    qDebug() << "Loaded" << readOk << "/" << totalRecords << "NGC records";
    return true;
}
#endif

bool NebulaMgr::judgetoInput(NebulaP e){

    bool catalogFlag = true;
    if(listmessier.contains(e->NGC_nb) || listmessier.contains(e->IC_nb + 7841)){
        if (Messier_show){
            //qDebug()<<e->NGC_nb<<"in mess\n";
            catalogFlag = true;
        }
        else{
            //qDebug()<<e->NGC_nb<<"not in mess\n";
            catalogFlag = false;
        }
    }
    else if(listcaldwell.contains(e->NGC_nb) || listcaldwell.contains(e->IC_nb + 7841)){
        if (Caldwell_show)
            catalogFlag = true;
        else
            catalogFlag = false;
    }
    else if(e->NGC_nb > 0 && e->NGC_nb <= 7840){
        if (NGC_show)
            catalogFlag = true;
        else
            catalogFlag = false;
    }
    else if(e->IC_nb > 0 && e->IC_nb <= 5386){
        if (IC_show)
            catalogFlag = true;
        else
            catalogFlag = false;
    }
    else{
        if (Other_show)
            catalogFlag = true;
        else
            catalogFlag = false;
    }

    bool typeFlag = true;

    const int NebGX=0;
    const int NebGXC=9;
    const int NebNB=3;
    const int NebSNR=10;
    const int NebPN=4;
    const int NebOC=1;
    const int NebGC=2;
    const int NebPrt=11;
    const int NebDN=5;
    const int NebClN=7;
    const int NebAss=12;
    const int NebSCu=13;
    const int NebS=14;
    const int NebCbS=15;
    const int NebIFN=16;
    const int NebDS=17;
    const int NebVS=18;
    const int NebNF=8;
    const int NebDup=19;

    switch (e->nType) {
    case NebGX:
        typeFlag = Gxy_show;
        break;
    case NebGXC:
        typeFlag = Gxy_Cluster_show;
        break;
    case NebNB:
        typeFlag = Nb_show;
        break;
    case NebSNR:
        typeFlag = SNR_show;
        break;
    case NebPN:
        typeFlag = PN_show;
        break;
    case NebOC:
        typeFlag = OCL_show;
        break;
    case NebGC:
        typeFlag = GCL_show;
        break;
    case NebPrt:
        typeFlag = GxyP_show;
        break;
    case NebDN:
        typeFlag = DarkNebula_show;
        break;
    case NebClN:
        typeFlag = Cluster_nebula_show;
        break;
    case NebAss:
        typeFlag = Ass_show;
        break;
    case NebSCu:
        typeFlag = StarCloud_show;
        break;
    case NebS:
        typeFlag = Stars_show;
        break;
    case NebCbS:
        typeFlag = CarbonStar_show;
        break;
    case NebIFN:
        typeFlag = IFN_show;
        break;
    case NebDS:
        typeFlag = DoubleStar_show;
        break;
    case NebVS:
        typeFlag = VariableStar_show;
        break;
    case NebNF:
        typeFlag = NotFound_show;
        break;
    case NebDup:
        typeFlag = Dup_show;
        break;
    default:
        typeFlag = true;
        break;
    }

    bool flagMag = true;
    if(e->mag == 99 && flag_nomag_show){
        flagMag = true;
    }
    else if(e->mag > limit_mag_show){
        flagMag = false;
    }
    else{
        flagMag = true;
    }


    bool flagsurfbr = true;
    if(e->surfBright == 99 && flag_nosurfbr_show){
        flagsurfbr = true;
    }
    else if(e->surfBright > limit_surfbr_show){
        flagsurfbr = false;
    }
    else{
        flagsurfbr = true;
    }


    bool flagSize = true;
    if(e->angularSize == 0 && flag_nosize_show){
        flagSize = true;
    }
    else if(e->angularSize < limit_size_show){
        flagSize = false;
    }
    else{
        flagSize = true;
    }

    //qDebug()<<e->NGC_nb<<e->IC_nb<<e->nType<<catalogFlag<<typeFlag<<tstf;
    return typeFlag&&catalogFlag&&flagMag&&flagSize&&flagsurfbr;
}

bool NebulaMgr::loadNGC(const QString& catNGC)
{
    QFile in(catNGC);
    if (!in.open(QIODevice::ReadOnly))
        return false;
    QDataStream ins(&in);
    ins.setVersion(QDataStream::Qt_4_5);

    int totalRecords=0;
    while (!ins.atEnd())
    {
        // Create a new Nebula record
        NebulaP e = NebulaP(new Nebula);
        e->readNGC(ins);

        if (1){
            //if (judgetoInput(e) && judgeObserved(e)){
            nebArray.append(e);
            nebGrid.insert(qSharedPointerCast<StelRegionObject>(e));
            if (e->NGC_nb!=0)
                ngcIndex.insert(e->NGC_nb, e);
        }
        ++totalRecords;
    }
    in.close();
    qDebug() << "Loaded" << totalRecords << "NGC records";
    return true;
}

bool NebulaMgr::loadNGCNames(const QString& catNGCNames)
{
    qDebug() << "Loading NGC name data ...";
    QFile ngcNameFile(catNGCNames);
    if (!ngcNameFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "NGC name data file" << QDir::toNativeSeparators(catNGCNames) << "not found.";
        return false;
    }

    // Read the names of the NGC objects
    QString name, record;
    int totalRecords=0;
    int lineNumber=0;
    int readOk=0;
    int nb;
    NebulaP e;
    QRegExp commentRx("^(\\s*#.*|\\s*)$");
    QRegExp transRx("_[(]\"(.*)\"[)]");
    while (!ngcNameFile.atEnd())
    {
        record = QString::fromUtf8(ngcNameFile.readLine());
        lineNumber++;
        if (commentRx.exactMatch(record))
            continue;

        totalRecords++;
        nb = record.mid(38,6).toInt();
        if (record[37] == 'I')
        {
            e = searchIC(nb);
        }
        else
        {
            e = searchNGC(nb);
        }

        // get name, trimmed of whitespace
        name = record.left(36).trimmed();

        if (e)
        {
            // If the name is not a messier number perhaps one is already
            // defined for this object
            if (name.left(2).toUpper() != "M " && name.left(2).toUpper() != "C ")
            {
                if (transRx.exactMatch(name)) {
                    e->englishName = transRx.capturedTexts().at(1).trimmed();
                }
                 else
                {
                    e->englishName = name;
                }
            }
            else if (name.left(2).toUpper() != "M " && name.left(2).toUpper() == "C ")
            {
                // If it's a caldwellnumber, we will call it a caldwell if there is no better name
                name = name.mid(2); // remove "C "

                // read the Caldwell number
                QTextStream istr(&name);
                int num;
                istr >> num;
                if (istr.status()!=QTextStream::Ok)
                {
                    qWarning() << "cannot read Caldwell number at line" << lineNumber << "of" << QDir::toNativeSeparators(catNGCNames);
                    continue;
                }

                e->C_nb=(unsigned int)(num);
                e->englishName = QString("C%1").arg(num);
            }
            else if (name.left(2).toUpper() == "M " && name.left(2).toUpper() != "C ")
            {
                // If it's a messiernumber, we will call it a messier if there is no better name
                name = name.mid(2); // remove "M "

                // read the Messier number
                QTextStream istr(&name);
                int num;
                istr >> num;
                if (istr.status()!=QTextStream::Ok)
                {
                    qWarning() << "cannot read Messier number at line" << lineNumber << "of" << QDir::toNativeSeparators(catNGCNames);
                    continue;
                }

                e->M_nb=(unsigned int)(num);
                e->englishName = QString("M%1").arg(num);
            }


            readOk++;
        }
        else
            qWarning() << "no position data for " << name << "at line" << lineNumber << "of" << QDir::toNativeSeparators(catNGCNames);
    }
    ngcNameFile.close();
    qDebug() << "Loaded" << readOk << "/" << totalRecords << "NGC name records successfully";

    return true;
}


void NebulaMgr::updateI18n()
{
    const StelTranslator& trans = StelApp::getInstance().getLocaleMgr().getSkyTranslator();
    foreach (NebulaP n, nebArray)
        n->translateName(trans);
}


//! Return the matching Nebula object's pointer if exists or NULL
StelObjectP NebulaMgr::searchByNameI18n(const QString& nameI18n) const
{
    QString objw = nameI18n.toUpper();

    QRegExp catNumRx8("^N(\\d+)$");
    if (catNumRx8.exactMatch(objw))
    {
        int num = catNumRx8.capturedTexts().at(1).toInt();
        foreach (const NebulaP& n, nebArray)
        {
            if (QString("NGC%1").arg(n->NGC_nb) == QString("NGC%1").arg(num) || QString("NGC %1").arg(n->NGC_nb) == QString("NGC %1").arg(num))
                return qSharedPointerCast<StelObject>(n);
        }
    }
    QRegExp catNumRx81("^I(\\d+)$");
    if (catNumRx81.exactMatch(objw))
    {
        int num = catNumRx81.capturedTexts().at(1).toInt();
        foreach (const NebulaP& n, nebArray)
        {
            if (QString("IC%1").arg(n->IC_nb) == QString("IC%1").arg(num) || QString("IC %1").arg(n->IC_nb) == QString("IC %1").arg(num))
                return qSharedPointerCast<StelObject>(n);
        }
    }



    // Search by NGC numbers (possible formats are "NGC31" or "NGC 31")
    if (objw.mid(0, 3) == "NGC")
    {
        foreach (const NebulaP& n, nebArray)
        {
            if (QString("NGC%1").arg(n->NGC_nb) == objw || QString("NGC %1").arg(n->NGC_nb) == objw)
                return qSharedPointerCast<StelObject>(n);
        }
    }

    // Search by common names
    foreach (const NebulaP& n, nebArray)
    {
        QString objwcap = n->nameI18.toUpper();
        if (objwcap==objw)
            return qSharedPointerCast<StelObject>(n);
    }

    // Search by IC numbers (possible formats are "IC466" or "IC 466")
    if (objw.mid(0, 2) == "IC")
    {
        foreach (const NebulaP& n, nebArray)
        {
            if (QString("IC%1").arg(n->IC_nb) == objw || QString("IC %1").arg(n->IC_nb) == objw)
                return qSharedPointerCast<StelObject>(n);
        }
    }


    // Search by Messier numbers (possible formats are "M31" or "M 31")
    if (objw.mid(0, 1) == "M")
    {
        foreach (const NebulaP& n, nebArray)
        {
            if (QString("M%1").arg(n->M_nb) == objw || QString("M %1").arg(n->M_nb) == objw)
                return qSharedPointerCast<StelObject>(n);
        }
    }

    // Search by Caldwell numbers (possible formats are "C31" or "C 31")
    if (objw.mid(0, 1) == "C")
    {
        foreach (const NebulaP& n, nebArray)
        {
            if (QString("C%1").arg(n->C_nb) == objw || QString("C %1").arg(n->C_nb) == objw)
                return qSharedPointerCast<StelObject>(n);
        }
    }

    return StelObjectP();
}


//! Return the matching Nebula object's pointer if exists or NULL
//! TODO split common parts of this and I18 fn above into a separate fn.
StelObjectP NebulaMgr::searchByName(const QString& name) const
{
    QString objw = name.toUpper();

    //silas
    QRegExp catNumRx1("^(\\d{1,4})$");
    if (catNumRx1.exactMatch(objw))
    {
        int num = catNumRx1.capturedTexts().at(1).toInt();
        foreach (const NebulaP& n, nebArray)
        {
            if (QString("NGC%1").arg(n->NGC_nb) == QString("NGC%1").arg(num) || QString("NGC %1").arg(n->NGC_nb) == QString("NGC %1").arg(num))
                return qSharedPointerCast<StelObject>(n);
        }
    }



    QRegExp catNumRx8("^N(\\d+)$");
    if (catNumRx8.exactMatch(objw))
    {
        int num = catNumRx8.capturedTexts().at(1).toInt();
        foreach (const NebulaP& n, nebArray)
        {
            if (QString("NGC%1").arg(n->NGC_nb) == QString("NGC%1").arg(num) || QString("NGC %1").arg(n->NGC_nb) == QString("NGC %1").arg(num))
                return qSharedPointerCast<StelObject>(n);
        }
    }
    QRegExp catNumRx81("^I(\\d+)$");
    if (catNumRx81.exactMatch(objw))
    {
        int num = catNumRx81.capturedTexts().at(1).toInt();
        foreach (const NebulaP& n, nebArray)
        {
            if (QString("IC%1").arg(n->IC_nb) == QString("IC%1").arg(num) || QString("IC %1").arg(n->IC_nb) == QString("IC %1").arg(num))
                return qSharedPointerCast<StelObject>(n);
        }
    }


    // Search by NGC numbers (possible formats are "NGC31" or "NGC 31")
    if (objw.mid(0, 3) == "NGC")
    {
        foreach (const NebulaP& n, nebArray)
        {
            if (QString("NGC%1").arg(n->NGC_nb) == objw || QString("NGC %1").arg(n->NGC_nb) == objw)
                return qSharedPointerCast<StelObject>(n);
        }
    }

    // Search by common names
    foreach (const NebulaP& n, nebArray)
    {
        QString objwcap = n->englishName.toUpper();
        if (objwcap==objw)
            return qSharedPointerCast<StelObject>(n);
    }

    // Search by IC numbers (possible formats are "IC466" or "IC 466")
    if (objw.mid(0, 2) == "IC")
    {
        foreach (const NebulaP& n, nebArray)
        {
            if (QString("IC%1").arg(n->IC_nb) == objw || QString("IC %1").arg(n->IC_nb) == objw)
                return qSharedPointerCast<StelObject>(n);
        }
    }

    // Search by Messier numbers (possible formats are "M31" or "M 31")
    if (objw.mid(0, 1) == "M")
    {
        foreach (const NebulaP& n, nebArray)
        {
            if (QString("M%1").arg(n->M_nb) == objw || QString("M %1").arg(n->M_nb) == objw)
                return qSharedPointerCast<StelObject>(n);
        }
    }

    // Search by Caldwell numbers (possible formats are "C31" or "C 31")
    if (objw.mid(0, 1) == "C")
    {
        foreach (const NebulaP& n, nebArray)
        {
            if (QString("C%1").arg(n->C_nb) == objw || QString("C %1").arg(n->C_nb) == objw)
                return qSharedPointerCast<StelObject>(n);
        }
    }

    return NULL;
}


//! Find and return the list of at most maxNbItem objects auto-completing the passed object I18n name
QStringList NebulaMgr::listMatchingObjectsI18n(const QString& objPrefix, int maxNbItem, bool useStartOfWords) const
{
    QStringList result;
    if (maxNbItem==0) return result;

    QString objw = objPrefix.toUpper();
    // Search by Messier objects number (possible formats are "M31" or "M 31")
    if (objw.size()>=1 && objw[0]=='M')
    {
        foreach (const NebulaP& n, nebArray)
        {
            if (n->M_nb==0) continue;
            QString constw = QString("M%1").arg(n->M_nb);
            QString constws = constw.mid(0, objw.size());
            if (constws==objw)
            {
                result << constws;
                continue;	// Prevent adding both forms for name
            }
            constw = QString("M %1").arg(n->M_nb);
            constws = constw.mid(0, objw.size());
            if (constws==objw)
                result << constw;
        }
    }

    // Search by IC objects number (possible formats are "IC466" or "IC 466")
    if (objw.size()>=1 && objw[0]=='I')
    {
        foreach (const NebulaP& n, nebArray)
        {
            if (n->IC_nb==0 || n->IC_nb>5386) continue;
            QString constw = QString("IC%1").arg(n->IC_nb);
            QString constws = constw.mid(0, objw.size());
            if (constws==objw)
            {
                result << constws;
                continue;	// Prevent adding both forms for name
            }
            constw = QString("IC %1").arg(n->IC_nb);
            constws = constw.mid(0, objw.size());
            if (constws==objw)
                result << constw;
        }
    }

    // Search by NGC numbers (possible formats are "NGC31" or "NGC 31")
    foreach (const NebulaP& n, nebArray)
    {
        if (n->NGC_nb==0) continue;
        QString constw = QString("NGC%1").arg(n->NGC_nb);
        QString constws = constw.mid(0, objw.size());
        if (constws==objw)
        {
            result << constws;
            continue;
        }
        constw = QString("NGC %1").arg(n->NGC_nb);
        constws = constw.mid(0, objw.size());
        if (constws==objw)
            result << constw;
    }

    //silas
    if (objw.size()>=1 && objw[0]=='N'){

        QRegExp catNumRx11("^N(\\d+)$");
        if (catNumRx11.exactMatch(objw))
        {
            int num = catNumRx11.capturedTexts().at(1).toInt();
            foreach (const NebulaP& n, nebArray)
            {

                if (n->NGC_nb==0) continue;
                QString constw = QString("NGC%1").arg(n->NGC_nb);
                QString constws = constw.mid(0, objw.size());
                if (constws==QString("NGC%1").arg(num))
                {
                    result << constws;
                    continue;
                }
                constw = QString("NGC %1").arg(n->NGC_nb);
                constws = constw.mid(0, objw.size());
                if (constws==QString("NGC %1").arg(num))
                    result << constw;
            }
        }
    }

    // Search by caldwell objects number (possible formats are "C31" or "C 31")
    if (objw.size()>=1 && objw[0]=='C')
    {
        foreach (const NebulaP& n, nebArray)
        {
            if (n->C_nb==0) continue;
            QString constw = QString("C%1").arg(n->C_nb);
            QString constws = constw.mid(0, objw.size());
            if (constws==objw)
            {
                result << constws;
                continue;	// Prevent adding both forms for name
            }
            constw = QString("C %1").arg(n->C_nb);
            constws = constw.mid(0, objw.size());
            if (constws==objw)
                result << constw;
        }
    }

    QString dson;
    bool find;
    // Search by common names
    //exclude pure number
    //silas
    QRegExp catNumRx11("^(\\d+)$");
    if (catNumRx11.exactMatch(objw))
    {
    }
    else{
        foreach (const NebulaP& n, nebArray)
        {
            dson = n->nameI18;
            find = false;
            if (useStartOfWords)
            {
                if (dson.mid(0, objw.size()).toUpper()==objw)
                    find = true;

            }
            else
            {
                if (dson.contains(objPrefix, Qt::CaseInsensitive))
                    find = true;
            }
            if (find)
                result << dson;
        }
    }





    //result.sort();
    if (maxNbItem > 0)
    {
        if (result.size()>maxNbItem) result.erase(result.begin()+maxNbItem, result.end());
    }
    return result;
}

//! Find and return the list of at most maxNbItem objects auto-completing the passed object English name
QStringList NebulaMgr::listMatchingObjects(const QString& objPrefix, int maxNbItem, bool useStartOfWords) const
{
    QStringList result;
    if (maxNbItem==0) return result;

     QString objw = objPrefix.toUpper();


     //silas
     if (objw.size()>=1 && objw[0]=='N'){

         QRegExp catNumRx11("^N(\\d+)$");
         if (catNumRx11.exactMatch(objw))
         {
             int num = catNumRx11.capturedTexts().at(1).toInt();
             result << QString("NGC%1").arg(num);

         }
     }
     //silas
     if (objw.size()>=1 && objw[0]=='I'){

         QRegExp catNumRx11("^I(\\d+)$");
         if (catNumRx11.exactMatch(objw))
         {
             int num = catNumRx11.capturedTexts().at(1).toInt();
             result << QString("IC%1").arg(num);

         }
     }
     else if (objw.size()>=1){
         QRegExp catNumRx11("^(\\d+)$");
         if (catNumRx11.exactMatch(objw))
         {
             int num = catNumRx11.capturedTexts().at(1).toInt();
             if (num <= 110 && num > 0)
                 result << QString("M%1").arg(num);
             if (num <= 7840 && num > 0)
                 result << QString("NGC%1").arg(num);
         }
     }

    // Search by Messier objects number (possible formats are "M31" or "M 31")
    if (objw.size()>=1 && objw[0]=='M')
    {
        foreach (const NebulaP& n, nebArray)
        {
            if (n->M_nb==0) continue;
            QString constw = QString("M%1").arg(n->M_nb);
            QString constws = constw.mid(0, objw.size());
            if (constws==objw)
            {
                result << constws;
                continue;	// Prevent adding both forms for name
            }
            constw = QString("M %1").arg(n->M_nb);
            constws = constw.mid(0, objw.size());
            if (constws==objw)
                result << constw;
        }
    }

    // Search by IC objects number (possible formats are "IC466" or "IC 466")
    if (objw.size()>=1 && objw[0]=='I')
    {
        foreach (const NebulaP& n, nebArray)
        {
            if (n->IC_nb==0||n->IC_nb>5386) continue;
            QString constw = QString("IC%1").arg(n->IC_nb);
            QString constws = constw.mid(0, objw.size());
            if (constws==objw)
            {
                result << constws;
                continue;	// Prevent adding both forms for name
            }
            constw = QString("IC %1").arg(n->IC_nb);
            constws = constw.mid(0, objw.size());
            if (constws==objw)
                result << constw;
        }
    }

    // Search by NGC numbers (possible formats are "NGC31" or "NGC 31")
    foreach (const NebulaP& n, nebArray)
    {
        if (n->NGC_nb==0) continue;
        QString constw = QString("NGC%1").arg(n->NGC_nb);
        QString constws = constw.mid(0, objw.size());
        if (constws==objw)
        {
            result << constws;
            continue;
        }
        constw = QString("NGC %1").arg(n->NGC_nb);
        constws = constw.mid(0, objw.size());
        if (constws==objw)
            result << constw;
    }

    // Search by caldwell objects number (possible formats are "C31" or "C 31")
    if (objw.size()>=1 && objw[0]=='C')
    {
        foreach (const NebulaP& n, nebArray)
        {
            if (n->C_nb==0) continue;
            QString constw = QString("C%1").arg(n->C_nb);
            QString constws = constw.mid(0, objw.size());
            if (constws==objw)
            {
                result << constws;
                continue;	// Prevent adding both forms for name
            }
            constw = QString("C %1").arg(n->C_nb);
            constws = constw.mid(0, objw.size());
            if (constws==objw)
                result << constw;
        }
    }

    QString dson;
    bool find;
    // Search by common names
    //exclude pure number
    //silas
    QRegExp catNumRx11("^(\\d+)$");
    if (catNumRx11.exactMatch(objw))
    {
    }
    else{
        foreach (const NebulaP& n, nebArray)
        {
            dson = n->englishName;
            find = false;
            if (useStartOfWords)
            {
                if (dson.mid(0, objw.size()).toUpper()==objw)
                    find = true;

            }
            else
            {
                if (dson.contains(objPrefix, Qt::CaseInsensitive))
                    find = true;
            }
            if (find)
                result << dson;
        }
    }


    //result.sort();
    if (maxNbItem > 0)
    {
        if (result.size()>maxNbItem) result.erase(result.begin()+maxNbItem, result.end());
    }

    return result;
}

