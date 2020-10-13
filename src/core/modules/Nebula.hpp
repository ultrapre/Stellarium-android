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

#ifndef _NEBULA_HPP_
#define _NEBULA_HPP_

#include <QString>
#include "StelObject.hpp"
#include "StelTranslator.hpp"
#include "StelTextureTypes.hpp"

class StelPainter;
class QDataStream;

class Nebula : public StelObject
{
friend class NebulaMgr;
public:
    Nebula();
    ~Nebula();

    //! Nebula support the following InfoStringGroup flags:
    //! - Name
    //! - CatalogNumber
    //! - Magnitude
    //! - RaDec
    //! - AltAzi
    //! - Distance
    //! - Size
    //! - Extra (contains the Nebula type, which might be "Galaxy", "Cluster" or similar)
    //! - PlainText
    //! @param core the StelCore object
    //! @param flags a set of InfoStringGroup items to include in the return value.
    //! @return a QString containing an HMTL encoded description of the Nebula.
    virtual QString getInfoString(const StelCore *core, const InfoStringGroup& flags) const;
    virtual QString getType() const {return "Nebula";}
    virtual Vec3d getJ2000EquatorialPos(const StelCore*) const {return XYZ;}
    virtual double getCloseViewFov(const StelCore* core = NULL) const;
    virtual float getVMagnitude(const StelCore* core) const;
    virtual float getSelectPriority(const StelCore* core) const;
    virtual Vec3f getInfoColor() const;
    virtual QString getNameI18n() const {return nameI18;}
    virtual QString getEnglishName() const {return englishName;}
    virtual double getAngularSize(const StelCore*) const {return angularSize*0.5;}
    virtual SphericalRegionP getRegion() const {return pointRegion;}

    // Methods specific to Nebula
    void setLabelColor(const Vec3f& v) {labelColor = v;}
    void setCircleColor(const Vec3f& v) {circleColor = v;}

    //! Get the printable nebula Type.
    //! @return the nebula type code.
    QString getTypeString() const;


    //! Get the printable nebula bmag.
    //! @return the nebula bmag code.
    QString getbmagString() const;

    //! Get the printable nebula bV.
    //! @return the nebula bV code.
    QString getbVString() const;

    //! Get the printable nebula surfBright.
    //! @return the nebula surfBright code.
    QString getsurfBrightString() const;

    //! Get the printable nebula posAngle.
    //! @return the nebula posAngle code.
    QString getposAngleString() const;

    //! Get the printable nebula nickName.
    //! @return the nebula nickName code.
    QString getnickNameString() const;

    //! Get the printable nebula dsp.
    //! @return the nebula dsp code.
    QString getdspString() const;

private:
    friend struct DrawNebulaFuncObject;

    //! @enum NebulaType Nebula types
    enum NebulaType
    {
        //NebGx=0,     //!< Galaxy
        //NebOc=1,     //!< Open star cluster
        //NebGc=2,     //!< Globular star cluster, usually in the Milky Way Galaxy
        //NebN=3,      //!< Bright emission or reflection nebula
        //NebPn=4,     //!< Planetary nebula
        //NebDn=5,     //!< ??? Dark Nebula?      Does not exist in current catalog
        //NebIg=6,     //!< ??? Irregular Galaxy? Does not exist in current catalog
        //NebCn=7,     //!< Cluster associated with nebulosity
        //NebUnknown=8 //!< Unknown type, catalog errors, "Unidentified Southern Objects" etc.

        NebGX=0,
        NebGXC=9,
        NebNB=3,
        NebSNR=10,
        NebPN=4,
        NebOC=1,
        NebGC=2,
        NebPrt=11,
        NebDN=5,
        NebClN=7,
        NebAss=12,
        NebSCu=13,
        NebS=14,
        NebCbS=15,
        NebIFN=16,
        NebDS=17,
        NebVS=18,
        NebNF=8,
        NebDup=19
    };

    //! Translate nebula name using the passed translator
    void translateName(const StelTranslator& trans) {nameI18 = trans.qtranslate(englishName);}

    bool readNGC(char *record);
    void readNGC(QDataStream& in);

    void drawLabel(StelPainter& sPainter, float maxMagLabel);
    void drawHints(StelPainter& sPainter, float maxMagHints);

    unsigned int M_nb;              // Messier Catalog number
    unsigned int NGC_nb;            // New General Catalog number
    unsigned int IC_nb;             // Index Catalog number
    unsigned int C_nb;              // Caldwell Catalog number
    QString englishName;            // English name (Science name)
    QString nameI18;                // Nebula name
    float mag;                      // Apparent magnitude

    float bmag;                     // blue magnitude
    float bV;                       // b-v magnitude
    float surfBright;               // Surface Bright
    float posAngle;                 // Position angle

    QString typeClass;               // nick name
    QString dsp;                    // Mono Type + description

    float angularSize;              // Angular size in degree
    Vec3d XYZ;                      // Cartesian equatorial position
    Vec3d XY;                       // Store temporary 2D position
    NebulaType nType;

    SphericalRegionP pointRegion;

    static StelTextureSP texCircle;				// The symbolic circle texture
            static StelTextureSP texCircleLarge;			// The symbolic circle texture for large objects
            static StelTextureSP texRegion;				// The symbolic dashed shape texture
            static StelTextureSP texGalaxy;				// Type 0
            static StelTextureSP texGalaxyLarge;			// Type 0_large
            static StelTextureSP texOpenCluster;			// Type 1
            static StelTextureSP texOpenClusterLarge;		// Type 1_large
            static StelTextureSP texOpenClusterXLarge;	// Type 1_extralarge
            static StelTextureSP texGlobularCluster;		// Type 2
            static StelTextureSP texGlobularClusterLarge;	// Type 2_large
            static StelTextureSP texPlanetaryNebula;		// Type 3
            static StelTextureSP texDiffuseNebula;		// Type 4
            static StelTextureSP texDiffuseNebulaLarge;	// Type 4_large
            static StelTextureSP texDiffuseNebulaXLarge;	// Type 4_extralarge
            static StelTextureSP texDarkNebula;			// Type 5
            static StelTextureSP texDarkNebulaLarge;		// Type 5_large
            static StelTextureSP texOpenClusterWithNebulosity;	// Type 6
            static StelTextureSP texOpenClusterWithNebulosityLarge;	// Type 6_large
    static float hintsBrightness;

    static Vec3f labelColor, circleColor;
    static float circleScale;       // Define the scaling of the hints circle
};

#endif // _NEBULA_HPP_

