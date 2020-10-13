/*
 * Stellarium
 * Copyright (C) 2007 Fabien Chereau
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

#ifndef _STELSKYDRAWER_HPP_
#define _STELSKYDRAWER_HPP_

#include "StelTextureTypes.hpp"
#include "StelProjectorType.hpp"
#include "VecMath.hpp"
#include "RefractionExtinction.hpp"

#include <QObject>

class StelToneReproducer;
class StelCore;
class StelPainter;

//! Contains the 2 parameters necessary to draw a star on screen.
//! the radius and luminance of the star halo texture.
//！包含在屏幕上绘制星星所需的2个参数。
//！星晕纹理的半径和亮度。
struct RCMag
{
	float radius;
	float luminance;
};

//! @class StelSkyDrawer
//! Provide a set of methods used to draw sky objects taking into account
//! eyes adaptation, zoom level, instrument model and artificially set magnitude limits
//！提供一套用于绘制天空物体的方法，同时考虑到
//！眼睛适应，缩放级别，仪器模型和人为设置的幅度限制
class StelSkyDrawer : public QObject
{
	Q_OBJECT
public:

	//! Constructor
	StelSkyDrawer(StelCore* core);
	//! Destructor
	~StelSkyDrawer();

	//! Init parameters from config file
	void init();

	//! Update with respect to the time and StelProjector/StelToneReproducer state
	//! @param deltaTime the time increment in second since last call.
	//！关于时间和StelProjector / StelToneReproducer状态的更新
	//！@param deltaTime自上次调用以来以秒为单位的时间增量。
	void update(double deltaTime);

	//! Set the proper openGL state before making calls to drawPointSource
	//! @param p a pointer to a valid instance of a Painter. The instance must be valid until postDrawPointSource() is called
	//！在调用drawPointSource之前，设置适当的openGL状态
	//！@param pa指向Painter的有效实例的指针。该实例必须有效，直到调用postDrawPointSource（）
	void preDrawPointSource(StelPainter* p);

	//! Finalize the drawing of point sources
	//！最终确定点源图
	void postDrawPointSource(StelPainter* sPainter);

	//! Draw a point source halo.
	//! @param sPainter the StelPainter to use for drawing.
	//! @param v the 3d position of the source in J2000 reference frame
	//! @param rcMag the radius and luminance of the source as computed by computeRCMag()
	//! @param bV the source B-V index
	//! @param checkInScreen whether source in screen should be checked to avoid unnecessary drawing.
	//! @return true if the source was actually visible and drawn
	
	//！绘制点源光晕。
	//！@param sPainter StelPainter用于绘图。
	//！@param v源在J2000参考框架中的3d位置
	//！@param rcMag由computeRCMag（）计算的源的半径和亮度
	//！@param bV源BV索引
	//！@param checkInScreen是否应检查屏幕中的源，以避免不必要的绘制。
	//！@return true如果源是实际可见和绘制的
	bool drawPointSource(StelPainter* sPainter, const Vec3f& v, const RCMag &rcMag, unsigned int bV, bool checkInScreen=false)
		{return drawPointSource(sPainter, v, rcMag, colorTable[bV], checkInScreen);}

	bool drawPointSource(StelPainter* sPainter, const Vec3f& v, const RCMag &rcMag, const Vec3f& bcolor, bool checkInScreen=false);

	//! Terminate drawing of a 3D model, draw the halo
	//! @param p the StelPainter instance to use for this drawing operation
	//! @param v the 3d position of the source in J2000 reference frame
	//! @param illuminatedArea the illuminated area in arcmin^2
	//! @param mag the source integrated magnitude
	//! @param color the object halo RGB color
	
	//！终止3D模型的绘制，绘制光晕
	//！@param p用于此绘图操作的StelPainter实例
	//！@param v源在J2000参考框架中的3d位置
	//！@paramilluminatedArea以arcmin ^ 2表示的照明区域
	//！@param mag源综合幅度
	//！@param color对象光晕RGB颜色
	void postDrawSky3dModel(StelPainter* p, const Vec3f& v, float illuminatedArea, float mag, const Vec3f& color = Vec3f(1.f,1.f,1.f));

	//! Compute RMag and CMag from magnitude.
	//! @param mag the object integrated V magnitude
	//! @param rcMag array of 2 floats containing the radius and luminance
	//! @return false if the object is too faint to be displayed
	
	//！根据幅度计算RMag和CMag。
	//！@param mag对象的积分V大小
	//！@param包含2个浮点的rcMag数组，包含半径和亮度
	//！如果对象太微弱而无法显示，则@return false
	bool computeRCMag(float mag, RCMag*) const;

	//! Report that an object of luminance lum with an on-screen area of area pixels is currently displayed
	//! This information is used to determine the world adaptation luminance
	//! This method should be called during the update operations of the main loop
	//! @param lum luminance in cd/m^2
	//! @param fastAdaptation adapt the eye quickly if true, other wise use a smooth adaptation
	
	//！报告当前正在显示屏幕区域为像素的亮度不佳的对象
	//！该信息用于确定世界适应亮度
	//！在主循环的更新操作期间应调用此方法
	//！@para lum亮度，以cd / m ^ 2为单位
	//！@param fastAdaptation如果为真，则快速适应眼睛，否则明智地使用平滑适应
	void reportLuminanceInFov(float lum, bool fastAdaptation=false);

	//! To be called before the drawing stage starts
	
	//！在绘图阶段开始之前被调用
	void preDraw();

	//! Compute the luminance for an extended source with the given surface brightness
	//! @param sb surface brightness in V magnitude/arcmin^2
	//! @return the luminance in cd/m^2
	
	//！计算具有给定表面亮度的扩展光源的亮度
	//！@param sb表面亮度，单位为V幅值/ arcmin ^ 2
	//！@以cd / m ^ 2返回亮度
	static float surfacebrightnessToLuminance(float sb);
	
	
	//! Compute the surface brightness from the luminance of an extended source
	//! @param lum luminance in cd/m^2
	//! @return surface brightness in V magnitude/arcmin^2
	
	//！根据扩展光源的亮度计算表面亮度
	//！@para lum亮度，以cd / m ^ 2为单位
	//！@返回表面亮度，单位为V幅值/ arcmin ^ 2
	static float luminanceToSurfacebrightness(float lum);

	//! Convert quantized B-V index to float B-V
	//！将量化的BV指数转换为浮动BV
	static inline float indexToBV(unsigned char bV)
	{
		return (float)bV*(4.f/127.f)-0.5f;
	}

	//! Convert quantized B-V index to RGB colors
	//！将量化的BV索引转换为RGB颜色
	static inline const Vec3f& indexToColor(unsigned char bV)
	{
		return colorTable[bV];
	}

public slots:
	//! Set the way brighter stars will look bigger as the fainter ones
	//！设置明亮的星星看起来像较暗的星星更大的方式
	void setRelativeStarScale(double b=1.0) {starRelativeScale=b;}
	//! Get the way brighter stars will look bigger as the fainter ones
	//！找出明亮的星星看起来像较暗的星星一样大的方法
	float getRelativeStarScale() const {return starRelativeScale;}

	//! Set the absolute star brightness scale
	//！设置恒星绝对亮度等级
	void setAbsoluteStarScale(double b=1.0) {starAbsoluteScaleF=b;}
	//! Get the absolute star brightness scale
	//！获得绝对的星星亮度等级
	float getAbsoluteStarScale() const {return starAbsoluteScaleF;}

	//! Set source twinkle amount.
	//！设置源闪烁量。
	void setTwinkleAmount(double b) {twinkleAmount=b;}
	//! Get source twinkle amount.
	//！获取源闪烁量。
	float getTwinkleAmount() const {return twinkleAmount;}

	//! Set flag for source twinkling.
	//！设置源闪烁的标志。
	void setFlagTwinkle(bool b) {flagStarTwinkle=b;}
	//! Get flag for source twinkling.
	//！获取源闪烁的标志。
	bool getFlagTwinkle() const {return flagStarTwinkle;}

	//! Set the parameters so that the stars disapear at about the limit given by the bortle scale
	//! The limit is valid only at a given zoom level (around 60 deg)
	//! See http://en.wikipedia.org/wiki/Bortle_Dark-Sky_Scale
	//！设置参数，使恒星消失在大约贝特刻度所给定的极限处
	//！该限制仅在给定的缩放级别（约60度）下有效
	//！参见http://en.wikipedia.org/wiki/Bortle_Dark-Sky_Scale
	void setBortleScale(int index);
	//! Get the current Bortle scale index
	//！获取当前的Bortle比例指数
	int getBortleScale() const {return bortleScaleIndex;}

	//! Get the magnitude of the currently faintest visible point source
	//! It depends on the zoom level, on the eye adapation and on the point source rendering parameters
	//! @return the limit V mag at which a point source will be displayed
	//！获取当前最模糊的可见点源的大小
	//！它取决于缩放级别，眼睛适应度和点源渲染参数
	//！@返回将显示点源的极限V mag
	float getLimitMagnitude() const {return limitMagnitude;}

	//! Toggle the application of user-defined star magnitude limit.
	//! If enabled, stars fainter than the magnitude set with
	//! setCustomStarMagnitudeLimit() will not be displayed.
	// FIXME: Exposed to scripts - make sure it synchs with the GUI. --BM
	//！切换用户定义的星等极限的应用。
	//！如果启用，则恒星将比使用
	//！setCustomStarMagnitudeLimit（）将不会显示。
	// FIXME：暴露给脚本-确保它与GUI同步。--BM
	void setFlagStarMagnitudeLimit(bool b) {flagStarMagnitudeLimit = b;}
	
	//! Toggle the application of user-defined deep-sky object magnitude limit.
	//! If enabled, deep-sky objects fainter than the magnitude set with
	//! setCustomNebulaMagnitudeLimit() will not be displayed.
	// FIXME: Exposed to scripts - make sure it synchs with the GUI. --BM
	//！切换用户定义的深空天体大小限制的应用。
	//！如果启用，深空物体将变得比设置的幅度暗
	//！setCustomNebulaMagnitudeLimit（）将不会显示。
	// FIXME：暴露给脚本-确保它与GUI同步。--BM
	void setFlagNebulaMagnitudeLimit(bool b) {flagNebulaMagnitudeLimit = b;}
	//! @return true if the user-defined star magnitude limit is in force.
	//！@return如果用户定义的星等极限有效，则为true。
	bool getFlagStarMagnitudeLimit() const {return flagStarMagnitudeLimit;}
	//! @return true if the user-defined nebula magnitude limit is in force.
	//！@return如果用户定义的星云大小限制生效，则为true。
	bool getFlagNebulaMagnitudeLimit() const {return flagNebulaMagnitudeLimit;}

	//! Get the value used for forced star magnitude limiting.
	//！获取用于强制恒星大小限制的值。
	float getCustomStarMagnitudeLimit() const {return customStarMagLimit;}
	//! Sets a lower limit for star magnitudes (anything fainter is ignored).
	//! In force only if flagStarMagnitudeLimit is set.
	//！设置星等的下限（任何微弱的现象都将被忽略）。
	//！仅当设置flagStarMagnitudeLimit时有效。
	void setCustomStarMagnitudeLimit(double limit) {customStarMagLimit=limit;}
	//! Get the value used for forced nebula magnitude limiting.
	//！获取用于强制星云大小限制的值。
	float getCustomNebulaMagnitudeLimit() const {return customNebulaMagLimit;}
	//! Sets a lower limit for nebula magnitudes (anything fainter is ignored).
	//! In force only if flagNebulaMagnitudeLimit is set.
	//！设置星云大小的下限（微弱的任何东西都将被忽略）。
	//！仅当设置flagNebulaMagnitudeLimit时有效。
	void setCustomNebulaMagnitudeLimit(double limit) {customNebulaMagLimit=limit;}

	//! Get the luminance of the faintest visible object (e.g. RGB<0.05)
	//! It depends on the zoom level, on the eye adapation and on the point source rendering parameters
	//! @return the limit V luminance at which an object will be visible
	//！获取最暗的可见对象的亮度（例如RGB <0.05）
	//！它取决于缩放级别，眼睛适应度和点源渲染参数
	//！@返回可见物体的极限V亮度
	float getLimitLuminance() const {return limitLuminance;}

	//! Set the value of the eye adaptation flag
	//！设置眼睛适应标志的值
	void setFlagLuminanceAdaptation(bool b) {flagLuminanceAdaptation=b;} //silas =b
	//! Get the current value of eye adaptation flag
	//！获取眼睛适应标志的当前值
	bool getFlagLuminanceAdaptation() const {return flagLuminanceAdaptation;} //silas fixed flagLuminanceAdaptation

	//! Informing the drawer whether atmosphere is displayed.
	//! This is used to avoid twinkling/simulate extinction/refraction.
	//！通知抽屉是否显示气氛。
	//！这用于避免闪烁/模拟消光/折射。
	void setFlagHasAtmosphere(bool b) {flagHasAtmosphere=b;}
	//! This is used to decide whether to apply refraction/extinction before rendering point sources et al.
	//！这用于决定是否在渲染点源之前应用折射/消光等。
	bool getFlagHasAtmosphere() const {return flagHasAtmosphere;}

	//! Set extinction coefficient, mag/airmass (for extinction).
	//！设置消光系数，mag / airmass（消光）。
	void setExtinctionCoefficient(double extCoeff) {extinction.setExtinctionCoefficient(extCoeff);}
	//! Get extinction coefficient, mag/airmass (for extinction).
	//！获取消光系数，mag / airmass（用于消光）。
	double getExtinctionCoefficient() const {return extinction.getExtinctionCoefficient();}
	//! Set atmospheric (ground) temperature in deg celsius (for refraction).
	//！以摄氏度为单位设置大气（地面）温度（用于折射）。
	void setAtmosphereTemperature(double celsius) {refraction.setTemperature(celsius);}
	//! Get atmospheric (ground) temperature in deg celsius (for refraction).
	//！获取摄氏度的大气（地面）温度（用于折射）。
	double getAtmosphereTemperature() const {return refraction.getTemperature();}
	//! Set atmospheric (ground) pressure in mbar (for refraction).
	//！设置大气压（地面）为mbar（用于折射）。
	void setAtmospherePressure(double mbar) {refraction.setPressure(mbar);}
	//! Get atmospheric (ground) pressure in mbar (for refraction).
	//！获得以mbar为单位的大气压（地面）（用于折射）。
	double getAtmospherePressure() const {return refraction.getPressure();}

	//! Get the current valid extinction computation class.
	//！获取当前有效的消光计算类。
	const Extinction& getExtinction() const {return extinction;}
	//! Get the current valid fefraction computation class.
	//！获取当前有效的分数计算类。
	const Refraction& getRefraction() const {return refraction;}

	//! Get the radius of the big halo texture used when a 3d model is very bright.
	//！获取3d模型非常明亮时使用的大光晕纹理的半径。
	float getBig3dModelHaloRadius() const {return big3dModelHaloRadius;}
	//! Set the radius of the big halo texture used when a 3d model is very bright.
	//！设置3d模型非常明亮时使用的大光晕纹理的半径。
	void setBig3dModelHaloRadius(float r) {big3dModelHaloRadius=r;}
	
private:
	// Debug
	float reverseComputeRCMag(float rmag) const;

	//! Compute the current limit magnitude by dichotomy
	//！通过二分法计算极限幅度
	float computeLimitMagnitude() const;

	//! Compute the current limit luminance by dichotomy
	float computeLimitLuminance() const;

	//! Get StelSkyDrawer maximum FOV.
	//！获取StelSkyDrawer的最大FOV。
	float getMaxAdaptFov(void) const {return maxAdaptFov;}
	//! Set StelSkyDrawer maximum FOV.
	//! Usually stars/planet halos are drawn fainter when FOV gets larger,
	//! but when FOV gets larger than this value, the stars do not become
	//! fainter any more. Must be >= 60.0.
	//！设置StelSkyDrawer最大FOV。
	//！通常，当FOV变大时，恒星/行星晕变弱，
	//！但是当FOV大于此值时，星星不会变
	//！昏了过去。必须> = 60.0。
	void setMaxAdaptFov(float fov) {maxAdaptFov = (fov < 60.f) ? 60.f : fov;}

	//! Get StelSkyDrawer minimum FOV.
	//！获取StelSkyDrawer的最小FOV。
	float getMinAdaptFov(void) const {return minAdaptFov;}
	//! Set StelSkyDrawer minimum FOV.
	//! Usually stars/planet halos are drawn brighter when FOV gets smaller.
	//! But when FOV gets smaller than this value, the stars do not become
	//! brighter any more. Must be <= 60.0.
	//！设置StelSkyDrawer的最小FOV。
	//！通常，当FOV变小时，恒星/行星晕变亮。
	//！但是当FOV小于此值时，星星不会变
	//！再亮一点。必须小于等于60.0。
	void setMinAdaptFov(float fov) {minAdaptFov = (fov > 60.f) ? 60.f : fov;}

	//! Set the scaling applied to input luminance before they are converted by the StelToneReproducer
	//！在StelToneReproducer转换输入亮度之前，设置应用于输入亮度的缩放比例
	void setInputScale(float in) {inScale = in;}
	//! Get the scaling applied to input luminance before they are converted by the StelToneReproducer
	//！在StelToneReproducer转换输入亮度之前，获取应用于输入亮度的缩放比例
	float getInputScale() const {return inScale;}

	//! Compute the luminance for a point source with the given mag for the current FOV
	//! @param mag V magnitude of the point source
	//! @return the luminance in log(cd/m^2)
	//！使用当前视场的给定mag计算点光源的亮度
	//！@param mag V点源的大小
	//！@以log（cd / m ^ 2）返回亮度
	inline float pointSourceMagToLuminance(float mag) const {return std::exp(pointSourceMagToLnLuminance(mag));}

	//! Compute the V magnitude for a point source with the given luminance for the current FOV
	//! @param lum the luminance in cd/m^2
	//! @return V magnitude of the point source
	//！为当前FOV计算具有给定亮度的点光源的V大小
	//！@param lum以cd / m ^ 2为单位的亮度
	//！@return V点源的幅度
	float pointSourceLuminanceToMag(float lum);

	//! Compute the log of the luminance for a point source with the given mag for the current FOV
	//! @param mag V magnitude of the point source
	//! @return the luminance in cd/m^2
	//！为当前FOV计算具有给定mag的点光源的亮度的对数
	//！@param mag V点源的大小
	//！@以cd / m ^ 2返回亮度
	float pointSourceMagToLnLuminance(float mag) const;

	//! Find the world adaptation luminance to use so that a point source of magnitude mag
	//! is displayed with a halo of size targetRadius
	//！找到要使用的世界适应亮度，以便将一个大小为mag的点源
	//！显示的光晕大小为targetRadius
	float findWorldLumForMag(float mag, float targetRadius);

	StelCore* core;
	StelToneReproducer* eye;

	Extinction extinction;
	Refraction refraction;

	float maxAdaptFov, minAdaptFov, lnfovFactor;
	bool flagStarTwinkle;
	float twinkleAmount;

	//! Informing the drawer whether atmosphere is displayed.
	//! This is used to avoid twinkling/simulate extinction/refraction.
	//！通知抽屉是否显示气氛。
	//！这用于避免闪烁/模拟消光/折射。
	bool flagHasAtmosphere;

	//! Controls the application of the user-defined star magnitude limit.
	//! @see customStarMagnitudeLimit
	//！控制用户定义的星等极限的应用。
	//！@see customStarMagnitudeLimit
	bool flagStarMagnitudeLimit;
	//! Controls the application of the user-defined nebula magnitude limit.
	//! @see customNebulaMagnitudeLimit
	//！控制用户定义的星云大小限制的应用。
	//！@see customNebulaMagnitudeLimit
	bool flagNebulaMagnitudeLimit;

	float starRelativeScale;
	float starAbsoluteScaleF;

	float starLinearScale;	// optimization variable

	//! Current magnitude limit for point sources
	float limitMagnitude;

	//! Current magnitude luminance
	float limitLuminance;

	//! User-defined magnitude limit for stars.
	//! Interpreted as a lower limit - stars fainter than this value will not
	//! be displayed.
	//! Used if flagStarMagnitudeLimit is true.
	//！用户定义的恒星大小限制。
	//！解释为下限-昏暗的恒星不会超过此值
	//！被显示。
	//！如果flagStarMagnitudeLimit为true，则使用此属性。
	float customStarMagLimit;
	//! User-defined magnitude limit for deep-sky objects.
	//! Interpreted as a lower limit - nebulae fainter than this value will not
	//! be displayed.
	//! Used if flagNebulaMagnitudeLimit is true.
	//! @todo Why the asterisks this is not in NebulaMgr? --BM
	//！用户定义的深空物体的大小限制。
	//！解释为下限-比此值微弱的星云不会
	//！被显示。
	//！如果flagNebulaMagnitudeLimit为true，则使用此方法。
	//！@todo为什么星号不在NebulaMgr中？--BM
	float customNebulaMagLimit;

	//! Little halo texture
	//！晕轮纹理
	StelTextureSP texHalo;

	//! Load B-V conversion parameters from config file
	//！从配置文件加载BV转换参数
	void initColorTableFromConfigFile(class QSettings* conf);

	//! Contains the list of colors matching a given B-V index
	//！包含与给定BV索引匹配的颜色列表
	static Vec3f colorTable[128];

	//! The current Bortle Scale index
	//！当前的Bortle量表索引
	int bortleScaleIndex;

	//! The scaling applied to input luminance before they are converted by the StelToneReproducer
	//！在StelToneReproducer转换输入亮度之前将其应用于输入亮度
	float inScale;

	// Variables used for GL optimization when displaying point sources
	//! Vertex format for a point source.
	//! Texture pos is stored in another separately.
	//显示点源时用于GL优化的变量
	//！点源的顶点格式。
	//！纹理pos分别存储在另一个中。
	struct StarVertex {
		Vec2f pos;
		unsigned char color[4];
	};
	
	//! Buffer for storing the vertex array data
	//！用于存储顶点数组数据的缓冲区
	StarVertex* vertexArray;

	//! Buffer for storing the texture coordinate array data.
	//！用于存储纹理坐标数组数据的缓冲区。
	unsigned char* textureCoordArray;
	
	class QOpenGLShaderProgram* starShaderProgram;
	struct StarShaderVars {
		int projectionMatrix;
		int texCoord;
		int pos;
		int color;
		int texture;
	};
	StarShaderVars starShaderVars;
	
	//! Current number of sources stored in the buffers (still to display)
	//！当前存储在缓冲区中的源数（仍显示）
	unsigned int nbPointSources;
	//! Maximum number of sources which can be stored in the buffers
	//！缓冲区中可以存储的最大源数
	unsigned int maxPointSources;

	//! The maximum transformed luminance to apply at the next update
	//！下次更新时要应用的最大变换亮度
	float maxLum;
	//! The previously used world luminance
	//！先前使用的世界亮度
	float oldLum;

	//! Big halo texture
	//！大光环纹理
	StelTextureSP texBigHalo;
	StelTextureSP texSunHalo;

	bool flagLuminanceAdaptation = false; //silas fixed

	float big3dModelHaloRadius;
};

#endif // _STELSKYDRAWER_HPP_
